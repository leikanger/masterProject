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
#include <fstream> //filestreams

#ifndef SYNAPSE_H_
#define SYNAPSE_H_

#include "../neuroElements/auron.h"
#include "../andreKildefiler/time.h"

// Declarations:
class i_axon;
class s_axon;
class i_dendrite;
class s_dendrite;
class K_dendrite;
class s_auron;
class K_auron;


// abstract class i_synapse:
class i_synapse : public timeInterface{

	// Overloaded in the model specific derived classes
 	i_axon* pPreNodeAxon;
	i_dendrite* pPostNodeDendrite; 

	// virtual inline void doTask() =0;   -- i_synapse remains abstract.
	void doCalculation() {} 	//-- doCalculation() is not used for the synapse...yet. 
	//							// 		- Might be important when synaptic plasticity is introduced.
	
	protected:
	// Defines whether the synapse is inhibitory or not
	const bool bInhibitoryEffect;
	// Defines the synaptic weight of the synapse
	double dSynapticWeight;

	// Log file, for logging synaptic transmission
	std::ofstream synTransmission_logFile;


	public:
	i_synapse(double dSynVekt_Arg, bool bInhibEffekt_Arg, std::string sKlasseNavn /*="synapse"*/ );
	//virtual ~i_synapse(); // Define to be virtual. 
	// Not implemented for i_synapse, but can not be pure virtual as it is called for derefereced i_auron pointers.
	
	double getSynWeight() const{ return dSynapticWeight; }
	bool getInhibitoryEffect() const{return bInhibitoryEffect; }

	friend std::ostream & operator<< (std::ostream & ut, i_axon* );
 	friend std::ostream & operator<< (std::ostream & ut, s_axon* pAxonArg );
};



class s_synapse : public i_synapse{

	inline void doTask(); 	// Defined in neuroElements.cpp

	// Overload node-subelement pointers from i_synapse:
 	s_axon* pPreNodeAxon;
	s_dendrite* pPostNodeDendrite; 

	// Make copy constructor private to disable copying:
	s_synapse(const s_synapse& arg);
	// Same with assignment:
	s_synapse& operator=(const s_synapse& arg);

	public:
	s_synapse(s_auron*, s_auron*, double dSynVekt_Arg =1, bool bInhibEffekt_Arg =false) ;
	~s_synapse();

	friend std::ostream & operator<< (std::ostream & ut, s_axon* );
};



class K_synapse : public i_synapse{
	inline void doTask(); 	// Defined in neuroElements.cpp
	
	// Planned further development: Schedule spatiotemporal delay by having member variables that gives the delay from/to the previous/next auron.
	//unsigned uTemporalDistanceFromSoma; // Distance from axon hillock to synapse.
	//unsigned uTemporalDistanceToSoma; 	// Distance from dendrite to soma.
	//void scheduleTransmission(); 		// Handles the scheduling of task(schedule after the predefined delay)

	// Overload abstract class i_synapse's pointers to next and previous element.
	K_auron* pPreNodeAuron;
	K_dendrite* pPostNodeDendrite; 

	// For recalculation of Kappa(by the postsynaptic neuron)
	double dPresynPeriodINVERSE;
	//const inline double getDerivativeOfTransmission() const;
	const inline double getTotalTransmission() const;


	// Make copy constructor private to disable copying:
	K_synapse(const K_synapse& arg);
	// Same with assignment:
	K_synapse& operator=(const K_synapse& arg);

	public:
	K_synapse(K_auron*, K_auron*, double dSynVekt_Arg =1, bool bInhibEffekt_Arg =false) ;
	virtual ~K_synapse();
	
	K_dendrite* getPostNodeDendrite(){
		return pPostNodeDendrite;
	}

	friend class K_dendrite;
	friend class K_auron;
};

#endif
// vim:fdm=marker:fmr=//{,//}
