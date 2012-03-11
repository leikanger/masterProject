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

#include <typeinfo> //For å bryke typeid..

using std::cout;
using std::cerr;
using std::endl;

// DEKLARASJONER:
//extern unsigned long ulTime;
extern void loggeFunk_K_auron(); // auron.h

#ifndef TID_H_
#define TID_H_



/****************************************
***   abstract class timeInterface:   ***
****************************************/
class timeInterface
{
	public:
	timeInterface(std::string s) : dEstimatedTaskTime(0), sClassName(s){}

	virtual void doTask() =0;
	virtual void doCalculation() =0;
	double dEstimatedTaskTime; 

	// For debugging(print to screen):
	std::string sClassName;
};


/**************************************
*** 	class time_class 			***
**************************************/
class time_class : public timeInterface {
	// variable to hold t_n, [time step number]:
	static unsigned long ulTime;
	
	// Lists for scheduling tasks and recalculation of elements
	static std::list<timeInterface*> pWorkTaskQueue;
	static std::list<timeInterface*> pCalculatationTaskQueue;

	// List that contain all periodic elements in the simulation:
	// 	All elements are checked each time step: If an element have dEstimatedTaskTime during the next iteration, it is inserted into pWorkTaskQueue.
	static std::list<timeInterface*> pPeriodicElements;

	protected:
	void doTask(); 			// Defined in neuroElement.cpp
	void doCalculation()	// Conduct calculations on all calculation tasks in pCalculatationTaskQueue.
	{ //{
		// Organize list so that every entry is unique:
		for( std::list<timeInterface*>::iterator iter = pCalculatationTaskQueue.begin(); iter != pCalculatationTaskQueue.end(); iter++ )
		{
			static std::list<timeInterface*>::iterator iter2;
		   	iter2	= iter; 
			iter2++;
			while(iter2!=pCalculatationTaskQueue.end()){
				// Check if iterators point at same memory address: If so, remove second element:
			 	if( (*iter2) == (*iter) ){ 
					// Iterate before iter2 is removed:
					static std::list<timeInterface*>::iterator pDeleteElement;
					pDeleteElement = iter2;
					iter2++;
					pCalculatationTaskQueue.erase(pDeleteElement);
					continue;
				}
				iter2++;
			}
		}
	
		// Call first elements .doCalculation() and pop first element until list is empty:
		while( !pCalculatationTaskQueue.empty() ){
			// Call first element's doCalculation():
			pCalculatationTaskQueue.front()->doCalculation();

			// Pop first element from pCalculatationTaskQueue(calculation is executed)
			pCalculatationTaskQueue.pop_front();
		}
	} //}

	// Funtion to push task to pWorkTaskQueue in present time iteration(only possible for a KM node).
	static inline void addTaskInPresentTimeIteration(timeInterface* pTimeClassArg_withTask);

	// Funtion to schedule calculation
	static inline void addCalculationIn_pCalculatationTaskQueue( timeInterface* pObject_arg)
	{
	 	pCalculatationTaskQueue.push_back( pObject_arg );
	}

	public:
	time_class() : timeInterface("time"){}

	// Functions to access private elements:

	static void addElementIn_pPeriodicElements( timeInterface* pArg )
	{
		pPeriodicElements.push_back( pArg );
	}
	static void addTaskIn_pWorkTaskQueue( timeInterface* pArg )
	{
	 	pWorkTaskQueue.push_back( pArg );
	}
	static inline const unsigned long getTime(){ return ulTime; }
	
	static void printAllElementsOf_pPeriodicElements()
	{
		cout<<"Print type of all elements in the pPeriodicElements list:\n";
		for( std::list<timeInterface*>::iterator pPE_iter = pPeriodicElements.begin() ; pPE_iter != pPeriodicElements.end() ; pPE_iter++ )
		{
			cout<<"\t[ " <<(*pPE_iter)->sClassName <<" ]\n";
		}
		cout<<"\n\n";
	}

	static void printAllElementsOf_pWorkTaskQueue()
	{ //{
		cout<<"All elements of pWorkTaskQueue: \n";
		int nIter = 0;
		// itererer gjennom ytre liste:
		for(std::list<timeInterface*>::iterator l_iter = pWorkTaskQueue.begin(); 	l_iter != pWorkTaskQueue.end() ; 	l_iter++ )
		{
			cout<<nIter <<")\t" <<typeid(*(*l_iter)).name() <<"\t(" <<(*l_iter)->sClassName <<"\tscheduled for time " <<(*l_iter)->dEstimatedTaskTime <<"\n";
			nIter++;
		}
		cout<<"\n\n";
	} //}


	// Allow class K_auron to access protected elements:
	friend class K_auron;
	// Also the following functions:
	friend void initialzeWorkTaskQueue();
	friend void* taskSchedulerFunction(void*);
};

#endif
// vim:fdm=marker:fmr=//{,//}
