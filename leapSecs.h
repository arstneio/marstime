/*
 * Header file for leapSecs.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <wordexp.h>

#ifndef leapSecs
#define leapSecs

/*int main(int argc, char* argv[]);*/

typedef struct{
  long updated; // when the current list was updated
  long expires; // when the current list expires
  int size; // number of time offset pairs
  long *times; // second after leap seconds are applied (UTC)
  int *offsets; // TAI-UTC starting at that time
} leapTable;

leapTable* getLeapTable();

/*
 * reads the file and stores the leap second info in the provided struct
 */
int parseFile(char *filename, leapTable* table);

/*
 * Returns TAI-UTC at time
 * = (number of leap seconds applied before time) + 10
 * Only valid in 1972 and later
 */
int getOffset(double time);

double ntp2unix(double ntptime);

char* timestr(time_t time);

int offset(long time, leapTable *table);

/*
 * Returns TAI in Unix time format
 * (actual seconds since 1970-01-01 00:00:00 TAI, including leap seconds)
 */
int UTCtoTAI(long time, leapTable *table);
 
#endif
