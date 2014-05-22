/*
 * Determines difference between UTC and TAI using file downloaded from NIST
 */

#include "leapSecs.h"

/* int main(int argc, char* argv[]){ */
/*   leapTable *table = malloc(sizeof(leapTable)); */
/*   if(table == NULL){ */
/*     printf("Unable to alloc table\n"); */
/*     exit(1); */
/*   }  */
/*   parseFile("leap-seconds", table); */
/*   char *str = timestr(table->updated); */
/*   printf("updated: %s\n", str); */
/*   free(str); */
/*   str = timestr(table->expires); */
/*   printf("expires: %s\n", str); */
/*   free(str); */
/*   printf("size: %d\n", table->size); */
/*   int i; */
/*   for(i=0; i<table->size; i++){ */
/*     str = timestr(table->times[i]); */
/*     printf("%s %d\n", str, table->offsets[i]); */
/*     free(str); */
/*   } */
  
/*   if(argc >= 2) */
/*     str = argv[1]; */
/*   else */
/*     str = ""; */
  
/*   return 0; */
/* } */

leapTable* getLeapTable(){
  leapTable *table = malloc(sizeof(table));
  if(table == NULL)
    exit(1);
  parseFile("/home/stephan/.marsTime/leap-seconds", table);
  return table;
}

int parseFile(char *filename, leapTable *table){
  /*wordexp_t p = malloc(sizeof(wordexp_t));*/
  /*wordexp(filename, pV*/
  FILE *fp = fopen(filename, "r");
  if(fp == NULL){
    printf("Cannot open file \"%s\"\n", filename);
    exit(1);
  }

  int size = 0; // number of leap second entries
  int arraySize = 1; // number of spaces in arrays to store entries
  table->times = malloc(arraySize*sizeof(double));
  if(table->times == NULL){
    printf("Unable to alloc times\n");
    exit(1);
  }

  table->offsets = malloc(arraySize*sizeof(int));
  if(table->offsets == NULL){
    printf("Unable to alloc offsets\n");
    exit(1);
  }

  

  char *buffer = malloc(30*sizeof(char));
  char *eoftest;
  eoftest = fgets(buffer, 30, fp);
  while(eoftest != NULL){
    //    printf("%s\n", eoftest);
    if(buffer[0] == '#'){
      if(buffer[1] == '$'){
	buffer = &buffer[2];
	//	dest = &(leapSecs->updated);
	sscanf(buffer, "%ld", &(table->updated));
	table->updated = ntp2unix(table->updated);
      }
      else if(buffer[1] == '@'){
	buffer = &buffer[2];
	//	dest = &(leapSecs->expires);
	sscanf(buffer, "%ld", &(table->expires));
	table->expires = ntp2unix(table->expires);
      }
      //      sscanf(buffer, "%d", dest);
  //      *dest = ntp2unix(*dest);
    }
    else if(buffer[0]>='0' && buffer[0]<='9'){
      size++;
      if(size>arraySize){
	arraySize *= 2;
	table->times = realloc(table->times, arraySize*sizeof(double));
	table->offsets = realloc(table->offsets, arraySize*sizeof(int));
      }
      sscanf(buffer, "%ld %d", &table->times[size-1], &table->offsets[size-1]);
      table->times[size-1] = ntp2unix(table->times[size-1]);
    }
    eoftest = fgets(buffer, 30, fp);
  }
  table->size = size;
  //fclose(fp);
  return 0;
}

/*
  converts the time given in seconds since 1900-01-01 to seconds since 1970-01-01
*/
double ntp2unix(double ntptime){
  return ntptime - 2208988800;
}

/*
  Returns a pointer to an ISO8601 formatted string of the given date
*/
char* timestr(time_t time){
  char *str = malloc(20*sizeof(char));
  char *format = "%F %T %z";
  strftime(str, 20, format, gmtime(&time));
  return str;
}

/*
 * Returns TAI-UTC at the given UTC time
 * This is only valid from 1972 until the expiration of the file
 * (usually 6-12 months in the future)
 */
int offset(long time, leapTable *table){
  if(time < table->times[0]){
    // out of range, no leap seconds yet
  }
  else if(time > table->expires){
    // out of range, leap seconds unknown
  }
  if(time >= table->times[table->size-1])
    return table->offsets[table->size-1];
  int low = 0;
  int high = table->size-1;
  int mid = (low+high)/2;
  while(high > low+1){
    if(time == table->times[mid])
      break;
    else if(time > table->times[mid]){
      low = mid;
    }
    else
      high = mid;
    mid = (low + high) / 2;
  }
  return table->offsets[mid];
}

/*
 * Returns TAI in Unix time 
 * (actual seconds since 1970-01-01 00:00:00 TAI, including leap seconds)
 */
int UTCtoTAI(long time, leapTable *table){
  return time + offset(time, table);
}

/*
 * Returns seconds since the epoch, given a string representing a date
 */
time_t getdateint(char *string){
  struct tm *time = malloc(sizeof(struct tm));
  time_t unixtime;
  if(time == NULL)
    exit(1);
  time = getdate(string);
  unixtime = mktime(time);
  free(time);
  return unixtime;
}

