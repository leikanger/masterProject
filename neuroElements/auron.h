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


// TODO Make all copy construcors private to disable copying. See line 536..

#include <fstream> //file streams

#ifndef AURON_H_
#define AURON_H_

#if GCC
	#include <iomanip> // For setprecision()   FUNKER IKKJE FOR clang++-kompilatoren..
#endif

#include "../andreKildefiler/main.h"
#include "../andreKildefiler/time.h"
#include "../neuroElements/axon.h"
#include "../neuroElements/dendrite.h"

using std::endl;
using std::cout;

/*****************************************************
** class recalculateKappa : public timeInterface 	**
** 	Used for recalculating K-node's activation level**
** 	  to avoid to large numerical errors. 			**
*****************************************************/
class recalcKappaClass : public timeInterface
{
	public:
	recalcKappaClass(K_auron* pConectedToKappaAuron_arg) : timeInterface("Kappa-recalc. obj."), pKappaAuron_obj(pConectedToKappaAuron_arg){
		// Set initial period between recalculation to a small number.
		dEstimatedTaskTime = 1; // Overwritten by first doTask() run.

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
	unsigned long ulTimestampForrigeFyring;

	// TODO: Try to implement dopamine(or other factor to define synaptic plasticity amplification. Inspired by nature..
	// Also here, Kappa mathematics can be utilized.

	// Create file stream for depol. log, if LOG_DEPOL is defined to true in main.h
	#if LOG_DEPOL 
		std::ofstream depol_logFile;
		std::ofstream actionPotential_depolLogFile;
	#endif
	std::ofstream actionPotential_logFile;
	
	// For å lage fin vertikal "strek" ved AP:
	inline virtual void writeAPtoLog()
	{
		// XXX Kommenterer ut for å lettere sjå gjennom log-fil:
		
		// Logger fyringstidspunkt:
		actionPotential_logFile<<time_class::getTime() <<";\n";
		//actionPotential_logFile.flush();

		
/*			// Lager en vertikal "strek" fra v=0 til v=Terskel*(110%)
			for(float fTerkelProsent_temp = 1.35; fTerkelProsent_temp>1.1; fTerkelProsent_temp-=0.001)
			{
				// TID: endre fra present time iteration til å være gitt av dEstimatedTaskTime i K_auron!
				depol_logFile 	<<time_class::getTime() <<"\t" <<fTerkelProsent_temp*FIRING_THRESHOLD <<"; \t #Action potential\n" ;
			}
*/

		// SKriver en enkelt linje med tidspunktet:
		#if LOG_DEPOL
//			depol_logFile 	<<time_class::getTime() <<"\t" <<0*FIRING_THRESHOLD <<"; \t #Action potential - APAPAP\n" ;
			// Logger depol-strek fra 1050 til 1200.
			for(int i = 1050; i<1200; i++){
				actionPotential_depolLogFile<<time_class::getTime() <<", " 		<<i <<";\n";
			}
			//actionPotential_depolLogFile.flush();
		#endif


	}
	inline void DEBUTsettMerkeIPlott()
	{
		#if LOG_DEPOL  
			// Lager en vertikal "strek" fra v=0 til v=Terskel*(110%)
			for(float fTerkelProsent_temp = 1.4; fTerkelProsent_temp<1.7; fTerkelProsent_temp+=0.001)
			{
				depol_logFile 	<<time_class::getTime() <<"\t" <<fTerkelProsent_temp*FIRING_THRESHOLD <<"; \t #Action potential\n" ;
			}
	 		//depol_logFile.flush();
		#endif
	}


	protected:
	virtual inline void doTask() =0;

	//container som inneholder alle auron som har vore til ila. programkjøringa:
	static std::list<i_auron*> pAllAurons;
	//static mdl. funk som destruerer alle i denne lista, men først de modellspesifikke K_auron og s_auron.
	static void callDestructorForAllAurons();

	// aktivitetsobjekt: Om dette er KANN eller SANN er avhengig av kva dAktivitetsVariabel skal bety (kappa eller depol..).
	double dAktivitetsVariabel;

	public:
	i_auron(std::string sNavn_Arg ="unnamed", double dStartAktVar = 0);
	~i_auron();

	// For utskrift / debugging:
	std::string sNavn;
	const std::string getNavn(){ return sNavn; }

	int getAktivityVar(){ return dAktivitetsVariabel; }

	// Overlagres forskjellig i s_auron og K_auron for å finne depol.
	virtual inline void writeDepolToLog() =0;

	//testfunksjon:
	void exiterNeuronTilFyringGjennomElectrode()
	{
		doTask(); //gjør samme som gamle: auron::fyr();
	}


	friend class s_auron;
	friend class K_auron;

	friend class i_axon; // VEKK med den?
	friend class s_axon;

	friend class i_synapse;
	friend class s_synapse;
	friend class K_synapse;
	
	friend class i_dendrite;
	friend class s_dendrite;
	friend class K_dendrite;

	friend std::ostream & operator<< (std::ostream & ut, i_axon* );

	friend int main(int, char**);
};  //}

class s_auron : public i_auron
{ //{

	inline void doTask();
	inline void doCalculation() { cout<<"s_auron::doCalculation()\n";} 		//XXX UTSETTER. Foreløpig gjør denne ingenting (anna enn å gjøre at s_auron ikkje er abstract)

	static void callDestructorForAllSpikingAurons();

	protected:
	/* TODO:
	* 	- gjør om slika at det ikkje finnes i i_auron: vil dermed ikkje lenger være overlagra. Definer først her..
	*/
	//Deler av auronet: 		OVERLAGRA fra i_auron
	s_axon* pOutputAxon; 			// Overlagrer i_auron::i_axon til s_auron::s_axon. Dette er alternativ til å caste pOutputAxon ved accessering til s_auron::pOutputAxon
 	s_dendrite* pInputDendrite;  	// Samme for pInputDendrite.

	public:
	s_auron(std::string sNavn_Arg ="s_auron", int nStartDepol = 0); 
	~s_auron();

	inline const double getCalculateDepol();

	inline virtual void writeDepolToLog()
	{

		// Plasserer all kode som har med å skrive depol til logg.
		#if LOG_DEPOL  // Kan sette om depol. skal skrives til logg i main.h
			// Handle accuracy for the depol-logfile:
			static unsigned long uIterationsSinceLastWrite = 0;

			// Unless it is time for writing to log, return.
			// (GÅR UT FRA AT DEN SKRIVER TIL LOG KVAR ITER. TODO Vurder å skrive om dette til å bare være eit tidspunkt som blir sammenligna med nå-tid! TODO
			if( (++uIterationsSinceLastWrite > uNumberOfIterationsBetweenWriteToLog) ){
				depol_logFile 	<<time_class::getTime() <<"\t" <<dAktivitetsVariabel <<"; \t #Depolarization\n" ;
				// Reset counter
				uIterationsSinceLastWrite = 0;
			}else{
				return;
			}
	
		#endif
	}
//{friend
	friend class s_axon;
	friend class s_synapse;
	friend class s_dendrite;
	friend std::ostream & operator<< (std::ostream & ut, i_axon* );

	friend int main(int, char**);
//}

}; //}

class K_auron : public i_auron
{ // {

	// Fra axon: 
	std::list<K_synapse*> pUtSynapser;
	//protected:
	void doTransmission();
	

	// TA VEKK:
	//                  		OVERLAGRA fra i_auron
	//K_axon* pOutputAxon; 			// Overlagrer i_auron::i_axon til K_auron::K_axon. Dette er alternativ til å caste pOutputAxon ved accessering til K_auron::pOutputAxon
 	K_dendrite* pInputDendrite;  	// Samme for pInputDendrite.
	
	// Kappa - loggfil:
	#if LOGG_KAPPA
		std::ofstream kappa_logFile;
	#endif

	// Kanskje dendrite skal implementeres i auron? Isåfall lag dette til ei lenka liste, og bruk ved å lese ut første element.
	double dChangeInKappa_this_iter;


	double dDepolAtStartOfTimeWindow;
	double dStartOfTimeWindow; 		// Start-tidspunkt for dette time window.
	double dLastFiringTime; 		// Start-tidspunkt for dette time window, dersom fyring var i denne iter..
	// Flytta til protected (@asdf1515): double dStartOfNextTimeWindow; 	// Start-tidspunkt for neste time window (brukes til å finne start-depol. for neste time window).

	double dLastCalculatedPeriod;
	double dPeriodINVERSE;
	double dChangeInPeriodINVERSE;

	inline const double& getKappa() const{ return dAktivitetsVariabel; }
	

	// For debugging: trenger ei liste over alle K_auron, slik at eg kan skrive log for depol kvar tidsiterasjon:
	// Legger til i constructor og fjærner i destructor (akkurat som for i_auron::pAllAurons)
	static std::list<K_auron*> pAllKappaAurons;
	static void callDestructorForAllKappaAurons();

	bool bAuronHarPropagertAtDenErInaktiv;


	// For å lage fin vertikal "strek" ved AP: OVERLAGRER fra i_auron: (for å få eksakt fyringstid!)
	inline virtual void writeAPtoLog()
	{
		// XXX Kommenterer ut for å lettere sjå gjennom log-fil:

		// Skriver fyringstidspunkt i loggfil for fyringstidspunkt:
		#if GCC
			actionPotential_logFile.precision(11);
		#endif
		actionPotential_logFile<<dLastFiringTime <<";\n";		
		//actionPotential_logFile.flush();

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
	inline void changeKappa_absArg( const double&);

	// Sjå @asdf1515
	double dStartOfNextTimeWindow; 	// Start-tidspunkt for neste time window (brukes til å finne start-depol. for neste time window).

	// Rekalkulering av kappa, for å unngå 'truncation error':
	inline virtual double recalculateKappa();
	recalcKappaClass kappaRecalculator;

	//Liste over alle Kappa auron: 	
	std::list<K_auron*> pAlleKappaAuron;

	inline void doTask();
	inline void doCalculation();

	inline void estimateFiringTimes(const double&);
	inline void estimateFiringTimes(); // Bruker ulTime..

	public:
	K_auron(std::string sNavn_Arg ="K_auron", double dStartKappa_arg = 0); 	
	~K_auron();




	inline double getCalculateDepol() const
	{

		// GAMMEL: return (dDepolAtStartOfTimeWindow - dAktivitetsVariabel)*exp(-(double)LEAKAGE_CONST  * (time_class::getTime() - ulStartOfTimewindow )) + dAktivitetsVariabel ;

		static double dDepolStatic;

		//dDepolStatic = (dDepolAtStartOfTimeWindow - dAktivitetsVariabel)*exp(-LEAKAGE_CONST  * (time_class::getTime() - dStartOfTimeWindow )) + dAktivitetsVariabel; 
		dDepolStatic = (dDepolAtStartOfTimeWindow - dAktivitetsVariabel)*exp(-LEAKAGE_CONST  * (dStartOfNextTimeWindow - dStartOfTimeWindow )) + dAktivitetsVariabel ;
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
		dDepolStatic = (dDepolAtStartOfTimeWindow - dAktivitetsVariabel)*exp(-LEAKAGE_CONST  * (dForTimeInstant_arg - dStartOfTimeWindow )) + dAktivitetsVariabel; //v(t)=K(1-e^-at)-v_+e^-at = (v_0 - K) e^-at + K   !

		//writeDepolToLog( dForTimeInstant_arg, dDepolStatic); // Tar vekk denne, for å få lov til å definere funksjonen som const.
		return dDepolStatic;
	}

	inline void writeDepolToLog( const double& dTimeArg, const double& dDepolArg)
	{
		#if LOG_DEPOL 
			#if GCC
				// Sette precision for output: Funker bare med #include <iomanip>; , som ikkje funker for clang++-compiler..
				depol_logFile.precision(11);
			#endif
	
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
			#if GCC
				// Sette precision for output: Funker bare med #include <iomanip>; , som ikkje funker for clang++-compiler..
				depol_logFile.precision(11);
			#endif
	
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
				kappa_logFile 	<<time_class::getTime() <<"\t" <<dAktivitetsVariabel <<"; \t #Kappa\n" ;
			}
		#endif
	}

	const std::list<K_synapse*> getUtSynapserP(){ return pUtSynapser; }


	friend class i_auron; 		// For i_auron to be able to call K_auron::callDestructorForAllKappaAurons()
	friend class K_synapse;
	friend class K_dendrite;
	friend class recalcKappaClass;

	friend void* taskSchedulerFunction(void* );
}; // }

class s_sensor_auron : public s_auron{
	// Function pointer:
	const double& (*pSensorFunction)(void);

	static std::list<s_sensor_auron*> pAllSensoryAurons;

	inline void updateSensoryValue();
	static void updateAllSensorAurons();
	
	public:
		s_sensor_auron( std::string sNavn_Arg ,  const double& (*pFunk_arg)(void) );

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
		K_sensory_auron( std::string sNavn_Arg ,  const double& (*pFunk_arg)(void) );

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
