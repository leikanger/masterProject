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
// 			s_auron funker ikkje lenger! TODO TODO
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
extern std::list<K_sensor_auron*> K_sensor_auron::pAllSensoryAurons;
extern std::list<s_sensor_auron*> s_sensor_auron::pAllSensoryAurons;

extern unsigned long time_class::ulTime;

unsigned long ulTemporalAccuracyPerSensoryFunctionPeriod 	=	DEFAULT_NUMBERofTIMESTEPS;
float fNumberOfSensoryFunctionPeriods 						=	DEFAULT_NUMBER_OF_SENSORY_FUNCTION_PERIODS;
unsigned long ulTotalNumberOfTimeSteps 						= 	ulTemporalAccuracyPerSensoryFunctionPeriod*fNumberOfSensoryFunctionPeriods;
unsigned uNumberOfIterationsBetweenWriteToLog;

// The program exits gracefully when bContinueExecution is set to [false]:
bool bContinueExecution = true;

bool bLogLogExecution = false; 	// For å lage log/log plot av feil: sett denne til true 	TODO TODO TODO TA VEKK! TODO TODO TODO
int nResolutionInLogLogErrorPlot; // TA VEKK 												TODO TODO TODO TA VEKK! TODO TODO TODO

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


/*****************************************
 **  int main(int argc, char *argv[]) 	**
 ** 	Reads in arguments from shell,  **
 ** 	and  TODO Skriv ferdig.. 		**
 ****************************************/
int main(int argc, char *argv[])
{
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
					case 'L': 		// TODO TODO TODO TA VEKK! TODO TODO TODO 
						bLogLogExecution = true;
						if( (nResolutionInLogLogErrorPlot =  atoi( &argv[nInputArgumentPos][2])) ){
							cout<<"log/log error plot with " <<nResolutionInLogLogErrorPlot <<" entries.\n"; 
							cerr<<"NOT IMPLEMENTED YET.\n";
						}else{
							cout<<"Can not read argument. Please follow the conventions:" <<endl;
							bPrintCallingConventions = true;
							//printArgumentConventions(argv[0]);
							//exit(EXIT_FAILURE);
						}
						break; 		// TODO TODO TODO TA VEKK:slutt TODO TODO TODO 
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
	if(ulTemporalAccuracyPerSensoryFunctionPeriod > LOGG_OPPLOYSING){
		uNumberOfIterationsBetweenWriteToLog = (int)(((float)ulTemporalAccuracyPerSensoryFunctionPeriod / (float)LOGG_OPPLOYSING) +0.5);
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

	// TODO TODO TODO TODO TODO TODO  TA VEKK! TODO TODO TODO TODO TODO 
//TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO 
	// Ny mekanisme: Lag log/log errorplot datafil. Kjører simuleringe med [100 : 100*2^nResolutionInLogLogErrorPlot] tidssteg. Lagrer slutt-verdien for depol. verdien.
	if( bLogLogExecution ){ 	// Skal kjøre for å generere datafil til log-log error plot.

			cout<<"FUNKER IKKJE ENDA. Avslutter!\n\n"; exit(0);

			// TODO : Lag en egen logge-fil for LogLogError-datafila.
			// Lager logge-fil (der data skal skrives).
			std::ofstream LogLog_logFile;
			std::ostringstream tempFilAdr;
			tempFilAdr<<"./datafiles_for_evaluation/log_LogLog_data_file" <<".oct";
	
			std::string tempStr( tempFilAdr.str() );
	
			// need c-style string for open() function:
			LogLog_logFile.open( tempStr.c_str() );

			LogLog_logFile<< "# Log/Log datafil med slutt-depol. ved ei kjøring med [100 : 100*(2^" <<nResolutionInLogLogErrorPlot <<")] iterasjoner.\n";
			LogLog_logFile<<"data=[";
			LogLog_logFile.flush();




			for( int i=0; i<nResolutionInLogLogErrorPlot; i++ )
			{
				// Resetter Tid
				time_class::ulTime = 0;
				// Setter temporalAccuracy:
	 			ulTemporalAccuracyPerSensoryFunctionPeriod = 100*pow(2,i);
				// Lager auron:
				K_sensor_auron KN("KN", &dynamiskSensorFunk);
				s_sensor_auron SN("SN", &dynamiskSensorFunk);

				// Starter kjøring:
				bContinueExecution = true; 
				cout 	<<"\nSTARTER kjøring nummer " <<i 
						<<" med temporalAccuracy " <<ulTemporalAccuracyPerSensoryFunctionPeriod <<".\tStarter på tid:" <<time_class::ulTime <<endl;
				// ********* taskSchedulerFunction(0) ***********
				taskSchedulerFunction(0);

				cout<<"sluttTid: " <<time_class::ulTime <<"\tdepol for KN og SN:\t[" <<KN.getCalculateDepol(time_class::ulTime) <<", " <<SN.dAktivitetsVariabel <<"]\n";
				cout<<"Siste fyringstid for SN:" <<SN.ulTimestampForrigeFyring <<endl;

				// Skriv slutt-verdi til logg-fil for log/log-plot (for KN og SN: lag to kolonner i data matrisa)
				LogLog_logFile 	<<KN.getCalculateDepol(time_class::getTime()) <<", "
								<<SN.dAktivitetsVariabel <<";\n";
//				delete KN;
//				delete SN;
			
			}
			
			// Avslutter LogLog-datafil:
			LogLog_logFile<<"]; \n";

//TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO  (SLUTT. Ta vekk det over!)

	}else{ 						// Normal execution:

	// Different test cases:

	#if 1 // Tests for a single sensory neuron:
		// Experiment 1:
		#if 0
			new K_sensor_auron("sKN", &statiskSensorFunk);
			new s_sensor_auron("sSN", &statiskSensorFunk);
		#endif

		// Linearily increasing depol. velocity:
		#if 0
			new K_sensor_auron("dKN", &linearilyIncreasingDepolVelocity);
			new s_sensor_auron("dSN", &linearilyIncreasingDepolVelocity);
		#endif

		// Experiment 2:
		#if 1
			new K_sensor_auron("dKN", &dynamiskSensorFunk);
			new s_sensor_auron("dSN", &dynamiskSensorFunk);
		#endif
	#else




			// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO RENSET tekst til hit!

// Lage ANN fra ei kant-matrise, og evt. send inn en vektor med vilkårlig antall auron (mindre enn dim(matrisa)). Kan også sende inn null auron. Vektoren med auron-peikere brukes for å la nokre auron være sensor-auron!
#if 0
	cout<<"\n\n\n\tFORTSETTER\n\n";


	cout<<"Try to construct ANN with K_auron-matrix:\n";

		// TRENGER IKKJE MATRISE: bruk heller std::vector<K_auron*>
	cout<<"\tInitialize K_auron*-vector:\n";
		std::vector<K_auron*> AlleAuron(3);
		//AlleAuron[0] = new K_sensor_auron("Ksensor1",statiskSensorFunkMedHoegAktivitet);
		AlleAuron[0] = new K_sensor_auron("K0Sensor", dynamiskSensorFunk);
		AlleAuron[2] = new K_sensor_auron("K2Sensor", dynamiskSensorFunk2);
	
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
	K_sensor_auron* Ks1 = new K_sensor_auron("K_sensor_auron", &sensorFunk1a);
	K_auron* k1 = new K_auron("k1");
	new K_synapse(Ks1, k1, true);

//	#else 	// 	KANN-Test
	 	//{ KANN: TEST-oppsett.
		#if 0

		cout<<"\n\nLAGER KANN\n\n";

		K_auron* K1 = new K_auron("K1" /*, arg2 = 0? */);
		K_auron* K2 = new K_auron("K2" );
		K_auron* K3 = new K_auron("K3" );
	
		K_sensor_auron* KsStatisk = new K_sensor_auron("KsStatisk", &statiskSensorFunk);
		new K_synapse(KsStatisk, K1, 200);
	
	

		K_sensor_auron* Ks1 = new K_sensor_auron( "Ks1", &sensorFunk1 );
		new K_synapse( Ks1, K1, 400, false ); 
	
		K_sensor_auron* Ks2 = new K_sensor_auron( "Ks2", &sensorFunk2 );
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
			K_auron* Ks1 = new K_sensor_auron("Ks1", &sensorFunk_TEST1_s1 );
			K_auron* Ks2 = new K_sensor_auron("Ks2", &sensorFunk_TEST1_s2 );
			K_auron* Ks3 = new K_sensor_auron("Ks3", &sensorFunk_TEST1_s3 );
			K_auron* Ks4 = new K_sensor_auron("Ks4", &sensorFunk_TEST1_s4 );
			K_auron* Ks5 = new K_sensor_auron("Ks5", &sensorFunk_TEST1_s5 );
		
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






		cout<<"******************************************\n*** BEGYNNER KJØRING AV ANN: ***\n******************************************\n\n";

//		K_sensor_auron::updateAllSensorAurons();

/******************************************* Starter void taskSchedulerFunction(void*); ****************************************************/
		taskSchedulerFunction(0);
/*******************************************************************************************************************************************/


		cout<<"\n\n\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\nAvslutter.\n\n\n";

		#if GCC	
			cout.precision(20);
		#endif
		//cout<<"siste depol. for [SN, KN]: \t[" <<Sd->dAktivitetsVariabel <<", " <<Kd->getCalculateDepol() <<"]\n\n";


		time_class::skrivUt_pWorkTaskQueue();


	

		cout<<"\n\n\n\nSkriver ut alle auron: \t\t";
		for( std::list<i_auron*>::iterator iter = i_auron::pAllAurons.begin() ;  iter != i_auron::pAllAurons.end() ;  iter++ )
		{
			cout<<"[ " <<(*iter)->sNavn <<" ]\t\t" 		<<"\tdEstimatedTaskTime :\t" <<(*iter)->dEstimatedTaskTime 
				<<endl;
		}
		cout<<"\n\n";
	
		#if DEBUG_UTSKRIFTS_NIVAA > 3
			time_class::skrivUt_pPeriodicElements();
		#endif


		// Avlutt alle loggane rett:
		i_auron::callDestructorForAllAurons();

	} // if( bLogLogExecution == false ){ .. }


	cout<<"\n\nWIN!\n\n\n";
	return 0;
}


void printArgumentConventions(std::string programKall)
{ //{
	cout <<"\nConventions for executing auron.out: \n"
		 <<"\t"<<programKall <<" [-options]\n"
		 <<"\t\tOptions: \n\t\t\t-r [n] \t number of iterations per forcing function period."
		 <<"\t\t\n           \t\t-n [n] \t float number of periods of sensor function(e.g. one half period can be called by -n0.5)"
		 <<"\t\t\n           \t\t-L [n] \t make log/log plot of error for [100:100*2^[n]] time iterations." // TODO TODO TODO FJÆRN! TODO TODO TODO
		 <<"\n\n\n\n\n";
} //}


/*****************************************************************
** 	void initialzeWorkTaskQueue() 								**
**		-skal kjøre en gang(kun 1) for å initialisere arbeidskø.**
**		Ligger som egen funksjon for å få ryddig kode i main()	**
*****************************************************************/
void initialzeWorkTaskQueue()
{ //{1
	// Sjekker om arbeidskø er initialisert fra før, eller: Slett tidligare element!
	while( ! time_class::pWorkTaskQueue.empty() ){
		time_class::pWorkTaskQueue.pop_front();
	}
	 

	
	// Lager instans av time, og legger den i det frie lageret.
//	time_class* pHovedskille = new time_class();
	// Legger til denne peikeren i arbeidskøa (som ligger som static-element i class time) :
// 	time_class::pWorkTaskQueue 	.push_back( pHovedskille );
 	time_class::pWorkTaskQueue 	.push_back( new time_class() );

	// No ligger peikeren pHovedskille som einaste element i pWorkTaskQueue. Kvar gang denne kjører doTask() vil den ikkje fjærne seg fra arbeidsliste, men flytte seg bakerst i køa isteden.

	// static bInitialisertAllerede vil eksistere kvar gang denne funksjonen kalles. Setter dermed bInitialisertAllerede til true for å forhindre at fleire time legges til arbeidsKoe.
	//bInitialisertAllerede = true;
} //}1


/*****************************************************************
** 	void* taskSchedulerFunction(void*)							**
** 																**
** 		- Har ansvaret for å schedule arbeidet. 				**
** 			- kaller nesteJobb.doTask()							**
** 			- For nesteJobb==time vil time_class::doTask() kalles 		**
** 				Dette itererer time, og gjør anna tidsrelevant 	**
** 					arbeid										**
** 																**
*****************************************************************/
void* taskSchedulerFunction(void* )
{ //{1
	
	/****************************
	**  Initierer kjøring :    **
	****************************/

	// Initialiserer tid: begynner på iter 1. Dette (t_0=1) er viktig for å få rett initiering av K_auron som begynner med en konst kappa (gir K=(v_0-K)e^(-a*(1-0)) istedenfor K=..*e^0)
	time_class::ulTime = 0;

	// Initierer arbeidskø (time_class::pWorkTaskQueue)   Om dette er gjort før, returnerer den bare..
	initialzeWorkTaskQueue();

	// Skriver ut pAlleKappaAuron:
	cout<<"SKRIVER ut pAlleKappaAuron\n";
	for( std::list<K_auron*>::iterator iter = K_auron::pAllKappaAurons.begin(); iter != K_auron::pAllKappaAurons.end(); iter++ )
		cout<<"\titer->sNavn: " <<(*iter)->sNavn <<endl;

	// Initialiserer 'time window' for alle K_auron:
	for( std::list<K_auron*>::iterator K_iter = K_auron::pAllKappaAurons.begin(); K_iter != K_auron::pAllKappaAurons.end(); K_iter++ )
	{
		// Initierer 'time window':
			// (*K_iter)->doTask();    // Løsninga for FDP: dette skaper en spike ved t=0 i plot av depol.
		// Setter v_0 til 0 og t_0 til [no]:
		(*K_iter)->dDepolAtStartOfTimeWindow = 0;
		(*K_iter)->dStartOfTimeWindow = (double)time_class::getTime();
		// Regner ut resulterende periode, osv.
		(*K_iter)->doCalculation();
	}

	// Initialiserer 'time window' for alle K_sensor_auron:
#if 0 	
	K_sensor_auron::updateAllSensorAurons();
#else
	for( std::list<K_sensor_auron*>::iterator K_iter = K_sensor_auron::pAllSensoryAurons.begin(); K_iter != K_sensor_auron::pAllSensoryAurons.end(); K_iter++ )
	{
		// Initierer 'time window':
			// (*K_iter)->doTask();    // Løsninga for FDP: dette skaper en spike ved t=0 i plot av depol.
		// Setter v_0 til 0 og t_0 til [no]:
		(*K_iter)->updateSensorValue() ;
		(*K_iter)->dStartOfTimeWindow = (double)time_class::getTime();
		// Regner ut resulterende periode, osv.
		(*K_iter)->doCalculation();
	}
#endif

	/* * * * * * * * Begynner vanlig kjøring av auroNett * * * * * * * * */

	// Hoved-loop:
	while( bContinueExecution )
	{

		// Poppe første element før utførelse, for å holde list pWorkTaskQueue fri fra dette elementet før utførelse kvar iterasjon.
		static timeInterface* pConsideredElementForThisIteration;
		pConsideredElementForThisIteration = time_class::pWorkTaskQueue.front();

		// Tar vekk jobben fra pWorkTaskQueue:
		time_class::pWorkTaskQueue.pop_front();

		// Kjør task:
		pConsideredElementForThisIteration->doTask();

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



/***************************
*** Utskriftsprosedyrer: ***
***************************/
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



// TODO TODO 
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
