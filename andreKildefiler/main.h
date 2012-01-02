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
//#include <vector>
#include <list>
#include <map>
#include <vector>
//#include <set>


#include <stdlib.h> 	//- for rand()
#include <cmath> 		//- for pow() funksjoner
//include <algorithm> 	- for_each()
//include <fstream> 	- file streams




extern unsigned long ulTemporalAccuracyPerSensoryFunctionOscillation;
extern int nNumberOfSensorFunctionOscillations;
extern unsigned long ulTotalNumberOfIterations;


//Defines:

#define GCC false // Må til for å bruke iomanip.h

// Velger KANN eller SANN for neste compilasjon
#define KANN 1
#define SANN 1

#define DEFAULT_ANTALL_TIDSITERASJONER 1000
#define DEFAULT_NUMBER_OF_SENSOR_FUNKTION_OSCILLATIONS 1 // ANtall ganger sinus-funksjonen skal gjøre full gjennomgang.
//#define MIN_PERIODE_MELLOM_REKALKULERING_AV_KAPPA 100

#define RECALC_c1 100 
#define RECALC_c2 250
#define RECALC_c3 10
#define RECALC_c4 0.5


#define LOGG_DEPOL false 	// Denne gir om implementasjonen skal skrive til depol-logg.
#define LOGG_KAPPA true 	// ...samma for kappa

//#define T 						(double)0.01 					//[ms]: tidsiterasjoner
//#define ALPHA   				(double)1  //[verdi]: Lekkkasje per tidsiterasjon = [lengde på tidsiter]*[lekkasjefaktor]

#define ALPHA   				(double)10  //[verdi]: Lekkkasje per tidsiterasjon = [ALPHA]/[ulTemporalAccuracyPerSensoryFunctionOscillation]
#define LEKKASJE_KONST   		(double) (ALPHA/ulTemporalAccuracyPerSensoryFunctionOscillation)
#define LEKKASJEFAKTOR_FOR_DEPOL (double)(1-LEKKASJE_KONST)

#define FYRINGSTERSKEL 1000.00000


#define OCTAVE_SLEEP_ETTER_PLOTTA 0



#define UTSKRIFT_AV_TID 1
#define UTSKRIFT_AV_TID_KVAR_Nte_ITER 100000

#define DOT_ENTER_UTSKRIFT_AV_TID 	0 // Betyr enter kvar n'te iter. (der n er tall som blir def.). Definer til 0 eller false for å slå av.
#define DEBUG_UTSKRIFTS_NIVAA 		0
#define DEBUG_SKRIV_UT_DESTRUCTOR 	0
#define DEBUG_SKRIV_UT_CONSTRUCTOR 	0



#define PI 3.14159265

#define DEBUG_EXIT(tekst) cerr<<"\n\nDEBUG_EXIT :\t" <<tekst <<"\nexit(99);\n\n"; exit(99);
//#define DEBUG(tekst) cerr<<"DEBUG :\t" <<tekst <<"\n";
#define DEBUG(tekst) ;


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
/*
#ifndef DENDRITE_H_
#include "../neuroElements/dendrite.h"
#endif*/

using std::cerr;
using std::cout;
using std::endl;





// deklarasjoner:

extern bool bContinueExecution;


class comparisonClass{
	public:
	static unsigned long ulNumberOfCallsToKappa_doCalculations;
	static unsigned long ulNumberOfCallsTo_doTask;
};






/*
	simulert, diskret TID
*/
//unsigned long ulTime; // ligg i main.h
// flytta inn som static time_class::ulTime;







#endif
// vim:fdm=marker:fmr=//{,//}
