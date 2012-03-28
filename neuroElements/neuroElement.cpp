// It is reccomended to use vim when viewing/editing this file. Vim-folds makes it simpler to browse the document.
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
#include <sstream> 	// For string streams
#include <typeinfo> // To acquire typeid

#include "synapse.h"
#include "auron.h"
#include "../andreKildefiler/main.h"
#include "../andreKildefiler/time.h"

// Declarations
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

	// Iterate through all aurons in i_auron::pAllSensoryAurons, and print each element:
	for( std::list<i_auron*>::iterator iter = i_auron::pAllAurons.begin() ;  iter != i_auron::pAllAurons.end() ;  iter++ )
	{
		// If dEstimatedTaskTime = 0, this variable has not been used, and "---" is written to screen
		// If dEstimatedTaskTime = DBL_MAX, Kappa<Tau and the neuron will never fire. Print "NaN"
		if((*iter)->dEstimatedTaskTime == 0)
			cout<<"\t[ " <<(*iter)->sNavn <<" ]\t\t" 		<<"\tdEstimatedTaskTime :\t" <<"\e[34m---\e[0m" <<endl;
		else if((*iter)->dEstimatedTaskTime == DBL_MAX)
			cout<<"\t[ " <<(*iter)->sNavn <<" ]\t\t" 		<<"\tdEstimatedTaskTime :\t" <<"\e[34mNaN\e[0m" <<endl;
		else
			cout<<"\t[ " <<(*iter)->sNavn <<" ]\t\t" 		<<"\tdEstimatedTaskTime :\t" <<(*iter)->dEstimatedTaskTime <<endl;
	}
	cout<<"\n";

	// First: Call destructor for all K_auron objects:
	K_auron::callDestructorForAllKappaAurons();

	// Then call destructor for all remaining i_aurons 
	// 	 (all s_aurons and also redundancy for K_aurons that for some reason remain undestructed)
	while( ! i_auron::pAllAurons.empty() )
	{
		#if DEBUG_PRINT_LEVEL > 2
		cout<<"Call destructor for auron " <<i_auron::pAllAurons.front()->sNavn <<endl;
		#endif
		// delete (call destructor for) first element. The destructor removes element's pointer from pAllAurons.
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

		// delete first element(call destructor). The destructor removes element's pointer from pAllKappaAurons and also from pAllAurons
	 	delete (*K_auron::pAllKappaAurons.begin());
	}
}
//}2
//{2 *** i_auron   
i_auron::i_auron(std::string sNavn_Arg /*="unnamed"*/, double dStartAktVar /*=0*/) 
	: timeInterface("auron"), dActivityVariable(dStartAktVar), sNavn(sNavn_Arg)
{
	#if LOG_DEPOL
		// Print to log file is set. Initiation of file stream(.oct file that is executable in octave.):
		// 	 (datafiles_for_evaluation is created at the initiation of auronSim. See int main(int,char**) )
		std::ostringstream tempDepolLoggFileAdr;
		tempDepolLoggFileAdr<<"./datafiles_for_evaluation/log_auron_" <<sNavn <<"-depol" <<".oct";
	
		// Need c-style string for open() function:
		std::string tempStr( tempDepolLoggFileAdr.str() );
		depol_logFile.open( tempStr.c_str() );

		// Log important variables as comments at the start of the octave log script:
		depol_logFile<<"# Run with "
					 <<"\n#\tAlpha = \t" <<LEAKAGE_CONST 
					 <<"\n#\tNumber of Iterations = \t" <<ulTemporalAccuracyPerSensoryFunctionPeriod <<"\n\n";
		depol_logFile<<"data=[";
		depol_logFile.flush();

		// Initiation of log file that shows spikes as a vertical line (from 1.05*Tau to 1.2*Tau)
		std::ostringstream tempFilAdr2;
		tempFilAdr2<<"./datafiles_for_evaluation/log_auron_" <<sNavn <<"-depolSPIKES" <<".oct";
	
		// need c-style string for open() function:
		std::string tempStr2( tempFilAdr2.str() );
		actionPotential_depolLogFile.open ( tempStr2.c_str() );

		actionPotential_depolLogFile 	<<"# Run with "
					 					<<"\n#\tAlpha = \t" <<LEAKAGE_CONST 
										<<"\n#\tNumber of Iterations = \t" <<ulTemporalAccuracyPerSensoryFunctionPeriod <<"\n\n";
		actionPotential_depolLogFile<<"data=[";
		actionPotential_depolLogFile.flush();
	#endif

	// Make a log file where the neuron's firing times are logged.
	std::ostringstream tempFilAdr3;
	tempFilAdr3<<"./datafiles_for_evaluation/log_auron_" <<sNavn <<"-firingTimes" <<".oct";


	// need c-style string for open() function:
	std::string tempStr3( tempFilAdr3.str() );
 	actionPotential_logFile.open( tempStr3.c_str() );

	actionPotential_logFile <<"# Run with "
					 		<<"\n#\tAlpha = \t" <<LEAKAGE_CONST 
							<<"\n#\tNumber of Iterations = \t" <<ulTemporalAccuracyPerSensoryFunctionPeriod <<"\n\n";
	actionPotential_logFile <<"data=[";

	actionPotential_logFile.flush();
}

i_auron::~i_auron()
{
	#if DEBUG_PRINT_DESCTRUCTOR
		cout<<"\tDESTRUCTOR: \ti_auron::~i_auron() : \t" <<sNavn <<"\t * * * * * * * * * * * * * * * * * * * * * * * * * \n";
	#endif

	// remove auron from pAllAurons:
	pAllAurons.remove(this);

	#if LOG_DEPOL
		// Finalize octave script to make it executable:
			// depolarization log file:
		depol_logFile 	<<"];\n\n"
						<<"plot( data([1:end],1), data([1:end],2), \"@;Depolarization;\");\n"
						<<"title \"Depolarization for auron " <<sNavn <<"\"\n"
						<<"xlabel Time\n" <<"ylabel \"Activity variable\"\n"
						//<<"axis([0 data(end,1) 0 1400 ]);\n"
						<<"sleep(" <<OCTAVE_SLEEP_AFTER_PLOTTING <<"); "
						;
		depol_logFile.flush();
		depol_logFile.close();
	
			// Action potential log file that represents firing by vertical depolarization line(from 1.05*Tau to 1.2*Tau)
		actionPotential_depolLogFile 	<<"];\n"
										<<"AP_logg=data;\n"
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

	// Set time stamps to [now]
	ulTimestampPreviousFiring = time_class::getTime();
	ulTimestampLastInput  = time_class::getTime();

	// Set initial state:
	dActivityVariable = nStartDepol;
	
	// Insert adress to the static list std::list<i_auron*> pAllAurons;
	i_auron::pAllAurons.push_back( this );

	//pOutputAxon og pInputDendrite
	pOutputAxon = new s_axon(this);
 	pInputDendrite = new s_dendrite(this);
}
s_auron::~s_auron() // ~s_auron is not run at the end of auroSim, only i_auron::~i_auron()
{
	// Only called if a s_auron id destructed before completion of auroSim. 
	// 	 At the completion of auroSim, ~i_auron is called for all elements of i_auron::pAllAurons...
	#if DEBUG_PRINT_DESCTRUCTOR
		cout<<"DESTRUCTOR:\ts_auron::~s_auron()\n";
	#endif
	
	delete pOutputAxon;
	delete pInputDendrite;
} 
//}2
//{2 *** K_auron
K_auron::K_auron(std::string sNavn_Arg /*="unnamed"*/, double dStartKappa_arg /*= 0*/) : i_auron(sNavn_Arg), kappaRecalculator(this)
{ //{
	// for debugging
	sClassName = "K_auron";

	ulTimestampPreviousFiring = time_class::getTime();

	// Insert adress to the static list std::list<i_auron*> pAllAurons;
	i_auron::pAllAurons.push_back( this );
	// Also insert pointer to pAllKappaAurons:
	pAllKappaAurons.push_back( this );
	
	// Push pointer to auron in std::list<timeInterface*> pPeriodicElements:
	time_class::addElementIn_pPeriodicElements( this );

	// Removed output axon, as it is redundant in the KM-simulation. This can also be done for K_dendrite..
	//pOutputAxon = new K_axon(this);
 	pInputDendrite = new K_dendrite(this);

	// Initialize action variable(kappa) to dStartKappa_arg.
	// Call changeKappa_diffArg(double) to initialize new time window normally(inserts element to pCalculatationTaskQueue..): 
	dActivityVariable = dStartKappa_arg;
	changeKappa_diffArg( 0 );

	// Initialize t_0 for first 'time window' (needed by first doCalculation() call)
	dStartOfTimeWindow = (double)time_class::getTime();

	#if LOGG_KAPPA
		// Make kappa-log. Same as other log files: octave-executable log file.
		std::ostringstream tempFilAdr;
		tempFilAdr<<"./datafiles_for_evaluation/log_auron_" <<sNavn <<"-kappa" <<".oct";
		std::string tempStr( tempFilAdr.str() );

		// Need c-style string for the function upen():
		kappa_logFile.open( tempStr.c_str() );
		kappa_logFile<<"data=[" <<time_class::getTime() <<"\t" <<dActivityVariable <<";\n";
		kappa_logFile.flush();
	#endif

	#if DEBUG_PRINT_CONSTRUCTOR
 		cout 	<<"\nConstructor: K_auron.\n"
				<<"\tKappa: \t\t\t\t" <<dActivityVariable <<endl
				<<"\tTau:   \t\t\t\t" <<FIRING_THRESHOLD <<endl
				<<"\n\n";
	#endif

	// Find v(t_0) as usually, with doCalculation():
	doCalculation();
} 
K_auron::~K_auron()
{
	#if DEBUG_PRINT_DESCTRUCTOR
		cout<<"Destructor:\tK_auron::~K_auron()\n";
	#endif
	
	// Element is removed from i_auron::pAllAurons by the i_auron-destructor i_auron::~i_auron()
	// Remove element from pAllKappaAurons:
	pAllKappaAurons.remove(this);

	// Axon and dendrite pointers are to be removed. Currently, the destruction of these elements are done in i_auron::~i_auron()

	// Destruct all out-synapses from this K_auron -- K_axon is removed from this implementation
	while( !pOutputSynapses.empty() ){
		delete (*pOutputSynapses.begin());
		// Call destructor for first remaining synapse. This removes synapse from pOutputSynapses until no remains..
	   	//Kaller destruktoren til første gjenværende synapse. Dette fører også til at synapsa fjærnes fra pOutputSynapses (og dendrite.pInputSynapses)
	}

	// Remember that pInputDendrite needs to be removed AFTER all pOutputSynapses
	delete pInputDendrite; // The variable pInputDendrite is to be removed! (not necessary in K_auron)

	// Finalize kappa-log, so that it is executable as an octave script:
	// 	(other log files are finalized in i_auron::~i_auron() )
	#if LOGG_KAPPA
		kappa_logFile 	<<time_class::getTime() <<"\t" <<dActivityVariable <<"];\n"
						<<"axis([0," <<time_class::getTime() <<"])\n"
						<<"plot( data([1:end],1), data([1:end],2), \"@;Kappa;\");\n"

						<<"title \"Activity variable for K-auron " <<sNavn <<"\"\n"
						<<"xlabel Time\n" <<"ylabel \"Activity variable\"\n"
						//<<axis([0 data(end,1) 0 1400 ]);\n"
						<<"sleep(" <<OCTAVE_SLEEP_AFTER_PLOTTING <<"); "
						;
		kappa_logFile.close();
	#endif

}
//}2
//{2 *** K_sensory_auron
// Set initial K-value to the return from (*pFunk_arg)(). 
K_sensory_auron::K_sensory_auron( std::string sName_Arg , const double& (*pFunk_arg)(void) ) : K_auron(sName_Arg, (*pFunk_arg)() )
{
	// Assign the sensor function to the funtion pointer pSensorFunction:
	pSensorFunction = pFunk_arg;
	// Add to pAllSensoryAurons list:
	pAllSensoryAurons.push_back(this);

	// Sample first value from pSensorFunction: save to dActivityVariable:
	dActivityVariable = (*pSensorFunction)();
	// Because sensed input is inserted into K_dendrite::newInputSignal(double), dActivityVariable is also saved to dSensedValue
	dSensedValue = dActivityVariable;

	// Initialize kappaRecalculator(in case auron later is implemented with input synapses)
	kappaRecalculator.pKappaAuron_obj = this;
	recalculateKappa();
}
//}2
//{2 *** s_sensory_auron
s_sensory_auron::s_sensory_auron( std::string sNavn_Arg , const double& (*pFunk_arg)(void) ) : s_auron(sNavn_Arg)
{
	// Assign the sensor function:
	pSensorFunction = pFunk_arg;
	// Add to pAllSensoryAurons-list:
	pAllSensoryAurons.push_back(this);
}
//}2
//}1 * AURON
//{1 * SYNAPSE
//{2 i_synapse
i_synapse::i_synapse(double dSynVekt_Arg, bool bInhibEffekt_Arg, std::string sKlasseNavn /*="synapse"*/ )
	: timeInterface(sKlasseNavn), bInhibitoryEffect(bInhibEffekt_Arg)
{
	dSynapticWeight = abs(dSynVekt_Arg);
	
	#if DEBUG_PRINT_CONSTRUCTOR
		cout<<"\tConstructor :\ti_synapse::i_synapse(unsigned uSynVekt_Arg, bool bInhibEffekt_Arg, string navn);\n";
	#endif
}
//}2
//{2 s_synapse
s_synapse::s_synapse(s_auron* pPresynAuron_arg, s_auron* pPostsynAuron_arg, double dSynVekt_Arg /*=1*/, bool bInhibEffekt_Arg /*=false*/) 
			:  i_synapse(dSynVekt_Arg, bInhibEffekt_Arg, "s_synapse"), 
				pPreNodeAxon(pPresynAuron_arg->pOutputAxon), 
				pPostNodeDendrite(pPostsynAuron_arg->pInputDendrite) 
{

	#if DEBUG_PRINT_CONSTRUCTOR
		cerr<<"Call s_synapse::s_synapse("  <<pPreNodeAxon->pElementOfAuron->sNavn 
			<<".pOutputAxon, " <<pPostNodeDendrite->pElementOfAuron->sNavn <<".pInputDendrite, ...)\n";
	#endif

	// Insert synapse as output synapse in presyn. axon and input synapse in postsyn. dendrite:
	pPreNodeAxon->pOutputSynapses.push_back(this);
	pPostNodeDendrite->pInputSynapses.push_back(this);
	
	// Initialize synapse's log-file:
	//{4 make a .oct - file, and make it ready for execution in octave:
	std::ostringstream tempFilAdr;
	tempFilAdr<<"./datafiles_for_evaluation/log_s_synapse_" <<pPresynAuron_arg->sNavn <<"-"  <<pPostsynAuron_arg->sNavn ;
	if(bInhibitoryEffect){ tempFilAdr<<"_inhi"; }
	else{ 			  tempFilAdr<<"_eksi"; }
	tempFilAdr<<".oct";

	// Need c-style string for open():
	std::string tempStr( tempFilAdr.str() );
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
		// Remove [this] pointer from prenode-axon:
		for( std::list<s_synapse*>::iterator iter = (pPreNodeAxon->pOutputSynapses).begin(); 
				iter != (pPreNodeAxon->pOutputSynapses).end() ; iter++ ){
			if( *iter == this ){
				#if DEBUG_PRINT_DESCTRUCTOR
					cout<<"\t~( [" <<pPreNodeAxon->pElementOfAuron->sNavn <<"] -> "; 						// Printing part 1
				#endif
				(pPreNodeAxon->pOutputSynapses).erase( iter );
				bPreOk = true;
				break;
			}
		}
	}
	if( !bPostOk ){
		// Remove [this] pointer from postnode-dendrite:
		for( std::list<s_synapse*>::iterator iter = pPostNodeDendrite->pInputSynapses.begin();
			   	iter != pPostNodeDendrite->pInputSynapses.end() ; iter++ ){
			if( *iter == this ){ 
				#if DEBUG_PRINT_DESCTRUCTOR
					cout<<"[" <<pPostNodeDendrite->pElementOfAuron->sNavn <<"] )\t"; 						// Printing part 2
				#endif
				(pPostNodeDendrite->pInputSynapses).erase( iter );
				bPostOk = true;
				break;
			}
		}
	}

	// Never happens. Redundant error test to be certain (this function is called so seldomly that this will not affect run time)
	if( (!bPreOk) || (!bPostOk) ){
		/// ERROR!!!:
		std::cerr<<"\n\n\n\n\nERROR ERROR ERROR\nat neuroEnhet.cpp id:asdf250@neuronElement.cpp."
				<<"Error in s_synapse destruction\n"
				<<"bPreOk (" <<pPreNodeAxon->pElementOfAuron->sNavn <<"):" <<bPreOk 
				<<"  ->  bPostOk (" <<pPostNodeDendrite->pElementOfAuron->sNavn <<"): " <<bPostOk 
				<<"\nNot deleted from the corresponding neuron if it has the value \"0\"\n"
				<<"TERMINATING";
		exit(EXIT_FAILURE);	
	}
	cout<<endl;


	// Finalize transmission log:
	synTransmission_logFile<<"];\n"
					<<"plot( data([1:end],1), data([1:end],2), \";Synaptic weight;\");\n"
					<<"title \"Synaptic weight for s_synapse: " <<pPreNodeAxon->pElementOfAuron->sNavn 
					<<" -> " <<pPostNodeDendrite->pElementOfAuron->sNavn <<"\"\n"
					<<"xlabel Time\n" <<"ylabel syn.w.\n"
					<<"sleep(" <<OCTAVE_SLEEP_AFTER_PLOTTING <<"); ";
	synTransmission_logFile.close();
}
//}2
//{2 K_synapse
K_synapse::K_synapse(K_auron* pPresynAuron_arg, K_auron* pPostsynAuron_arg, double dSynVekt_Arg /*=1*/, bool bInhibEffekt_Arg /*=false*/ )
 :  i_synapse(dSynVekt_Arg      , bInhibEffekt_Arg, "K_synapse")
   	, pPreNodeAuron(pPresynAuron_arg), pPostNodeDendrite(pPostsynAuron_arg->pInputDendrite)
{
	#if DEBUG_PRINT_CONSTRUCTOR
		cout<<"Constructor :\tK_synapse::K_synapse(" <<pPreNodeAuron->sNavn <<", " <<pPostNodeDendrite->pElementOfAuron->sNavn
		   	<<".pInputDendrite, ...)\n";
	#endif


	// Plan: make K_auron::newOutputSyn(double) that can create spatiotemporal delay to the synapose (dTemporalDistanceFromSoma).
	// 	 so that this is the delay before transmission in that synapse.
	// For this project, the synapse's pointer is inserted into the single axon compartment:
	pPreNodeAuron->pOutputSynapses.push_back(this);

	// TODO: For KM-node: remove axon AND dendrite!
	//pPreNodeAxon->pOutputSynapses.push_back(this); //OBSOLETE
	pPostNodeDendrite->pInputSynapses.push_back(this);

	// Create log file stream for logging synaptic transmissions:
	// 	The name of the log either ends on inhibitory or excitatory to make it possible to log more than one synapse from j to i.
	std::ostringstream tempFilAdr;
	tempFilAdr<<"./datafiles_for_evaluation/log_transmission_K_synapse_" <<pPresynAuron_arg->sNavn <<"-"  <<pPostsynAuron_arg->sNavn ;
	if(bInhibitoryEffect){ tempFilAdr<<"_inhibitory"; }
	else{ 			  tempFilAdr<<"_excitatory"; }
	tempFilAdr<<".oct";

	// Need c-style string for open():
	std::string tempStr( tempFilAdr.str() );
	synTransmission_logFile.open( tempStr.c_str() );

	synTransmission_logFile<<"data=[0 0;\n";
	synTransmission_logFile.flush();


	// Initialize connection by doing the first transmission:
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
		// Remove [this] pointer from prenode:
		for( std::list<K_synapse*>::iterator iter = pPreNodeAuron->pOutputSynapses.begin(); 
				iter != pPreNodeAuron->pOutputSynapses.end() ; iter++ ){
			if( *iter == this ){
				#if DEBUG_PRINT_DESCTRUCTOR
				cout<<"\t~( [" <<pPreNodeAuron->sNavn <<"] -> "; 				 // Print to screen, part 1
				#endif
				(pPreNodeAuron->pOutputSynapses).erase( iter );
				bPreOk = true;
				break;
			}
		}
	}
	if( !bPostOk ){
		// Remove [this] pointer from postnode:
		for( std::list<K_synapse*>::iterator iter = pPostNodeDendrite->pInputSynapses.begin(); 
				iter != pPostNodeDendrite->pInputSynapses.end() ; iter++ ){
			if( *iter == this ){ 
				#if DEBUG_PRINT_DESCTRUCTOR
				cout<<"[" <<pPostNodeDendrite->pElementOfAuron->sNavn <<"] )\t"; // Print to screen, part 2
				#endif
				(pPostNodeDendrite->pInputSynapses).erase( iter );
				bPostOk = true;
				break;
			}
		}
	}
	// Redundant test: Will "never" happen.
	// 	Because this destructor is called so seldomly, the next test will not affect total run time of the simulation.
	if( (!bPreOk) || (!bPostOk) ){
		/// ERROR:
		std::cerr<<"\n\n\n\n\nERROR ERROR ERROR\nIn neuroElement.cpp id#asdf250@neuroElement.cpp\n"
				<<"bPreOk (" <<pPreNodeAuron->sNavn <<"):" <<bPreOk <<"  ->  bPostOk ("
			   	<<pPostNodeDendrite->pElementOfAuron->sNavn <<"): " <<bPostOk 
				<<"\nOk unless value is zero..\nTERMINATING\n\n";
		exit(-9);	
	}
	cout<<endl;

	// Finalize filestream: synTransmission_logFile
	// 	(make log script executable in octave)
	synTransmission_logFile<<"];\n"
					<<"plot( data([1:end],1), data([1:end],2), \".r;Synaptic transmission;\");\n"
					<<"title \"Synaptic transmission from s_synapse: " <<pPreNodeAuron->sNavn 
					<<" to " <<pPostNodeDendrite->pElementOfAuron->sNavn <<"\"\n"
					<<"xlabel Time\n" <<"ylabel Syn.Transmission\n"
					//<<"print ./eps/eps_transmission_" <<pPreNodeAuron->sNavn <<"->" 
					// 			<<pPostNodeDendrite->pElementOfAuron->sNavn <<".eps -deps -color\n"
					<<"sleep(" <<OCTAVE_SLEEP_AFTER_PLOTTING <<"); ";
	synTransmission_logFile.close();
}
//}2
//}1 * SYNAPSE
//{1 * AXON
//{2 ***  i_axon
i_axon::i_axon( std::string sKlasseNavn /*="axon"*/) : timeInterface(sKlasseNavn)
{
}
i_axon::~i_axon()
{
	// pOutputSynapses contains pointers only: pOutputSynapses.clear() will therefore not detroy objects.
	// Call destructor of objects explicitly:
	while( !pOutputSynapses.empty() ){
		// Call destructor for first remaining synapse. 
		// 	 - this also removes its pointer from pOutputSynapses (and dendrite::pInputSynapses)
		delete (*pOutputSynapses.begin()); 
	}
}
//}2 *** i_axon
//{2 ***  s_axon
s_axon::s_axon(s_auron* pAuronArg) : i_axon("s_axon"), pElementOfAuron(pAuronArg)
{
	#if DEBUG_PRINT_CONSTRUCTOR
		cout<<"\tConstructor :\ts_axon::s_axon(s_auron*)\n";
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
i_dendrite::i_dendrite( std::string sNavn /*="dendrite"*/) : timeInterface(sNavn)
{}
i_dendrite::~i_dendrite()
{
	// Deconstruct all input synapses:
	while( !pInputSynapses.empty() ){
		// delete-operator on the deallocated synapse pointer. Synapse's destructor removes pointers to it.
	 	delete (*pInputSynapses.begin() );
	}
}
//}2
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
// TODO Remove K_dendrite entirely and make multible compartment simulation with single compartment implementation!
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
	// Destruct synapses by deleting the deallocated pointer
	while( !pInputSynapses.empty() ){
		#if DEBUG_PRINT_DESCTRUCTOR
			cerr<<"DEBUG ~K_dendrite: \tDestruerer neste synapse\n";
		#endif
	 	delete (*pInputSynapses.begin() );
	}
}
//}2 K_dendrite
//}1 X DENDRITE



/*************************************************************
****** 													******
******  		OTHER FUNCTIONS :           			******
****** 													******
*************************************************************/

//{1  * KANN
// newInputSignal(double) - new edge transmission (as the differential of the synaptic transmission)
inline void K_dendrite::newInputSignal( double dNewSignal_arg )
{ //{2
	// Save time of initiation for the next time window, and update kappa: 
	pElementOfAuron->dStartOfTimeWindow = (double)time_class::getTime();
	pElementOfAuron->changeKappa_diffArg( dNewSignal_arg );
} //}2

inline void K_auron::changeKappa_diffArg( const double& dInputDerived_arg)//int derivedInput_arg )
{
	// Arg is added to Kappa, but the effect of a changed Kappa is computed in .doCalculation()
	// 	 This is done by having a variable that represents $\Delta \kappa$, 
	// 		and letting this variable be added to $\kappa$ by .doCalculation()
	dChangeInKappa_this_iter +=  dInputDerived_arg ;

	// Insert element into pCalculatationTaskQueue, so that its .doCalculation() function is executed after this iteration.
	time_class::addCalculationIn_pCalculatationTaskQueue( this );

	#if LOGG_KAPPA
	writeKappaToLog();
	#endif

	DEBUG_L3( <<sNavn <<"\t:\tTid:\t" <<time_class::getTime() <<" ,\tKappa :\t" <<dActivityVariable );
}
//}1

//{1  * SANN

inline void s_dendrite::newInputSignal( double dNewSignal_arg )
{ //{2 .. }

	// Check whether input is blocked due to absolute refraction period-simulation:
	// 	Commented, to remove potential error source in comparison. Refraction interval is fundamentally different between the two models.
 	// if( bBlockInput_refractionTime ) return;

	// Compute leakage of depolarization since last time:
	calculateLeakage();

	// Add new input signal, scaled by the leakage const(1-leakage) to get the right input.
	pElementOfAuron->dActivityVariable +=  dNewSignal_arg * LEAKAGE_CONST;  
	// The correct way of doing this is to divide by alpha in K_dendrite::newInputSignal() and do nothing here.
	// Because nothing is done in K_dendrite::newInputSignal() and multiplication here, the result is the same..
	// Have not changed formula yet: TODO

	#if DEBUG_PRINT_LEVEL > 2
	cout<<time_class::getTime() <<"\ts_dendrite::newInputSignal( " <<dNewSignal_arg <<" ); \t\tives depol. :  " 
		<<pElementOfAuron->dActivityVariable <<"\n";
	#endif
	
	pElementOfAuron->ulTimestampLastInput = time_class::getTime();

	// Check if depolarization goes to suprathreshold levels. Fire A.P. if it does:
	if( pElementOfAuron->dActivityVariable > FIRING_THRESHOLD )
	{
		// Block input due to absolute refraction time: (commented)
		//bBlockInput_refractionTime = true;

		// Simulation of spatioteporal delay from AP initiation at axon hillock:
		time_class::addTaskIn_pWorkTaskQueue( pElementOfAuron );

		// Reset v(t) now, in addition to when the neuron fires to avoid re-firing due to more input this iteration.
		pElementOfAuron->dActivityVariable = 0;
	}

	// Write depolarization to log when firing:
	//pElementOfAuron->writeDepolToLog();
} //}2

//TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO HER ER EG TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO 
inline void s_dendrite::calculateLeakage()
{ //{2 /** Only for SANN:  ***/

	if( pElementOfAuron->ulTimestampLastInput != time_class::getTime() )
	{
		// regner ut, og trekker fra lekkasje av depol til postsyn neuron.
	 	pElementOfAuron->dActivityVariable *= pow( (double)(1-LEAKAGE_CONST), (double)(time_class::getTime() - pElementOfAuron->ulTimestampLastInput) );
	
		pElementOfAuron->writeDepolToLog();
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

	if( ulTimestampPreviousFiring == time_class::getTime() )
	{
		DEBUG_L1(<<"\n\n************************\nFeil?\nTo fyringer på en iterasjon? \nFeilmelding au#103 @ auron.h\n************************\n\n");
		return;
	}

 	#if DEBUG_PRINT_LEVEL>0
	cout<<"\t* \e[34ms\e[0m *\t\e[4;32mAction Potential\e[0;0m for neuron " <<sNavn <<"\t\t\t\t[time step] = [\e[1;33m" <<time_class::getTime() <<"\e[0m]\n";
 	#endif
	// FYRER A.P.:
//	#if DEBUG_PRINT_LEVEL > 1
//		cout	<<"\tS S " <<sNavn <<" | S | " <<sNavn <<" | S | S | S | | " <<sNavn <<" | S | | " <<sNavn <<" | S | | " <<sNavn <<"| S | S | S | S |\t"
//				<<sNavn <<".doTask()\t\e[33mFYRER\e[0m neuron " <<sNavn <<".\t\t| S S | \t  | S |  \t  | S |\t\tS | " <<time_class::getTime() <<" |\n";
//	#endif

	//Axon hillock: send aksjonspotensial 	-- innkapsling gir at a xon skal ta hånd om all output:
	// bestiller at a xon skal fyre NESTE tidsiterasjon. Simulerer tidsdelay i a xonet.
	time_class::addTaskIn_pWorkTaskQueue( pOutputAxon );


	// Registrerer fyringstid (for feisjekk (over) osv.) 
	ulTimestampPreviousFiring = time_class::getTime();


	//Resetter depol.verdi 
	dActivityVariable = 0; 

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
 	for( std::list<s_synapse*>::iterator iter = pOutputSynapses.begin(); iter != pOutputSynapses.end(); iter++ )
	{ // Legger alle pOutputSynapses inn i time_class::pWorkTaskQueue: (FIFO-kø)
		time_class::addTaskIn_pWorkTaskQueue( *iter );
	}

	 //Skriver til logg etter refraction-period.
	 //pElementOfAuron->writeDepolToLog();
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
#if DEBUG_PRINT_LEVEL>2
		cout<<"\t* \e[35mK\e[0m *\t\e[4;32mAction Potential\e[0;0m for neuron " <<sNavn <<"\t\t\t\t[time step, est.t.time] = [" 
			<<time_class::getTime() <<"\e[0m,\e[32m " <<dEstimatedTaskTime <<"\e[0m]\n";
#else
		cout<<"\t* \e[35mK\e[0m *\t\e[4;32mAction Potential\e[0;0m for neuron " <<sNavn <<"\t\t\t\t[time step] = ["
		   	<<time_class::getTime() <<"\e[0m]\n";
#endif
	}else if( dEstimatedTaskTime < time_class::getTime() ) //Dersom dEstimatedTaskTime er < enn nå-tid: at den skulle fyre forrige iter..
	{
		// Error to fire at this time:
		#if DEBUG_PRINT_LEVEL>0
		cout<<"\t* * *\t\e[4;32mFIRING\e[31m ERROR\e[0;0m neuron " <<sNavn <<"\t\t\t\t\t[time > (int)est.time]: [\e[1;33m" <<time_class::getTime() <<"\e[0m>\e[1;31m " <<dEstimatedTaskTime <<"\e[0m]\n";
		#endif
	}else //if( dEstimatedTaskTime > time_class::getTime()+1 ) //Dersom dEstimatedTaskTime < nå-tid: at den skulle fyre forrige iter..
	{
		cout<<"\t* * *\t\e[4;32mFIRING\e[31m ERROR\e[0;0m neuron " <<sNavn <<"\t\t\t\t\t[est.t.time != [now] : [tid, (int)est.tid] = [\e[1;33m" <<time_class::getTime() <<"\e[0m<\e[1;31m " <<dEstimatedTaskTime <<"\e[0m]\n";
		DEBUG_L2( <<"Depol.:\t" <<getCalculateDepol());
		exit(2); // XXX Terminate
	}



	//Utskrift til skjerm:
	DEBUG_L2(
			<<sNavn <<".doTask()" <<"\t[t=" <<time_class::getTime() <<" est.tt:" <<dEstimatedTaskTime <<"]\t[v(t),v(t_n)=[\e[01;31m" 
			<<getCalculateDepol(dEstimatedTaskTime) <<"\e[0m, " <<getCalculateDepol()
			<<"\t | K=" <<dActivityVariable  <<"\tSiste v_0:" <<dDepolAtStartOfTimeWindow 
			);


	#if LOG_DEPOL
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
	// 	(Set dStartOfTimeWindow to dEstimatedTaskTime)
	dDepolAtStartOfTimeWindow = 0; 
	dStartOfTimeWindow = dEstimatedTaskTime; // IMPORTANT: use dEstimatedTaskTime instead of dStartOfNextTimeWindow for firing. 

	// Save time of firing: When the period is recalculated, the exact start of time window is imporant. 
	// 	- Do not use ulTime because it is always less than the exact firing time: ln(-X) -> \infty
	dLastFiringTime = dEstimatedTaskTime;

	// It is important that the esimatedPeriod is computed after dEstimatedTaskTime += dLastCalculatedPeriod !
	// 	If I run estimateFiringTimes() or doCalculation() before, a large error is the result.

	// Logger AP (vertikal strek)
	writeAPtoLog(); //Viktig: Må gjøres etter at dLastFiringTime er skrevet til!

	// Pga. nye mekansimen som gjør det mulig å fyre denne iter: (estimateFiringTimes(double) oppdaterer dEstimatedTaskTime)
	// TODO Trur ikkje denne gjør noko spesiellt: samme som estimateFiringTimes(void)! XXX Sjekk og fiks! TODO
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
	// Increase by one (ulTime+1) to show t_n for the new time step.
	if(ulTime % uNumberOfIterationsBetweenPrintToScreen  == 0)		
		cout<<"\e[33m"	<<ulTime <<"\e[0m" <<"\t TIME \t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - = "<<ulTime <<"\n";
	#if DEBUG_PRINT_LEVEL>3
	if(ulTime % PRINT_TIME_EVERY_Nth_ITER  == 0)		
		cout<<"\n\t- - - In time_class::doTask() - Increase time from :   - - - - = \e[4;39m"	<<ulTime <<" -> " <<ulTime+1 <<"\e[0m :\n";
	#endif
	#endif


	// Push self-pointer to back of pWorkTaskQueue:
	pWorkTaskQueue.push_back(this);	


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
	* Update all s_sensory_aurons *
	*****************************/
	#if SANN
	s_sensory_auron::updateAllSensorAurons();
	#endif


// XXX Denne sto over #ifKANN og #ifSANN. Tenker at dette er meir rett..
	// Commence scheduled computations:
	doCalculation();

	// Iterate t_n:
	ulTime++;
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
 	DEBUG_L4(<<"[K, T] = " <<dActivityVariable <<", " <<FIRING_THRESHOLD );

	// Lagrer v_0 og t_0 for neste 'time window':
	//dDepolAtStartOfTimeWindow = getCalculateDepol(time_class::getTime());
	dDepolAtStartOfTimeWindow = getCalculateDepol();
	dStartOfTimeWindow = time_class::getTime();

#if 0 //{ UTKOMMENTERT
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
	// Linja under, dActivityVariable += dChangeInKappa_this_iter SKAL VEKK!
	// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO 

	// Oppdaterer Kappa
	dActivityVariable += dChangeInKappa_this_iter;
	dChangeInKappa_this_iter = 0;

	// Beregner periode og estimert fyringstid for neste spike:
	//estimateFiringTimes(time_class::getTime()); // doCalculation() skjer alltid først i kvar iter. 		id:est490@neuroElement.cpp
	estimateFiringTimes(); //Bruker ulTime siden dette skjer først, kvar iter..

	writeDepolToLog();

	// XXX Dette er nytt!
	// FETT! Dette gjør at størrelsen på comp. time step bare gir kor ofte kappa skal oppdateres. Fyring, og init av time window kan skje heilt separat fra dette!
	// Sjekker om den skal fyre denne iter: (før neste iter)
	// dEstimatedTaskTime RUNDES ALLTID NED!! Skal ikkje sjekke om den er over eller under halvegs, bare kva steg den har starta på..

	// Hugs at eg ser på neste iter: dEstimatedTaskTime < t+2 istadenfor +1, skriver derfor ut [TID, ..]=[getTime()+1,..]
	if( dEstimatedTaskTime < time_class::getTime()+2 ){ //TODO
		DEBUG_L2(<<"K_auron::doCalc()\tK_auron schduled to fire this iteration:\n\t\t[TID, dEstimatedTaskTime] :\t[" 
				 <<time_class::getTime()+1 <<", " <<dEstimatedTaskTime <<"]");
		time_class::addTaskInPresentTimeIteration( this );
//cerr<<"K_auron::doCalc(): Legger til fyring\t" <<"\t[TID, dEstimatedTaskTime] :\t[" <<time_class::getTime()	<<", " <<dEstimatedTaskTime <<"]\n";
	}
}






inline void K_auron::estimateFiringTimes(const double& dTimeInstant_arg)
{ //{
	if( dActivityVariable > FIRING_THRESHOLD ){

		// Berenger dPeriodINVERSE og dChangeInPeriodINVERSE:
		// dLastCalculatedPeriod gir synaptisk overføring. Perioden er uavhengig av spatiotemporal effekts. Dermed: +A simulerer en refraction time på A tidssteg. ref:asdf5415@neuroElement.cpp
		dLastCalculatedPeriod  = (( log( dActivityVariable / (dActivityVariable - (double)FIRING_THRESHOLD) ) / (double)LEAKAGE_CONST)) 	;	//							+1 	
#if 0 // kommenterer ut 13.03 Vettafaen om eg trenger det..
		static double dPeriodInverse_static_local;

		dPeriodInverse_static_local = 1/dLastCalculatedPeriod;
		dChangeInPeriodINVERSE = dPeriodInverse_static_local - dPeriodINVERSE;
		dPeriodINVERSE = dPeriodInverse_static_local;

		// Beregner v_0 og fyringstid P_d(K)
		DEBUG_L3(
				<<"estimatedPeriod(): \t[t_0, K, v_0]: \t[" <<dStartOfTimeWindow <<", " <<dActivityVariable <<", " <<dDepolAtStartOfTimeWindow <<"]:\t"
				<<"\t\tand finally getCalculateDepol(" <<dTimeInstant_arg <<") = \e[33;1m" <<getCalculateDepol(dTimeInstant_arg) <<"\e[0;0m\n"
				);
#endif

// TODO XXX Denne er diskret tid? Fra starten av denne iter?
		// Oppdaterer dEstimatedTaskTime! 
		dEstimatedTaskTime = ( dStartOfTimeWindow 	+ log( (dActivityVariable-dDepolAtStartOfTimeWindow)/(dActivityVariable-(double)FIRING_THRESHOLD) )   / (double)LEAKAGE_CONST );
		//DEBUG_L2(<<"\t\t\e[32mdEstimatedTaskTime\e[0m for neuron " <<sNavn <<", time:" <<dTimeInstant_arg <<" = \e[39;1m" <<dEstimatedTaskTime <<"\e[0;0m");

	}else{ // Kommer hit dersom dActivityVariable <= FIRING_THRESHOLD
		//cout<<"\n\nadf52908i@neuroElement.cpp \e[33mIkkje implementert ferdig estimatedPeriod(dTime) for K<T\e[0m\n\n";
		//exit(EXIT_FAILURE);

		// Set period (close) to infty.
		dLastCalculatedPeriod  = DBL_MAX;
		dEstimatedTaskTime = DBL_MAX;
#if 0 // kommenterer ut 13.03 Trur ikkje eg trenger det til rapporten (først nyttig når syn.trans)
		dChangeInPeriodINVERSE = -dPeriodINVERSE;
		dPeriodINVERSE = 0;

		// Set estimated firing time to the maximal value(ca. 10^308)
		dEstimatedTaskTime = DBL_MAX;

		DEBUG_L2(
				<<"estimatedPeriod(): \t[t_0, K, v_0, t^f]: \t[" <<dStartOfTimeWindow <<", " <<dActivityVariable <<", " <<dDepolAtStartOfTimeWindow <<", "
			   	<<dEstimatedTaskTime	<<"]:\t"
				<<"\t\tand finally getCalculateDepol(" <<dTimeInstant_arg <<") = \e[31;0m" <<getCalculateDepol(dTimeInstant_arg) <<"\e[0;0m\n"
				);
#endif

		//exit(0);
	} //slutt if( dActivityVariable > FIRING_THRESHOLD ){X}else{ ... }
} //}

// Ny variant, uten float-tid:
inline void K_auron::estimateFiringTimes()
{ //{

	if( dActivityVariable < FIRING_THRESHOLD ){
		DEBUG_L3(
				<<"\n\nadf52908i@neuroElement.cpp \e[33mIkkje implementert ferdig\e[0m estimatedPeriod(dTime) for K<T\n\n"
				);
		// Set isi-period to max:
		dLastCalculatedPeriod  = DBL_MAX;
		dEstimatedTaskTime = DBL_MAX;
	}else{
		dLastCalculatedPeriod =((log( dActivityVariable / (dActivityVariable - (double)FIRING_THRESHOLD) ) / (double)LEAKAGE_CONST));	//							+1 	
		dEstimatedTaskTime = ( dStartOfTimeWindow 	+ log( (dActivityVariable-dDepolAtStartOfTimeWindow)/(dActivityVariable 
														- (double)FIRING_THRESHOLD) )   / (double)LEAKAGE_CONST );
	}

#if 0
	if( dActivityVariable > FIRING_THRESHOLD ){

		// Berenger dPeriodINVERSE og dChangeInPeriodINVERSE:
		// dLastCalculatedPeriod gir synaptisk overføring. Perioden er uavhengig av spatiotemporal effekts. Dermed: +A simulerer en refraction time på A tidssteg. ref:asdf5415@neuroElement.cpp
		dLastCalculatedPeriod  = (( log( dActivityVariable / (dActivityVariable - (double)FIRING_THRESHOLD) ) / (double)LEAKAGE_CONST)) 	;	//							+1 	
#if 0 // kommenterer ut 13.03 Vettafaen om eg trenger det..
		static double dPeriodInverse_static_local;

		dPeriodInverse_static_local = 1/dLastCalculatedPeriod;
		dChangeInPeriodINVERSE = dPeriodInverse_static_local - dPeriodINVERSE;
		dPeriodINVERSE = dPeriodInverse_static_local;

		// Beregner v_0 og fyringstid P_d(K)
		DEBUG_L3(
				<<"estimatedPeriod(): \t[t_0, K, v_0]: \t[" <<dStartOfTimeWindow <<", " <<dActivityVariable <<", " <<dDepolAtStartOfTimeWindow <<"]:\t"
				<<"\t\tand finally getCalculateDepol(" <<time_class::getTime() <<") = \e[33;1m" <<getCalculateDepol(time_class::getTime()) <<"\e[0;0m\n"
				);
#endif

// TODO XXX Denne er diskret tid? Fra starten av denne iter?
		// Oppdaterer dEstimatedTaskTime! 
		dEstimatedTaskTime = ( dStartOfTimeWindow 	+ log( (dActivityVariable-dDepolAtStartOfTimeWindow)/(dActivityVariable-(double)FIRING_THRESHOLD) )   / (double)LEAKAGE_CONST );
		//DEBUG_L2(<<"\t\t\e[32mdEstimatedTaskTime\e[0m for neuron " <<sNavn <<", time:" <<time_class::getTime() <<" = \e[39;1m" <<dEstimatedTaskTime <<"\e[0;0m");

	}else{ // Kommer hit dersom dActivityVariable <= FIRING_THRESHOLD
		DEBUG_L2(
				<<"\n\nadf52908i@neuroElement.cpp \e[33mIkkje implementert ferdig\e[0m estimatedPeriod(dTime) for K<T\n\n"
				);
		//exit(EXIT_FAILURE);

		// Set period (close) to infty.
		dLastCalculatedPeriod  = DBL_MAX;
		dEstimatedTaskTime = DBL_MAX;
#if 0 // kommenterer ut 13.03 Trur ikkje eg trenger det til rapporten (først nyttig når syn.trans)
		dChangeInPeriodINVERSE = -dPeriodINVERSE;
		dPeriodINVERSE = 0;

		// Set estimated firing time to the maximal value(ca. 10^308)
		dEstimatedTaskTime = DBL_MAX;

		DEBUG_L2(
				<<"estimatedPeriod(): \t[t_0, K, v_0, t^f]: \t[" <<dStartOfTimeWindow <<", " <<dActivityVariable <<", " <<dDepolAtStartOfTimeWindow <<", "
			   	<<dEstimatedTaskTime	<<"]:\t"
				<<"\t\tand finally getCalculateDepol(" <<time_class::getTime() <<") = \e[31;0m" <<getCalculateDepol() <<"\e[0;0m\n"
				);
#endif

		//exit(0);
	} //slutt if( dActivityVariable > FIRING_THRESHOLD ){X}else{ ... }
#endif
} //}



	
/**************************************************************
****** 		K_sensory_auron - update sensor funktions  	******* 			XXX ok XXX
**************************************************************/
void K_sensory_auron::updateAllSensorAurons()
{ //{
	// Iterate through list pAllSensoryAurons and call updateAllSensorAurons() for each element
	for( std::list<K_sensory_auron*>::iterator sensorIter = pAllSensoryAurons.begin(); sensorIter != pAllSensoryAurons.end(); sensorIter++)
	{
		(*sensorIter)->updateSensoryValue();
	}
} //}1

inline void K_sensory_auron::updateSensoryValue()
{ //{
	DEBUG_L3(<<"K_sensory_auron::updateSensoryValue()");

	// Update sensed value. Two variables to find the change in sensed value
	dLastSensedValue = dSensedValue;
	dSensedValue =  (*pSensorFunction)();

	// Save time for initiation of new time window(define this to happen at start of iteration for sensory aurons)
	dStartOfNextTimeWindow = (double)time_class::getTime(); // Set it to [now] before result is computed in changeKappa_diffArg(-)
	changeKappa_diffArg(   (dSensedValue-dLastSensedValue) );  
} //}
	

/**************************************************************
****** 		s_sensory_auron - update sensor funktions  	******* 		XXX almost ok XXX
**************************************************************/
inline void s_sensory_auron::updateAllSensorAurons()
{ //{
	// Iterate through list pAllSensoryAurons and call updateAllSensorAurons() for each element
	for( std::list<s_sensory_auron*>::iterator sensorIter = pAllSensoryAurons.begin() 	; 	sensorIter != pAllSensoryAurons.end() ; sensorIter++)
	{
		(*sensorIter)->updateSensoryValue();
	}
} //}

inline void s_sensory_auron::updateSensoryValue()
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
	double dKappaFeil_temp = dActivityVariable - dKappa_temp;

	// TESTER: Blir bare bullshit! XXX
	// Test på nytt! (skrevet: 22.10.2011)
	double dKappa_derived_temp = 0;
 	for( std::list<K_synapse*>::iterator iter = pInputDendrite->pInputSynapses.begin() ; iter!=pInputDendrite->pInputSynapses.end() ; iter++)
	{
		#if DEBUG_PRINT_LEVEL > 2
		cout<<"Derived Transmission: " <<(*iter)->getDerivativeOfTransmission() <<endl;
		#endif
 		dKappa_derived_temp += (*iter)->getDerivativeOfTransmission();
	}
	// TIL her.

	#if DEBUG_PRINT_LEVEL > 2
	cout<<"[Kappa, dKappa_temp, dKappaFeil_temp] : " <<dActivityVariable <<", " <<dKappa_temp <<", " <<dKappaFeil_temp <<"\tderived-transmission: " <<dKappa_derived_temp
		<<" => Kappa+transmission = " <<dActivityVariable+dKappa_derived_temp
		<<endl;
	#endif
	dActivityVariable = dKappa_temp;


	return dKappaFeil_temp;	
} //}

inline double K_sensory_auron::recalculateKappa()
{
	// In this work, a sensory auron does not receive input from other neurons.

	// TODO No er dette bare en sensor (Har ikkje muligheten for å få input fra andre neuron. Dette kan eg kanskje implementere om eg har tid..)
	#if 0
	static double dOldActivityVariable;
	dOldActivityVariable = dActivityVariable;
	updateSensoryValue();
	// Er dette rett :
	return dActivityVariable-dOldActivityVariable;
	#endif
	return 0;
}
// SKAL VEKK:
inline double K_dendrite::recalculateKappa()
{ //{
	double dKappa_temp = 0;
 	for( std::list<K_synapse*>::iterator iter = pInputSynapses.begin() ; iter!=pInputSynapses.end() ; iter++)
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
 	for( std::list<K_synapse*>::iterator iter = pOutputSynapses.begin(); iter != pOutputSynapses.end(); iter++ )
	{ // Legger alle pOutputSynapses inn i time_class::pWorkTaskQueue: (FIFO-kø)
		time_class::addTaskIn_pWorkTaskQueue( *iter );
	}
}



const double K_synapse::getDerivativeOfTransmission()
{
	return pPreNodeAuron->dChangeInPeriodINVERSE * dSynapticWeight;
}
const double K_synapse::getTotalTransmission()
{
	return (1-2*bInhibitoryEffect)*(pPreNodeAuron->dPeriodINVERSE)*dSynapticWeight;
}



// vim:fdm=marker:fmr=//{,//}
