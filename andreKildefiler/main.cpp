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
#include "sensorFunk.h"


#include "../andreKildefiler/ANN.h"

void initialiserArbeidsKoe();
void skrivUtArgumentKonvensjoner(std::string);
void* taskSchedulerFunction(void*);

//deklarasjoner
extern std::list<timeInterface*> 				time_class::pWorkTaskQue;
extern std::list<timeInterface*> 				time_class::pCalculatationTaskQue;
extern std::list<timeInterface*> 				time_class::pPeriodicElements;

extern std::list<i_auron*> i_auron::pAllAurons;
extern std::list<K_auron*> K_auron::pAllKappaAurons;
extern std::list<K_sensor_auron*> K_sensor_auron::pAllSensorAurons;
extern std::list<s_sensor_auron*> s_sensor_auron::pAllSensorAurons;

extern unsigned long time_class::ulTime;

unsigned long ulTemporalAccuracyPerSensoryFunctionOscillation = 	DEFAULT_ANTALL_TIDSITERASJONER;
float fNumberOfSensorFunctionOscillations = 						DEFAULT_NUMBER_OF_SENSOR_FUNKTION_OSCILLATIONS;
unsigned long ulTotalNumberOfIterations = 						ulTemporalAccuracyPerSensoryFunctionOscillation*fNumberOfSensorFunctionOscillations;
unsigned uNumberOfIterationsBetweenWriteToLog; // Declared in main.h, initialized in 


std::ostream & operator<<(std::ostream& ut, i_auron* pAuronArg );

bool bContinueExecution = true; // Sjå main.h

bool bLogLogExecution = false; 	// For å lage log/log plot av feil: sett denne til true
int nResolutionInLogLogErrorPlot;



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
//	static int nTidsgrenseForSimulering = DEFAULT_ANTALL_TIDSITERASJONER;
//	return nTidsgrenseForSimulering;
//}



int main(int argc, char *argv[])
{




	//Leser inn argumenter:  //{1
	if(argc > 1 ) //{ 	  //	 (argc>1 betyr at det står meir enn bare programkall)
	{
		int innArgumentPos = 0;
		bool bWriteOutCallingConventions = false;

		while( ++innArgumentPos < argc ){
			
			if(argv[innArgumentPos][0] == '-'){

				cerr<<"argv[" <<innArgumentPos <<"] == " <<argv[innArgumentPos] <<endl;
				switch( argv[innArgumentPos][1] ){
					case 'h':
						skrivUtArgumentKonvensjoner(argv[0]);
						exit(0);
						break;
					case 'L':
						bLogLogExecution = true;
						if( (nResolutionInLogLogErrorPlot =  atoi( &argv[innArgumentPos][2])) ){
							cout<<"log/log error plot with " <<nResolutionInLogLogErrorPlot <<" entries.\n"; 
							cerr<<"NOT IMPLEMENTED YET.\n";
						}else{
							cout<<"Can not read argument. Please follow the conventions:" <<endl;
							bWriteOutCallingConventions = true;
							//skrivUtArgumentKonvensjoner(argv[0]);
							//exit(-1);
						}
						break;
					case 'r':
						// Sjekker om antall iterasjoner er i samme argument (uten mellomrom):
						if( 		(ulTemporalAccuracyPerSensoryFunctionOscillation = atoi( &argv[innArgumentPos][2])) ) 	
							cout<<"Temporal resolution set to " <<ulTemporalAccuracyPerSensoryFunctionOscillation <<" (time steps per sensory function period).\n";
						// Ellers: sjekker om det er på neste argument (med mellomrom):
						else if( 	(ulTemporalAccuracyPerSensoryFunctionOscillation = atoi( argv[innArgumentPos+1]) ) ){
							++innArgumentPos;
							cout<<"Anntall tidsiterasjoner er satt til " <<ulTemporalAccuracyPerSensoryFunctionOscillation <<" per sensor funtion oscillation\n";
						}else{
							cout<<"Can not read argument. Please follow the conventions!\tIgnores argument." <<endl;
							bWriteOutCallingConventions = true;
							//skrivUtArgumentKonvensjoner(argv[0]);
							//exit(-1);
						}
						break;
					case 'n':
						if( 		(fNumberOfSensorFunctionOscillations = atof( &argv[innArgumentPos][2])) ) 	
							// Sjekker om antall oscillasjoner er i samme argument (uten mellomrom):
							cout<<"Number of forcing function oscillations set to be " <<fNumberOfSensorFunctionOscillations <<"\n";
							// Ellers: sjekker om det er på neste argument (med mellomrom):
						else if( 	(fNumberOfSensorFunctionOscillations = atof( argv[innArgumentPos+1]) ) ){
							++innArgumentPos;
							cout<<"Number of forcing function oscillations set to be " <<fNumberOfSensorFunctionOscillations <<"\n";
						}else{
							cout<<"Can not read argument. Please follow the conventions! \tIgnores argument." <<endl;
							//skrivUtArgumentKonvensjoner(argv[0]);
							bWriteOutCallingConventions = true;
							//exit(-1);
							//continue;
						}

						if( fNumberOfSensorFunctionOscillations < 0){
							cout<<"Number of sensory function oscillations set to an invalid number (" <<fNumberOfSensorFunctionOscillations <<"). Try again.\n";
							exit(-1);
						}
						
						break;
					default:
						cout<<"\nFEIL INPUT\n\n";
						break;
				}
			}else{
				cout<<"Argument " <<innArgumentPos+1 <<" unrecognized: " <<argv[innArgumentPos] <<".\n";
				cout<<"Please follow calling convenctions: \n\n";
				//skrivUtArgumentKonvensjoner(argv[0]);
				bWriteOutCallingConventions=true;
			}
		} //}
				

		//skrivUtArgumentKonvensjoner(argv[0]); skriver ut en slags hjelpe-tekst: kva som er lov å skrive som argument..
		if( bWriteOutCallingConventions ){
			skrivUtArgumentKonvensjoner(argv[0]);
		}


		cout<<"\n";

	}else{ //  if(argc > 1) : else
		cout<<"No arguments listed. Continue with default values:\tNumber of iterations: " <<DEFAULT_ANTALL_TIDSITERASJONER <<endl;
		ulTemporalAccuracyPerSensoryFunctionOscillation = DEFAULT_ANTALL_TIDSITERASJONER;

		skrivUtArgumentKonvensjoner(argv[0]);
	} 

	// Setter totalt antall iterasjoner:
	ulTotalNumberOfIterations = fNumberOfSensorFunctionOscillations * ulTemporalAccuracyPerSensoryFunctionOscillation;

	// Set uNumberOfIterationsBetweenWriteToLog (to restrict number of points in log file)
	if(ulTemporalAccuracyPerSensoryFunctionOscillation > LOGG_OPPLOYSING){
		uNumberOfIterationsBetweenWriteToLog = (int)(((float)ulTemporalAccuracyPerSensoryFunctionOscillation / (float)LOGG_OPPLOYSING) +0.5);
		cout<<"Restricting number of entries in log file: Write log every " <<uNumberOfIterationsBetweenWriteToLog <<" iteration.\n";
	}else{
		cout<<"No nead to restrict number of log entries due to few iterations.\n\n";
	}
	cout<<endl;
	//}1
	// Ferdig med å lese inn argumenter
	

	// Returverdien på systemkallet returnerer -1 (eller andre feilmeldinger) ved feil og 0 når det går bra.
	// Dersom ./datafiles_for_evaluation/ ikkje finnes, lages den. Dersm den finnes gjør ikkje kallet noke:
	if( system("mkdir datafiles_for_evaluation") != 0 ){
		cout<<"Could not make directory for log files [./datafiles_for_evaluation/]."
			<<"\n\tIn case this directory does not exist, please make this directory"
		   <<"\n\tmanually in the current directory.\n\n"; 
	}
	//Renser opp i ./datafiles_for_evaluation/
	if( system("rm ./datafiles_for_evaluation/log_*.oct") != 0)
		cout<<"Could not remove old log files. Please do this manually to avoid accidendtally plotting old results.\n";

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
	 			ulTemporalAccuracyPerSensoryFunctionOscillation = 100*pow(2,i);
				// Lager auron:
				K_sensor_auron KN("KN", &dynamiskSensorFunk);
				s_sensor_auron SN("SN", &dynamiskSensorFunk);

				// Starter kjøring:
				bContinueExecution = true; 
				cout 	<<"\nSTARTER kjøring nummer " <<i 
						<<" med temporalAccuracy " <<ulTemporalAccuracyPerSensoryFunctionOscillation <<".\tStarter på tid:" <<time_class::ulTime <<endl;
				// ********* taskSchedulerFunction(0) ***********
				taskSchedulerFunction(0);

				cout<<"sluttTid: " <<time_class::ulTime <<"\tdepol for KN og SN:\t[" <<KN.getCalculateDepol() <<", " <<SN.dAktivitetsVariabel <<"]\n";
				cout<<"Siste fyringstid for SN:" <<SN.ulTimestampForrigeFyring <<endl;

				// Skriv slutt-verdi til logg-fil for log/log-plot (for KN og SN: lag to kolonner i data matrisa)
				LogLog_logFile 	<<KN.getCalculateDepol() <<", "
								<<SN.dAktivitetsVariabel <<";\n";
//				delete KN;
//				delete SN;
			
			}
			
			// Avslutter LogLog-datafil:
			LogLog_logFile<<"]; \n";


	}else{ 						// Skal kjøre på vanlig måte.

		// Testoppsett:
	// Blanda:


	#if 0 //Setter opp test for å teste einskild neuron:
		//STATISK
		#if 1
			new K_sensor_auron("_sKN", &statiskSensorFunk);
			new s_sensor_auron("_sSN", &statiskSensorFunk);
		#endif

		//LINEÆRT ØKANDE DEPOL. VELOCITY 
		#if 0
			new K_sensor_auron("_dKN", &linearilyIncreasingDepolVelocity);
			new s_sensor_auron("_dSN", &linearilyIncreasingDepolVelocity);
		#endif

		//DYNAMISK
		#if 1
			new K_sensor_auron("_dKN", &dynamiskSensorFunk);
			new s_sensor_auron("_dSN", &dynamiskSensorFunk);
		#endif
	#else



	#if 0
		// Tester class ANN:
		cout<<"TESTER ANN\n";


		QuadraticMatrix<double> A(2,"A");
	
		A(0,1)= 0.5;
		cout<<"Lager kantmatrise:\n\n";
		A.printMatrix();

		cout<<"Lager ANN\n";
		ANN<K_auron> Ktest(A);

	 	cout<<"SKRIVER UT omega-matrise\n";
		Ktest.printEdgeMatrix();

	cout<<"Win!\n";
cout<<"\n\nSLUTTER asdf1235@maion.cpp\n\n";
//exit(0);
	#endif


#if 1
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
#else
	cout<<"TESTER\n"; 
	K_sensor_auron 	DKs1("_dKsN", &statiskSensorFunkMedHoegAktivitet);
	K_auron 		DK1("_dKN");
	new K_synapse(&DKs1, &DK1, 1);
//exit(0); // OK_ Dette funker ikkje. Da er det ikkje matrise-shitten iverftall!!
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

	

		cout<<"\n\n\n\nSkriver ut alle auron: \t\t";
		for( std::list<i_auron*>::iterator iter = i_auron::pAllAurons.begin() ;  iter != i_auron::pAllAurons.end() ;  iter++ )
		{
			cout<<"[ " <<(*iter)->sNavn <<" ]\t";
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


void skrivUtArgumentKonvensjoner(std::string programKall)
{ //{
	cout <<"\n\nConventions for executing auron.out: \n"
		 <<"\t"<<programKall <<"[-options] [number of iterations]\n"
		 <<"\t\tOptions: \n\t\t\t-r [n] \t number of iterations per sensor function oscillation."
		 <<"\t\t\n           \t\t-n [n] \t number of periods for sensor function - one half oscillation given by -n0.5"
		 <<"\t\t\n           \t\t-L [n] \t make log/log plot of error for [100:100*2^[n]] time iterations."
		 <<"\n\n\n\n\n";
} //}


/*****************************************************************
** 	void initialiserArbeidsKoe() 								**
**		-skal kjøre en gang(kun 1) for å initialisere arbeidskø.**
**		Ligger som egen funksjon for å få ryddig kode i main()	**
*****************************************************************/
void initialiserArbeidsKoe()
{ //{1
	// Sjekker om arbeidskø er initialisert fra før, eller: Slett tidligare element!
	while( ! time_class::pWorkTaskQue.empty() ){
		time_class::pWorkTaskQue.pop_front();
	}
	 

	
	// Lager instans av time, og legger den i det frie lageret.
//	time_class* pHovedskille = new time_class();
	// Legger til denne peikeren i arbeidskøa (som ligger som static-element i class time) :
// 	time_class::pWorkTaskQue 	.push_back( pHovedskille );
 	time_class::pWorkTaskQue 	.push_back( new time_class() );

	// No ligger peikeren pHovedskille som einaste element i pWorkTaskQue. Kvar gang denne kjører doTask() vil den ikkje fjærne seg fra arbeidsliste, men flytte seg bakerst i køa isteden.

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

	// Initierer arbeidskø (time_class::pWorkTaskQue)   Om dette er gjort før, returnerer den bare..
	initialiserArbeidsKoe();

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
		(*K_iter)->dStartOfTimeWindow = (double)time_class::getTid();
		// Regner ut resulterende periode, osv.
		(*K_iter)->doCalculation();
	}

	// Initialiserer 'time window' for alle K_sensor_auron:
#if 0 	
	K_sensor_auron::updateAllSensorAurons();
#else
	for( std::list<K_sensor_auron*>::iterator K_iter = K_sensor_auron::pAllSensorAurons.begin(); K_iter != K_sensor_auron::pAllSensorAurons.end(); K_iter++ )
	{
		// Initierer 'time window':
			// (*K_iter)->doTask();    // Løsninga for FDP: dette skaper en spike ved t=0 i plot av depol.
		// Setter v_0 til 0 og t_0 til [no]:
		(*K_iter)->updateSensorValue() ;
		(*K_iter)->dStartOfTimeWindow = (double)time_class::getTid();
		// Regner ut resulterende periode, osv.
		(*K_iter)->doCalculation();
	}
#endif

	/* * * * * * * * Begynner vanlig kjøring av auroNett * * * * * * * * */

	// Hoved-loop:
	while( bContinueExecution )
	{

		// Setter igang utføring av neste jobb i lista:
		time_class::pWorkTaskQue.front() ->doTask(); 		//Dette er i orden, siden pWorkTaskQue er av type list<timeInterface*> og alle arvinger av timeInterface har overlagra funksjonen doTask().

		// Tar vekk jobben fra pWorkTaskQue:
		time_class::pWorkTaskQue.pop_front();
			
		//Evt annet som skal gjøres kvart timessteg. Type sjekke etter andre events, legge til fleire synapser, etc.
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
