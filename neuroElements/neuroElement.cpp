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
			cout<<"\t[ " <<(*iter)->sName <<" ]\t\t" 		<<"\tdEstimatedTaskTime :\t" <<"\e[34m---\e[0m" <<endl;
		else if((*iter)->dEstimatedTaskTime == DBL_MAX)
			cout<<"\t[ " <<(*iter)->sName <<" ]\t\t" 		<<"\tdEstimatedTaskTime :\t" <<"\e[34mNaN\e[0m" <<endl;
		else
			cout<<"\t[ " <<(*iter)->sName <<" ]\t\t" 		<<"\tdEstimatedTaskTime :\t" <<(*iter)->dEstimatedTaskTime <<endl;
	}
	cout<<"\n";

	// First: Call destructor for all K_auron objects:
	K_auron::callDestructorForAllKappaAurons();

	// Then call destructor for all remaining i_aurons 
	// 	 (all s_aurons and also redundancy for K_aurons that for some reason remain undestructed)
	while( ! i_auron::pAllAurons.empty() )
	{
		#if DEBUG_PRINT_LEVEL > 2
		cout<<"Call destructor for auron " <<i_auron::pAllAurons.front()->sName <<endl;
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
		cout<<"Call destructor for K_auron " <<K_auron::pAllKappaAurons.front()->sName <<endl;
		#endif

		// delete first element(call destructor). The destructor removes element's pointer from pAllKappaAurons and also from pAllAurons
	 	delete (*K_auron::pAllKappaAurons.begin());
	}
}
//}2
//{2 *** i_auron   
i_auron::i_auron(std::string sName_Arg /*="unnamed"*/, double dStartAktVar /*=0*/) 
	: timeInterface("auron"), dActivityVariable(dStartAktVar), sName(sName_Arg)
{
	#if LOG_DEPOL
		// Print to log file is set. Initiation of file stream(.oct file that is executable in octave.):
		// 	 (datafiles_for_evaluation is created at the initiation of auronSim. See int main(int,char**) )
		std::ostringstream tempDepolLoggFileAdr;
		tempDepolLoggFileAdr<<"./datafiles_for_evaluation/log_auron_" <<sName <<"-depol" <<".oct";
	
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
		tempFilAdr2<<"./datafiles_for_evaluation/log_auron_" <<sName <<"-depolSPIKES" <<".oct";
	
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
	tempFilAdr3<<"./datafiles_for_evaluation/log_auron_" <<sName <<"-firingTimes" <<".oct";


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
		cout<<"\tDESTRUCTOR: \ti_auron::~i_auron() : \t" <<sName <<"\t * * * * * * * * * * * * * * * * * * * * * * * * * \n";
	#endif

	// remove auron from pAllAurons:
	pAllAurons.remove(this);

	#if LOG_DEPOL
		// Finalize octave script to make it executable:
			// depolarization log file:
		depol_logFile 	<<"];\n\n"
						<<"plot( data([1:end],1), data([1:end],2), \"@;Depolarization;\");\n"
						<<"title \"Depolarization for auron " <<sName <<"\"\n"
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
s_auron::s_auron(std::string sName_Arg /*="unnamed"*/, int nStartDepol /*=0*/) : i_auron(sName_Arg, nStartDepol)
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
K_auron::K_auron(std::string sName_Arg /*="unnamed"*/, double dStartKappa_arg /*= 0*/) : i_auron(sName_Arg), kappaRecalculator(this)
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
		tempFilAdr<<"./datafiles_for_evaluation/log_auron_" <<sName <<"-kappa" <<".oct";
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

						<<"title \"Activity variable for K-auron " <<sName <<"\"\n"
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
s_sensory_auron::s_sensory_auron( std::string sName_Arg , const double& (*pFunk_arg)(void) ) : s_auron(sName_Arg)
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
		cerr<<"Call s_synapse::s_synapse("  <<pPreNodeAxon->pElementOfAuron->sName 
			<<".pOutputAxon, " <<pPostNodeDendrite->pElementOfAuron->sName <<".pInputDendrite, ...)\n";
	#endif

	// Insert synapse as output synapse in presyn. axon and input synapse in postsyn. dendrite:
	pPreNodeAxon->pOutputSynapses.push_back(this);
	pPostNodeDendrite->pInputSynapses.push_back(this);
	
	// Initialize synapse's log-file:
	//{4 make a .oct - file, and make it ready for execution in octave:
	std::ostringstream tempFilAdr;
	tempFilAdr<<"./datafiles_for_evaluation/log_s_synapse_" <<pPresynAuron_arg->sName <<"-"  <<pPostsynAuron_arg->sName ;
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
					cout<<"\t~( [" <<pPreNodeAxon->pElementOfAuron->sName <<"] -> "; 						// Printing part 1
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
					cout<<"[" <<pPostNodeDendrite->pElementOfAuron->sName <<"] )\t"; 						// Printing part 2
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
				<<"bPreOk (" <<pPreNodeAxon->pElementOfAuron->sName <<"):" <<bPreOk 
				<<"  ->  bPostOk (" <<pPostNodeDendrite->pElementOfAuron->sName <<"): " <<bPostOk 
				<<"\nNot deleted from the corresponding neuron if it has the value \"0\"\n"
				<<"TERMINATING";
		exit(EXIT_FAILURE);	
	}
	cout<<endl;


	// Finalize transmission log:
	synTransmission_logFile<<"];\n"
					<<"plot( data([1:end],1), data([1:end],2), \";Synaptic weight;\");\n"
					<<"title \"Synaptic weight for s_synapse: " <<pPreNodeAxon->pElementOfAuron->sName 
					<<" -> " <<pPostNodeDendrite->pElementOfAuron->sName <<"\"\n"
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
		cout<<"Constructor :\tK_synapse::K_synapse(" <<pPreNodeAuron->sName <<", " <<pPostNodeDendrite->pElementOfAuron->sName
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
	tempFilAdr<<"./datafiles_for_evaluation/log_transmission_K_synapse_" <<pPresynAuron_arg->sName <<"-"  <<pPostsynAuron_arg->sName ;
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
				cout<<"\t~( [" <<pPreNodeAuron->sName <<"] -> "; 				 // Print to screen, part 1
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
				cout<<"[" <<pPostNodeDendrite->pElementOfAuron->sName <<"] )\t"; // Print to screen, part 2
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
				<<"bPreOk (" <<pPreNodeAuron->sName <<"):" <<bPreOk <<"  ->  bPostOk ("
			   	<<pPostNodeDendrite->pElementOfAuron->sName <<"): " <<bPostOk 
				<<"\nOk unless value is zero..\nTERMINATING\n\n";
		exit(-9);	
	}
	cout<<endl;

	// Finalize filestream: synTransmission_logFile
	// 	(make log script executable in octave)
	synTransmission_logFile<<"];\n"
					<<"plot( data([1:end],1), data([1:end],2), \".r;Synaptic transmission;\");\n"
					<<"title \"Synaptic transmission from s_synapse: " <<pPreNodeAuron->sName 
					<<" to " <<pPostNodeDendrite->pElementOfAuron->sName <<"\"\n"
					<<"xlabel Time\n" <<"ylabel Syn.Transmission\n"
					//<<"print ./eps/eps_transmission_" <<pPreNodeAuron->sName <<"->" 
					// 			<<pPostNodeDendrite->pElementOfAuron->sName <<".eps -deps -color\n"
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
		cout<<"\tDESTRUCTOR :\ts_axon::~s_axon() for auron \t" <<pElementOfAuron->sName <<"\n";
	#endif
}
//}2
//}1
//{1 * DENDRITE
//{2 *** i_dendrite
i_dendrite::i_dendrite( std::string sName /*="dendrite"*/) : timeInterface(sName)
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

	//bBlockInput_refractionTime = false;
}
s_dendrite::~s_dendrite()
{
 	#if DEBUG_PRINT_DESCTRUCTOR
		cout<<"\tDesructor :\ts_dendrite::~s_dendrite() \t for auron \t" <<pElementOfAuron->sName <<"\n";
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
		cout<<"\tDestructor :\tK_dendrite::~K_dendrite() \t\t for auron \t" <<pElementOfAuron->sName <<"\n";
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

	DEBUG_L3( <<sName <<"\t:\tTid:\t" <<time_class::getTime() <<" ,\tKappa :\t" <<dActivityVariable );
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

inline void s_dendrite::calculateLeakage()
{ //{2 /** Only for SANN:  ***/
	// Check whether it is done already, this iter:
	if( pElementOfAuron->ulTimestampLastInput != time_class::getTime() )
	{
		// Compute leakage, and subtract from neuron's depolarization 
	 	pElementOfAuron->dActivityVariable *= pow( (double)(1-LEAKAGE_CONST), (double)(time_class::getTime() - pElementOfAuron->ulTimestampLastInput) );
	
		// Write depol to log after leakage is subtracted:
		pElementOfAuron->writeDepolToLog();
	}
} //}2
//}1



/*************************************************************
****** 													******
******  		doTask() -- collected in one place		******
****** 													******
*************************************************************/

//  		* 	SANN
//{1
/* 	s_auron::doTask() 	:  		Fire A.P. */
inline void s_auron::doTask()
{ //{ ** AURON

	if( ulTimestampPreviousFiring == time_class::getTime() )
	{
		DEBUG_L1(
			<<"\n\n*********************\t\e[31mError\e[0m?\nTwo firings in one time step? \n"
			<<"Error msg: au#103 @ neuroElement.cpp\n*********************\n\n");
		return;
	}

 	#if DEBUG_PRINT_LEVEL>0
	cout<<"\t* \e[34ms\e[0m *\t\e[4;32mAction Potential\e[0;0m for auron " 
		<<sName <<"\t\t\t\t[time step] = [\e[1;33m" <<time_class::getTime() <<"\e[0m]\n";
 	#endif

	// Schedule axon hillock: Simulate action potential propagation in axon. Starts, next iteration.
	time_class::addTaskIn_pWorkTaskQueue( pOutputAxon );

	// Save time of firing to ulTimestampPreviousFiring:
	ulTimestampPreviousFiring = time_class::getTime();

	// Reset depol. value:
	dActivityVariable = 0; 

	// Log action potential time and vertical line in depol. plot
	writeAPtoLog();
} //}
/* 	s_axon::doTask()   	:		Simulate spatiotemporal delay in the axon 	*/
inline void s_axon::doTask()
{ //{ ** AXON

	// Unblock dendrite: when action potential reach the axon, the absolute refraction time of 1ms is over.
	//pElementOfAuron->pInputDendrite ->bBlockInput_refractionTime = false;

	#if DEBUG_PRINT_LEVEL > 3
	DEBUG_L3(
			<<"s_axon::doTask()\tInserts all its output synapses to pWorkTaskQueue. Part of auron: "
		   	<<pElementOfAuron->sName <<" - - - - - - - - - - - - - - - \n";
	#endif

	// Push all output synapses to pWorkTaskQueue:
 	for( std::list<s_synapse*>::iterator iter = pOutputSynapses.begin(); iter != pOutputSynapses.end(); iter++ )
	{ // Push to the back of pWorkTaskQueue: FIFO queue
		time_class::addTaskIn_pWorkTaskQueue( *iter );
	}

	// Print to log after refraction period:
	//pElementOfAuron->writeDepolToLog();
} //}
/*  s_synapse::doTask() : 		Simulate transmission in synapse */
inline void s_synapse::doTask()
{ //{ ** SYNAPSE

	// If the synapse has inhibitory effect, send inhibitory signal (subtraction).
	// This is done by multiplying with the factor (1-2*bInhibitoryEffect)
 	pPostNodeDendrite->newInputSignal( (1-2*bInhibitoryEffect) * 	( 1000 * dSynapticWeight )   );

	// Compute synaptic plasticity, here.

	// Log synaptic transmission (will be imporant when synaptic plasticity is introduced):
	synTransmission_logFile 	<<"\t" <<time_class::getTime() <<"\t" <<(1-2*bInhibitoryEffect) * dSynapticWeight
								<<" ;   \t#Synaptic transmission\n" ;
} //}
/* s_dendrite::doTask() is a way of simulating spatiotemporal delay for the neuron */
inline void s_dendrite::doTask()
{ //{ DENDRITE
	// Insert next element in the signal path(the auron) to pWorkTaskQueue
	time_class::addTaskIn_pWorkTaskQueue( pElementOfAuron );
} //}
//}1

//			* 	KANN
//{1
/* K_auron::doTask() 	: 		Fire A.P. */
inline void K_auron::doTask()
{ //{2 ** AURON
	if( (unsigned)dEstimatedTaskTime == time_class::getTime() ){
		// It is correct to fire, this instant:
#if DEBUG_PRINT_LEVEL>2 // Print additional debug info if DEBUG_PRINT_LEVEL is more than 2
		cout<<"\t* \e[35mK\e[0m *\t\e[4;32mAction Potential\e[0;0m for auron " <<sName <<"\t\t\t\t[time step, est.t.time] = [" 
			<<time_class::getTime() <<"\e[0m,\e[32m " <<dEstimatedTaskTime <<"\e[0m]\n";
#else 					// Else, only print that it fires (same form as s_auron::doTask() )
		cout<<"\t* \e[35mK\e[0m *\t\e[4;32mAction Potential\e[0;0m for auron " <<sName <<"\t\t\t\t[time step] = ["
		   	<<time_class::getTime() <<"\e[0m]\n";
#endif
	}else if( dEstimatedTaskTime < time_class::getTime() ) // If dEstimatedTaskTime is less than [now] means that this is a late firing!
	{
		// Error to fire at this instant:
		#if DEBUG_PRINT_LEVEL>0
		cout<<"\t* * *\t\e[4;32mFIRING\e[31m ERROR\e[0;0m auron "
		   	<<sName <<"\t\t\t\t\t[time > (int)est.time]: [\e[1;33m" 
			<<time_class::getTime() <<"\e[0m>\e[1;31m " <<dEstimatedTaskTime <<"\e[0m]\n";
		#endif
	}else //if( dEstimatedTaskTime > time_class::getTime()+1 ) //Means that this is an early firing. Pring error message, and continue..
	{
		cout<<"\t* * *\t\e[4;32mFIRING\e[31m ERROR\e[0;0m auron " 
			<<sName <<"\t\t\t\t\t[time < (int)est.time]: [\e[1;33m" 
			<<time_class::getTime() <<"\e[0m<\e[1;31m " <<dEstimatedTaskTime <<"\e[0m]\n";
		DEBUG_L2( <<"Depol.:\t" <<getCalculateDepol());
		cerr<<"TERMINATES\n";
		exit(EXIT_FAILURE); // XXX Terminate
	}

	// Print to screen:
	DEBUG_L2(
			<<sName <<".doTask()" <<"\t[t=" <<time_class::getTime() <<" est.tt:" <<dEstimatedTaskTime <<"]\t[v(t),v(t_n)=[\e[01;31m" 
			<<getCalculateDepol(dEstimatedTaskTime) <<"\e[0m, " <<getCalculateDepol()
			<<"\t | K=" <<dActivityVariable  <<"\tLast v_0:" <<dDepolAtStartOfTimeWindow 
			);

	#if LOG_DEPOL
		// For higher precision on depol. log: Set precision for logging of double-variable: //{
		depol_logFile.precision(11);
	
		// Write action action-potential log entry: one at Firing Thread and one at x-axis at time dEstimatedTaskTime:
		depol_logFile 	<<dEstimatedTaskTime <<"\t" <<FIRING_THRESHOLD <<"; \t #Action potential \t\tAPAPAP\n" ;
		depol_logFile 	<<dEstimatedTaskTime <<"\t" <<0 			 <<"; \t #Action potential \t\tAPAPAP\n" ;
	//	depol_logFile.flush();
		//}
	#endif


	// Initialize new 'time window':
	// 	 (Set dStartOfTimeWindow to dEstimatedTaskTime)
	dDepolAtStartOfTimeWindow = 0; 
	dStartOfTimeWindow = dEstimatedTaskTime; // IMPORTANT: use dEstimatedTaskTime when firing. 

	// Save time of firing: When the period is recalculated, the exact start of time window is imporant. 
	// 	- Do not use ulTime because it is always less than the exact firing time: ln(-X) = imaginary number
	dLastFiringTime = dEstimatedTaskTime;

	// It is important that estimatedPeriod is computed after dEstimatedTaskTime += dLastCalculatedPeriod !
	// 	If estimateFiringTimes() or doCalculation() is executed first, a large error is the result.

	// Log AP (vertical line in depol. plot and also log time of firing)
	writeAPtoLog(); // Important: Have to be executed after dLastFiringTime is written to.
	
	// Because of the new mechanism that enables multiple firings every time step:
	// TODO Do not think estimateFiringTimes(double) is different from estimateFiringTimes(void) TODO Check and fix!
	// estimateFiringTimes(dLastFiringTime); 
	estimateFiringTimes();

	// Check if the next estimated firing time is in the present time step:
	//if( dEstimatedTaskTime < time_class::getTime()+1 )
	if( (int)dEstimatedTaskTime == time_class::getTime() )
		time_class::addTaskInPresentTimeStep(this);
} //}2
/* K_synapse::doTask() 	: 		Simulates transmission in the synapse */
inline void K_synapse::doTask()
{ //{2 ** SYNAPSE
	// Instead of having an if-sentence, one call is used to enable more pipelining
	// 	(Multiply signal with [ 1-2*bInhibitoryEffect ] to get the right transmission)
	pPostNodeDendrite->newInputSignal( (1-2*bInhibitoryEffect) * dSynapticWeight * pPreNodeAuron->dChangeInPeriodINVERSE );

	double dPresynDeltaPeriodeINVERSE_temp =  pPreNodeAuron->dChangeInPeriodINVERSE;
	#if DEBUG_PRINT_LEVEL > 2
 	cout<<"K_synapse::doTask()\tdSynapticWeight: " <<dSynapticWeight 
		<<", preNode->dChangeInPeriodINVERSE: " <<dPresynDeltaPeriodeINVERSE_temp <<"\tTransmission: "
		<<(1-2*bInhibitoryEffect) * dSynapticWeight * dPresynDeltaPeriodeINVERSE_temp
		<<endl;
	#endif

	synTransmission_logFile
		<<time_class::getTime() <<"  \t"
		<<(1-2*bInhibitoryEffect) * dSynapticWeight * dPresynDeltaPeriodeINVERSE_temp
		<<" ; \n";
//	synTransmission_logFile.flush();
} //}2

// Brutal error check. For further work: remove K_dendrite!
inline void K_dendrite::doTask()
{ // DENDRITE (not in use)
	// XXX Brutal error check: This funtion is never to be called..
	cout<<"\n\nBrutal feilsjekk @ K_dendrite::doTask() : exit(EXIT_FAILURE);\n\n";

	exit(EXIT_FAILURE);
} 

//}1            *       KANN end

// 		* 	time_class
/* time_class::doTask()	: 	Organize time: iterate time and ceep pWorkTaskQueue and time organized according to defined rules */
void time_class::doTask()
{ 	//{2
	// Check whether simulation is complete: in this case, initialize graceful termination.
	if( ulTime >= ulTotalNumberOfTimeSteps )
	{
		bContinueExecution = false;
		return;
	}

	#if PRINT_TIME_ITERATION
	// Increase by one (ulTime+1) to show t_n for the new time step.
	if((ulTime+1) % uNumberOfIterationsBetweenPrintToScreen  == 0)		
		cout<<"\e[33m"	<<ulTime+1 <<"\e[0m" <<"\t TIME\t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - = "<<ulTime <<"\n";
	#if DEBUG_PRINT_LEVEL>3
	cout<<"\e[33m"	<<ulTime <<"->" <<ulTime+1 <<"\e[0m" <<"\t- - -  time_class::doTask() - Increase time from :   - - - - = \e[4;39m"
		<<ulTime <<" -> " <<ulTime+1 <<"\e[0m :\n";
	#endif
	#endif

	// Push self-pointer to back of pWorkTaskQueue:
	pWorkTaskQueue.push_back(this);	

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

	// Commence scheduled computations:
	doCalculation();

	// Iterate t_n:
	ulTime++;
}//}2
//}1


/******************************************************************
****** 														*******
****** 			doCalculation() -- samla på en plass 		*******
****** 														*******
******************************************************************/
void K_auron::doCalculation()
{ //{
 	DEBUG_L3(<<"K_auron " <<sName <<".doCalculation()\t\t" <<sName <<".doCalculation()\t\tTime: " <<time_class::getTime() );
 	DEBUG_L4(<<"[K, T] = " <<dActivityVariable <<", " <<FIRING_THRESHOLD );

	// Initialize new time window: Save v_0 and t_0 at time of initiation
	dDepolAtStartOfTimeWindow = getCalculateDepol();
	dStartOfTimeWindow = time_class::getTime();

	// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO 
	// Do not use edge transmission. This does not make the implementation more effective because a floating point variable is used.
	// 	(edge transmission as the derivative is intended to make implementation more effective, as only those with changed transmission
	// 		have to be computed. When utilizing double--variable, this is always. Edge transmission only makes impl. more complex)
	// TODO remove dChangeInKappa_this_iter - mechanism!
	// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO 

	// Update Kappa
	dActivityVariable += dChangeInKappa_this_iter;
	dChangeInKappa_this_iter = 0;

	// Estimate period and next firing time
	estimateFiringTimes(); // Use ulTime-variant, as this always happens first every iter.

	writeDepolToLog();

	// Check whether auron fires before next iteration ( estTt<t+1+1 because time is iterated after doCalculation() )
	if( dEstimatedTaskTime < time_class::getTime()+2 ){
		DEBUG_L2(<<"K_auron::doCalc()\tK_auron schduled to fire this iteration:\n\t\t[TID, dEstimatedTaskTime] :\t[" 
				 <<time_class::getTime()+1 <<", " <<dEstimatedTaskTime <<"]");
		time_class::addTaskInPresentTimeStep( this );
	}
}

/* estimateFiringTimes() - estimates period and next firing time */
inline void K_auron::estimateFiringTimes()
{ //{
	if( dActivityVariable < FIRING_THRESHOLD ){
		DEBUG_L3(
				<<"\n\nadf52908i@neuroElement.cpp \e[33mestimatedPeriod(dTime) for K<T\e[0m\n\n"
				);
		// Set isi-period to max:
		dLastCalculatedPeriod  = DBL_MAX;
		dEstimatedTaskTime = DBL_MAX;
	}else{
		dLastCalculatedPeriod =((log( dActivityVariable / (dActivityVariable - (double)FIRING_THRESHOLD) ) / (double)LEAKAGE_CONST));
		dEstimatedTaskTime = ( dStartOfTimeWindow 	+ log( (dActivityVariable-dDepolAtStartOfTimeWindow)/(dActivityVariable 
														- (double)FIRING_THRESHOLD) )   / (double)LEAKAGE_CONST );
	}
} //}

/**************************************************************
****** 		K_sensory_auron - update sensor funktions  	*******
**************************************************************/
void K_sensory_auron::updateAllSensorAurons()
{ //{1
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
****** 		s_sensory_auron - update sensor funktions  	*******
**************************************************************/
inline void s_sensory_auron::updateAllSensorAurons()
{ //{
	// Iterate through list pAllSensoryAurons and call updateAllSensorAurons() for each element
	for( std::list<s_sensory_auron*>::iterator sensorIter = pAllSensoryAurons.begin() ;
		 sensorIter != pAllSensoryAurons.end() ; sensorIter++)
	{
		(*sensorIter)->updateSensoryValue();
	}
} //}

inline void s_sensory_auron::updateSensoryValue()
{ //{
	pInputDendrite->newInputSignal( ( (*pSensorFunction)() )); 
} //}

/******************************
**   Recalculate Kappa :     **
******************************/
void recalcKappaClass::doTask()
{ //{
	// Recalculate Kappa for the associated K_auron.
	// 	 - calls K_auron::recalculateKappa() that recalculates kappa for that K_auron object and returns new Kappa
	double dError = pKappaAuron_obj->recalculateKappa();

	// Use a FIR filter to compute next calculation
	static double sdSecondLastValue =0;
	static double sdLastValue 		=0;
	static double sdValue 			=0;
	static double sdTemp;

	// Compute when next run of recalcKappaClass.doTask()
	// 	 (from the altered sigmoid curve, described in MS report, with FIR effect -- moving average)
	sdTemp = sdValue;
	sdValue = ( (RECALC_c1+RECALC_c2) - (RECALC_c2 / ( 1+exp(-(RECALC_c4*dError - RECALC_c3)) )) 
				+ 	sdLastValue + sdSecondLastValue )  / 3 ;
	sdSecondLastValue = sdLastValue;
	sdLastValue = sdTemp;
	
	#if DEBUG_PRINT_LEVEL > 2
	cout<<pKappaAuron_obj->sName <<" - recalcKappaClass::doTask() :\tNew period for recalcKappaClass: \t"
	   	<<sdValue <<"\t\tfor error: " <<dError <<"\t\tTime:\t" <<time_class::getTime() <<endl;
	#endif

	// Schedule recalcKappaClass to run at the computed time:
	dEstimatedTaskTime = (double)time_class::getTime() + sdValue;
} //}

/****************************************
* K_auron::recalculateKappa() 			*
* 	-ret: Error from old kappa to new. 	*
****************************************/
inline double K_auron::recalculateKappa()
{ //{
	// recalcKappaClass::doTask() calls the associated K_auron's recalculateKappa() function.
	// 	 This funtion goes through all input synapses, and sums their transmission K_ij from K_synapse::getWij()

	// Find new kappa by calling pInputDendrite's recalculateKappa() function.
	double dKappa_temp = pInputDendrite->recalculateKappa();
	// Find error: to be used to schedule next calculation
	double dKappaFeil_temp = dActivityVariable - dKappa_temp;

	// Set K_auron's Kappa to be the value found by K_dendrite
	dActivityVariable = dKappa_temp;

	return dKappaFeil_temp;	
} //}

/* K_sensory_auron have been defined to receive no synapses in this work */
inline double K_sensory_auron::recalculateKappa()
{
	// In this work, a sensory auron does not receive input from other neurons.

	return 0;
}

/****************************************************
* K_dendrite::recalculateKappa() 					*
* 	-arg: 	void 									*
* 	-ret: 	double, new kappa 						*
* 													
* K_dendrite handles the recalculation of kappa 	*
*	(sums size of all input transmissions)  		*
 ***************************************************/
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


inline void time_class::addTaskInPresentTimeStep(timeInterface* pObjWithTask_arg)
{
	DEBUG_L3(<<"time_class::addTaskInPresentTimeStep(..)");

	// Finds right place for the new element. Insert at right place according to dEstimatedTaskTime
	for( std::list<timeInterface*>::iterator iter = pWorkTaskQueue.begin(); iter != pWorkTaskQueue.end(); iter++ )
	{

		// If the iter-element's dEstimatedTaskTime if after the new element's, insert element immediately after iter
		if( (*iter)->dEstimatedTaskTime > pObjWithTask_arg->dEstimatedTaskTime){
 			DEBUG_L3(<<"insert element at the right place in pWorkTaskQueue(in the present iteration)");
			pWorkTaskQueue.insert(iter, pObjWithTask_arg); 	//"insert()" inserts element before [iter].
			return;
		}

		// Check if iter points at the time separation object. In this case, insert element here.
 		if( (*iter)->sClassName == "time" )
		{
			// This is the last element in the present time iteration. Insert element last.
			pWorkTaskQueue.insert(iter, pObjWithTask_arg); 	//"insert()" inserts element before [iter].
			DEBUG_L3(<<"insert element " <<pObjWithTask_arg->sClassName <<" at the end of pWorkTaskQueue!");
			return;
		}
	}

	#if DEBUG_PRINT_LEVEL>4
	printAllElementsOf_pWorkTaskQueue();
	#endif
}


/* A funtion to simulate transmissions. To be done at every new time window(new kappa) */
inline void K_auron::doTransmission()
{
	#if DEBUG_PRINT_LEVEL > 3
 	cout<<"K_auron::doTransmission()\tInsert all output synapses in pWorkTaskQueue. For auron: "<<sName <<" - - - - - - - - - - - - - - - \n";
	#endif

	// Insert all output synapses into pWorkTaskQueue:
 	for( std::list<K_synapse*>::iterator iter = pOutputSynapses.begin(); iter != pOutputSynapses.end(); iter++ )
	{
		time_class::addTaskIn_pWorkTaskQueue( *iter );
	}
}


const double K_synapse::getTotalTransmission() const
{
	return (1-2*bInhibitoryEffect)*(pPreNodeAuron->dPeriodINVERSE)*dSynapticWeight;
}



// vim:fdm=marker:fmr=//{,//}
