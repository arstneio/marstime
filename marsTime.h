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

#ifndef marstime
#define marstime

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include "leapSecs.h"
#define PI M_PI
#define DEG (PI/180)

////////////////////////////////////////////////////////////////////////////////
// Structures
////////////////////////////////////////////////////////////////////////////////

/*
 * Structure for storing a time zone (which also includes an epoch to start
 * numbering sols)
 */
typedef struct{
  long startsol; // sol 0, in MSD
  double offset; // martian seconds ahead of MTC
  char *epochName; // name at beginning of str
  char *zoneName; // name at end of str
  int digits; // min digits to be displayed in sol
} timeZone;

/*
 * Broken down date storing sols since an epoch, and martian hours, minutes,
 * and seconds
 */
typedef struct{
  long sol;
  int hour, min, sec;
  timeZone *tz;
} soldate;

/*
 * Broken down time storing a Martian calendar date and time of day
 */
typedef struct{
  long year;
  int month, sol, hour, min, sec;
  timeZone *tz;
} marsCalDate;

/*int main(int argc, char *argv[]);*/

// Definitions of time zones used to show local time and sol count for rovers/landers

// MTC
// Coordinated Mars Time (also AMT, AAT)
timeZone MTC;
  /*.startsol = 0,*/
  /*.offset = 0.0,*/
  /*.epochName = "MSD",*/
  /*.zoneName = "MTC",*/
  /*.digits = 5*/
  /*};*/

// Pathfinder
/*const timeZone pathfinder = {*/
/*.startsol = 43904, // Sol 1 = 1997-07-04 16:56:55*/
/*.offset = -7981., // AAT-02:13:01*/
/*.epochName = "MP", // Mars Pathfinder (unofficial)*/
/*.zoneName = "",*/
/*.digits = 4*/
/*};*/

/*// Spirit*/
/*const timeZone spirit = {*/
/*.startsol = 46215, // Sol 1 = 2004-01-04 04:35*/
/*.offset = 39840., // AAT+11:00:04*/
/*.epochName = "MER-A", // Mars Exploration Rover A*/
/*.zoneName = "",*/
/*.digits = 4*/
/*};*/

/*// Opportunity*/
/*const timeZone opportunity= {*/
/*.startsol = 46235, // Sol 1 = 2004-01-25 05:05*/
/*.offset = 3666., // AAT-01:01:06*/
/*.epochName = "MER-B", // Mars Exploration Rover B*/
/*.zoneName = "",*/
/*.digits = 4*/
/*};*/

/*// Phoenix*/
/*const timeZone phoenix= {*/
/*.startsol = 47776, // Sol 0 = 2008-05-25 23:53:52*/
/*.offset = -30396., // AAT-08:26:36, LMST at 233.35 E*/
/*.epochName = "MSL", // Mars Science Laboratory*/
/*.zoneName = "",*/
/*.digits = 4*/
/*};*/

/*// Curiosity*/
/*const timeZone curiosity = {*/
/*.startsol = 46269, // Sol 0 = 2012-08-05 05:51*/
/*.offset = 32981.736, // AAT+09:09:41.736, LMST at 137.4239 E*/
/*.epochName = "MSL", // Mars Science Laboratory*/
/*.zoneName = "",*/
/*.digits = 4*/
/*};*/

timeZone pathfinder;
timeZone spirit;
timeZone opportunity;
timeZone phoenix;
timeZone curiosity;

////////////////////////////////////////////////////////////////////////////////
// Conversions between Terran times
////////////////////////////////////////////////////////////////////////////////

/*
 * Converts International Atomic Time to Terrestrial Time
 */
double TAItoTT(double TAI);

/*
 * Converts Terrestrial Time to International Atomic Time
 */
double TTtoTAI(double TT);

/*
 * Converts a struct timeval with microsecond precision as returned by
 * gettimeofday to a double precision floating point number representing seconds
 * since the epoch
 */
double StructToFloat(struct timeval *structTime);

/*
 * Converts the elements of of a struct timeval to floating point
 */
double splitToFloat(time_t tv_sec, suseconds_t tv_usec);

/*
 * Converts time given in seconds since the Unix epoch (TT) to days since the
 * J2000 epoch (TT)
 */
double TTtoJ2K(double TT);

/*
 * Converts time in seconds since the Unix epoch (TAI) to days since the
 * J2000 epoch (TT)
 */
double TAItoJ2K(double TAI);

/*
 * Converts UTC timeval struct as returned by gettimeofday to double precision floating point TAI
 */
double UTCstructToTAIfloat(struct timeval *structTime, leapTable *table);

////////////////////////////////////////////////////////////////////////////////
// Conversions from Terran to Martian Times
////////////////////////////////////////////////////////////////////////////////

/*
 * Converts days since the J2000 epoch (TT) to Mars Sol Date
 * (floating point sols since 1873-12-29)
 * Equation C-2 (modified)
 */
double J2KtoMSD(double J2K);

/*
 * Converts days since the J2000 epoch (TT) to Mars Sol Date
 * (floating point sols since 1873-12-29)
 * Equation C-2 (modified)
 */
double J2KtoMSD(double J2K);

////////////////////////////////////////////////////////////////////////////////
// Conversions between Martian times
////////////////////////////////////////////////////////////////////////////////

/*
 * Determine Equation of Time (degrees)
 * Equation C-1
 */
double EOT(double J2K);

/*
 * Calculate Local Mean Solar Time
 * MSD is Mars Sol Date, the floating point part of which is MTC (Coordinated Mars Time)
 * (Formerly known as AMT (Aery Mean Time))
 * long is the longitude in degrees west
 * Equation C-4
 */
double LMST(double MSD, double lon);

/*
 * Calculates Local True Solar Time
 * Uses the equation of time to calculate the true solar time based on the
 * position of the sun in the sky at the given time and longitude
 * Equation C-5
 */
double LTST(double MSD, double lon);

/*
 * Converts floating point MSD to broken down time with sol, hour, minute,
 * second
 */
soldate* MSDtoSoldate(double MSD, timeZone *tz);

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
char* soldateToString(soldate *soldate);

////////////////////////////////////////////////////////////////////////////////
// Martian Orbital Parameters
////////////////////////////////////////////////////////////////////////////////

/*
 * Determines Mars mean anomaly (degrees)
 * Equation B-1
 */
double meanAnom(double J2K);

/*
 * Determine angle of Fiction Mean Sun (degrees)
 * Equation B-2
 */
double FMS(double J2K);

/*
 * Determine Perturbers
 * Equation B-3
 */
double PBS(double J2K);

/*
 * Determine Equation of Center
 * Equation B-4
 */
double EOC(double J2K);

/*
 * Determine areocentric solar longitude
 * Equation B-5
 */
double Ls(double J2K);

////////////////////////////////////////////////////////////////////////////////
// Conversions from Martian to Terran Times
////////////////////////////////////////////////////////////////////////////////

/*
 * Converts Mars Sol Date to days since the J2000 epoch
 * MSDtoJ2K(J2KtoMSD(x)) == x
 * Equation C-2 (inverse)
 */
double MSDtoJ2K(double MSD);

////////////////////////////////////////////////////////////////////////////////
// Additional calculations
////////////////////////////////////////////////////////////////////////////////

/*
 * Determine solar declination (planetographic)
 * Eq. D-1
 */
double solDec(double Ls);

/*
 * Determine heliocentric distance (au)
 * Eq. D-2
 */
double sunDist(double meanAnom);

/*
 * Determine heliocentric longitude (degrees)
 * Eq. D-3
 */
double sunLong(double J2K);

/*
 * Determine heliocentric latitude (deg)
 * Eq. D-4
 */
double sunLat(double J2K);

/*
 * Determine local solar elevation
 * Eq. D-5
 */
double sunElev(double solDec, double lat, double lon);

/*
 * Determine local solar azimuth (deg)
 *
 * The second element of the sun's location as seen from a point on Mars' surface
 * is its azimuth, i.e., compass angle relative to due north.
 * Eq. D-6
 */
double sunAz(double solDec, double lat, double lon);

#endif
