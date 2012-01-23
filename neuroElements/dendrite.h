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

//Deklarasjoner:
class i_synapse;
class s_synapse;
class s_auron;

/*****************************************************************
** class dendritt 			-  	Notater:
** 		- Skal vekk. Class dendrite skal ikkje være med i K_auron! 	
** 			(i_dendrite skal vekk. Alt innhold skal inn i s_dendrite..)
**
*****************************************************************/
class i_dendrite : public timeInterface{
	i_auron* pElementOfAuron;
	std::list<i_synapse*> pInnSynapser;

 	virtual inline void doTask() =0;
	// XXX XXX XXX Utsetter doCalculation() for alle dendrite: (definerer den til å gjøre ingenting her for å unngå at klassene under blir abstract..)
	virtual void doCalculation() {}

	virtual inline void newInputSignal( double ) =0;

	public:
	i_dendrite(std::string sNavn /*="dendrite"*/);
	~i_dendrite();

	friend int main(int, char**);
	friend std::ostream & operator<< (std::ostream & ut, s_axon* pAxonArg );

	friend class i_synapse;
	friend class i_axon;
	friend class i_auron;

	friend class s_synapse;
	friend class s_axon;
	friend class s_auron;
	friend class s_dendrite;

	friend class K_synapse;
	friend class K_auron;
	friend class K_dendrite;
};


class s_dendrite : public i_dendrite{
	inline void doTask();
	inline void feedbackToDendrite();

	// Overlagrer i_dendrite::pElementOfAuron og i_dendrite::pInnSynapser, slik at desse blir modellspesifikke.
	// 	Ligger også i i_dendrite, slik at i_dendrite* også kan kalle pElementOfAuron og pInnSynapser.
	s_auron* pElementOfAuron;
	std::list<s_synapse*> pInnSynapser;

	inline void newInputSignal( double /*nNewSignal*/ );
	inline void calculateLeakage(); 		//Bare for SANN

	bool bBlockInput_refractionTime; 		//Bare for SANN 	//Blokkere input når refraction period eller når depol er over terskel.

	public:
	//Constructor: 	
	s_dendrite( s_auron* pPostSynAuron_Arg ); // : pElementOfAuron(pPostSynAuron_Arg)  		Definisjon i neuroElement.cpp
	~s_dendrite();

	//Destructor:
	/* kommenterer ut mens s_synapser ikkje er laga enda.. //{
	~sDendritt()
	{
		// destruerer alle innsynapser:
	 	while( !pInnSynapser.empty() )
		{
	 		cout<<"~dendritt: fjerner innsyn fra " <<pInnSynapser.back()->pPreNode->sNavn <<" til " <<pInnSynapser.back()->pPostNode->sNavn <<"( " <<pInnSynapser.size() <<" synapser igjen)."	<<endl;
			pInnSynapser.pop_back(); 	//pop_back() fjærner siste ledd i std::vector. Kaller også destructor for dette elementet.
			// ~synapse tar hånd om (fjærner) presynaptisk kobling til denne synapsen. TODO
		}
	}
	*/ //}

	friend class s_auron;
	friend class s_sensor_auron;

	friend class s_axon;
	friend class s_synapse;
	friend std::ostream & operator<< (std::ostream & ut, s_axon* );

	friend int main(int, char**); //TODO SLETT
};

class K_dendrite : public i_dendrite{
 	inline void doTask();
	//inline void feedbackToDendrite();

	// Overlagrer i_dendrite::pElementOfAuron og i_dendrite::pInnSynapser, slik at desse blir modellspesifikke.
	K_auron* pElementOfAuron;
	std::list<K_synapse*> pInnSynapser;
	
	inline double recalculateKappa();

	inline void newInputSignal(double);

	public:
	K_dendrite( K_auron* );
	~K_dendrite();

	K_auron* getElementOfAuron(){ return pElementOfAuron; }

	friend class K_synapse;
	friend class K_auron;
};

#endif
// vim:fdm=marker:fmr=//{,//}
