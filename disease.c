#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "evtstatmac.h"

#define rnd() (double)(rand())/(float)(RAND_MAX)
 
typedef enum {
  stat_unknownDisease, stat_positive, stat_negative
} statusDisease;
typedef enum {
  evt_sick, evt_fine, evt_next
} evtDisease;
char *strStatDisease[3] = {(char*)"unknown", 
    (char*)"Positive", (char*)"Negative"};
char *strEvtDisease[3] = {(char*)"SickIndividual", 
    (char*)"FineIndividual", (char*)"next"};

int main(int argc, char **argv) {
  srandom(time(NULL));
  
  int nbState = 3;
  int nbEvent = 3;
  EvtStatMac *theESM = 
    ESMCreate(stat_unknownDisease, nbState, nbEvent);
  ESMResetNull(theESM);
  // Set the probabilities
  ESMSetPEvtGivFrom(theESM, stat_unknownDisease, 
    evt_fine, 0.999);
  ESMSetPEvtGivFrom(theESM, stat_unknownDisease, 
    evt_sick, 0.001);
  ESMSetPToGivFromEvt(theESM, stat_unknownDisease,
    evt_fine, stat_positive, 0.01);
  ESMSetPToGivFromEvt(theESM, stat_unknownDisease,
    evt_fine, stat_negative, 0.99);
  ESMSetPToGivFromEvt(theESM, stat_unknownDisease, 
    evt_sick, stat_positive, 0.99);
  ESMSetPToGivFromEvt(theESM, stat_unknownDisease, 
    evt_sick, stat_negative, 0.01);
  // The following probabilities are necessary to calculate P(status)
  // They represent the act of taking the next measurement in the 
  // population
  ESMSetPEvtGivFrom(theESM, stat_positive, evt_next, 1.0);
  ESMSetPEvtGivFrom(theESM, stat_negative, evt_next, 1.0);
  ESMSetPToGivFromEvt(theESM, stat_positive, evt_next, 
    stat_unknownDisease, 1.0);
  ESMSetPToGivFromEvt(theESM, stat_negative, evt_next, 
    stat_unknownDisease, 1.0);
  // Print the ESM
  printf("theESM:\n");
  ESMPrint(theESM, stdout, strStatDisease, strEvtDisease);
  // Get the probability of being sick given a positive test
  float prob = ESMGetPEvtGivFromTo(theESM, stat_unknownDisease,
    evt_sick, stat_positive);
  printf("Prob of Sick individual given one Positive (0.0902): %.4f\n",
    prob);
  // Update the probability of being sick with the new estimate
  printf("Update prob of Sick with previous prob\n");
  ESMSetPEvtGivFrom(theESM, stat_unknownDisease, 
    evt_fine, 1.0 - prob);
  ESMSetPEvtGivFrom(theESM, stat_unknownDisease, 
    evt_sick, prob);
  // Print the updated ESM
  printf("theESM updated:\n");
  ESMPrint(theESM, stdout, strStatDisease, strEvtDisease);
  // Get the probability of being sick given a 2nd positive test
  prob = ESMGetPEvtGivFromTo(theESM, stat_unknownDisease,
    evt_sick, stat_positive);
  printf("Prob of Sick individual given two Positive (0.9075): %.4f\n",
    prob);
  // Free memory
  ESMFree(&theESM);

  return 0;
}
