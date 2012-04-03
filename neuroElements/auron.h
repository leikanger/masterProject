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


// Notes:
// 	- have made all copy constructors private to disable copying. Same with assignment

#include <fstream> //file streams

#ifndef AURON_H_
#define AURON_H_

#include <iomanip> // For setprecision()

#include "../andreKildefiler/main.h"
#include "../andreKildefiler/time.h"
#include "../neuroElements/axon.h"
#include "../neuroElements/dendrite.h"

using std::endl;
using std::cout;

/*****************************************************
** class recalculateKappa : public timeInterface 	**
** 	Used for recalculating K-node's activation level**
** 	  to avoid large numerical errors. 				**
*****************************************************/
class recalcKappaClass : public timeInterface
{
	public:
	/* constructor */
	recalcKappaClass(K_auron* pConectedToKappaAuron_arg) : timeInterface("Kappa-recalc. obj."), pKappaAuron_obj(pConectedToKappaAuron_arg){
		// Set initial period between recalculation to a small number.
		dEstimatedTaskTime = 1; // Set to new value at first .doTask() run

		// Insert self-pointer to pPeriodicElements:
		time_class::addElementIn_pPeriodicElements( this );
	}

	inline void doTask();
	virtual void doCalculation(){
		// Error to use for this class. Print error message and terminate:
		cout<<"Brutal error check: recalcKappaClass::doCalculation() not implemented yet. Error id: asdf2351@auron.h\nTerminate.\n\n";
		exit(EXIT_FAILURE);
	}

	K_auron* pKappaAuron_obj;

	friend class K_auron;
};


/*****************************
**  Auron class hierarchy: 	**
*****************************/

/** i_auron **/
class i_auron : public timeInterface
{ 	//{		
	// The variables pOutputAxon and pInputDendrite is overloaded in the derived classes. E.g. s_auron use [s_axon* pOutputAxon].
	// 	  The effect of this is that all members of i_axon can be accessed from the outside, while modelspecific elements can be called from the inside a s_NODE (other s_{element}s).

	// Parts of the node:
	i_axon* pOutputAxon; 
 	i_dendrite* pInputDendrite; 

	// Not strictly necessary, but enables a smoother implementation.
	unsigned long ulTimestampLastInput;
	// For error check. Does not concern KM any longer. Remove or move to SN.
	unsigned long ulTimestampPreviousFiring;

	// TODO: Try to implement dopamine(or other factor to define synaptic plasticity amplification. Inspired by nature..
	// The Kappa formalism might be especially good for implementing DA neurons.

	// Create file stream for depol. log, if LOG_DEPOL is defined to true in main.h
	#if LOG_DEPOL 
		std::ofstream depol_logFile;
		std::ofstream actionPotential_depolLogFile;
	#endif
	std::ofstream actionPotential_logFile;
	
	// Log that writes a fine vertikal line at AP:
	inline virtual void writeAPtoLog()
	{
		// Log firing time:
		actionPotential_logFile<<time_class::getTime() <<";\n";

		// Log firing time by vertical line in depol-log
		#if LOG_DEPOL
			// Write depol line from 1050 to 1200
			for(int i = 1050; i<1200; i++){
				actionPotential_depolLogFile<<time_class::getTime() <<", " 		<<i <<";\n";
			}
		#endif
	}

	// Making copy constructor private to disable copying:
	i_auron(const i_auron& arg);
	// Same with assignment operator:
	i_auron& operator=(const i_auron& arg);

	protected:
	// container for all aurons:
	static std::list<i_auron*> pAllAurons;
	// Static member function that calls destructor for all aurons in this list(after the model specific K_auron and s_auron).
	static void callDestructorForAllAurons();

	// dActivityVariable hold different variables for KM and NIM (kappa or depol.)
	double dActivityVariable;

	public:
	i_auron(std::string sName_Arg ="unnamed", double dStartAktVar = 0);
	virtual ~i_auron();

	// For printing / debugging:
	std::string sName;
	const std::string& getName() const{ return sName; }

	// Overloaded differently in class s_auron and class K_auron. Writes depol to log
	virtual inline void writeDepolToLog() =0;

	friend class s_auron;
	friend class K_auron;

	friend class i_dendrite;
	friend class s_dendrite;
	friend class K_dendrite;
};  //}

class s_auron : public i_auron
{ //{

	inline void doTask();
	inline void doCalculation() { cout<<"s_auron::doCalculation()\tNOT IN USE\n";} // Does nothing, yet.

	static void callDestructorForAllSpikingAurons();

	// Make copy constructor private to disable copying:
	s_auron(const s_auron& arg);
	// Same with assignment:
	s_auron& operator=(const s_auron& arg);

	protected:
	// Parts of the node, designed as in the biological neuron. OVERLOADED FROM i_auron
	s_axon* pOutputAxon; 			// Overload from <i_auron::i_axon> to <s_auron::s_axon>.
 	s_dendrite* pInputDendrite;  	// Same for pInputDendrite.

	public:
	s_auron(std::string sName_Arg ="s_auron", int nStartDepol = 0); 
	virtual ~s_auron();

	virtual void writeDepolToLog()
	{
		#if LOG_DEPOL  // Defined in main.h
			// Handle resolution for the depol-logfile:
			static unsigned long uIterationsSinceLastWrite = 0;

			// Return unless it is time for a new log entry..
			// (based on write to log every iteration. Not so important that this is entirely correct)
			if( (++uIterationsSinceLastWrite > uNumberOfIterationsBetweenWriteToLog) ){
				depol_logFile 	<<time_class::getTime() <<"\t" <<dActivityVariable <<"; \t #Depolarization\n" ;
				// Reset counter
				uIterationsSinceLastWrite = 0;
			}else{
				return;
			}
	
		#endif
	}

// FRIENDS
	friend class s_synapse;
	friend int main(int, char**); // To be able to run callDestructorForAllSpikingAurons()

}; //}

class K_auron : public i_auron
{ // {
	// Have stared to remove axon and dendrite:
	// From axon: 
	std::list<K_synapse*> pOutputSynapses;
	//(protected:)
	void doTransmission();
	
	// TO BE REMOVED:
	//K_axon* pOutputAxon; 		
 	K_dendrite* pInputDendrite; 
	
	// Kappa - logfile:
	#if LOGG_KAPPA
		std::ofstream kappa_logFile;
	#endif

	double dChangeInKappa_this_iter;

	double dDepolAtStartOfTimeWindow;
	double dStartOfTimeWindow; 		// Start-time for this time window.
	double dLastFiringTime; 		// Start-time for this time window, in case of firing this iter..

	// For synaptic transmission
	double dLastCalculatedPeriod;
	double dPeriodINVERSE;
	double dChangeInPeriodINVERSE;

	// Static elements that keep an overview of all kappa aurons:
	static std::list<K_auron*> pAllKappaAurons;
	static void callDestructorForAllKappaAurons();

	virtual void writeAPtoLog()
	{
		// Log firing time to log
		actionPotential_logFile.precision(11);
		actionPotential_logFile<<dLastFiringTime <<";\n";		
//TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO 		HER ER EG 	 TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO 
		#if LOG_DEPOL 
			depol_logFile 	<<dLastFiringTime <<"\t" <<FIRING_THRESHOLD <<"; \t #Action potential - APAPAP\n" ;
			depol_logFile 	<<dLastFiringTime <<"\t" <<0 <<"; \t #Action potential - APAPAP\n" ;

			// Skriver til actionPotential_depolLogFile:
			// Logger depol-strek fra 1050 til 1200.
			for(int i = 1050; i<1200; i++){
				actionPotential_depolLogFile<<dLastFiringTime <<", " 		<<i <<";\n";
			}
			//actionPotential_depolLogFile.flush();
		#endif

		#if 0 //KOMMENTERER UT. 

		#if LOG_DEPOL 
			// Lager en vertikal "strek" fra v=0 til v=Terskel*(110%)
			for(float fTerkelProsent_temp = 1.35; fTerkelProsent_temp>1.1; fTerkelProsent_temp-=0.001)
			{
				// TID: endre fra present time iteration til å være gitt av dEstimatedTaskTime i K_auron!
				depol_logFile 	<<dEstimatedTaskTime <<"\t" <<fTerkelProsent_temp*FIRING_THRESHOLD <<"; \t #Action potential\n" ;
			}
	 		depol_logFile.flush();
		#endif
		#endif
	}



	// Making copy constructor private to disable copying:
	K_auron(const K_auron& arg);
	// Same with assignment:
	K_auron& operator=(const K_auron& arg);



	protected:
	inline void changeKappa_diffArg( const double& );
	//inline void changeKappa_absArg( const double&);

	// See @asdf1515
	double dStartOfNextTimeWindow; 	// Start-time for next time window. Used to find start-depol. for time window.
	// Flytta til protected (@asdf1515): double dStartOfNextTimeWindow; 	// Start-tidspunkt for neste time window (brukes til å finne start-depol. for neste time window).

	// Recalculation of kappa to avoid integration errors:
	inline virtual double recalculateKappa();
	recalcKappaClass kappaRecalculator;

	// List of all Kappa aurons:
	static std::list<K_auron*> pAlleKappaAuron;

	inline void doTask();
	inline void doCalculation();

	inline void estimateFiringTimes(); 

	public:
	K_auron(std::string sName_Arg ="K_auron", double dStartKappa_arg = 0); 	
	virtual ~K_auron();




	inline double getCalculateDepol() const
	{

		// GAMMEL: return (dDepolAtStartOfTimeWindow - dActivityVariable)*exp(-(double)LEAKAGE_CONST  * (time_class::getTime() - ulStartOfTimewindow )) + dActivityVariable ;

		static double dDepolStatic;

		//dDepolStatic = (dDepolAtStartOfTimeWindow - dActivityVariable)*exp(-LEAKAGE_CONST  * (time_class::getTime() - dStartOfTimeWindow )) + dActivityVariable; 
		dDepolStatic = (dDepolAtStartOfTimeWindow - dActivityVariable)*exp(-LEAKAGE_CONST  * (dStartOfNextTimeWindow - dStartOfTimeWindow )) + dActivityVariable ;
	   	//v(t)=K(1-e^-at)-v_+e^-at = (v_0 - K) e^-at + K   !

		// Next line makes this funtion non-const. Comment and mark funtion const for optimalization
		//writeDepolToLog( time_class::getTime() , dDepolStatic);
		return dDepolStatic;
	}
		
	inline const double getCalculateDepol(const double& dForTimeInstant_arg) const
	{

	// TODO Hindre negative depol. før fyring. Dette kan brukes for å få rett depol. ved fyring (en metode)
		#if 0 
			// Sjekker om den har fyrt denne iter. Isåfall vil dStartOfTimeWindow > dForTimeInstant_arg
			if( dLastFiringTime > dForTimeInstant_arg ){
				cerr<<"\n\n\n\n\e[31mERROR asdf1352@auron.h\e[0m\tdLastFiringTime>dForTimeInstant_arg.\tgetCalculateDepol() returnerer 0\t(" <<dLastFiringTime <<">" <<dForTimeInstant_arg <<"\n";
				exit(0);
				//return 0; // Dette er litt feil, fordi nå vil den sei at v_0 var 0 for en tid som er litt siden.
			}
		#endif

		// TODO PLAN: getCalculateDepol(*) skal ikkje skrive ut noke. Dette er dårlig stil(roter til utskrift)..
			// Difor: fjærn dDepolStatic! TODO
		static double dDepolStatic;

		// Går over til bedre tidssoppløysing: double-precition float number format!
		dDepolStatic = (dDepolAtStartOfTimeWindow - dActivityVariable)*exp(-LEAKAGE_CONST  * (dForTimeInstant_arg - dStartOfTimeWindow )) + dActivityVariable; //v(t)=K(1-e^-at)-v_+e^-at = (v_0 - K) e^-at + K   !

		//writeDepolToLog( dForTimeInstant_arg, dDepolStatic); // Tar vekk denne, for å få lov til å definere funksjonen som const.
		return dDepolStatic;
	}

	inline void writeDepolToLog( const double& dTimeArg, const double& dDepolArg)
	{
		#if LOG_DEPOL 
			// Set precision for output: Only works when #include <iomanip>;
			depol_logFile.precision(11);
	
			// Handle accuracy for the depol-logfile:
			static unsigned long uIterationsSinceLastWrite = 0;
			uIterationsSinceLastWrite++;

			// Unless it is time for writing to log, return.
			if( uIterationsSinceLastWrite < uNumberOfIterationsBetweenWriteToLog ){
				return;
			}

			// Reset counter
			uIterationsSinceLastWrite = 0;

			// Skriver dDepolAtStartOfTimeWindow til logg: (Tid er gitt i prosent av heile kjøretid)
			depol_logFile 	<<dTimeArg <<"\t" <<dDepolArg <<"; \t #Depol\n" ;
			//depol_logFile.flush();
		#endif
		
	}
	inline void writeDepolToLog()
	{
		#if LOG_DEPOL 
			// Sette precision for output: Funker bare med #include <iomanip>; , som ikkje funker for clang++-compiler..
			depol_logFile.precision(11);
	
			// Handle accuracy for the depol-logfile:
			static unsigned long uIterationsSinceLastWrite = 0;
			uIterationsSinceLastWrite++;

			// Unless it is time for writing to log, return.
			if( uIterationsSinceLastWrite < uNumberOfIterationsBetweenWriteToLog ){
				return;
			}

			// Reset counter
			uIterationsSinceLastWrite = 0;

			// Skriver dDepolAtStartOfTimeWindow til logg: (Tid er gitt i prosent av heile kjøretid)
			//if(getCalculateDepol(time_class::getTime())>0) //Måte å få fekk negative resultat. Veldig dårlig stil! Skuler feil..
				depol_logFile 	<<(unsigned long)time_class::getTime() <<"\t" <<getCalculateDepol(time_class::getTime()) <<"; \t #Depol\n" ;
			//depol_logFile.flush();
		#endif
	}
	inline void writeKappaToLog()
	{
		#if LOGG_KAPPA

			// Handle accuracy for the depol-logfile:
			static unsigned long uIterationsSinceLastWrite = 0;
			uIterationsSinceLastWrite++;

			// Unless it is time for writing to log, return.
			if( uIterationsSinceLastWrite < uNumberOfIterationsBetweenWriteToLog ){
				return;
			}else{
				// Reset counter and write to log
				uIterationsSinceLastWrite = 0;
				// Write dDepolAtStartOfTimeWindow to log:
				kappa_logFile 	<<time_class::getTime() <<"\t" <<dActivityVariable <<"; \t #Kappa\n" ;
			}
		#endif
	}

	// For testing with ANN-matrix--initiation:
	const std::list<K_synapse*> getUtSynapserP(){ return pOutputSynapses; }


	friend class i_auron; 		// For i_auron to be able to call K_auron::callDestructorForAllKappaAurons()
	friend class K_synapse;
	friend class K_dendrite;
	friend class recalcKappaClass;

	friend void* taskSchedulerFunction(void* );
}; // }

class s_sensory_auron : public s_auron{
	// Function pointer:
	const double& (*pSensorFunction)(void);

	static std::list<s_sensory_auron*> pAllSensoryAurons;

	inline void updateSensoryValue();
	static void updateAllSensorAurons();

	// Make copy constructor private to disable copying:
	s_sensory_auron(const s_sensory_auron& arg);
	// Same with assignment:
	s_sensory_auron& operator=(const s_sensory_auron& arg);
	
	public:
		s_sensory_auron( std::string sName_Arg ,  const double& (*pFunk_arg)(void) );

		// To be used for debugging.
		double getSensedValue()
		{
	 		return (*pSensorFunction)();
		}

	friend class time_class;
};

class K_sensory_auron : public K_auron{
	// Function pointer:
	const double& (*pSensorFunction)(void);

	// Need two values to find the change(for calculation of edge transmisison)
	double dSensedValue;
	double dLastSensedValue;

	// Static list containg all sensory neurons: to be checked by updateAllSensoryNeurons
	static std::list<K_sensory_auron*> pAllSensoryAurons;

	void updateSensoryValue();
	static void updateAllSensorAurons();


	// Make copy constructor private to disable copying:
	K_sensory_auron(const K_sensory_auron& arg);
	// Same with assignment:
	K_sensory_auron& operator=(const K_sensory_auron& arg);


	
	protected:
	inline double recalculateKappa();

	public:
		// Constructor take funtion pointer as argument
		K_sensory_auron( std::string sName_Arg ,  const double& (*pFunk_arg)(void) );

		// To be used for debugging.
		double getSensedValue()
		{
	 		return (*pSensorFunction)();
		}

	friend class time_class;
	friend void* taskSchedulerFunction(void*);
};




#endif /* AURON_H_ */
// vim:fdm=marker:fmr=//{,//}
