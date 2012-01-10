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
	
	static std::list<timeInterface*> pWorkTaskQue;
	static std::list<timeInterface*> pCalculatationTaskQue;

	// Liste som sjekkes ved kvar tidsiterering: Dersom eit element har dEstimatedTaskTime til neste tidssted legges peiker inn i pWorkTaskQue.
	static std::list<timeInterface*> pPeriodicElements;

	protected:
	void doTask();
	void doCalculation()
	{ //{
		/**************************************************************************************
		*** Gjennomføre kalkulering på alle kalkuleringsoppgaver (pCalculatationTaskQue) 	***
		**************************************************************************************/

		// TODO TODO TA VEKK (redundant test..)
		if( pCalculatationTaskQue.empty() ) return;

		// Organiserer liste slik at kvar oppføring er unik:
		for( std::list<timeInterface*>::iterator iter = pCalculatationTaskQue.begin(); iter != pCalculatationTaskQue.end(); iter++ )
		{
			std::list<timeInterface*>::iterator iter2 = iter; 
			iter2++;
			while(iter2!=pCalculatationTaskQue.end()){
				// ser om iteratorene peker til samme minneadresse (samme timeInterface-element). Isåfall: fjærn det andre elementet.
			 	if( (*iter2) == (*iter) ){ 
					// Øker iterator før eg sletter element på iter2.
					std::list<timeInterface*>::iterator slettIter = iter2;
					iter2++;
					pCalculatationTaskQue.erase(slettIter);
					continue;
				}
				iter2++;
			}
		}
	
		while( !pCalculatationTaskQue.empty() ){
			// Kaller pCalculatationTaskQue.front()->pCalculatationTaskQue();
			pCalculatationTaskQue.front()->doCalculation();
			pCalculatationTaskQue.pop_front();
		}
	} //}


#if 1
	static inline void addTaskInPresentTimeIteration(timeInterface* pTimeClassArg)
	{
// 		// XXX Dette er bra, men eg skal gjøre det for alle K_auron når eg går inn i en tidsiter. 
// 			// Da kan eg også gjøre det her!
/*		for( std::list<time_class*>::iterator iter = pWorkTaskQue.begin(); iter != pWorkTaskQue.end(); iter++ )
		{
	 		if( typeid(*iter) == typeid(K_auron*) )
			{
	 			
			}
		}
*/		 
		
		// Foreløpig legger eg den bare til først i denne iter: (etter nåværande elem.)
		if( pWorkTaskQue.size() > 0 ){
			std::list<timeInterface*>::iterator iter = pWorkTaskQue.begin();
			++iter;
			pWorkTaskQue.insert(iter, pTimeClassArg ); 
		}else{
			cerr<<"FEIL: asdf529@time.h\n\nTERMINERER\n";
			exit(9);
		}
	}
#endif

	static inline void addCalculationIn_pCalculatationTaskQue( timeInterface* pObject_arg)
	{
	 	pCalculatationTaskQue.push_back( pObject_arg );
	}


	const static void skrivUt_pWorkTaskQue()
	{ //{
		cout<<"Skriver ut pWorkTaskQue: \n";
		int nIter = 0;
		// itererer gjennom ytre liste:
		for(std::list<timeInterface*>::iterator l_iter = pWorkTaskQue.begin(); 	l_iter != pWorkTaskQue.end() ; 	l_iter++ )
		{
			cout<<nIter <<"\t" <<(*l_iter)->sClassName <<endl;
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
	static void addTaskIn_pWorkTaskQue( timeInterface* pArg )
	{
	 	pWorkTaskQue.push_back( pArg );
	}
	static const inline unsigned long getTid(){ return ulTime; }
	
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

	friend void initialiserArbeidsKoe();
	friend void* taskSchedulerFunction(void*);
	friend int main(int, char**);
};





#endif
// vim:fdm=marker:fmr=//{,//}
