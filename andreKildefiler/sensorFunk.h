
#include "../neuroElements/auron.h"
#include "../neuroElements/synapse.h"

// Because of the design of the sensory auron, different analytic funtions can be used to assess the simulation models.
// 	This include file is a way of collecting all sensory funtions.
// 	Experiment 1 in the report considers staticSensoryFunc()

// Experiment 1
inline const double& staticSensoryFunc()
{
	// Optimalization: Because this funtion is called every time step, optimalization is imporant.
	// Instead of making a new variable and returning that var.'s value, make a static variable and returt the reference to it.
	static double retVal;
	retVal = 1.100000000000*(double)FIRING_THRESHOLD ;
	return retVal;
}
// Experiment 2
inline const double& dynamicSensoryFunc()
{
//	return (2.1+sin( 2*PI * ( (double)time_class::getTime() / ((double)ulTemporalAccuracyPerSensoryFunctionPeriod) ))   )* (double)FIRING_THRESHOLD;
// ELLER:
	static double retVal;
	retVal = (2.1+sin( 2*PI * ( (double)time_class::getTime() / ((double)ulTemporalAccuracyPerSensoryFunctionPeriod) ))   )* (double)FIRING_THRESHOLD;
	return retVal;
}
// Used to illustrate the sensor funtion in sec. 3.4.1 The Sensory Function
inline const double& sensoryFunctionExample()
{
	static double retVal;
	if( time_class::getTime() <2000)
	 	retVal = (2*FIRING_THRESHOLD *  (1 - cos( 1*3.14*(float)time_class::getTime()/1000 )) );
	else 
	 	retVal = (FIRING_THRESHOLD *  (1 - cos( 2*3.14*(float)time_class::getTime()/1000 )) );

	return retVal;
}


// Denne skaper ikkje større arbeidsmengde for SANN(motsatt av antatt). Dette er nok fordi SANN ikkje får inn synaptisk input enda, men beregner input som KANN gjør det(ved hjelp av sensory funtion ca= kappa)
inline const double& dynamicSensoryFuncWith10Xactivity()
{
	static double retVal;
	retVal = 10* dynamicSensoryFunc();
	return retVal;
}



inline double staticSensoryFuncWithHighActivity(){
	return 3.0*(double)FIRING_THRESHOLD;
}

inline double linearilyIncreasingDepolVelocity()
{
	return FIRING_THRESHOLD + ((double)time_class::getTime() / ulTemporalAccuracyPerSensoryFunctionPeriod) * FIRING_THRESHOLD ;
}

inline double sensorFunk1()
{
	#define SVINGNINGS_AMP 2.133
	return ( 1.1 * FIRING_THRESHOLD*( (1 + sin( 2 * 3.14*(float)time_class::getTime()/1000 ))) );
	#undef SVINGNINGS_AMP
}
inline double sensorFunk1a()
{
	#define SVINGNINGS_AMP 2.133
	return ( 1.1 * FIRING_THRESHOLD*( (2 + sin( 2 * 3.14*(float)time_class::getTime()/1000 ))) );
	#undef SVINGNINGS_AMP
}

inline double dynamicSensoryFunc2()
{
	if( time_class::getTime() < 2000)
		return (  FIRING_THRESHOLD * 1.50 * (1 - sin( 10* 3.14*(float)time_class::getTime()/1000 )) );
	else // At time t_n=2000, swith to an other period/amplitude.
	{
		return  (FIRING_THRESHOLD * 2 * 1.5 * (1 - cos( 1*3.14*(float)time_class::getTime()/1000 )) );
	}
}

inline double sensorFunk3a()
{
	if(time_class::getTime() < 1000)
		return 0;
	else
		return 2*FIRING_THRESHOLD;
}
// TEST FOR sf3() etterpå (for å sjå om vi fortsatt har delay for overføring av kappa)
inline double sensorFunk3()
{
	if(time_class::getTime() < 2000)
	 	return  (FIRING_THRESHOLD *  (1 - cos( 1*3.14*(float)time_class::getTime()/1000 )) );
	else if( time_class::getTime() < 3000)
		return 0;
	else
		return FIRING_THRESHOLD*2;
}
inline double sensorFunk4()
{
	//return (( 1+((float)time_class::getTime() / 300) ) * FIRING_THRESHOLD);
	return ( FIRING_THRESHOLD*( 2 * (1 + sin( 3.14*(float)time_class::getTime()/4000 ))) );
}

// TEST 1 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
inline double sensorFunk_TEST1_s2()
{
	if(time_class::getTime() > 2500 && time_class::getTime() < 7000)
		return 1.8*FIRING_THRESHOLD;
	else
		return 0.5*FIRING_THRESHOLD; //Dette gir samme resultat som 0. Tester det..
}
/*
inline double sensorFunk_TEST1_s3()
{
	if( 4800 < time_class::getTime() < 5100 )
		return 1.5*FIRING_THRESHOLD;
	else
		return 0.1*FIRING_THRESHOLD;
} */
inline double sensorFunk_TEST1_s4()
{
	return 1.3*FIRING_THRESHOLD;
}
inline double sensorFunk_TEST1_s5()
{
	return 2*(FIRING_THRESHOLD + FIRING_THRESHOLD * sin(2*PI*((float)time_class::getTime()/2000)) );
}
// SLUTT test * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

