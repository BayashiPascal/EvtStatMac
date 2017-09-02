#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "evtstatmac.h"

#define rnd() (double)(rand())/(float)(RAND_MAX)
 
typedef enum {
  stat_unknownHair, stat_short, stat_long
} statusHair;
typedef enum {
  evt_male, evt_female, evt_next
} evtHair;
char *strStatHair[3] = {(char*)"unknown", 
    (char*)"ShortHair", (char*)"LongHair"};
char *strEvtHair[3] = {(char*)"Male", 
    (char*)"Female", (char*)"next"};

int main(int argc, char **argv) {
  srandom(time(NULL));
  // Create the ESM
  int nbStatus = 3;
  int nbEvent = 3;
  EvtStatMac *theESM = ESMCreate(stat_unknownHair, nbStatus, nbEvent);
  // Set the probabilities
  ESMResetNull(theESM);
  ESMSetPEvtGivFrom(theESM, stat_unknownHair, evt_male, 0.5);
  ESMSetPEvtGivFrom(theESM, stat_unknownHair, evt_female, 0.5);
  ESMSetPToGivFromEvt(theESM, stat_unknownHair, evt_male, 
    stat_short, 0.9);
  ESMSetPToGivFromEvt(theESM, stat_unknownHair, evt_male, 
    stat_long, 0.1);
  ESMSetPToGivFromEvt(theESM, stat_unknownHair, evt_female, 
    stat_short, 0.5);
  ESMSetPToGivFromEvt(theESM, stat_unknownHair, evt_female, 
    stat_long, 0.5);
  // The following probabilities are necessary to calculate P(status)
  // They represent the act of taking the next measurement in the 
  // population
  ESMSetPEvtGivFrom(theESM, stat_short, evt_next, 1.0);
  ESMSetPEvtGivFrom(theESM, stat_long, evt_next, 1.0);
  ESMSetPToGivFromEvt(theESM, stat_short, evt_next, 
    stat_unknownHair, 1.0);
  ESMSetPToGivFromEvt(theESM, stat_long, evt_next, 
    stat_unknownHair, 1.0);
  // Print the ESM
  printf("theESM:\n");
  ESMPrint(theESM, stdout, strStatHair, strEvtHair);
  // Get the probability male given long hair
  float prob = ESMGetPEvtGivFromTo(theESM, stat_unknownHair,
    evt_male, stat_long);
  printf("Probability of Male given Long hair (0.1667): %.4f\n", prob);
  // Free memory
  ESMFree(&theESM);

  return 0;
}
