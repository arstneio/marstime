/*
 * Program to convert between Terran and Martian time
 * Primarily uses TAI or TT for earth and MSD/MTC for Mars
 *
 * Algorithms involving Mars time taken from
 * http://www.giss.nasa.gov/tools/mars24/help/algorithm.html
 *
 * Stephan Ebert
 * 2012-07-10
 */

#include "marsTime.h"

// Initialize definitions
void initDefs(){
  // Definitions of time zones used to show local time and sol count for rovers/landers

  // MTC
  // Coordinated Mars Time (also AMT, AAT)
  timeZone MTCtemp = {
    .startsol = 0,
    .offset = 0.0,
    .epochName = "MSD",
    .zoneName = "MTC",
    .digits = 5
  };

  MTC = MTCtemp;

  // Pathfinder
  timeZone pathfindertemp = {
    .startsol = 43904, // Sol 1 = 1997-07-04 16:56:55
    .offset = -7981., // AAT-02:13:01
    .epochName = "MP", // Mars Pathfinder (unofficial)
    .zoneName = "",
    .digits = 4
  };

  pathfinder = pathfindertemp;

  // Spirit
  timeZone spirittemp = {
    .startsol = 46215, // Sol 1 = 2004-01-04 04:35
    .offset = 39840., // AAT+11:00:04
    .epochName = "MER-A", // Mars Exploration Rover A
    .zoneName = "",
    .digits = 4
  };

  spirit = spirittemp;

  // Opportunity
  const timeZone opportunitytemp = {
    .startsol = 46235, // Sol 1 = 2004-01-25 05:05
    .offset = -3666., // AAT-01:01:06
    .epochName = "MER-B", // Mars Exploration Rover B
    .zoneName = "",
    .digits = 4
  };

  opportunity = opportunitytemp;

  // Phoenix
  const timeZone phoenixtemp = {
    .startsol = 47776, // Sol 0 = 2008-05-25 23:53:52
    .offset = -30396., // AAT-08:26:36, LMST at 233.35 E
    .epochName = "MPh", // Mars Phoenix (unofficial)
    .zoneName = "",
    .digits = 4
  };

  phoenix = phoenixtemp;

  // Curiosity
  const timeZone curiositytemp = {
    .startsol = 49269, // Sol 0 = 2012-08-05 05:51
    .offset = 32981.736, // AAT+09:09:41.736, LMST at 137.4239 E
    .epochName = "MSL", // Mars Science Laboratory
    .zoneName = "",
    .digits = 4
  };

  curiosity = curiositytemp;
}

////////////////////////////////////////////////////////////////////////////////
// Conversions between Terran times
////////////////////////////////////////////////////////////////////////////////

/*
 * Converts International Atomic Time to Terrestrial Time
 */
double TAItoTT(double TAI){
  return TAI + 32.184;
}

/*
 * Converts Terrestrial Time to International Atomic Time
 */
double TTtoTAI(double TT){
  return TT - 32.184;
}

/*
 * Converts a struct timeval with microsecond precision as returned by
 * gettimeofday to a double precision floating point number representing seconds
 * since the epoch
 */
double StructToFloat(struct timeval *structTime){
  return structTime->tv_sec + (double)(structTime->tv_usec)/1e6;
}

/*
 * Converts the elements of of a struct timeval to floating point
 */
double splitToFloat(time_t tv_sec, suseconds_t tv_usec){
  return tv_sec + (double)tv_usec/1e6;
}

/*
 * Converts time given in seconds since the Unix epoch (TT) to days since the
 * J2000 epoch (TT)
 */
double TTtoJ2K(double TT){
  return TT/86400 - 10957.5;
}

/*
 * Inverse of preceding function
 */
double J2KtoTT(double J2K){
  return 86400 * (J2K + 10957.5);
}

/*
 * Converts time in seconds since the Unix epoch (TAI) to days since the
 * J2000 epoch (TT)
 */
double TAItoJ2K(double TAI){
  return (TAI + 32.184) / 86400 - 10957.5;
}

/*
 * Inverse of preceding function
 */
double J2KtoTAI(double J2K){
  return 86400 * (J2K + 10957.5) - 32.184;
}

/*
 * Converts UTC timeval struct as described in gettimeofday(2) to double 
 * precision floating point TAI
 */
double UTCstructToTAIfloat(struct timeval *structTime, leapTable *table){
  structTime->tv_sec = UTCtoTAI(structTime->tv_sec, table);
  return StructToFloat(structTime);
} 

////////////////////////////////////////////////////////////////////////////////
// Conversions from Terran to Martian Times
////////////////////////////////////////////////////////////////////////////////

/*
 * Converts days since the J2000 epoch (TT) to Mars Sol Date
 * (floating point sols since 1873-12-29)
 * Equation C-2 (modified)
 */
double J2KtoMSD(double J2K){
  return (J2K-4.5) / 1.027491252 + 44796.0 - 0.00096;
}

/*
 * Converts seconds since the Unix epoch (TAI) to days since J2000 epoch (TT)
 */
double TAItoMSD(double TAI){
  return J2KtoMSD(TAItoJ2K(TAI));
}

////////////////////////////////////////////////////////////////////////////////
// Conversions between Martian times
////////////////////////////////////////////////////////////////////////////////

/*
 * Determine Equation of Time (degrees)
 * Equation C-1
 */
double EOT(double J2K){
  int Lsval = Ls(J2K)*DEG;
  return 2.861*sin(2*Lsval) - 0.071*sin(4*Lsval) + 0.002*sin(6*Lsval)-(EOC(J2K));
}

/*
 * Calculate Local Mean Solar Time
 * MSD is Mars Sol Date, the floating point part of which is MTC (Coordinated Mars Time)
 * (Formerly known as AMT (Aery Mean Time))
 * long is the longitude in degrees west
 * Equation C-4
 */
double LMST(double MSD, double lon){
  return MSD - lon/360;
}

/*
 * Calculates Local True Solar Time
 * Uses the equation of time to calculate the true solar time based on the
 * position of the sun in the sky at the given time and longitude
 * Equation C-4
 */
double LTST(double MSD, double lon){
  return 0;
}

/*
 * Determine subsolar longitude
 * Eq. C-5
 */
//double subsolLon(double 

/*
 * Converts floating point MSD to broken down time with sol, hour, minute,
 * second
 */
soldate* MSDtoSoldate(double MSD, timeZone *tz){
  /*printf("%f\n", MSD);*/
  MSD -= tz->startsol;
  /*printf("%f\n", MSD);*/
  MSD += tz->offset/86400;
  /*printf("offset=%f\n", tz->offset);*/
  /*printf("%f\n", MSD);*/
  long sol;
  int hour, min, sec;
  sol = MSD;
  MSD -= sol;
  if(MSD<0){
    MSD += 1;
    sol -= 1;
  }
  MSD *= 24;
  hour = MSD;
  MSD -= hour;
  MSD *= 60;
  min = MSD;
  MSD -= min;
  MSD *= 60;
  sec = MSD;
  soldate *date = malloc(sizeof(soldate));
  if(date == NULL){
    printf("Cannot allocate memory for soldate\n");
    exit(1);
  }
  date->sol = sol;
  date->hour = hour;
  date->min = min;
  date->sec = sec;
  date->tz = tz;
  return date;
}

/*
 * Converts floating point MSD to broken down time representing a date on the
 * Darian Calendar
 */
marsCalDate MSDtoDarian(double MSD);

/*
 * Returns a pointer to a string representing the date in the format
 * SSSSS HH:MM:SS
 * e.g. MSD 44796 12:34:56 MTC
 *      MSL 0034 14:54:49
 */
char* soldateToString(soldate *soldate){
  /*printf("sol=%d\n", soldate->sol);*/
  int soldigits = soldate->tz->digits;
  /*printf("%08X\n", soldate->tz);*/
  /*printf("soldigits=%d\n", soldigits);*/
  int length = 18 + soldigits;
  char *str = malloc(length * sizeof(char));
  char *format = malloc(6 * sizeof(char));
  snprintf(format, 27, "%%s %%0%dld %%02d:%%02d:%%02d %%s", soldigits);
  snprintf(str, length, format, soldate->tz->epochName, soldate->sol, soldate->hour,
      soldate->min, soldate->sec, soldate->tz->zoneName);
  free(format);
  return str;
}

////////////////////////////////////////////////////////////////////////////////
// Martian Orbital Parameters
////////////////////////////////////////////////////////////////////////////////

/*
 * Determines Mars mean anomaly (degrees)
 * Equation B-1
 */
double meanAnom(double J2K){
  return 19.3780 + 0.52402075 * J2K;
}

/*
 * Determine angle of Fiction Mean Sun (degrees)
 * Equation B-2
 */
double FMS(double J2K){
  return 270.3863 + 0.52403840 * J2K;
}

/*
 * Determine Perturbers
 * Equation B-3
 */
double PBS(double J2K){
  int pertA[] = {0.0071, 0.0057, 0.0039, 0.0037, 0.0021, 0.0020, 0.0018};
  int pertTau[] = {2.2353, 2.7543, 1.1177, 15.7866, 2.1354, 2.4694, 32.8493};
  int pertPhi[] = {49.409, 168.173, 191.837, 21.736, 15.704, 95.528, 49.095};
  double sum = 0.0;
  int i;
  double degperday = 360*DEG/365.25;
  for(i=0; i<7; i++){
    sum += pertA[i] * cos(degperday * J2K / pertTau[i]) + pertPhi[i];
  }
  return sum;
}

/*
 * Determine Equation of Center
 * Equation B-4
 */
double EOC(double J2K){
  double M = meanAnom(J2K) * DEG;
  return (10.691 + 3e-7 * J2K) * sin(M) + 0.623 * sin(2*M) + 0.050 * sin(3*M) +
    0.005 * sin(4*M) + 0.0005 * sin(5*M) + PBS(J2K);
}

/*
 * Determine areocentric solar longitude
 * Equation B-5
 */
double Ls(double J2K){
  return FMS(J2K) + EOC(J2K);
}

////////////////////////////////////////////////////////////////////////////////
// Conversions from Martian to Terran Times
////////////////////////////////////////////////////////////////////////////////

/*
 * Converts Mars Sol Date to days since the J2000 epoch
 * MSDtoJ2K(J2KtoMSD(x)) == x
 * Equation C-2 (inverse)
 */
double MSDtoJ2K(double MSD){
  return 1.027491252 * (MSD - 44796.0 + 0.00096) + 4.5;
}

////////////////////////////////////////////////////////////////////////////////
// Additional calculations
////////////////////////////////////////////////////////////////////////////////

/*
 * Determine solar declination (planetographic)
 * Eq. D-1
 */
double solDec(double Ls){
  Ls = Ls*DEG;
  return asin(0.42565*sin(Ls)) + 0.25 * sin(Ls)/DEG;
}

/*
 * Determine heliocentric distance (au)
 * Eq. D-2
 */
double sunDist(double meanAnom){
  meanAnom = meanAnom*DEG;
  return 1.523679 * (1.00436 - 0.09309*cos(meanAnom) - 0.004336*cos(2*meanAnom)
	      - 0.00031*cos(3*meanAnom) - 0.00003*cos(4*meanAnom));
}

/*
 * Determine heliocentric longitude (degrees)
 * Eq. D-3
 */
double sunLong(double J2K){
  double Lsval = Ls(J2K);
  return Lsval + 85.061-0.015*sin((71+2*Lsval)*DEG)-5.5e-6*J2K;
}

/*
 * Determine heliocentric latitude (deg)
 * Eq. D-4
 */
double sunLat(double J2K){
  return -(1.8497-2.23e-5*J2K)*sin((Ls(J2K)-144.50+2.57e-6*J2K)*DEG);
}

/*
 * Determine local solar elevation
 * Eq. D-5
 */
//double sunElev(double J2K, double lat, double lon){
//  double solDecVal = solDec(Ls(J2K)*DEG)*DEG;
//}  
  
/*
 * Determine local solar azimuth (deg)
 *
 * The second element of the sun's location as seen from a point on Mars' surface
 * is its azimuth, i.e., compass angle relative to due north.
 * Eq. D-6
 */
double sunAz(double solDec, double lat, double lon){
  return 0;
}
