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
#include <sstream>

#include <typeinfo> //For å sjekke typeid

#include "synapse.h"
#include "auron.h"
#include "../andreKildefiler/main.h"
#include "../andreKildefiler/time.h"

std::ostream & operator<< (std::ostream& ut, i_auron* pAuronArg );


extern unsigned long ulTemporalAccuracyPerSensoryFunctionPeriod;
extern bool bContinueExecution;

/*************************************************************
****** 													******
******  		CONTRUCTORS OG DESCTRUCTORS 			******
****** 													******
*************************************************************/

//{1 * AURON
//{2 *** callDestructorForAllAurons() og callDestructorForAllKappaAurons() 
void i_auron::callDestructorForAllAurons()
{
	cout<<"\n\n\n\nCall destructor for all i_auron elements:\n";
	// TODO Ta vekk [i_auron::] fra kallet under?
	for( std::list<i_auron*>::iterator iter = i_auron::pAllAurons.begin() ;  iter != i_auron::pAllAurons.end() ;  iter++ )
	{
		// Todo: Lag en sjekk som ser om dEstimatedTaskTime er MAKS. Da skal eg bare skrive ut '-'.
		cout<<"\t[ " <<(*iter)->sNavn <<" ]\t\t" 		<<"\tdEstimatedTaskTime :\t" <<(*iter)->dEstimatedTaskTime 
			<<endl;
	}
	cout<<"\n";


	// Først kall destructor for alle kappa-auron!
	K_auron::callDestructorForAllKappaAurons();

	// Kaller destructor for alle gjenværende udestruerte auron (alle ikkje-K_auron):
	// Sletter alle auron i i_auron::pAllAurons
	while( ! i_auron::pAllAurons.empty() )
	{
		#if DEBUG_PRINT_LEVEL > 2
		cout<<"Call destructor for auron " <<i_auron::pAllAurons.front()->sNavn <<endl;
		#endif
		// delete first element(call destructor); The destructor removes element from pAllAurons.
	 	delete (*i_auron::pAllAurons.begin());
	}
}
void K_auron::callDestructorForAllKappaAurons()
{
	// Call destructor for all remaining K_aurons (listed in K_auron::pAllKappaAurons):
	while( ! K_auron::pAllKappaAurons.empty() )
	{
		#if DEBUG_PRINT_LEVEL > 2
		cout<<"Call destructor for K_auron " <<K_auron::pAllKappaAurons.front()->sNavn <<endl;
		#endif

		// delete first element(call destructor); The destructor removes element from pAllKappaAurons.
	 	delete (*K_auron::pAllKappaAurons.begin());
	}
}

//}2
//{2 *** i_auron   
i_auron::i_auron(std::string sNavn_Arg /*="unnamed"*/, double dStartAktVar /*=0*/) : timeInterface("auron"), dAktivitetsVariabel(dStartAktVar), sNavn(sNavn_Arg)
{

	#if LOGG_DEPOL
		// Printing to log file. Initiation of file stream and creation of a .oct file that is executable in octave.
		std::ostringstream tempDepolLoggFileAdr;
		tempDepolLoggFileAdr<<"./datafiles_for_evaluation/log_auron_" <<sNavn <<"-depol" <<".oct";
	
		std::string tempStr( tempDepolLoggFileAdr.str() );
	
		// need c-style string for open() function:
		depol_logFile.open( tempStr.c_str() );

		depol_logFile<< "# Kjøring med "
					 <<"\n#\tAlpha = \t" <<LEAKAGE_CONST 
					 <<"\n#\tAntall Iter = \t" <<ulTemporalAccuracyPerSensoryFunctionPeriod <<"\n\n";
		depol_logFile<<"data=[";
		depol_logFile.flush();

		// Printing to log file. Initiation of file stream and creation of a .oct file that is executable in octave.
		std::ostringstream tempFilAdr2;
		tempFilAdr2<<"./datafiles_for_evaluation/log_auron_" <<sNavn <<"-depolSPIKES" <<".oct";
		std::string tempStr2( tempFilAdr2.str() );
	
		// need c-style string for open() function:
		actionPotential_depolLogFile.open ( tempStr2.c_str() );

		actionPotential_depolLogFile 	<< "# Kjøring med "
					 					<<"\n#\tAlpha = \t" <<LEAKAGE_CONST 
					 					<<"\n#\tAntall Iter = \t" <<ulTemporalAccuracyPerSensoryFunctionPeriod <<"\n\n";
		actionPotential_depolLogFile<<"data=[";
		actionPotential_depolLogFile.flush();
	#endif

	// Lager en loggfil for tidspunkt for action potential:
	std::ostringstream tempFilAdr3;
	tempFilAdr3<<"./datafiles_for_evaluation/log_auron_" <<sNavn <<"-firingTimes" <<".oct";

	std::string tempStr3( tempFilAdr3.str() );

	// need c-style string for open() function:
 	actionPotential_logFile.open( tempStr3.c_str() );
	actionPotential_logFile << "# Kjøring med "
				 			<<"\n#\tAlpha = \t" <<LEAKAGE_CONST 
				 			<<"\n#\tAntall Iter = \t" <<ulTemporalAccuracyPerSensoryFunctionPeriod <<"\n\n"
							<<"data=[";
	actionPotential_logFile.flush();



}
i_auron::~i_auron()
{
	#if DEBUG_PRINT_DESCTRUCTOR
		cout<<"\tDESTRUCTOR: \ti_auron::~i_auron() : \t" <<sNavn <<"\t * * * * * * * * * * * * * * * * * * * * * * * * * \n";
	#endif

	// remove auron from pAllAurons:
	pAllAurons.remove(this);

	#if LOGG_DEPOL
		// Finalize octave script to make it executable:
		depol_logFile 	<<"];\n\n"
						<<"plot( data([1:end],1), data([1:end],2), \"@;Depolarization;\");\n"
						<<"title \"Depolarization for auron " <<sNavn <<"\"\n"
						<<"xlabel Time\n" <<"ylabel \"Activity variable\"\n"
						//<<"axis([0, " <<time_class::getTime() <<", 0, " <<FIRING_THRESHOLD*1.3 <<"]);\n"
						//<<"axis([0, " <<time_class::getTime() <<", 0, " <<FIRING_THRESHOLD*1.3 <<"]);\n"
						<<"akser=([0 data(end,1) 0 1400 ]);\n"
		//				<<"print(\'./eps/eps_auron" <<sNavn <<"-depol.eps\', \'-deps\');\n"
						<<"sleep(" <<OCTAVE_SLEEP_AFTER_PLOTTING <<"); "
						;
		depol_logFile.flush();
		depol_logFile.close();
	
		// Avslutter log-file for AP (som lager en vertikal strek fra 1050 til 1200.
		actionPotential_depolLogFile 	<<"];\n"
										<<"AP_logg=data;\n"
										<<"# source 'ADRESSE TIL DEPOL-FIL';\n"
										<<"\nplot( data(1:end, 1), data(1:end,2), \"@b;Depol;\", AP_logg(1:end,1),AP_logg(1:end,2), \".b;;\");\n";
		actionPotential_depolLogFile.flush();
		actionPotential_depolLogFile.close();
		
	#endif
	
	// Loggfil for firing time:
	actionPotential_logFile<<"];";
	actionPotential_logFile.flush();
	actionPotential_logFile.close();
}
//}2
//{2 *** s_auron
s_auron::s_auron(std::string sNavn_Arg /*="unnamed"*/, int nStartDepol /*=0*/) : i_auron(sNavn_Arg, nStartDepol)
{
	// for debugging
	sClassName = "s_auron";

	ulTimestampForrigeFyring = time_class::getTime();
	ulTimestampLastInput  = time_class::getTime();

	// Setter initial-tilstant:
	dAktivitetsVariabel = nStartDepol;
	
	// Legger til auron* i std::list<i_auron*> pAllAurons;
	i_auron::pAllAurons.push_back( this );

	//pOutputAxon og pInputDendrite
	pOutputAxon = new s_axon(this);
 	pInputDendrite = new s_dendrite(this);
}
s_auron::~s_auron() // Blir ikkje kjørt automagisk i slutten av programmet. Da går i_auron::~i_auron();
{
	//Vil bare kalles dersom eit s_auron slettes før avslutting. Ved destruering av alle i_auron::pAllAurons kalles bare ~i_auron.
	#if DEBUG_PRINT_DESCTRUCTOR
		cout<<"DESTRUCTOR:\ts_auron::~s_auron()\n";
	#endif
	
	//i_auron tar seg av dette.. (???)
	delete pOutputAxon;
	delete pInputDendrite;

	//Etter ~s_auron() kalles ~i_auron()
} 
//}2
//{2 *** K_auron
K_auron::K_auron(std::string sNavn_Arg /*="unnamed"*/, double dStartKappa_arg /*= 0*/) : i_auron(sNavn_Arg), kappaRecalculator(this)
{ //{
	// for debugging
	sClassName = "K_auron";

	ulTimestampForrigeFyring = time_class::getTime();

	// Because it is public derivative of i_auron, that constructor is also executed. The object is therefore automatically added to pAllAurons.
	// Also push pointer to pAllKappaAurons:
	pAllKappaAurons.push_back( this );
	
	// Push pointer to auron in std::list<timeInterface*> pPeriodicElements:
	time_class::addElementIn_pPeriodicElements( this );

//XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX 
// XXX XXX AXON OG DENDRITE SKAL VEKK! XXX XXX
//XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX 
	//p OutputAxon og pInputDendrite
	//p OutputAxon = new K_axon(this);
 	pInputDendrite = new K_dendrite(this);

	// Initialiserer aktivitetsvariablene kappa til å være verdien dStartKappa_arg: Lagrer dit, så kaller eg changeKappa_derivedArg(0) for å få gjort det som skal gjøres. Trenger at K>T når dette gjøres..
	dAktivitetsVariabel = dStartKappa_arg;
	changeKappa_derivedArg( 0 );

	// Initierer første 'time window':
	dStartOfTimeWindow = (double)time_class::getTime();

	#if LOGG_KAPPA
		// Utskrift til logg. LOGG-initiering (lag ei .oct fil som er kjørbar)
		std::ostringstream tempFilAdr;
		tempFilAdr<<"./datafiles_for_evaluation/log_auron_" <<sNavn <<"-kappa" <<".oct";
		std::string tempStr( tempFilAdr.str() );

		// trenger c-style string for open():
		kappa_logFile.open( tempStr.c_str() );
		kappa_logFile<<"data=[" <<time_class::getTime() <<"\t" <<dAktivitetsVariabel <<";\n";
		kappa_logFile.flush();
	#endif

	#if DEBUG_PRINT_CONSTRUCTOR
 		cout 	<<"\nConstructor: K_auron.\n"
				<<"\tKappa: \t\t\t\t" <<dAktivitetsVariabel <<endl
				<<"\tTau:   \t\t\t\t" <<FIRING_THRESHOLD <<endl
				<<"\n\n";
	#endif

	// Finner v(t_0) på normal måte: doCalculation();
	doCalculation();

} 
K_auron::~K_auron()
{
	#if DEBUG_PRINT_DESCTRUCTOR
		cout<<"Destructor:\tK_auron::~K_auron()\n";
	#endif
	
	// pAllAurons - element fjærnes i i_auron. 
	// Fjærner pAllKappaAurons-element i K_auron::~K_auron() :
	pAllKappaAurons.remove(this);


// XXX XXX AXON OG DENDRITE SKAL VEKK! XXX XXX
	//i_auron tar seg ikkje av dette.. Sletter axon og dendrite:
	//delete p OutputAxon;
	// delete pInputDendrite; FORELØPIG ER DENNE FLYTTA TIL ETTER sletting av synapsene.

// kommentarFIX01@neuroElement.cpp XXX XXX 
	// Sletter alle utsynapser fra K_auronet. K_auron har ikkje dendrite, så dette er spesiellt for K_auron.
	while( !pUtSynapser.empty() ){
		delete (*pUtSynapser.begin()); //Kaller destruktoren til første gjenværende synapse. Dette fører også til at synapsa fjærnes fra pUtSynapser (og dendrite.pInnSynapser)
	}

//JESS! Det var det som skulle til! HURRA!
//  Hugs på at pInputDendrite må fjærnes ETTER alle pOutputSynapser! XXX
	delete pInputDendrite; //TODO Denne skal vekk!


	// Rett slutt på utskriftsfil-logg:
	#if LOGG_KAPPA
		kappa_logFile<<time_class::getTime() <<"\t" <<dAktivitetsVariabel <<"];\n"
						<<"axis([0," <<time_class::getTime() <<"])\n"
						<<"plot( data([1:end],1), data([1:end],2), \"@;Kappa;\");\n"

						<<"title \"Activity variable for K-auron " <<sNavn <<"\"\n"
						<<"xlabel Time\n" <<"ylabel \"Activity variable\"\n"
						//<<"akser=[0 data(end,1) 0 1400 ]; axis(akser);\n"
						//<<"print(\'./eps/eps_auron" <<sNavn <<"-kappa.eps\', \'-deps\');\n"
						<<"sleep(" <<OCTAVE_SLEEP_AFTER_PLOTTING <<"); "
						;
		kappa_logFile.close();
	#endif

}
//}2
//{2 *** K_sensory_auron
K_sensory_auron::K_sensory_auron( std::string sNavn_Arg , double (*pFunk_arg)(void) ) : K_auron(sNavn_Arg, (*pFunk_arg)() ) //Setter init-K for K_auron til returen fra (pFunk_arg)()..
{
	// Assign the sensor function:
	pSensorFunction = pFunk_arg;
	// Add to pAllSensoryAurons list:
	pAllSensoryAurons.push_back(this);
	pAllAurons.push_back( this ); 

	// Tar første sample av pSensorFunction:
	dAktivitetsVariabel = (*pSensorFunction)(); // Trur den beregner pSendedValue av 0. Blir dette feil?
	dSensedValue = dAktivitetsVariabel;

	// Initialiserer kappaRecalculator i tilfelle sensorauron seinere skal få mulighet for input-synapser:
// XXX Har ikkje med recalculate kappa for sensor-auron: sensor-auron kan kun få input fra sensed element. asdf41412@neuroElement.cpp
	kappaRecalculator.pKappaAuron_obj = this;
	recalculateKappa();
}
//}2
//{2 *** s_sensor_auron
s_sensor_auron::s_sensor_auron( std::string sNavn_Arg , double (*pFunk_arg)(void) ) : s_auron(sNavn_Arg)
{
	// Assign the sensor function:
	pSensorFunction = pFunk_arg;
	// Add to pAllSensoryAurons list:
	pAllSensoryAurons.push_back(this);
}
//}2
//}1 * AURON

//{1 * SYNAPSE
//{2 i_synapse
i_synapse::i_synapse(double dSynVekt_Arg, bool bInhibEffekt_Arg, std::string sKlasseNavn /*="synapse"*/ ) : timeInterface(sKlasseNavn), bInhibitoryEffect(bInhibEffekt_Arg)
{
	dSynapticWeight = abs(dSynVekt_Arg);
	dSynapticWeightChange = 0;
	
	#if DEBUG_PRINT_CONSTRUCTOR
		cout<<"\tConstructor :\ti_synapse::i_synapse(unsigned uSynVekt_Arg, bool bInhibEffekt_Arg, string navn);\n";
	#endif
}
//}2
//{2 s_synapse
s_synapse::s_synapse(s_auron* pPresynAuron_arg, s_auron* pPostsynAuron_arg, double dSynVekt_Arg /*=1*/, bool bInhibEffekt_Arg /*=false*/) 
			:  i_synapse(dSynVekt_Arg, bInhibEffekt_Arg, "s_synapse"), pPreNodeAxon(pPresynAuron_arg->pOutputAxon), pPostNodeDendrite(pPostsynAuron_arg->pInputDendrite) 
{

	#if DEBUG_PRINT_CONSTRUCTOR
		cerr<<"Kaller s_synapse::s_synapse(" <<pPreNodeAxon->pElementOfAuron->sNavn <<".pOutputAxon, " <<pPostNodeDendrite->pElementOfAuron->sNavn <<".pInputDendrite, ...)\n";
	#endif

	// Legger til synapse som utsyn. i presyn. axon og innsyn. i postsyn. dendrite:
	pPreNodeAxon->pUtSynapser.push_back(this);
	pPostNodeDendrite->pInnSynapser.push_back(this);
	
	// Initialisering av synapsens logg-fil:
	//{4 lag ei .oct - fil, og gjør klar for å kjøres i octave:
	std::ostringstream tempFilAdr;
	tempFilAdr<<"./datafiles_for_evaluation/log_s_synapse_" <<pPresynAuron_arg->sNavn <<"-"  <<pPostsynAuron_arg->sNavn ;
	if(bInhibitoryEffect){ tempFilAdr<<"_inhi"; }
	else{ 			  tempFilAdr<<"_eksi"; }
	tempFilAdr<<".oct";

	std::string tempStr( tempFilAdr.str() );

	// trenger c-style string for open():
	synTransmission_logFile.open( tempStr.c_str() );
	synTransmission_logFile<<"data=[0 0;\n";
	synTransmission_logFile.flush();
	//}4

}
s_synapse::~s_synapse()
{
	bool bPreOk  = false;
	bool bPostOk = false;

	#if DEBUG_PRINT_DESCTRUCTOR
		cout<<"\tDESTRUCTOR :\ts_synapse::~s_synapse() : \t";
	#endif


	if( !bPreOk ){ 
		//fjærner seg sjølv fra prenode:
		for( std::list<s_synapse*>::iterator iter = (pPreNodeAxon->pUtSynapser).begin(); iter != (pPreNodeAxon->pUtSynapser).end() ; iter++ ){
			if( *iter == this ){
				#if DEBUG_PRINT_DESCTRUCTOR
					cout<<"\t~( [" <<pPreNodeAxon->pElementOfAuron->sNavn <<"] -> "; 						// utskrift del 1
				#endif
				(pPreNodeAxon->pUtSynapser).erase( iter );
				bPreOk = true;
				break;
			}
		}
	}
	if( !bPostOk ){
		//fjærner seg sjølv fra postnode:
		for( std::list<s_synapse*>::iterator iter = pPostNodeDendrite->pInnSynapser.begin(); iter != pPostNodeDendrite->pInnSynapser.end() ; iter++ ){
			if( *iter == this ){ 
				#if DEBUG_PRINT_DESCTRUCTOR
					cout<<"[" <<pPostNodeDendrite->pElementOfAuron->sNavn <<"] )\t"; 						// utskrift del 2
				#endif
				(pPostNodeDendrite->pInnSynapser).erase( iter );
				bPostOk = true;
				break;
			}
		}
	}

	// Vil aldri skje: Redundant feiltest for sikkerhetskkuld:
	if( (!bPreOk) || (!bPostOk) ){
		/// FEIL:
		std::cerr<<"\n\n\n\n\nFEIL FEIL FEIL!\nSjekk neuroEnhet.cpp #asdf250. Feil i s_synapse destruksjon. (~s_synapse )\n";
		std::cerr<<"I test om eg vart sletta  fra presyn. neuron og postsyn. neuron: failed\t\t" 
			<<"bPreOk (" <<pPreNodeAxon->pElementOfAuron->sNavn <<"):" <<bPreOk <<"  ->  bPostOk (" <<pPostNodeDendrite->pElementOfAuron->sNavn <<"): " <<bPostOk 
			<<"\n(Eg er ikkje sletta fra det aktuelle neuronet (dei med verdi 0)\n";
		std::cerr<<"Eg ligger mellom (neuron: presyn - postsyn): " <<pPreNodeAxon->pElementOfAuron->sNavn <<" - " <<pPostNodeDendrite->pElementOfAuron->sNavn <<endl;
		exit(-9);	
	}
	cout<<endl;


	//{4 Rett slutt på utskriftsfil-logg:
	synTransmission_logFile<<"];\n"
					<<"plot( data([1:end],1), data([1:end],2), \";Synaptic weight;\");\n"

					<<"title \"Synaptic weight for s_synapse: " <<pPreNodeAxon->pElementOfAuron->sNavn <<" -> " <<pPostNodeDendrite->pElementOfAuron->sNavn <<"\"\n"
					<<"xlabel Time\n" <<"ylabel syn.w.\n"
					//<<"akser=[0 data(end,1) 0 1400 ]; axis(akser);\n"
					//<<"print(\'eps_" <<pPreNodeAxon->pElementOfAuron->sNavn <<"->" <<pPostNodeDendrite->pElementOfAuron->sNavn <<".eps\', \'-deps\');\"\n"
			//		<<"print(\'eps__s_synapse_" <<pPreNodeAxon->pElementOfAuron->sNavn <<"->" <<pPostNodeDendrite->pElementOfAuron->sNavn <<".eps\', \'-deps\');" 	
					<<"sleep(" <<OCTAVE_SLEEP_AFTER_PLOTTING <<"); ";
	synTransmission_logFile.close();
	//}4

}
//}2
//{2 K_synapse
K_synapse::K_synapse(K_auron* pPresynAuron_arg, K_auron* pPostsynAuron_arg, double dSynVekt_Arg /*=1*/, bool bInhibEffekt_Arg /*=false*/ , unsigned uTemporalDistanceFromSoma_arg /* =1*/)
 :  i_synapse(dSynVekt_Arg * FIRING_THRESHOLD      , bInhibEffekt_Arg, "K_synapse") , uTemporalDistanceFromSoma(uTemporalDistanceFromSoma_arg) , pPreNodeAuron(pPresynAuron_arg), pPostNodeDendrite(pPostsynAuron_arg->pInputDendrite)
{
	#if DEBUG_PRINT_CONSTRUCTOR
		cout<<"Constructor :\tK_synapse::K_synapse(" <<pPreNodeAuron->sNavn <<", " <<pPostNodeDendrite->pElementOfAuron->sNavn <<".pInputDendrite, ...)\n";
	#endif

	// TODO TODO Lag K_auron::nyUtSynapse() som kan tilordne delay til synapsen (uTemporalDistanceFromSoma).
	// Foreløpig skriver eg noko som gjør jobben, men som er dårlig:
	// kommentarID_asd603@neuroElement.cpp
	pPreNodeAuron->pUtSynapser.push_back(this);
	uTemporalDistanceFromSoma = 1;

// XXX XXX  TA VEKK axon OG dendrite XXX XXX
	//pPre NodeAxon->pUtSynapser.push_back(this);
	pPostNodeDendrite->pInnSynapser.push_back(this);


	// lag ei .oct - fil, og gjør klar for å kjøres i octave:
	std::ostringstream tempFilAdr;
	tempFilAdr<<"./datafiles_for_evaluation/log_transmission_K_synapse_" <<pPresynAuron_arg->sNavn <<"-"  <<pPostsynAuron_arg->sNavn ;
	if(bInhibitoryEffect){ tempFilAdr<<"_inhi"; }
	else{ 			  tempFilAdr<<"_eksi"; }
	tempFilAdr<<".oct";

	std::string tempStr( tempFilAdr.str() );
	// trenger c-style string for open():
	synTransmission_logFile.open( tempStr.c_str() );
	synTransmission_logFile<<"data=[0 0;\n";
	synTransmission_logFile.flush();


	// Initierer overføring:
	doTask();
}

K_synapse::~K_synapse()
{
	bool bPreOk  = false;
	bool bPostOk = false;


	#if DEBUG_PRINT_DESCTRUCTOR
		cout<<"\t\tDESTRUCTOR :\tK_synapse::~K_synapse() : \t";
	#endif

	if( !bPreOk ){
		//fjærner seg sjølv fra prenode:
		for( std::list<K_synapse*>::iterator iter = pPreNodeAuron->pUtSynapser.begin(); iter != pPreNodeAuron->pUtSynapser.end() ; iter++ ){
			if( *iter == this ){
				#if DEBUG_PRINT_DESCTRUCTOR
					cout<<"\t~( [" <<pPreNodeAuron->sNavn <<"] -> "; 				// utskrift del 1
				#endif
				(pPreNodeAuron->pUtSynapser).erase( iter );
				bPreOk = true;
				break;
			}
		}
	}
	if( !bPostOk ){
		//fjærner seg sjølv fra postnode:
		for( std::list<K_synapse*>::iterator iter = pPostNodeDendrite->pInnSynapser.begin(); iter != pPostNodeDendrite->pInnSynapser.end() ; iter++ ){
			if( *iter == this ){ 
				#if DEBUG_PRINT_DESCTRUCTOR
					cout<<"[" <<pPostNodeDendrite->pElementOfAuron->sNavn <<"] )\t"; 				// utskrift del 2
				#endif
				(pPostNodeDendrite->pInnSynapser).erase( iter );
				bPostOk = true;
				break;
			}
		}
	}
	// Redundant test: Vil "aldri" skje. 
	if( (!bPreOk) || (!bPostOk) ){
		/// FEIL:
		std::cerr<<"\n\n\n\n\nFEIL FEIL FEIL!\nSjekk neuroEnhet.cpp #asdf250. Feil i s_synapse destruksjon. (~s_synapse )\n";
		std::cerr<<"I test om eg vart sletta  fra presyn. neuron og postsyn. neuron: failed\t\t" 
			<<"bPreOk (" <<pPreNodeAuron->sNavn <<"):" <<bPreOk <<"  ->  bPostOk (" <<pPostNodeDendrite->pElementOfAuron->sNavn <<"): " <<bPostOk 
			<<"\n(Eg er ikkje sletta fra det aktuelle neuronet (dei med verdi 0)\n";
		std::cerr<<"Eg ligger mellom (neuron: presyn - postsyn): " <<pPreNodeAuron->sNavn <<" - " <<pPostNodeDendrite->pElementOfAuron->sNavn <<endl;
		exit(-9);	
	}
	cout<<endl;

	// Rett slutt på utskriftsfil-logg:
	synTransmission_logFile<<"];\n"
					<<"plot( data([1:end],1), data([1:end],2), \".r;Synaptic transmission;\");\n"
					<<"title \"Synaptic transmission from s_synapse: " <<pPreNodeAuron->sNavn <<" to " <<pPostNodeDendrite->pElementOfAuron->sNavn <<"\"\n"
					<<"xlabel Time\n" <<"ylabel Syn.Transmission\n"
					//<<"akser=[0 data(end,1) 0 1400 ]; axis(akser);\n"
					//<<"print ./eps/eps_transmission_" <<pPreNodeAuron->sNavn <<"->" <<pPostNodeDendrite->pElementOfAuron->sNavn <<".eps -deps -color\n"
					<<"sleep(" <<OCTAVE_SLEEP_AFTER_PLOTTING <<"); ";
	synTransmission_logFile.close();
	 
}
//}2
//}1 * SYNAPSE

//{1 * AXON
//{2 ***  i_axon
i_axon::i_axon( std::string sKlasseNavn ="dendrite") : timeInterface(sKlasseNavn)
{
}
i_axon::~i_axon()
{
	// pUtSynapser inneholder bare peikere, så pUtSynapser.clear() vil ikkje føre til destruksjon av synapsene. Kaller destruksjonen eksplisitt:
	while( !pUtSynapser.empty() ){
		// Kaller destruktor for første gjenværende synapse ved å frigjør det elementet som ligg i det frie lageret:
		// 		- Dette fører også til at synapsa fjærnes fra pUtSynapser (og dendrite.pInnSynapser)
		delete (*pUtSynapser.begin()); 
	}
}
//}2 *** i_axon
//{2 ***  s_axon
s_axon::s_axon(s_auron* pAuronArg) : i_axon("s_axon"), pElementOfAuron(pAuronArg)
{
	#if DEBUG_PRINT_CONSTRUCTOR
		cout<<"\tConstructor :\ts_axon::s_axon(s_auron*)\n";//for \tauron " <<pAuronArg->sNavn <<endl;		
	#endif
}
s_axon::~s_axon()
{
	#if DEBUG_PRINT_DESCTRUCTOR
		cout<<"\tDESTRUCTOR :\ts_axon::~s_axon() for auron \t" <<pElementOfAuron->sNavn <<"\n";
	#endif
}
//}2
//}1

//{1 * DENDRITE
//{2 *** i_dendrite
i_dendrite::i_dendrite(/*i_auron* pElementOfAuron_arg,*/ std::string sNavn ="dendrite") : timeInterface(sNavn)
{ 
	//pElementOfAuron = pElementOfAuron_arg;
}
// i_dendrite skal vekk: Innholdet skal over i s_dendrite::~s_dendrite og K_auron::~K_auron!
i_dendrite::~i_dendrite()
{
	// Destruerer alle innsynapser.
	while( !pInnSynapser.empty() ){
	 	delete (*pInnSynapser.begin() );
	}
}
//}2
// Skal gjøres om: ikkje lenger arve fra i_dendrite (?)
//{2 *** s_dendrite
s_dendrite::s_dendrite( s_auron* pPostSynAuron_Arg ) : i_dendrite("s_dendrite"), pElementOfAuron(pPostSynAuron_Arg)
{
	#if DEBUG_PRINT_CONSTRUCTOR
		cout<<"\tConstructor :\ts_dendrite::s_dendrite( s_auron* )\n";
	#endif

	bBlockInput_refractionTime = false;
}
s_dendrite::~s_dendrite()
{
 	#if DEBUG_PRINT_DESCTRUCTOR
		cout<<"\tDesructor :\ts_dendrite::~s_dendrite() \t for auron \t" <<pElementOfAuron->sNavn <<"\n";
	#endif
}
//}2 s_dendrite 
// SKAL VEKK: (Ta inn i K_auron::K_auron)
//{2 *** K_dendrite
K_dendrite::K_dendrite( K_auron* pPostSynAuron_Arg ) : i_dendrite("K_dendrite" ), pElementOfAuron(pPostSynAuron_Arg)
{
	#if DEBUG_PRINT_CONSTRUCTOR
		cout<<"\tConstructor :\tK_dendrite::K_dendrite( K_auron* ) \t\t[TOM]\n";
	#endif
}
K_dendrite::~K_dendrite()
{
	
	#if DEBUG_PRINT_DESCTRUCTOR
		cout<<"\tDestructor :\tK_dendrite::~K_dendrite() \t\t for auron \t" <<pElementOfAuron->sNavn <<"\n";
	#endif
	// Destruerer her først: fordi synapser må avslutte logg..
	while( !pInnSynapser.empty() ){
		#if DEBUG_PRINT_DESCTRUCTOR
			cerr<<"DEBUG ~K_dendrite: \tDestruerer neste synapse\n";
		#endif
	 	delete (*pInnSynapser.begin() );
	}
}
//}2 XXX K_dendrite
//}1 X DENDRITE



/*************************************************************
****** 													******
******  		ANDRE FUNKSJONER:           			******
****** 													******
*************************************************************/

//{1  * KANN
inline void K_dendrite::newInputSignal( double dNewSignal_arg )
{ //{2
	// TODO Legg inn spatiotemporal differensiering for ulike synapser. Dette gjør at eg må gjøre om heile strukturen til dette opplegget.

	// TODO TODO TODO ESTIMER Tidspunk for overføring. No setter eg bare oppdateringstidsspunkt for kappa til starten av tidsiterasjonen.. TODO TODO TODO
	pElementOfAuron->dStartOfTimeWindow = (double)time_class::getTime();
	pElementOfAuron->changeKappa_derivedArg( dNewSignal_arg );
} //}2

inline void K_auron::changeKappa_derivedArg( double dInputDerived_arg)//int derivedInput_arg )
{
	// Arg legges til Kappa no, og effektene av endringa kalkuleres i .doCalculation().
	dChangeInKappa_this_iter +=  dInputDerived_arg ;

	// TODO TODO SKAL eg gjøre noke med dStartOfNextTimeWindow her (tidspunkt for oppdatering av kappa)?

	// Legger den i pCalculatationTaskQueue, slik at effekt av all endring i kappa ila. tidsiterasjonen beregnes etter iterasjonen.

	time_class::addCalculationIn_pCalculatationTaskQueue( this );
	//TODO Test å kjøre doCalculation() direkte!
	//doCalculation(); //XXX153@neuroElement.cpp


//TODO TODO TODO Her er en debug-sak, som ikkje har innvirkning på resten av programmet:
//	if( getCalculateDepol() > FIRING_THRESHOLD ){
//		cerr<<"K_auron::changeKappa_derivedArg()\tFEIL: v(t)>Tau :\tv(t)=" <<getCalculateDepol() <<endl ;
//	}
//TODO TODO TODO ... til hit.



	#if LOGG_KAPPA
		writeKappaToLog();
	#endif

	#if DEBUG_PRINT_LEVEL > 3
	cout<<sNavn <<"\t:\tTid:\t" <<time_class::getTime() <<" ,\tKappa :\t" <<dAktivitetsVariabel <<endl;
	#endif
}

// XXX XXX XXX FARLIG ! Lager føkk når den endrer kappa, og doCalculation(). Kalkulerer v_0 fra gammel tid, men ny kappa. XXX IKKJE BRUK!
inline void K_auron::changeKappa_absArg(double dNewKappa)
{ //{
	cout<<"IKKJE Bruk changeKappa_absArg() ! Den innfører potensial for feil som ødelegger alt. Ikkje ferdig.\nAvlutter."; exit(-9);

	// Kanskje ikkje naudsynt, siden vi ikkje legger til element i pCalculatationTaskQueue..
	//dChangeInKappa_this_iter = 0; // Hindrer .doCalculation() å endre kappa ytterligare.
	dAktivitetsVariabel = dNewKappa;

	// Foreløpig definerer eg eit sensorauron som en rein sensor (Tar ikkje imot input fra andre auron): 	// I dette tilfellet fjærner eg en potensiell feilkilde:
	dChangeInKappa_this_iter = 0;

	// Legger den i pCalculatationTaskQueue, slik at effekt av all endring i kappa ila. tidsiterasjonen beregnes etter iterasjonen.
	time_class::addCalculationIn_pCalculatationTaskQueue( this );
	
	#if LOGG_KAPPA
		writeKappaToLog();
	#endif
} //}
//}1

//{1  * SANN

inline void s_dendrite::newInputSignal( double dNewSignal_arg )
{ //{2 .. }

	// Sjekker om input er blokkert grunnet 'refraction period':
	// TODO HAR tatt det vekk fra både KANN og SANN, for å fjærne en feilkilde i sammenligninga. TODO OTOD TODO TODO  	NESTE LINJE SKAL VÆRE MED om vi skal ha med refraction time i simuleringen.
	// XXX id:asdf21344@neuroElement.cpp
 	// if( bBlockInput_refractionTime ) return;

	// beregner lekkasje av depol siden sist:
	calculateLeakage();

// VIKTIG123@neuroElement.cpp :  LEAKAGE_CONST for SANN: Sjå LEAKAGE_CONST under..

	// Bli heilt sikker på LEAKAGE_CONST: XXX XXX XXX LEAKAGE_CONST XXX XXX xxx XXX
	pElementOfAuron->dAktivitetsVariabel +=  dNewSignal_arg * LEAKAGE_CONST;  //SJEKKA FOR MOTSATT: at K_auron::changeKappa_derivedArg(..) delte på \alpha før endring av kappa. Dette vil (teoretisk) gi samme resultat. 
																		// Eg har ikkje gjort det om, fordi eg allerede har gjort alle forsøka til artikkelen med slik det er no..
										// TODO TODO TODO FIKS DETTE: SJå asdf1235 (over)

	#if DEBUG_PRINT_LEVEL > 2
	cout<<time_class::getTime() <<"\ts_dendrite::newInputSignal( " <<dNewSignal_arg <<" ); \t\tgir depol. :  " <<pElementOfAuron->dAktivitetsVariabel <<"\n";
	#endif
	
	pElementOfAuron->ulTimestampLastInput = time_class::getTime();


	// Dersom auron går over fyringsterskel: fyr A.P.
	if( pElementOfAuron->dAktivitetsVariabel > FIRING_THRESHOLD )
	{
		// SJEKKER Å TA BORT FOR Å FJÆRNE FEIL når eg sammenligner med KANN. TODO ta tilbake dersom dette ref.time skal være med! asdf21344@neuroElement.cpp
		// Blokkerer videre input grunnet 'refraction time':
		//bBlockInput_refractionTime = true;

		// Spatioteporal delay from AP initialization at axon hillock:
		time_class::addTaskIn_pWorkTaskQueue( pElementOfAuron );
	}

	// Skriver til log for aktivitetsVar:
	//pElementOfAuron->writeDepolToLog();
} //}2

inline void s_dendrite::calculateLeakage()
{ //{2 /** Bare for SANN:  ***/

	if( pElementOfAuron->ulTimestampLastInput != time_class::getTime() )
	{
		// regner ut, og trekker fra lekkasje av depol til postsyn neuron.
	 	pElementOfAuron->dAktivitetsVariabel *= pow( (double)(1-LEAKAGE_CONST), (double)(time_class::getTime() - pElementOfAuron->ulTimestampLastInput) );
	
		// Gjøres i s_dendrite::newInputSignal(): 
		//ulTimestampLastInput = time_class::getTime(); 
	}
} //}2

//}1



/*************************************************************
****** 													******
******  		doTask() -- samla på en plass. 			******
****** 													******
*************************************************************/

//  		* 	SANN

/* 	s_auron::doTask() 	:  		Fyrer A.P. */
inline void s_auron::doTask()
{ //{ ** AURON

	if( ulTimestampForrigeFyring == time_class::getTime() )
	{
		DEBUG_L1(<<"\n\n************************\nFeil?\nTo fyringer på en iterasjon? \nFeilmelding au#103 @ auron.h\n************************\n\n");
		return;
	}

	// FYRER A.P.:
	#if DEBUG_PRINT_LEVEL > 1
		cout	<<"\tS S " <<sNavn <<" | S | " <<sNavn <<" | S | S | S | | " <<sNavn <<" | S | | " <<sNavn <<" | S | | " <<sNavn <<"| S | S | S | S |\t"
				<<sNavn <<".doTask()\t\e[33mFYRER\e[0m neuron " <<sNavn <<".\t\t| S S | \t  | S |  \t  | S |\t\tS | " <<time_class::getTime() <<" |\n";
	#endif

	//Axon hillock: send aksjonspotensial 	-- innkapsling gir at a xon skal ta hånd om all output:
	// bestiller at a xon skal fyre NESTE tidsiterasjon. Simulerer tidsdelay i a xonet.
	time_class::addTaskIn_pWorkTaskQueue( pOutputAxon );


	// Registrerer fyringstid (for feisjekk (over) osv.) 
	ulTimestampForrigeFyring = time_class::getTime();


	//Resetter depol.verdi 
	dAktivitetsVariabel = 0; 

	writeAPtoLog();

} //}
/* 	s_axon::doTask()   	:		Simulerer delay i axonet 	*/
inline void s_axon::doTask()
{ //{ ** AXON

	// Avblokkerer dendritt. Opner den for meir input. Foreløpig er dette måten 'refraction time' funker på.. (etter 2 ms-dendrite og auron overføring..)
	// XXX id:asdf21344@neuroElement.cpp
	//pElementOfAuron->pInputDendrite ->bBlockInput_refractionTime = false;

	#if DEBUG_PRINT_LEVEL > 3
 	cout<<"s_axon::doTask()\tLegger inn alle outputsynapser i arbeidskø. Mdl. av auron: " <<pElementOfAuron->sNavn <<" - - - - - - - - - - - - - - - \n";
	#endif

	// Legger til alle utsynapser i pWorkTaskQueue:
 	for( std::list<s_synapse*>::iterator iter = pUtSynapser.begin(); iter != pUtSynapser.end(); iter++ )
	{ // Legger alle pUtSynapser inn i time_class::pWorkTaskQueue: (FIFO-kø)
		time_class::addTaskIn_pWorkTaskQueue( *iter );
	}

	 //Skriver til logg etter refraction-period.
	 pElementOfAuron->writeDepolToLog();
} //}
/*  s_synapse::doTask() : 		Simulerer overføring i synapse */
inline void s_synapse::doTask()
{ //{2 ** SYNAPSE

	// Dersom synapsen har inhibitorisk effekt: send inhibitorisk signal (subtraksjon). Ellers: eksiter postsyn. auron.
 	pPostNodeDendrite->newInputSignal( (1-2*bInhibitoryEffect) * 	( 1000 * dSynapticWeight )   );

	// XXX Kalkulere syn.p.?

	// Loggfører syn.weight
	synTransmission_logFile 	<<"\t" <<time_class::getTime() <<"\t" <<(1-2*bInhibitoryEffect) * dSynapticWeight
						<<" ;   \t#Synpaptic weight\n" ;
//	synTransmission_logFile.flush();

} //}2
/* s_dendrite::doTask() : 		Simulerer delay for input ved dendrite */
inline void s_dendrite::doTask()
{ //{1 DENDRITE
	// s_dendrite::doTask() er en metode for å få inn spatiotemporal delay for neuronet (simulere ikkje-instant overføring)
	time_class::addTaskIn_pWorkTaskQueue( pElementOfAuron );
} //}1

//			* 	KANN

/* K_auron::doTask() 	: 		Fyrer A.P. */
inline void K_auron::doTask()
{ //{1 ** AURON
	//DEBUG_L2(<<"before:\tK_auron::doTask() \tgetCalculateDepol(\e[33m" <<dEstimatedTaskTime <<"\e[33m): " <<getCalculateDepol(dEstimatedTaskTime) <<"\n\n");
	
	if( (unsigned)dEstimatedTaskTime == time_class::getTime() ){
		// Not an error to fire:
		cout<<"\t* * *\t\e[4;32mFIRE\e[0;0m neuron " <<sNavn <<"\t\t\t\t[time, est.t.time] = [\e[1;33m" <<time_class::getTime() <<"\e[0m,\e[32m " <<dEstimatedTaskTime <<"\e[0m]\n";
	}else if( dEstimatedTaskTime < time_class::getTime() ) //Dersom dEstimatedTaskTime er mindre enn nå-tid: at den skulle fyre forrige iter..
	{
		// Error to fire at this time:
		cout<<"\t* * *\t\e[4;32mFIRING\e[31m ERROR\e[0;0m neuron " <<sNavn <<"\t\t\t\t\t[time > (int)est.time]: [\e[1;33m" <<time_class::getTime() <<"\e[0m>\e[1;31m " <<dEstimatedTaskTime <<"\e[0m]\n";
	}else //if( dEstimatedTaskTime > time_class::getTime()+1 ) //Dersom dEstimatedTaskTime er mindre enn nå-tid: at den skulle fyre forrige iter..
	{
		cout<<"\t* * *\t\e[4;32mFIRING\e[31m ERROR\e[0;0m neuron " <<sNavn <<"\t\t\t\t\t[est.t.time != [now] : [tid, (int)est.tid] = [\e[1;33m" <<time_class::getTime() <<"\e[0m<\e[1;31m " <<dEstimatedTaskTime <<"\e[0m]\n";
		DEBUG_L2( <<"Depol.:\t" <<getCalculateDepol());
		exit(2); // XXX Returnerer uten å gjøre noe.
	}



	//Utskrift til skjerm:
	DEBUG_L2(
			<<"[t="<<time_class::getTime() <<" est.tt:" <<dEstimatedTaskTime <<"]\tv(t_n)=" <<getCalculateDepol(dEstimatedTaskTime)
		   	<<"\t" <<sNavn <<".doTask()\t  [periode/1000]=" <<dLastCalculatedPeriod/1000
			<<"\t | K=" <<dAktivitetsVariabel  <<"\tSiste v_0:" <<dDepolAtStartOfTimeWindow 
			);


	#if LOGG_DEPOL
		// For more accurate depol.-curves (log files) //{
		#if GCC
			// Set precision for output: only works with #include <iomanip>, that does not work for clang++ compiler..
			depol_logFile.precision(11);
		#endif
	
		// Write action action-potential log entry: one at Firing Thread and one at x-axis at time dEstimatedTaskTime:
		depol_logFile 	<<dEstimatedTaskTime <<"\t" <<FIRING_THRESHOLD <<"; \t #Action potential \t\tAPAPAP\n" ;
		depol_logFile 	<<dEstimatedTaskTime <<"\t" <<0 			 <<"; \t #Action potential \t\tAPAPAP\n" ;
	//	depol_logFile.flush();
		//}
	#endif



	// Initialize new 'time window':
		// (Setter dStartOfTimeWindow til dEstimatedTaskTime -- estimert tidsspunkt for fyring..)
	dDepolAtStartOfTimeWindow = 0; 
	dStartOfTimeWindow = dEstimatedTaskTime; // IMPORTANT: use dEstimatedTaskTime instead of dStartOfNextTimeWindow for firing. Firing is a special situation for initiation of a new time window!

	// Save time of firing: When the period is recalculated, the exact start of time window is imporant. Cannot use ulTime because it is always less than the exact firing time: ln(-X) -> \infty
	dLastFiringTime = dEstimatedTaskTime;

	// It is important that the esimatedPeriod is computed after dEstimatedTaskTime += dLastCalculatedPeriod !
	// 	If I run estimateFiringTimes() or doCalculation() before, a large error is the result.

	// Logger AP (vertikal strek)
	writeAPtoLog(); //Viktig: Må gjøres etter at dLastFiringTime er skrevet til!

	// Pga. nye mekansimen som gjør det mulig å fyre denne iter: (estimateFiringTimes(double) oppdaterer dEstimatedTaskTime)
	estimateFiringTimes(dLastFiringTime); // Kan også bruke dLastFiringTime 			id:est490@neuroElement.cpp
	//estimateFiringTimes(); // bruker ulTime..

	// Sjekker om den skal fyre igjen denne iter:
	if( dEstimatedTaskTime < time_class::getTime()+1 )
		time_class::addTaskInPresentTimeIteration(this);

	DEBUG_L2(<<"AFTER\tK_auron::doTask() \tgetCalculateDepol(\e[33m" <<dLastFiringTime <<"\e[0m): " <<getCalculateDepol(dEstimatedTaskTime) );
} //}1
/* K_synapse::doTask() 	: 		Simulerer overføring i synapsen */
inline void K_synapse::doTask()
{ //{1 ** SYNAPSE

	// Istedenfor å sende inn pos. eller neg. signal avhengig av bInhibitoryEffect: [ 1-2*bInhibitoryEffect ]  Gir enten +1 eller -1. 
	pPostNodeDendrite->newInputSignal( (1-2*bInhibitoryEffect) * dSynapticWeight * pPreNodeAuron->dChangeInPeriodINVERSE );

	
	double dPresynDeltaPeriodeINVERSE_temp =  pPreNodeAuron->dChangeInPeriodINVERSE;
	#if DEBUG_PRINT_LEVEL > 2
 	cout<<"K_synapse::doTask()\tdSynapticWeight: " <<dSynapticWeight 
		<<", preNode->dChangeInPeriodINVERSE: " <<dPresynDeltaPeriodeINVERSE_temp <<"\tOverføring: "
		<<(1-2*bInhibitoryEffect) * dSynapticWeight * dPresynDeltaPeriodeINVERSE_temp
		<<endl;
	#endif

	synTransmission_logFile
		<<time_class::getTime() <<"  \t"
		<<(1-2*bInhibitoryEffect) * dSynapticWeight * dPresynDeltaPeriodeINVERSE_temp
		<<" ; \n";
//	synTransmission_logFile.flush();
} //}1

// TODO SKAL VEKK: 
inline void K_dendrite::doTask()
{ // DENDRITE (ikkje i bruk)
	// TODO Trur ikkje denne skal være med lenger. Legger til brutal feilsjekk: exit(-1);
	cout<<"\n\nBrutal feilsjekk @ K_dendrite::doTask() : exit(-1);\n\n";

	exit(-1);
} 

//}1            *       KANN slutt

// 		* 	time_class
/* time_class::doTask() 	: 		Organiserer tid: doTask() itererer tid og holder pWorkTaskQueue og tid i orden etter definerte regler */
void time_class::doTask()
{ 	//{1 
	// Check whether simulation is complete: in this case, initialize graceful termination.
	if( ulTime >= ulTotalNumberOfTimeSteps )
	{
		bContinueExecution = false;
		return;
	}

	#if PRINT_TIME_ITERATION
	if(ulTime % PRINT_TIME_EVERY_Nth_ITER  == 0)		
		cout<<"\n\t* * *\tIterate time: \t * * * * * * * In time_class::doTask() - Increase from:   * * * * * * * * * * = \e[4;39m"	<<ulTime <<"->" <<ulTime+1 <<"\e[0m :\n";
	#endif
	#if PRINT_TIME_ITERATION
	// Increase by one (ulTime+1) to show t_n for the new time step.
	if(ulTime % PRINT_TIME_EVERY_Nth_ITER  == 0)		
		cout<<"\t* * *\tTIME:" <<"\e[33m"	<<ulTime+1 <<"\e[0m" <<"\t * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * = "	<<ulTime <<"\n";
	#endif


	// Push self-pointer to back of pWorkTaskQueue:
	pWorkTaskQueue.push_back(this);	

	// Iterate t_n:
	ulTime++;

	/*************************************************
	* Push scheduled tasks to back of pWorkTaskQueue *  	DEPRECATED
	*************************************************/
// Kommentert ut fordi dette ordnes i doCalculation()
// XXX Trur dette er deprecated! Kommenterer ut! //{
#if 0 
	for( std::list<timeInterface*>::iterator pPE_iter = pPeriodicElements.begin() ; pPE_iter != pPeriodicElements.end() ; pPE_iter++ )
	{
		// Typekonverderer dEstimatedTaskTime til unsigned long, og sjekker om elementet er planlagt å gjennomføre noe neste iter (legger til 0.5 for å få rett avrunding):
		if( (unsigned long)( (*pPE_iter)->dEstimatedTaskTime) == ulTime )
		{
			cout<<"dEstimatedTaskTime == ulTime: \e[1;34mlegger\e[0m til " <<(*pPE_iter)->sClassName <<" i pWorkTaskQueue\n";
			//Denne legger til elementet på slutten. Det er litt feil.
			//addTaskIn_pWorkTaskQueue( (*pPE_iter) );
			addTaskInPresentTimeIteration( (*pPE_iter) );
			//Skal difor heller bruke addTaskInPresentTimeIteration() TODO Da må eg også flytte pWorkTaskQueue.push_back(this) til toppen (heilt i toppen?)
		}
 	}
#endif //}


	/******************************
	* Update all K_sensory_aurons *
	******************************/
	#if KANN
	K_sensory_auron::updateAllSensorAurons();
	#endif

	/*****************************
	* Update all s_sensor_aurons *
	*****************************/
	#if SANN
	s_sensor_auron::updateAllSensorAurons();
	#endif

// XXX Denne sto over #ifKANN og #ifSANN. Tenker at dette er meir rett..
	// Commence scheduled computations:
	doCalculation();



}//}1





/******************************************************************
****** 														*******
****** 			doCalculation() -- samla på en plass 		*******
****** 														*******
******************************************************************/
// TODO Vurder å ta inn double-format tid.
void K_auron::doCalculation()
{ //{
 	DEBUG_L3(<<"K_auron " <<sNavn <<".doCalculation()\t\t" <<sNavn <<".doCalculation()\t\tTid: " <<time_class::getTime() );
 	DEBUG_L4(<<"[K, T] = " <<dAktivitetsVariabel <<", " <<FIRING_THRESHOLD );

	// Lagrer v_0 og t_0 for neste 'time window':
	dDepolAtStartOfTimeWindow = getCalculateDepol(time_class::getTime());
	dStartOfTimeWindow = time_class::getTime();

#if 0 //{
	// Skal gjøre slik at dette er redundant test: DEFINERER AT ALLE AURON OPPDATERES KVAR ITER! TODO TODO
	if( (unsigned)dStartOfTimeWindow < time_class::getTime()+1 ){
		// Viktig å kalkulere depol med GAMMEL Kappa! Ellers får vi hopp i depol!
		// Lagrer v_0 og t_0 for neste 'time window':
		dDepolAtStartOfTimeWindow = getCalculateDepol();
		// TODO Dette må endres når eg begynner med synaptisk input for nodene! Da blir det litt annaleis ?
		//dStartOfTimeWindow = dStartOfNextTimeWindow; 
		dStartOfTimeWindow = time_class::getTime();
	}else{
		// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
		// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
		
		// Tar bort feilene fra 0'te iter:
		if( time_class::getTime() > 0){
			cerr<<"ELSE: dStartOfTimeWindow > TID :\t" <<dStartOfTimeWindow <<" > " <<time_class::getTime() <<"\t\tDette skal aldri skje!\tsleep(5) asdf235@neuroElement.cpp\n";
			sleep(5);
		}
	}
#endif	//}

	// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO 
	// DEPRECATED!
	// Linja under, dAktivitetsVariabel += dChangeInKappa_this_iter SKAL VEKK!
	// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO 

	// Oppdaterer Kappa
	dAktivitetsVariabel += dChangeInKappa_this_iter;
	dChangeInKappa_this_iter = 0;

	// Beregner periode og estimert fyringstid for neste spike:
	estimateFiringTimes(time_class::getTime()); // doCalculation() skjer alltid først i kvar iter. 		id:est490@neuroElement.cpp
	//estimateFiringTimes(); //Bruker ulTime

	// XXX Dette er nytt!
	// FETT! Dette gjør at størrelsen på comp. time step bare gir kor ofte kappa skal oppdateres. Fyring, og init av time window kan skje heilt separat fra dette!
	// Sjekker om den skal fyre denne iter: (før neste iter)
	// dEstimatedTaskTime RUNDES ALLTID NED!! Skal ikkje sjekke om den er over eller under halvegs, bare kva steg den har starta på..

	if( dEstimatedTaskTime < time_class::getTime()+1 ){ //TODO
		DEBUG_L4(<<"K_auron::doCalc()\tK_auron schduled to fire this iteration:\t[TID, dEstimatedTaskTime] :\t[" <<time_class::getTime() <<", " <<dEstimatedTaskTime <<"]");
		time_class::addTaskInPresentTimeIteration( this );
		time_class::printAllElementsOf_pWorkTaskQueue();
	}
}






inline void K_auron::estimateFiringTimes(double dTimeInstant_arg)
{ //{
	if( dAktivitetsVariabel > FIRING_THRESHOLD ){
		static double dPeriodInverse_static_local;

		// Berenger dPeriodINVERSE og dChangeInPeriodINVERSE:
		// dLastCalculatedPeriod gir synaptisk overføring. Perioden er uavhengig av spatiotemporal effekts. Dermed: +A simulerer en refraction time på A tidssteg. ref:asdf5415@neuroElement.cpp
		dLastCalculatedPeriod  = (( log( dAktivitetsVariabel / (dAktivitetsVariabel - (double)FIRING_THRESHOLD) ) / (double)LEAKAGE_CONST)) 	;	//							+1 	
		dPeriodInverse_static_local = 1/dLastCalculatedPeriod;
		dChangeInPeriodINVERSE = dPeriodInverse_static_local - dPeriodINVERSE;
		dPeriodINVERSE = dPeriodInverse_static_local;

		// Beregner v_0 og fyringstid P_d(K)
		DEBUG_L3(
				<<"estimatedPeriod(): \t[t_0, K, v_0]: \t[" <<dStartOfTimeWindow <<", " <<dAktivitetsVariabel <<", " <<dDepolAtStartOfTimeWindow <<"]:\t"
				<<"\t\tand finally getCalculateDepol(" <<dTimeInstant_arg <<") = \e[33;1m" <<getCalculateDepol(dTimeInstant_arg) <<"\e[0;0m\n"
				);

// TODO XXX Denne er diskret tid? Fra starten av denne iter?
		// Oppdaterer dEstimatedTaskTime! 
		dEstimatedTaskTime = ( dStartOfTimeWindow 	+ log( (dAktivitetsVariabel-dDepolAtStartOfTimeWindow)/(dAktivitetsVariabel-(double)FIRING_THRESHOLD) )   / (double)LEAKAGE_CONST );
		//DEBUG_L2(<<"\t\t\e[32mdEstimatedTaskTime\e[0m for neuron " <<sNavn <<", time:" <<dTimeInstant_arg <<" = \e[39;1m" <<dEstimatedTaskTime <<"\e[0;0m");

	}else{ // Kommer hit dersom dAktivitetsVariabel <= FIRING_THRESHOLD
		//cout<<"\n\nadf52908i@neuroElement.cpp \e[33mIkkje implementert ferdig estimatedPeriod(dTime) for K<T\e[0m\n\n";
		//exit(EXIT_FAILURE);

		// Set period (close) to infty.
		dLastCalculatedPeriod  = DBL_MAX;
		dChangeInPeriodINVERSE = -dPeriodINVERSE;
		dPeriodINVERSE = 0;

		// Set estimated firing time to the maximal value(ca. 10^308)
		dEstimatedTaskTime = DBL_MAX;

		DEBUG_L2(
				<<"estimatedPeriod(): \t[t_0, K, v_0, t^f]: \t[" <<dStartOfTimeWindow <<", " <<dAktivitetsVariabel <<", " <<dDepolAtStartOfTimeWindow <<", "
			   	<<dEstimatedTaskTime	<<"]:\t"
				<<"\t\tand finally getCalculateDepol(" <<dTimeInstant_arg <<") = \e[31;0m" <<getCalculateDepol(dTimeInstant_arg) <<"\e[0;0m\n"
				);

		//exit(0);
	} //slutt if( dAktivitetsVariabel > FIRING_THRESHOLD ){X}else{ ... }
} //}

// Gammel variant, uten float-tid: KOMMENTERT UT!
#if 0
inline void K_auron::estimateFiringTimes()
{ //{
	//cout<<"\n\nGAMMEL K_auron::estimateFiringTimes(). IKKJE BRUK!\n[\e[31mTERMINERER\e[0m]\n\n\n\n\n"; exit(0);
	 
	//***********************************************
	//*  Beregn estimert fyringstid:   				*
	//***********************************************
	if( dAktivitetsVariabel > FIRING_THRESHOLD){
		DEBUG_L1(<<"FEIL plass. Ikkje fiksa, heilt..\nasdf234@neuroElement.cpp\nTERMINERER");
		exit(EXIT_FAILURE);

		static double dPeriodInverse_static_local;

		// Berenger dPeriodINVERSE og dChangeInPeriodINVERSE:
		// dLastCalculatedPeriod gir synaptisk overføring. Perioden er uavhengig av spatiotemporal effekts. Dermed: +A simulerer en refraction time på A tidssteg. ref:asdf5415@neuroElement.cpp
		dLastCalculatedPeriod  = (( log( dAktivitetsVariabel / (dAktivitetsVariabel - (double)FIRING_THRESHOLD) ) / (double)LEAKAGE_CONST)) 	;	//							+1 	
		dPeriodInverse_static_local = 1/dLastCalculatedPeriod;
		dChangeInPeriodINVERSE = dPeriodInverse_static_local - dPeriodINVERSE;
		dPeriodINVERSE = dPeriodInverse_static_local;

		double tempDouble = getCalculateDepol(); //For tid: ulTime
		cout<<"estimatedPeriod(): \t[t_0, K, v_0]: \t[" <<dStartOfTimeWindow <<", " <<dAktivitetsVariabel <<", " <<dDepolAtStartOfTimeWindow <<"]:\t\e[1;39m" <<dLastCalculatedPeriod <<"\e[0;0m"
			<<"\t\tog tilslutt getCalculateDepol(ulTime): \e[1;39m" <<tempDouble <<"\e[0;0m\n";

		dEstimatedTaskTime = ( dStartOfTimeWindow 	+ log( (dAktivitetsVariabel-dDepolAtStartOfTimeWindow)/(dAktivitetsVariabel-(double)FIRING_THRESHOLD) )   /  ((double)ALPHA/(double)ulTemporalAccuracyPerSensoryFunctionPeriod));

		// GÅR HER UT IFRA AT Kappa endres kvar iterasjon! Seier at [no] er samme som dStartOfTimeWindow: dette er bare rett dersom vi oppdaterer start of time window kvar iter!
																										
		/*
		* 	Her er eit problem:
		* 		Refraction time: Dersom vi bare legger til en, så vil oppladning starte umiddelbart. Når kappa blir endra, så vil oppladninga (som starter for tidlig) være det einaste som er viktig. Dette skaper problemer.
		* 		XXX XXX XXX
		*       ? ? ? ? ? ? 
		*/

	
		// Propagerer aktivitetsnivå. Gjør umiddelbart doTransmission(). Denne tar hand om spatiotemporale effekter!
		doTransmission();

		// For else-biten når det skjer (sjå ned)
		bAuronHarPropagertAtDenErInaktiv = false;

	}else{
		cout<<"\n\nadf52908i@neuroElement.cpp \e[33mIkkje implementert ferdig estimatedPeriod(dTime) for K<T\e[0m\n\n";

		// Set period (close) to infty.
		dLastCalculatedPeriod  = DBL_MAX;
		dChangeInPeriodINVERSE = -dPeriodINVERSE;
		dPeriodINVERSE = 0;

		// Set estimated firing time to the maximal value(ca. 10^308)
		dEstimatedTaskTime = DBL_MAX;

		//exit(0);



		if(time_class::getTime() == 0){return;}
		cout<<"estimateFiringTimes() når \tK < T   (id: asdf23r4)\t" <<sNavn <<"\n";
		cout<<"\n\nadf52908i@neuroElement.cpp Ikkje implementert estimatedPeriod(dTime) for K<T\n\e[31mAVLUTTER\e[0m\n\n"; 
		exit(EXIT_FAILURE); //XXX MERK! EXIT()!
		//{


		// setter planlagt task time til no, slik at den aldri vil fyre pga. dEstimatedTaskTime. (når den sjekker nest gang, så vil [no] være i fortida..)
		// TODO TODO TODO TODO TODO TODO UTESTET, men å sette den til null(som før) vil ikkje gå bra!
//		dEstimatedTaskTime = 1E99; //(double)time_class::getTime();
		
		// Setter dLastCalculatedPeriod, dChangeInPeriodINVERSE, dPeriodINVERSE.
		dLastCalculatedPeriod = 0; 	// Er dette greit?    SKUMMELT! (men funker).
		dChangeInPeriodINVERSE = -dPeriodINVERSE; 
		dPeriodINVERSE = 0;
		
		#if DEBUG_PRINT_LEVEL > 4
			cout<<"Kappa er mindre enn Tau. Setter dEstimatedTaskTime = [no] (vil ikkje ha noko å sei for fyringa).\n";
			cerr<<"Setter [dPeriodINVERSE, dPeriodInverse_static_local, dChangeInPeriodINVERSE, dLastCalculatedPeriod] til [ "
			 	<<dPeriodINVERSE <<","
				<<dChangeInPeriodINVERSE <<", "
				<<dLastCalculatedPeriod <<" ]\n ";
		#endif
		
		if( !bAuronHarPropagertAtDenErInaktiv )
		{
			// Propagerer resultatet:
			doTransmission();
	 		
			bAuronHarPropagertAtDenErInaktiv = true;
		}
		//}
	}
} //}
#endif
	
/**************************************************************
****** 		K_sensory_auron - update sensor funktions  	******* 			XXX ok XXX
**************************************************************/
void K_sensory_auron::updateAllSensorAurons()
{ //{
	// Iterate through list pAllSensoryAurons and call updateAllSensorAurons() for each element
	for( std::list<K_sensory_auron*>::iterator sensorIter = pAllSensoryAurons.begin(); sensorIter != pAllSensoryAurons.end(); sensorIter++)
	{
		(*sensorIter)->updateSensorValue();
	}
} //}1

inline void K_sensory_auron::updateSensorValue()
{ //{
	DEBUG_L3(<<"K_sensory_auron::updateSensorValue()");

	// Update sensed value. Two variables to find the change in sensed value
	dLastSensedValue = dSensedValue;
	dSensedValue =  (*pSensorFunction)();

	// Save time for initiation of new time window(define this to happen at start of iteration for sensory aurons)
	dStartOfNextTimeWindow = (double)time_class::getTime(); // Set it to [now] before result is computed in changeKappa_derivedArg(-)
	changeKappa_derivedArg(   (dSensedValue-dLastSensedValue) );  
} //}
	

/**************************************************************
****** 		s_sensory_auron - update sensor funktions  	******* 		XXX almost ok XXX
**************************************************************/
inline void s_sensor_auron::updateAllSensorAurons()
{ //{
	// Iterate through list pAllSensoryAurons and call updateAllSensorAurons() for each element
	for( std::list<s_sensor_auron*>::iterator sensorIter = pAllSensoryAurons.begin() 	; 	sensorIter != pAllSensoryAurons.end() ; sensorIter++)
	{
		(*sensorIter)->updateSensorValue();
	}
} //}

inline void s_sensor_auron::updateSensorValue()
{ //{
//	static double sdLastValue = 0;
//	static double sdValue = 0;
//	sdLastValue = sdValue;
//	sdValue = (*pSensorFunction)();
//	pInputDendrite->newInputSignal( ( sdValue ));
	pInputDendrite->newInputSignal( ( (*pSensorFunction)() )); 
} //}


/******************************
**   Recalculate Kappa :     **
******************************/
void recalcKappaClass::doTask()
{ //{

	// Rekalkuler Kappa.
	// Trenger en funksjon K_auron::recalculateKappa() som
	// 	- rekalkulerer kappa for auronet.
	// 	- returnerer rekalkulert kappa.
	double dFeil;

	dFeil = pKappaAuron_obj->recalculateKappa();

	static double sdSecondLastValue =0;
	static double sdLastValue 		=0;
	static double sdValue 			=0;
	static double sdTemp;

	// Beregner når neste oppdatering av recalcKappaClass-kjøring:
	// Med sigmoid-kurve (beskrevet i FDP-rapport, med 3-ledds FIR-effekt)
	sdTemp = sdValue;
	sdValue = ( (RECALC_c1+RECALC_c2) - (RECALC_c2 / ( 1+exp(-(RECALC_c4*dFeil - RECALC_c3)) )) 		+ 	sdLastValue + sdSecondLastValue )  / 3 ;
	sdSecondLastValue = sdLastValue;
	sdLastValue = sdTemp;
	
	#if DEBUG_PRINT_LEVEL > 2
	cout<<pKappaAuron_obj->sNavn <<" - recalcKappaClass::doTask() :\tny periode for recalc kappa: \t" <<sdValue <<"\t\tfor feil: " <<dFeil <<"\t\tTid:\t" <<time_class::getTime() <<endl;
	#endif

	// Schedule recalcKappaClass til å kjøre på kalkulert tid:
	dEstimatedTaskTime = (double)time_class::getTime() + sdValue;

} //}

inline double K_auron::recalculateKappa()
{ //{
	// Plan:
	//  - Rekalkulerer kappa for dendrite.
	// 		- skal hente ut K_ij fra alle innsynapsene. Dette kan den gjøre ved å kalle K_synapse::getWij();
	double dKappa_temp = pInputDendrite->recalculateKappa();
	double dKappaFeil_temp = dAktivitetsVariabel - dKappa_temp;

	// TESTER: Blir bare bullshit! XXX
	// Test på nytt! (skrevet: 22.10.2011)
	double dKappa_derived_temp = 0;
 	for( std::list<K_synapse*>::iterator iter = pInputDendrite->pInnSynapser.begin() ; iter!=pInputDendrite->pInnSynapser.end() ; iter++)
	{
		#if DEBUG_PRINT_LEVEL > 2
		cout<<"Derived Transmission: " <<(*iter)->getDerivedTransmission() <<endl;
		#endif
 		dKappa_derived_temp += (*iter)->getDerivedTransmission();
	}
	// TIL her.

	#if DEBUG_PRINT_LEVEL > 2
	cout<<"[Kappa, dKappa_temp, dKappaFeil_temp] : " <<dAktivitetsVariabel <<", " <<dKappa_temp <<", " <<dKappaFeil_temp <<"\tderived-transmission: " <<dKappa_derived_temp
		<<" => Kappa+transmission = " <<dAktivitetsVariabel+dKappa_derived_temp
		<<endl;
	#endif
	dAktivitetsVariabel = dKappa_temp;


	return dKappaFeil_temp;	
} //}

inline double K_sensory_auron::recalculateKappa()
{
	// In this work, a sensory auron does not receive input from other neurons.

	// TODO No er dette bare en sensor (Har ikkje muligheten for å få input fra andre neuron. Dette kan eg kanskje implementere om eg har tid..)
	#if 0
	static double dOldActivityVariable;
	dOldActivityVariable = dAktivitetsVariabel;
	updateSensorValue();
	// Er dette rett :
	return dAktivitetsVariabel-dOldActivityVariable;
	#endif
	return 0;
}
// SKAL VEKK:
inline double K_dendrite::recalculateKappa()
{ //{
	double dKappa_temp = 0;
 	for( std::list<K_synapse*>::iterator iter = pInnSynapser.begin() ; iter!=pInnSynapser.end() ; iter++)
	{
		#if DEBUG_PRINT_LEVEL > 4
		cout<<"Total Transmission: " <<(*iter)->getTotalTransmission() <<endl;
		#endif
 		dKappa_temp += (*iter)->getTotalTransmission();
	}
	return dKappa_temp;
} //}


/*******************
**     ANNA:      **
*******************/


inline void time_class::addTaskInPresentTimeIteration(timeInterface* pTimeClassArg_withTask)
{
	DEBUG_L3(<<"Inne i time_class::addTaskInPresentTimeIteration(..)");

	// Finner rett plass for nye elementet (sortert etter dEstimatedTaskTime)
	for( std::list<timeInterface*>::iterator iter = pWorkTaskQueue.begin(); iter != pWorkTaskQueue.end(); iter++ )
	{
		//cout<<"\e[35mSJEKKER\e[0m om det er type: time_class*\t[sCName, typeid().name()] = [" <<(*iter)->sClassName <<", " <<typeid(iter).name() <<"]\t\nid_asdf452@neuroElement.cpp\n";

/*********** (BARE UTSKRIFT) ************/
#if 0	//{ Sjekker alle K_auron:
		if((*iter)->sClassName =="K_auron"){
			cout<<"\e[31mdEstimatedTT\e[0m for [(*iter) pArg] = [" <<(*iter)->dEstimatedTaskTime <<", " <<pTimeClassArg_withTask->dEstimatedTaskTime <<"]\t\tAv type ["
				<<(*iter)->sClassName <<" " <<(*iter)->sClassName <<"]\n";
			if(iter == pWorkTaskQueue.end()){
				cout<<"iter == pWorkTaskQueue.end()\n\n";
			}
		}
#endif
		//}
/*********/

		// Dersom iter-elementets dEstimatedTaskTime er etter det nye elementets dEstimatedTaskTime, er dette første oppføring som skal fyre etter (*iter). Legger dermed til elementet før dette.
		if( (*iter)->dEstimatedTaskTime > pTimeClassArg_withTask->dEstimatedTaskTime){
 			DEBUG_L3(<<"insert element in right place in in pWorkTaskQueue(in the present iteration)");
			pWorkTaskQueue.insert(iter, pTimeClassArg_withTask); 	//"insert()" inserts element before [iter].
			return;
		}

		// Sjekker om gjeldende element er av type time_class. Isåfall er vi på siste element, og elementet skal inn før det(insert() sørger for dette).
 		if( (*iter)->sClassName == "time" )
		{
			// Har kommet til siste elementet. Nye elementet skal dermed ligge heilt sist!
			pWorkTaskQueue.insert(iter, pTimeClassArg_withTask); 	//insert legger til elementet før [iter]!
			DEBUG_L3(<<"insert element " <<pTimeClassArg_withTask->sClassName <<" at the end of pWorkTaskQueue!");
			return;
		}
	}

	
	// Foreløpig legger eg den bare til først i denne iter: (etter nåværande elem.) TODO TODO TODO TODO TODO  DET ER FEIL Å GJØRE! Fiks! TODO TODO TODO
	//pWorkTaskQueue.push_front(pTimeClassArg_withTask);
	#if DEBUG_PRINT_LEVEL>4
	printAllElementsOf_pWorkTaskQueue();
	#endif
}






// Delte opp for å kunne skille mellom å propagere kappa, og å fyre AP, for å kunne ha refraction period.
inline void K_auron::doTransmission()
{
	#if DEBUG_PRINT_LEVEL > 3
 	cout<<"K_auron::doTransmission()\tLegger inn alle outputsynapser i arbeidskø. Auron: " <<sNavn <<" - - - - - - - - - - - - - - - \n";
	#endif


	/* TODO Flytt overføringsdelay over hit: fra K_auron::doCalculation() - i ligninga av estimert fyringstid: plusser på en for dendrite og en for axon. 
	Ta vekk den siste, og legg delay inn som element i kvar synapse. Ordner delay her.
	Delay kan dermed være forskjellig for kvar synapse!
	TODO */

	// Legg til alle utsynapser i pWorkTaskQueue:
 	for( std::list<K_synapse*>::iterator iter = pUtSynapser.begin(); iter != pUtSynapser.end(); iter++ )
	{ // Legger alle pUtSynapser inn i time_class::pWorkTaskQueue: (FIFO-kø)
		time_class::addTaskIn_pWorkTaskQueue( *iter );
	}
}



const double K_synapse::getDerivedTransmission()
{
	return pPreNodeAuron->dChangeInPeriodINVERSE * dSynapticWeight;
}
const double K_synapse::getTotalTransmission()
{
	return (1-2*bInhibitoryEffect)*(pPreNodeAuron->dPeriodINVERSE)*dSynapticWeight;
}

inline void K_synapse::print()
{ //{
 	cout<<"Synapse mellom " <<pPreNodeAuron->sNavn <<" og " <<(pPostNodeDendrite->pElementOfAuron)->sNavn 
		<<" med W_ij = " <<dSynapticWeight 
		<<" og presyn. periodeINVERSE: "  <<pPreNodeAuron->dChangeInPeriodINVERSE <<" \t Gir (total) overføring: " <<getTotalTransmission() 
		<<endl;

} //}


// vim:fdm=marker:fmr=//{,//}
