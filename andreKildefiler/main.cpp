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






// 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	
//   Det kan skape feil å legge til eit neuron til neste iter. Dersom den får null input neste iter, blir vil den ikkje fyre neste iter, selv om den er estimert til f.eks. ?.5 (altså midt i iter).
// 		Løsninga er å kun legge til fyring i gjeldende iter. KUN bruke den nye egenskapen til KANN!
// 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 
// a	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 
// 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 
// a	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 
// 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 	TODO 



//TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO 
// 			Innfør atexit(&i_auron::callDestructorForAllAurons) ! Dette står i rapporten, og er også veldig lurt!
// 				Kanskje eg heller skal lage en egen avsluttingsfunksjon, der eg også skriver ut alle viktige element. Isåfall må eg skrive om rapportan!. Dette er bedre!
// 				Kanskje eg også skal handtere SIGTERM signal? Bare for å briefe litt? JA!
// 			s_auron funker ikkje lenger! TODO TODO
// 			lager av en eller anna grunn: to K_auron istadenfor eitt av samme node (med navn [dKN])!
// 				LAGER DOBBELT OPP for KN: Skriver ivertfall to oppføringer i lista alleAuron. NEI: slik er det: sletter først alle element i K_auron::pAllKappaAurons, så alle i i_auron::pAllAurons. Fiks det slik at dette ikkje skjer(ikkje skrive ut to oppføringer av samme!)
//TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO 








/*
 * main.cpp
 *
 *  Created on: 9. feb. 2011
 *      Author: kybpc1036
 */


#include "main.h"
#include "../neuroElements/auron.h"
#include "../neuroElements/synapse.h"
#include "time.h"
#include "sensorFunk.h" 				// TODO TODO TODO Endre til sensoryFuntions.h TODO TODO TODO


#include "../andreKildefiler/ANN.h" 	// TODO TODO TODO Endre navn på andreKildefiler/ TODO TODO TODO 


/*** declarations ***/
	/*** funtion declations ***/
void initialzeWorkTaskQueue();
void printArgumentConventions(std::string);
void* taskSchedulerFunction(void*);

std::ostream & operator<<(std::ostream& ut, i_auron* pAuronArg ); 

	/*** variable declarations ***/
extern std::list<timeInterface*> 				time_class::pWorkTaskQueue;
extern std::list<timeInterface*> 				time_class::pCalculatationTaskQueue;
extern std::list<timeInterface*> 				time_class::pPeriodicElements;

extern std::list<i_auron*> i_auron::pAllAurons;
extern std::list<K_auron*> K_auron::pAllKappaAurons;
extern std::list<K_sensory_auron*> K_sensory_auron::pAllSensoryAurons;
extern std::list<s_sensor_auron*> s_sensor_auron::pAllSensoryAurons;

extern unsigned long time_class::ulTime;

unsigned long ulTemporalAccuracyPerSensoryFunctionPeriod 	=	DEFAULT_NUMBERofTIMESTEPS;
float fNumberOfSensoryFunctionPeriods 						=	DEFAULT_NUMBER_OF_SENSORY_FUNCTION_PERIODS;
unsigned long ulTotalNumberOfTimeSteps 						= 	ulTemporalAccuracyPerSensoryFunctionPeriod*fNumberOfSensoryFunctionPeriods;
unsigned uNumberOfIterationsBetweenWriteToLog;

// The program exits gracefully when bContinueExecution is set to [false]:
bool bContinueExecution = true;

/*** end: declarations ***/


// TODO TODO TODO Skal eg heller bruke referanse-returnerende funksjon (inline), og referere til stroustrup? TODO TODO TODO
//{ Alternativt med referanse-returnerende funk. : 
/**********************************************************************************
*** 	int& nAntallTidsiterasjoner()
*** 		returnerer referanse til static int nTidsgrenseForSimulering 
***					(som eksisterer heile kjøretida til programmet).
*** 		Vil dermed være mulig å skrive nAntallTidsiterasjoner()=5;
***
**********************************************************************************/
//int& nAntallTidsiterasjoner()
// {
//	static int nTidsgrenseForSimulering = DEFAULT_NUMBERofTIMESTEPS;
//	return nTidsgrenseForSimulering;
//}


/*****************************************************************
** void main(int argc, char *argv[])							**
** 	-arguments: number and array of arguments from shell		**
** 	-return: 	int 											**
** 	Funtion: 	Program's main funtion. 						**
*****************************************************************/
int main(int argc, char *argv[])
{
	// Register atexit()-function: funtion to be called when auroSim terminates.
	// Call static i_auron member funtion i_auron::callDestructorForAllAurons() when auroSim terminates:
	atexit(&i_auron::callDestructorForAllAurons);


	cout<<"Set default text print style: \e[0;39m Weak text, default terminal text colour.\n\n";

	//Read in arguments from shell:  //{1
	if(argc > 1 ) //{ 	  // (first argument is the program call)
	{
		int nInputArgumentPos = 0;
		bool bPrintCallingConventions = false;

		while( ++nInputArgumentPos < argc ){
			// If first character in the argument at nInputArgumentPos is '-', examine argument:
			if(argv[nInputArgumentPos][0] == '-'){

				cout<<"argv[" <<nInputArgumentPos <<"] == " <<argv[nInputArgumentPos] <<endl;
				switch( argv[nInputArgumentPos][1] ){
					case 'h':
						printArgumentConventions(argv[0]);
						exit(EXIT_SUCCESS);
						break;
					case 'r':
						// Check whether the number of iterations is written in the same argument( like: -r100 )
						if( 		(ulTemporalAccuracyPerSensoryFunctionPeriod = atoi( &argv[nInputArgumentPos][2])) ) 	
							cout<<"Temporal resolution set to " <<ulTemporalAccuracyPerSensoryFunctionPeriod <<" (time steps per sensory function period).\n";
						// othervise: see if the number of iterations is written in the next argument
						else if( 	(ulTemporalAccuracyPerSensoryFunctionPeriod = atoi( argv[nInputArgumentPos+1]) ) ){
							++nInputArgumentPos;
							cout<<"Temporal resolution set to " <<ulTemporalAccuracyPerSensoryFunctionPeriod <<" (time steps per sensory function period).\n";
						}else{
							cout<<"Can not read argument. Please follow the conventions!\tArgument Ignored." <<endl;
							bPrintCallingConventions = true;
							//printArgumentConventions(argv[0]);
							//exit(EXIT_FAILURE);
						}
						break;
					case 'n':
						if( 		(fNumberOfSensoryFunctionPeriods = atof( &argv[nInputArgumentPos][2])) ) 	
							// Check whether the number of sensory funtion periods is written in the same argument( like: -n4 )
							cout<<"Number of forcing function periods set to be " <<fNumberOfSensoryFunctionPeriods <<"\n";
							// othervise: see if the number of periods is written in the next argument
						else if( 	(fNumberOfSensoryFunctionPeriods = atof( argv[nInputArgumentPos+1]) ) ){
							++nInputArgumentPos;
							cout<<"Number of forcing function periods set to be " <<fNumberOfSensoryFunctionPeriods <<"\n";
						}else{
							cout<<"Can not read argument. Please follow the conventions!\tArgument Ignored." <<endl;
							bPrintCallingConventions = true;
							//printArgumentConventions(argv[0]);
							//exit(EXIT_FAILURE);
						}

						if( fNumberOfSensoryFunctionPeriods < 0){
							cout<<"Number of sensory function oscillations set to an invalid number (" <<fNumberOfSensoryFunctionPeriods <<"). Terminates program.\n";
							printArgumentConventions(argv[0]);
							exit(EXIT_FAILURE);
						}
						break;
					default:
						cout<<"\nCan not read argument \e[0;1m" <<argv[nInputArgumentPos] <<"\e[0;0m.\nPlease follow the programs argument conventions:\n";
						printArgumentConventions(argv[0]);
						exit(EXIT_FAILURE);
						break;
				}
			}else{
				cout<<"Argument " <<nInputArgumentPos+1 <<" unrecognized: \e[0;1m" <<argv[nInputArgumentPos] <<"\e[0;0m.\n";
				cout<<"Please follow calling convenctions: \n\n";
				printArgumentConventions(argv[0]);
				exit(EXIT_FAILURE);
			}
		} //}
				
		// printArgumentConventions(argv[0]): print the argument convention for auroSim
		if( bPrintCallingConventions ){
			printArgumentConventions(argv[0]);
		}
		cout<<endl;

	}else{ // else, for the statement [if(argc > 1)]
		ulTemporalAccuracyPerSensoryFunctionPeriod = DEFAULT_NUMBERofTIMESTEPS;
		//cout<<"No arguments listed. Continue with default values:\tNumber of iterations: " <<DEFAULT_NUMBERofTIMESTEPS <<endl;
		cout<<"No arguments listed. Continue with default values:\tTemporal accuracy per forcing function: " <<ulTemporalAccuracyPerSensoryFunctionPeriod
			<<"\n\t\tNumber of forcing function periods set to be " <<fNumberOfSensoryFunctionPeriods <<"\n";
		printArgumentConventions(argv[0]);
	} 

	// Set total number of time steps:
	ulTotalNumberOfTimeSteps = fNumberOfSensoryFunctionPeriods * ulTemporalAccuracyPerSensoryFunctionPeriod;
	cout<<"Giving a total of \e[0;1m" <<ulTotalNumberOfTimeSteps<<"\e[0;0m time steps for this run of auroSim.\n\n";

	// Set uNumberOfIterationsBetweenWriteToLog (to restrict number of points in log file)
	if(ulTemporalAccuracyPerSensoryFunctionPeriod > LOGG_RESOLUTION){
		uNumberOfIterationsBetweenWriteToLog = (int)(((float)ulTemporalAccuracyPerSensoryFunctionPeriod / (float)LOGG_RESOLUTION) +0.5);
		cout<<"Restricting number of entries in log file: Write log every " <<uNumberOfIterationsBetweenWriteToLog <<" iteration.\n";
	}else{
		cout<<"No nead to restrict number of log entries due to few iterations.\n\n";
	}
	cout<<"\n\n\n";
	//}1 Finished reading in arguments.
	

	// The return value from system calls is zero if successful. Important to check return, and print error message if unsuccessful:
	if( system("mkdir datafiles_for_evaluation") != 0 ){
		//cout<<"Could not make directory for log files [./datafiles_for_evaluation/]."
		cout<<"\tIn case this directory does not exist, please make this directory manually.\n\n"; 
	}
	// Clean up directory ./datafiles_for_evaluation/ to avoid plotting old results:
	if( system("rm ./datafiles_for_evaluation/log_*.oct") != 0)
		cout<<"Could not remove old log files. Please do this manually to avoid plotting old results.\n";


	// The experiments:

	#if 1 // Tests for a single sensory neuron:
		// Experiment 1:
		#if 0
			new K_sensory_auron("sKN", &staticSensoryFunc);
			new s_sensor_auron("sSN", &staticSensoryFunc);
		#endif

		// Experiment 2:
		#if 1
			#if KANN
			new K_sensory_auron("dKN", &dynamicSensoryFunc);
			#endif
			#if SANN
			new s_sensor_auron("dSN", &dynamicSensoryFunc);
			#endif
		#endif

		// Illustration of sensory funtion (fig. 3.6 in the report when auroSim is called with arguments [./auroSim.out -r1000 -n4] )
		//new K_sensory_auron("dKN2", &sensoryFunctionExample);
	#else



			// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO RENSET tekst til hit!

// Lage ANN fra ei kant-matrise, og evt. send inn en vektor med vilkårlig antall auron (mindre enn dim(matrisa)). Kan også sende inn null auron. Vektoren med auron-peikere brukes for å la nokre auron være sensor-auron!
#if 0
	cout<<"\n\n\n\tFORTSETTER\n\n";


	cout<<"Try to construct ANN with K_auron-matrix:\n";

		// TRENGER IKKJE MATRISE: bruk heller std::vector<K_auron*>
	cout<<"\tInitialize K_auron*-vector:\n";
		std::vector<K_auron*> AlleAuron(3);
		//AlleAuron[0] = new K_sensory_auron("Ksensor1",staticSensoryFuncWithHighActivity);
		AlleAuron[0] = new K_sensory_auron("K0Sensor", dynamicSensoryFunc);
		AlleAuron[2] = new K_sensory_auron("K2Sensor", dynamicSensoryFunc2);
	
		QuadraticMatrix<double> KMat(3);
		KMat(0,1) = 50;
		KMat(2,1) = -50;

	cout<<"\tInitialize ANN from K_auron*-vector and dEdgeMatrix:\n";
		ANN<K_auron> Ktest2(KMat, AlleAuron);

	cerr<<"\n\nSkriver ut kant-matrise:\n";
		Ktest2.printEdgeMatrix();

	cerr<<"OK\n";
#endif	

	
	#endif //Avslutter #if som starter over 1-aurons-testen (dette over er else..)

//{ KOMMENTERT UT
//  BARE KAPPA:
	#if 0
	K_sensory_auron* Ks1 = new K_sensory_auron("K_sensory_auron", &sensorFunk1a);
	K_auron* k1 = new K_auron("k1");
	new K_synapse(Ks1, k1, true);

//	#else 	// 	KANN-Test
	 	//{ KANN: TEST-oppsett.
		#if 0

		cout<<"\n\nLAGER KANN\n\n";

		K_auron* K1 = new K_auron("K1" /*, arg2 = 0? */);
		K_auron* K2 = new K_auron("K2" );
		K_auron* K3 = new K_auron("K3" );
	
		K_sensory_auron* KsStatisk = new K_sensory_auron("KsStatisk", &staticSensoryFunc);
		new K_synapse(KsStatisk, K1, 200);
	
	

		K_sensory_auron* Ks1 = new K_sensory_auron( "Ks1", &sensorFunk1 );
		new K_synapse( Ks1, K1, 400, false ); 
	
		K_sensory_auron* Ks2 = new K_sensory_auron( "Ks2", &sensorFunk2 );
		new K_synapse( Ks2, K1, 400, false ); 
		new K_synapse( Ks2, KsStatisk, 333, false);

		new K_synapse( KsStatisk, K1, 300, true );

	
		new K_synapse( Ks1, K2, 400);
		new K_synapse( Ks2, K2, 200);
		new K_synapse( Ks1, K3, 250);
		new K_synapse( Ks2, K3, 100);
		new K_synapse( K2, K3, 100);
		new K_synapse( K1, K3, 200, true);
		new K_synapse( K3, K1, 100);
		new K_synapse( K3, K1, 100, true);

		#endif




		#if 0 // Testoppsett 1, KANN
			K_auron* Ks1 = new K_sensory_auron("Ks1", &sensorFunk_TEST1_s1 );
			K_auron* Ks2 = new K_sensory_auron("Ks2", &sensorFunk_TEST1_s2 );
			K_auron* Ks3 = new K_sensory_auron("Ks3", &sensorFunk_TEST1_s3 );
			K_auron* Ks4 = new K_sensory_auron("Ks4", &sensorFunk_TEST1_s4 );
			K_auron* Ks5 = new K_sensory_auron("Ks5", &sensorFunk_TEST1_s5 );
		
			K_auron* Kt1 = new K_auron("Kt1");
			new K_synapse(Ks1, Kt1, 100);
			new K_synapse(Ks2, Kt1, 100);
			new K_synapse(Ks3, Kt1, 100);
			new K_synapse(Ks4, Kt1, 100);
			new K_synapse(Ks5, Kt1, 50, true);
		
		
			K_auron* Kt2 = new K_auron("Kt2");
			new K_synapse(Ks1, Kt2, 20, true);
			new K_synapse(Ks2, Kt2, 100);
			new K_synapse(Ks3, Kt2, 20, true);
			new K_synapse(Ks4, Kt2, 20, true);
			new K_synapse(Ks5, Kt2, 50);
			new K_synapse(Kt1, Kt2, 200);
			
			K_auron* Kt3 = new K_auron("Kt3");
			new K_synapse(Kt1, Kt3, 200);
			new K_synapse(Kt2, Kt3, 200);
		#endif

	//} Slutt KANN-testopplegg
	#endif
//}






	cout<<"******************************************\n***  START RUN OF auroSIM : ****\n******************************************\n\n";

/******************************************* Call taskSchedulerFunction(void*); ****************************************************/
	taskSchedulerFunction(0);
/***********************************************************************************************************************************/


	cout<<"\n\n\nXXXXXX Successful run. Complete log and clean up. XXXXX\n\n\n";



//		cout<<"\n\n\n\nPrint all elements of i_auron:\n";
//		for( std::list<i_auron*>::iterator iter = i_auron::pAllAurons.begin() ;  iter != i_auron::pAllAurons.end() ;  iter++ )
//		{
//			cout<<"\t[ " <<(*iter)->sNavn <<" ]\t\t" 		<<"\tdEstimatedTaskTime :\t" <<(*iter)->dEstimatedTaskTime 
//				<<endl;
//		}
//		cout<<"\n\n";
	
	#if DEBUG_PRINT_LEVEL>4
		/* TODO Lag en egen avsluttingsfunksjon(og registrer den med atexit(void (*)(void)).). Den skal gjøre følgende:*/
		time_class::printAllElementsOf_pWorkTaskQueue();
		time_class::printAllElementsOf_pPeriodicElements();
	#endif


	cout<<"\n\n----------------------------\nEnd of main()\n----------------------------\n";

	return 0;
}


/*****************************************************************
** void printArgumentConventions() 								**
** 	-arguments: void 											**
** 	-return: 	void 											**
** 	Funtion: 	prints out argument convention for auroSim 		**
*****************************************************************/
void printArgumentConventions(std::string programCall)
{ //{
	cout <<"\nConventions for executing auroSim: \n"
		 <<"\t"<<programCall <<" [-options]\n"
		 <<"\t\tOptions: \n\t\t\t-r [n] \t number of iterations per forcing function period."
		 <<"\t\t\n           \t\t-n [n] \t float number of periods of sensor function(e.g. one half period can be called by -n0.5)"
		 <<"\n\n\n\n\n";
} //}


/*****************************************************************
** void initialzeWorkTaskQueue() 								**
** 	-arg: 	void 												**
** 	-ret: 	void 												**
** 	Funtion: 													**
** 		Initialize pWorkTaskQueue by inserting an object of  	** 
** 		  time_class(called timeSepartionObj in the report). 	**
*****************************************************************/
void initialzeWorkTaskQueue()
{ //{1
	// Guard to prevent reinitialization(only one object of time_class is needed in pWorkTaskQueue)
	static bool bPreviouslyInitialized = false;
	if(bPreviouslyInitialized)
		return;
	 
	// Push pointer to element of time_class, allocated in the free store:
 	time_class::pWorkTaskQueue 	.push_back( new time_class() );

	// This element will be inserted by its doTask() funtion. Unless illegal access removes it, this cause timeSepartionObj to remain in pWorkTaskQueue
	// 	until the simulation terminates.

	// Set the static funtion variable bPreviouslyInitialized to [true] to prevent reinitialization.
	bPreviouslyInitialized = true;
} //}1


/*****************************************************************
** void* taskSchedulerFunction(void*)							**
** 	-arg: 	void*												**
** 	-arg: 	void*												**
** 	Funtion: 													**
** 		(Have the form required by pthread to be ready for  	**
** 		implementation of auroSim with concurrency). 			**
** 		Main loop of auroSim: 									**
** 		while(bContinueExecution == true) : 					**
** 			- pop first element of pWorkTaskQueue 				**
** 			- call this element's doTask() function 			**
** 																**
*****************************************************************/
void* taskSchedulerFunction(void* )
{ //{1
	
	/****************************
	**  Initialize Run: 	   **
	****************************/

	// Initialize time:
	time_class::ulTime = 0;

	// Initialize pWorkTaskQueue by inserting one, and only one, object of time_class:
	// ( limiting to one is handled by initialzeWorkTaskQueue() )
	initialzeWorkTaskQueue();

	// DEBUG: Print all K_auron-objects:
	cout<<"Prints all pAlleKappaAuron objects\n";
	for( std::list<K_auron*>::iterator iter = K_auron::pAllKappaAurons.begin(); iter != K_auron::pAllKappaAurons.end(); iter++ )
		cout<<"\titer->sNavn: " <<(*iter)->sNavn <<endl;

	// Initialize first 'time window' for all K_aurons:
	for( std::list<K_auron*>::iterator K_iter = K_auron::pAllKappaAurons.begin(); K_iter != K_auron::pAllKappaAurons.end(); K_iter++ )
	{
		// Initialize 'time window'(set v_0 and t_0 to [now]):
		(*K_iter)->dDepolAtStartOfTimeWindow = 0;
		(*K_iter)->dStartOfTimeWindow = (double)time_class::getTime();
		// Compute the activation level's corresponding period for all K_aurons:
		(*K_iter)->doCalculation();
	}

	// Initialize first 'time window' for all K_sensory_aurons:
	for( std::list<K_sensory_auron*>::iterator K_iter = K_sensory_auron::pAllSensoryAurons.begin(); K_iter != K_sensory_auron::pAllSensoryAurons.end(); K_iter++ )
	{
		// Initialize first 'time window':
		(*K_iter)->updateSensoryValue() ;
		(*K_iter)->dStartOfTimeWindow = (double)time_class::getTime();
		// Compute the resulting period for the forcing funtions input flow:
		(*K_iter)->doCalculation();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * 
	 * * * * * * * * auroSim's main loop * * * * * * * * *
	 * * * * * * * * * * * * * * * * * * * * * * * * * * */
	while( bContinueExecution )
	{
		// Pop first element before execution of its doTask():
		// 	(to keep pWorkTaskQueue free of this element during execution of its task)
		static timeInterface* pConsideredElementForThisIteration;
		pConsideredElementForThisIteration = time_class::pWorkTaskQueue.front();

		// Remove element from pWorkTaskQueue:
		time_class::pWorkTaskQueue.pop_front();

		// Execute Task:
		pConsideredElementForThisIteration->doTask();

// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO Fiks dårligere depol-kurve, eller skriv om rapport TODO TODO TODO TODO
#if 0 //GAMMEL (til 10.02.1012) 

		// Setter igang utføring av neste jobb i lista:
		time_class::pWorkTaskQueue.front() ->doTask(); 		//Dette er i orden, siden pWorkTaskQueue er av type list<timeInterface*> og alle arvinger av timeInterface har overlagra funksjonen doTask().

		// Tar vekk jobben fra pWorkTaskQueue:
		time_class::pWorkTaskQueue.pop_front();
			
		//Evt annet som skal gjøres kvart timessteg. Type sjekke etter andre events, legge til fleire synapser, etc.
#endif
	}

	return 0;
} //}1


// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO  Rydd opp! Fjærn alt som ikkje brukes! TODO TODO TODO TODO TODO  
/*************************
*** Print procedures:  ***
*************************/
std::ostream & operator<< (std::ostream & ut, i_auron* pAuronArg )
{ //{
	// For now: Print the adress in the i_auron pointer:
	ut<<(void*)pAuronArg;

//	ut<<"| " <<pAuronArg->getNavn() <<"  | verdi: " <<pAuronArg->getAktivityVar();// <<" \t|\tMed utsynapser:\n";
	
	// Innsynapser:
	//for( std::vector<synapse*>::iterator iter = neuroArg.pInnSynapser.begin(); iter != neuroArg.pInnSynapser.end(); iter++ ){
	// 	ut 	<<"\t" <<(*iter)->pPreNode->navn <<" -> " <<neuroArg.navn <<"\t|" <<endl;
	// }

	// Utsynapser:
	//for( std::vector<synapse*>::iterator iter = neuroArg.pUtSynapser.begin(); iter != neuroArg.pUtSynapser.end(); iter++ ){
	// 	ut 	<<"\t\t\t|\t" <<neuroArg.navn <<" -> " <<(*iter)->pPostNode->navn <<endl;
	//		//<< (*iter)->ulAntallSynapticVesiclesAtt <<" antall syn.vesicles att.  TIL "
	// }


	return ut;
} //}



// TODO TODO  Ta vekk ?
std::ostream & operator<< (std::ostream & ut, s_axon* pAxonArg ) //XXX Skal gjøres til i_axon* istaden for s_axon* som argument! XXX
{ //{
	ut<<"Utsynapser fra axon tilhørende neuron " <<(pAxonArg->pElementOfAuron)->sNavn <<endl; 

	// Utsynapser:
		//			TODO gjør om x++ til ++x, siden ++x slepper å lage en "temporary".
	for( std::list<s_synapse*>::iterator iter = pAxonArg->pUtSynapser.begin(); iter != pAxonArg->pUtSynapser.end(); iter++ ){
	 	ut 	<<"\t\t\t|\t" <<(pAxonArg->pElementOfAuron)->sNavn <<" -> "    <<(*iter)->pPostNodeDendrite->pElementOfAuron->sNavn <<"\t\t|\n";
	}


	return ut;
} //}





// vim:fdm=marker:fmr=//{,//}
