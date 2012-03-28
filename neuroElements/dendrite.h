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
#ifndef DENDRITE_H_
#define DENDRITE_H_

#include "../andreKildefiler/time.h"
#include "../andreKildefiler/main.h"
#include "../neuroElements/synapse.h"
#include "../neuroElements/auron.h"

/*****************************************************************
** class dendritt 			-  	Notes:
** 		- Can remove this class. Only needed for NIM nodes. 
** 			Can use task scheduling instead..
**
*****************************************************************/
class i_dendrite : public timeInterface{
	// Pointers to the next and previous element. Are overloaded to the model specific vertions in derived classes(s_dendrite and K_dendrite)
	i_auron* pElementOfAuron;
	std::list<i_synapse*> pInputSynapses;

 	//virtual inline void doTask() =0; -- Stays pure virtual..
	// Define doCalculation() to do nothing. Not used for dendrite, yet.
	// 	 Derived classes thus does not have do define this funtion..
	virtual void doCalculation() {}

	// Different for the two simulation models. Defined pure virtual.
	virtual inline void newInputSignal( double ) =0;

	public:
	i_dendrite(std::string sNavn /*="dendrite"*/);
	~i_dendrite();
};


class s_dendrite : public i_dendrite{
	inline void doTask(); 				// Defined in neuroElements.cpp

	// Overload i_dendrite::pElementOfAuron and i_dendrite::pInputSynapses to be modelspecific for the NIM simulation model:
	s_auron* pElementOfAuron;
	std::list<s_synapse*> pInputSynapses;

	inline void newInputSignal( double /*nNewSignal*/ );
	inline void calculateLeakage(); 		// Only for NIM dendrites.

	bool bBlockInput_refractionTime; 		// Only for NIM dendrites.
	// This is how the NIM simulation simulates absolute refraction time after firing.

	public:
	s_dendrite( s_auron* pPostSynAuron_Arg );
	virtual ~s_dendrite();

	// Friend declarations:
	friend class s_sensory_auron;
	friend class s_synapse;
	friend std::ostream & operator<< (std::ostream & ut, s_axon* );
};

class K_dendrite : public i_dendrite{
 	inline void doTask(); // Defined in neuroElements.cpp

	// Overload i_dendrite::pElementOfAuron and i_dendrite::pInputSynapses, so that these point to model specific variants.
	K_auron* pElementOfAuron;
	std::list<K_synapse*> pInputSynapses;
	
	// called by auron::recalculateKappa() and does the actual recalculation. Most efficient this way.
	inline double recalculateKappa();

	inline void newInputSignal(double);

	public:
	K_dendrite( K_auron* );
	virtual ~K_dendrite();

	K_auron* getElementOfAuron(){ return pElementOfAuron; }

	friend class K_synapse;
	friend class K_auron;
};

#endif
// vim:fdm=marker:fmr=//{,//}
