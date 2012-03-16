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
#include <iostream>
#include <string>
#include <list>
#include <map>
#include <vector>

#include <stdlib.h> 	//- for rand() functions
#include <cmath> 		//- for pow() functions 
//include <fstream> 	//- file streams

// For string-strems (to convert int, float, etc to string..)
#include <sstream> 

// For DBL_MAX: The maximal value for double data type.
#include <float.h>

// Variable declarations:
extern unsigned long ulTemporalAccuracyPerSensoryFunctionPeriod;
extern int nNumberOfSensorFunctionOscillations;
extern unsigned long ulTotalNumberOfTimeSteps;
extern unsigned uNumberOfIterationsBetweenWriteToLog;


//Defines:

#define GCC false // iomanip.h can not be used with compiler clang++. Define flag GCC when gcc is used: this enables the use of setprecision() for cout of floats.

// Defines whether KANN(kappa M-ANN) is used, and if SANN(NIM-ANN) is used for next compilation of auroSim
#define KANN 1
#define SANN 0

// Debug print level and the printing of time:
#define DEFAULT_NUMBER_OF_SENSORY_FUNCTION_PERIODS 1.5 // number of forcing funtion periods.
#define DEFAULT_NUMBERofTIMESTEPS 	1000000 				 // default temporal resolution, each forcing funtion period.

#define PRINT_TIME_ITERATION 		true  //means [true]
#define PRINT_TIME_EVERY_Nth_ITER 	20000


// Define the form of K-recalculation-curve (appendix B3 in the technical report)
#define RECALC_c1 100 
#define RECALC_c2 250
#define RECALC_c3 10
#define RECALC_c4 0.5

#define PI 3.14159265


#define ALPHA   				(double)10  //[value]: Leakage constant (1-l)*1000, to be multiplied to v(t_n) to find (per mille) leakage at time t_n. 
#define LEAKAGE_CONST   		(double) (ALPHA/ulTemporalAccuracyPerSensoryFunctionPeriod)
//#define LEAKAGE_FACTOR_FOR_DEPOL (double)(1-LEAKAGE_CONST)

#define FIRING_THRESHOLD 1000.00000




/*****************************
** 	Set which var. is logged**
*****************************/
#define LOG_DEPOL true 		// Defines whether depolarization is to be logged by the auroSim
#define LOGG_KAPPA false 		// ... same for Kappa
#define LOGG_RESOLUTION 10000

#define OCTAVE_SLEEP_AFTER_PLOTTING 0


/*************************
** 	Debug print level 	**
*************************/
#define DEBUG_PRINT_LEVEL 			0 	// Debug print level: 0 means that no additional information is printed.
#define DEBUG_PRINT_DESCTRUCTOR 	0 	// Print in destructors if this is set to true (=1)
#define DEBUG_PRINT_CONSTRUCTOR 	0 	// Print in constructors if this is set to true (=1)


/***** DEBUG_L?(<<text): prints [text] if DEBUG_PRINT_LEVEL is ? or larger *******/  
/***  :  ***/
// debug level 1: does nothing unless DEBUG_PRINT_LEVEL is larger than 0
#if DEBUG_PRINT_LEVEL>0
	#define DEBUG_L1(TEXT_IN_std_ostream_FORMATE) cout<<"[DEBUG L1]:\t" TEXT_IN_std_ostream_FORMATE <<"\n" 	;
#else
	#define DEBUG_L1(X) ;
#endif
// debug level 2: does nothing unless DEBUG_PRINT_LEVEL is larger than 1
#if DEBUG_PRINT_LEVEL>1 
	#define DEBUG_L2(TEXT_IN_std_ostream_FORMATE) cerr<<"\t[DEBUG L2]:\t" TEXT_IN_std_ostream_FORMATE <<"\n" ;
#else
	#define DEBUG_L2(X) ;
#endif
// debug level 3: does nothing unless DEBUG_PRINT_LEVEL is larger  than 2
#if DEBUG_PRINT_LEVEL>2
	#define DEBUG_L3(TEXT_IN_std_ostream_FORMATE) cerr<<"\t\t[DEBUG L3]:\t" TEXT_IN_std_ostream_FORMATE <<"\n";
#else
	#define DEBUG_L3(X) ;
#endif
// debug level 4: does nothing unless DEBUG_PRINT_LEVEL is larger than 3
#if DEBUG_PRINT_LEVEL>3
	#define DEBUG_L4(TEXT_IN_std_ostream_FORMATE) cerr<<"\t\t\t[DEBUG L4]:\t" TEXT_IN_std_ostream_FORMATE <<"\n";
#else
	#define DEBUG_L4(X) ;
#endif
// debug level 5: does nothing unless DEBUG_PRINT_LEVEL is larger than 4
#if DEBUG_PRINT_LEVEL>4
	#define DEBUG_L5(TEXT_IN_std_ostream_FORMATE) cerr<<"\t\t\t\t[DEBUG L5]:\t" TEXT_IN_std_ostream_FORMATE <<"\n";
#else
	#define DEBUG_L5(X) ;
#endif


#if GCC
	#include <iomanip> // For setprecision()   FUNKER IKKJE FOR clang++-kompilatoren..
#endif


#ifndef MAIN_H_
#define MAIN_H_

#include "../neuroElements/auron.h"
#include "time.h"

#ifndef SYNAPSE_H_
#include "../neuroElements/synapse.h"
#endif

using std::cerr;
using std::cout;
using std::endl;

// declarations:
extern bool bContinueExecution;



#endif
// vim:fdm=marker:fmr=//{,//}
