//{ GNU GPL v.3
/***************************************************************************
*           auronNet - Implementation of event-driven spiking ANN          *
*                           -------------------                            *
* copyright            : (C) 2011 by Per R. Leikanger                      *
* email                : leikange@gmail.com                                *
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/
//}
#ifndef TID_H_
#define TID_H_

// TODO sjekk om desse er nødvendig (kommenter ut, når kompilering funker..)
//#include "main.h"
#include "../neuroElements/auron.h"

#include <typeinfo> //For å bryke typeid..

using std::cout;
using std::cerr;
using std::endl;

// DEKLARASJONER:
//extern unsigned long ulTime;
void loggeFunk_K_auron(); // auron.h



/****************************************
***      abstract class !             ***
****************************************/
class timeInterface
{
	public:
	timeInterface(std::string s) : dEstimatedTaskTime(0), sClassName(s){}

	virtual void doTask() =0;
	virtual void doCalculation() =0;
	//unsigned long dEstimatedTaskTime; 
	double dEstimatedTaskTime; 

	//for debugging:
	std::string sClassName;
};







/*********************************************************************************
** 	class time  																	**
** 		- time skal bare være en instans av denne klassen (i planen så langt). 		**
** 		 																		**
*********************************************************************************/
class time_class : public timeInterface {
	static unsigned long ulTime;
	
	static std::list<timeInterface*> pWorkTaskQueue;
	static std::list<timeInterface*> pCalculatationTaskQueue;

	// Liste som sjekkes ved kvar tidsiterering: Dersom eit element har dEstimatedTaskTime til neste tidssted legges peiker inn i pWorkTaskQueue.
	static std::list<timeInterface*> pPeriodicElements;

	protected:
	void doTask();
	void doCalculation()
	{ //{
		/**************************************************************************************
		*** Gjennomføre kalkulering på alle kalkuleringsoppgaver (pCalculatationTaskQueue) 	***
		**************************************************************************************/

		//  TA VEKK (redundant test?)
		if( pCalculatationTaskQueue.empty() ) return;

		// Organiserer liste slik at kvar oppføring er unik:
		for( std::list<timeInterface*>::iterator iter = pCalculatationTaskQueue.begin(); iter != pCalculatationTaskQueue.end(); iter++ )
		{
			std::list<timeInterface*>::iterator iter2 = iter; 
			iter2++;
			while(iter2!=pCalculatationTaskQueue.end()){
				// ser om iteratorene peker til samme minneadresse (samme timeInterface-element). Isåfall: fjærn det andre elementet.
			 	if( (*iter2) == (*iter) ){ 
					// Øker iterator før eg sletter element på iter2.
					std::list<timeInterface*>::iterator slettIter = iter2;
					iter2++;
					pCalculatationTaskQueue.erase(slettIter);
					continue;
				}
				iter2++;
			}
		}
	
		while( !pCalculatationTaskQueue.empty() ){
			// Kaller pCalculatationTaskQueue.front()->pCalculatationTaskQueue();
			pCalculatationTaskQueue.front()->doCalculation();

// BARE PISS TODO Men kan være nyttig kode..
#if 0
			// Legger til task i neste iterasjon, dersom dette er dEstimatedTaskTime:
			if( (unsigned)(pCalculatationTaskQueue.front()->dEstimatedTaskTime) < ulTime+2 ){ // Dersom den er før [nå+2], kan vi legge den til i pWorkTaskQueue (da er den mest sansynligvis til neste iter(Dette er jo siste ting som skjer!)
								//															// (doCalculation() er siste som skjer før elem. flyttes over i pWorkTaskQueue (i time_class::doTask() kalles doCalc() rett før flytting av elem)
				// FUNKER IKKJE: 	doTask(); (fører til at den bruker veldig lang tid (venter lenge..)
				cout<<"\e[1;33mOK:\t\e[0m dEsmatedTaskTime < ulTime+1: \e[31m" <<(pCalculatationTaskQueue.front())->dEstimatedTaskTime <<"\e[0m < \e[33m" <<ulTime <<"+1\e[0m]\n";	
			}else{
				cout<<"\e[1;31mFEIL:\t\e[0m dEsmatedTaskTime !< ulTime+1: \e[31m" <<(pCalculatationTaskQueue.front())->dEstimatedTaskTime <<"\e[0m !< \e[33m" <<ulTime <<"+1\e[0m]\n";	
			}
#endif
	
			// Går videre (popper første elementet)
			pCalculatationTaskQueue.pop_front();
		}



	} //}


	static inline void addTaskInPresentTimeIteration(timeInterface* pTimeClassArg_withTask);


	static inline void addCalculationIn_pCalculatationTaskQueue( timeInterface* pObject_arg)
	{
	 	pCalculatationTaskQueue.push_back( pObject_arg );
	}


	const static void skrivUt_pWorkTaskQueue()
	{ //{
		cout<<"Skriver ut pWorkTaskQueue: \n";
		int nIter = 0;
		// itererer gjennom ytre liste:
		for(std::list<timeInterface*>::iterator l_iter = pWorkTaskQueue.begin(); 	l_iter != pWorkTaskQueue.end() ; 	l_iter++ )
		{
			cout<<nIter <<"\t" <<typeid(*(*l_iter)).name() <<"\t(" <<(*l_iter)->sClassName <<"\tplanlagt til " <<(*l_iter)->dEstimatedTaskTime <<"\n";
			nIter++;
		}
		cout<<"\n\n";
	} //}


	public:
	time_class() : timeInterface("time"){}

	static void addElementIn_pPeriodicElements( timeInterface* pArg )
	{
		pPeriodicElements.push_back( pArg );
	}
	static void addTaskIn_pWorkTaskQueue( timeInterface* pArg )
	{
	 	pWorkTaskQueue.push_back( pArg );
	}
	static const inline unsigned long getTime(){ return ulTime; }
	
	static const void skrivUt_pPeriodicElements()
	{
		cout<<"Skriver ut pPeriodicElements-lista:\n";
		for( std::list<timeInterface*>::iterator pPE_iter = pPeriodicElements.begin() ; pPE_iter != pPeriodicElements.end() ; pPE_iter++ )
		{
			cout<<"[ " <<(*pPE_iter)->sClassName <<" ]\n";
		}
		cout<<"\n\n";
	}

	// Viktig med inkapsling!

	friend class K_auron;
	friend class i_auron;

	friend void initialzeWorkTaskQueue();
	friend void* taskSchedulerFunction(void*);
	friend int main(int, char**);
};





#endif
// vim:fdm=marker:fmr=//{,//}
