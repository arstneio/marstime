/*
 * Main function and CLI interface for marsTime
 *
 * Stephan Ebert
 * 2012-08-03
 */

#include "marsTime.h"

/*extern timeZone MTC;*/

int main(int argc, char *argv[]){
  leapTable *leaptable = getLeapTable();
  initDefs();
  struct timeval tv; // = malloc(sizeof(struct timeval));
  gettimeofday(&tv, NULL);
  double tai = UTCstructToTAIfloat(&tv, leaptable);
  /*double tai = UTCtoTAI(1211759632, leaptable);*/
  /*printf("TAI=%lf\n", tai);*/
  double j2k = TAItoJ2K(tai);
  /*printf("J2000=%lf\n", j2k);*/
  double msd = J2KtoMSD(j2k);
  /*printf("MSD=%lf\n", msd);*/
  soldate *marsdate = MSDtoSoldate(msd, &curiosity);
  /*printf("Sol:%ld\nHr: %d\nMin:%d\nSec:%d\n", marsdate->sol, marsdate->hour, */
  /*marsdate->min, marsdate->sec);*/
  char *str = soldateToString(marsdate);
  printf("%s\n", str);
  return 0;
}
