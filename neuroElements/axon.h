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
#ifndef AXON_H_
#define AXON_H_

#include "../andreKildefiler/time.h"
#include "../andreKildefiler/main.h"
#include "../neuroElements/synapse.h"
#include "../neuroElements/auron.h"

// Declarations:
class i_synapse;
class s_synapse;
class K_synapse;
class i_auron;
class s_auron;
class K_auron;
class s_axon;

/* abstract class */
class i_axon : public timeInterface{
	// Abstract because doTask() have not been overloaded.
	virtual void doCalculation(){} // To declare avoid that derivatives need to define doCalculation(). Not needed for axon.
	
	// The i_auron* is overloaded with a pointer to objects of classes derived from i_auron (s_auron or K_auron).
	// -same for the synapses.
	i_auron* pElementOfAuron;
	std::list<i_synapse*> pUtSynapser;

	public:
	i_axon(std::string sKlasseNavn /*="dendrite"*/);
	~i_axon();
};

class s_axon : public i_axon{
	inline void doTask(); // Defined in neuroElements.cpp

	// Overloaded from the i_auron* pElementOfAuron and list<i_synapse*> pOutputAxon from i_axon:
	s_auron* pElementOfAuron;
	std::list<s_synapse*> pUtSynapser;

	public:
	s_axon(s_auron* pAuronArg);
	~s_axon();

	friend class s_synapse;

	friend std::ostream & operator<< (std::ostream & ut, s_axon* );
};



#endif
// vim:fdm=marker:fmr=//{,//}
