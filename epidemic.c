#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "evtstatmac.h"
#include "tgapaint.h"

#define rnd() (double)(rand())/(float)(RAND_MAX)
 
typedef enum {
  stat_0, stat_1, stat_2, stat_3, stat_4, stat_5, 
  stat_6, stat_7, stat_8, stat_9, stat_10, stat_11
} statusEpidemic;
typedef enum {
  evt_toWeek1, evt_toWeek2, evt_toWeek3, evt_toWeek4, evt_toWeek5,
  evt_toWeek6, evt_toWeek7, evt_toWeek8, evt_toWeek9, evt_toWeek10,
  evt_toWeek11, evt_toWeek12, evt_toWeek13, evt_toWeek14, evt_toWeek15,
  evt_toWeek16, evt_toWeek17, evt_toWeek18, evt_toWeek19, evt_toWeek20,
  evt_toWeek21, evt_toWeek22, evt_toWeek23, evt_toWeek24, evt_toWeek25,
  evt_toWeek26, evt_toWeek27, evt_toWeek28, evt_toWeek29, evt_toWeek30,
  evt_toWeek31, evt_toWeek32, evt_toWeek33, evt_toWeek34, evt_toWeek35,
  evt_toWeek36, evt_toWeek37, evt_toWeek38, evt_toWeek39, evt_toWeek40,
  evt_toWeek41, evt_toWeek42, evt_toWeek43, evt_toWeek44, evt_toWeek45,
  evt_toWeek46, evt_toWeek47, evt_toWeek48, evt_toWeek49, evt_toWeek50,
  evt_toWeek51, evt_toWeek52, evt_toWeek53
} evtEpidemic;
char *strStatEpidemic[12] = {
  (char*)"Cat 0", (char*)"Cat 1", (char*)"Cat 2", (char*)"Cat 3", 
  (char*)"Cat 4", (char*)"Cat 5", (char*)"Cat 6", 
  (char*)"Cat 7", (char*)"Cat 8", (char*)"Cat 9", 
  (char*)"Cat 10", (char*)"Cat 11"
};
char *strEvtEpidemic[53] = {
  (char*)"toWeek1", (char*)"toWeek2", (char*)"toWeek3", 
  (char*)"toWeek4", (char*)"toWeek5", (char*)"toWeek6", 
  (char*)"toWeek7", (char*)"toWeek8", (char*)"toWeek9", 
  (char*)"toWeek10", 
  (char*)"toWeek11", (char*)"toWeek12", (char*)"toWeek13", 
  (char*)"toWeek14", (char*)"toWeek15", (char*)"toWeek16",
  (char*)"toWeek17", (char*)"toWeek18", (char*)"toWeek19", 
  (char*)"toWeek20", 
  (char*)"toWeek21", (char*)"toWeek22", (char*)"toWeek23", 
  (char*)"toWeek24", (char*)"toWeek25", (char*)"toWeek26",
  (char*)"toWeek27", (char*)"toWeek28", (char*)"toWeek29", 
  (char*)"toWeek30", 
  (char*)"toWeek31", (char*)"toWeek32", (char*)"toWeek33", 
  (char*)"toWeek34", (char*)"toWeek35", (char*)"toWeek36", 
  (char*)"toWeek37", (char*)"toWeek38", (char*)"toWeek39", 
  (char*)"toWeek40", 
  (char*)"toWeek41", (char*)"toWeek42", (char*)"toWeek43", 
  (char*)"toWeek44", (char*)"toWeek45", (char*)"toWeek46", 
  (char*)"toWeek47", (char*)"toWeek48", (char*)"toWeek49", 
  (char*)"toWeek50", 
  (char*)"toWeek51", (char*)"toWeek52", (char*)"toWeek53"
};
int nbSampleEpidemic = 619;
int dataEpidemic[1857] = {
  4,41,4,4,42,4,4,43,6,6,44,6,6,45,5,5,46,7,7,47,8,8,48,9,9,49,10,10,
  50,9,9,51,8,8,52,8,8,1,7,7,2,6,6,3,5,5,4,4,4,5,4,4,6,5,5,7,4,4,8,4,
  4,9,4,4,10,4,4,11,4,4,12,3,3,13,3,3,14,3,3,15,3,3,16,3,3,17,2,2,18,
  3,3,19,3,3,20,3,3,21,2,2,22,2,2,23,3,3,24,2,2,25,2,2,26,2,2,27,2,2,
  28,2,2,29,2,2,30,2,2,31,2,2,32,2,2,33,3,3,34,2,2,35,3,3,36,2,2,37,3,
  3,38,3,3,39,5,5,40,5,5,41,5,5,42,5,5,43,5,5,44,5,5,45,5,5,46,5,5,47,
  5,5,48,5,5,49,6,6,50,6,6,51,6,6,52,7,7,1,7,7,2,7,7,3,7,7,4,7,7,5,8,
  8,6,9,9,7,10,10,8,9,9,9,8,8,10,7,7,11,7,7,12,6,6,13,6,6,14,5,5,15,
  5,5,16,4,4,17,4,4,18,4,4,19,3,3,20,3,3,21,3,3,22,3,3,23,3,3,24,3,3,
  25,3,3,26,3,3,27,2,2,28,2,2,29,2,2,30,3,3,31,3,3,32,3,3,33,3,3,34,3,
  3,35,3,3,36,3,3,37,3,3,38,4,4,39,5,5,40,5,5,41,6,6,42,6,6,43,6,6,44,
  5,5,45,5,5,46,6,6,47,6,6,48,6,6,49,6,6,50,6,6,51,6,6,52,6,6,53,6,6,
  1,6,6,2,6,6,3,6,6,4,7,7,5,7,7,6,8,8,7,7,7,8,7,7,9,6,6,10,6,6,11,6,6,
  12,5,5,13,5,5,14,5,5,15,4,4,16,4,4,17,4,4,18,3,3,19,3,3,20,3,3,21,3,
  3,22,3,3,23,3,3,24,2,2,25,2,2,26,2,2,27,2,2,28,2,2,29,2,2,30,2,2,31,
  2,2,32,2,2,33,3,3,34,3,3,35,3,3,36,3,3,37,3,3,38,4,4,39,4,4,40,4,4,
  41,5,5,42,5,5,43,5,5,44,5,5,45,5,5,46,5,5,47,5,5,48,5,5,49,6,6,50,6,
  6,51,6,6,52,7,7,1,7,7,2,7,7,3,7,7,4,7,7,5,8,8,6,9,9,7,8,8,8,7,7,9,6,
  6,10,5,5,11,4,4,12,4,4,13,4,4,14,4,4,15,4,4,16,3,3,17,2,2,18,2,2,19,
  2,2,20,2,2,21,2,2,22,3,3,23,3,3,24,2,2,25,2,2,26,2,2,27,2,2,28,2,2,
  29,2,2,30,2,2,31,2,2,32,2,2,33,2,2,34,2,2,35,2,2,36,3,3,37,3,3,38,4,
  4,39,5,5,40,5,5,41,5,5,42,5,5,43,6,6,44,5,5,45,6,6,46,6,6,47,6,6,48,
  6,6,49,6,6,50,6,6,51,7,7,52,7,7,1,8,8,2,8,8,3,8,8,4,8,8,5,8,8,6,8,8,
  7,8,8,8,7,7,9,6,6,10,6,6,11,5,5,12,5,5,13,5,5,14,5,5,15,4,4,16,4,4,
  17,4,4,18,3,3,19,2,2,20,2,2,21,2,2,22,2,2,23,3,3,24,3,3,25,3,3,26,2,
  2,27,2,2,28,2,2,29,2,2,30,2,2,31,2,2,32,2,2,33,2,2,34,2,2,35,2,2,36,
  2,2,37,3,3,38,4,4,39,5,5,40,5,5,41,5,5,42,5,5,43,5,5,44,5,5,45,5,5,
  46,5,5,47,5,5,48,5,5,49,6,6,50,6,6,51,7,7,52,8,8,1,9,9,2,9,9,3,9,9,
  4,9,9,5,8,8,6,7,7,7,6,6,8,5,5,9,5,5,10,4,4,11,4,4,12,4,4,13,4,4,14,
  4,4,15,3,3,16,3,3,17,3,3,18,3,3,19,3,3,20,3,3,21,3,3,22,2,2,23,2,2,
  24,2,2,25,3,3,26,2,2,27,2,2,28,2,2,29,3,3,30,4,4,31,4,4,32,4,4,33,3,
  3,34,4,4,35,5,5,36,5,5,37,6,6,38,7,7,39,6,6,40,7,7,41,6,6,42,6,6,43,
  6,6,44,7,7,45,7,7,46,7,7,47,8,8,48,9,9,49,9,9,50,9,9,51,8,8,52,7,7,1,
  7,7,2,7,7,3,6,6,4,5,5,5,5,5,6,5,5,7,5,5,8,4,4,9,4,4,10,3,3,11,3,3,
  12,3,3,13,3,3,14,3,3,15,3,3,16,2,2,17,2,2,18,2,2,19,2,2,20,2,2,21,
  2,2,22,2,2,23,2,2,24,1,1,25,1,1,26,1,1,27,1,1,28,1,1,29,1,1,30,1,1,
  31,1,1,32,1,1,33,1,1,34,2,2,35,2,2,36,2,2,37,2,2,38,4,4,39,4,4,40,5,
  5,41,4,4,42,4,4,43,4,4,44,4,4,45,4,4,46,3,3,47,4,4,48,4,4,49,5,5,50,
  5,5,51,6,6,52,7,7,1,8,8,2,8,8,3,8,8,4,8,8,5,8,8,6,8,8,7,7,7,8,7,7,9,
  5,5,10,5,5,11,4,4,12,3,3,13,3,3,14,3,3,15,3,3,16,2,2,17,2,2,18,1,1,
  19,1,1,20,1,1,21,1,1,22,1,1,23,1,1,24,1,1,25,1,1,26,1,1,27,1,1,28,1,
  1,29,1,1,30,1,1,31,1,1,32,1,1,33,1,1,34,1,1,35,1,1,36,1,1,37,1,1,38,
  2,2,39,3,3,40,4,4,41,4,4,42,4,4,43,4,4,44,4,4,45,4,4,46,4,4,47,5,5,
  48,5,5,49,5,5,50,5,5,51,5,5,52,5,5,53,6,6,1,6,6,2,5,5,3,6,6,4,6,6,5,
  6,6,6,7,7,7,8,8,8,8,8,9,8,8,10,7,7,11,6,6,12,5,5,13,4,4,14,4,4,15,4,
  4,16,3,3,17,3,3,18,2,2,19,2,2,20,2,2,21,2,2,22,2,2,23,1,1,24,1,1,25,
  2,2,26,1,1,27,1,1,28,2,2,29,2,2,30,1,1,31,1,1,32,1,1,33,1,1,34,1,1,
  35,1,1,36,2,2,37,2,2,38,4,4,39,5,5,40,5,5,41,5,5,42,5,5,43,5,5,44,5,
  5,45,5,5,46,5,5,47,5,5,48,5,5,49,6,6,50,6,6,51,7,7,52,8,8,1,8,8,2,8,
  8,3,8,8,4,9,9,5,10,10,6,10,10,7,10,10,8,9,9,9,8,8,10,7,7,11,6,6,12,5,
  5,13,5,5,14,5,5,15,4,4,16,3,3,17,3,3,18,3,3,19,3,3,20,2,2,21,3,3,22,
  3,3,23,2,2,24,2,2,25,2,2,26,2,2,27,1,1,28,1,1,29,1,1,30,0,0,31,0,0,
  32,1,1,33,1,1,34,1,1,35,1,1,36,1,1,37,2,2,38,4,4,39,5,5,40,4,4,41,4,
  4,42,4,4,43,4,4,44,4,4,45,4,4,46,4,4,47,5,5,48,5,5,49,5,5,50,6,6,51,
  6,6,52,6,6,1,6,6,2,6,6,3,6,6,4,6,6,5,7,7,6,8,8,7,8,8,8,8,8,9,7,7,10,
  6,6,11,5,5,12,5,5,13,4,4,14,4,4,15,4,4,16,3,3,17,3,3,18,3,3,19,2,2,
  20,2,2,21,2,2,22,2,2,23,2,2,24,1,1,25,1,1,26,1,1,27,1,1,28,1,1,29,1,
  1,30,1,1,31,1,1,32,1,1,33,1,1,34,2,2,35,1,1,36,1,1,37,2,2,38,3,3,39,
  4,4,40,4,4,41,4,4,42,4,4,43,4,4,44,4,4,45,4,4,46,4,4,47,5,5,48,5,5,
  49,5,5,50,6,6,51,6,6,52,7,7,1,7,7,2,7,7,3,7,7,4,8,8,5,10,10,6,11,11,
  7,11,11,8,11,11,9,9,9,10,7,7,11,6,6,12,5,5,13,5,5,14,4,4,15,4,4,16,
  3,3,17,3,3,18,2,2,19,2,2,20,2,2,21,2,2,22,2,2,23,2,2,24,2,2,25,2,2,
  26,1,1,27,0,0,28,0,0,29,0,0,30,0,0,31,1,1,32,0,0,33,1
};

int main(int argc, char **argv) {
  srandom(time(NULL));
  
  // Create the ESM
  int nbState = 12;
  int nbEvent = 53;
  EvtStatMac *epidemicESM = ESMCreate(stat_0, nbState, nbEvent);
  // Learn from samples
  printf("Learning from epidemic data\n");
  ESMResetNull(epidemicESM);
  for (int iSample = 0; iSample < nbSampleEpidemic; ++iSample) {
    esmStat from = dataEpidemic[3 * iSample];
    esmEvt event = dataEpidemic[3 * iSample + 1] - 1;
    esmStat to = dataEpidemic[3 * iSample + 2];
    ESMLearn(epidemicESM, from, event, to);
  }
  // Create artificial data sample and save as a graph
  // with samples as background
  printf("Artificial data series (week/category):\n");
  TGA *graph = NULL;
  short coeff[2] = {4, 10};
  VecShort *dim = VecShortCreate(2);
  VecSet(dim, 0, coeff[0] * nbEvent);
  VecSet(dim, 1, coeff[1] * nbState);
  TGAPixel pix;
  pix._rgba[0] = pix._rgba[1] = pix._rgba[2] = 255;
  pix._rgba[3] = 255;
  graph = TGACreate(dim, &pix);
  TGAPencil *pen = TGAGetBlackPencil();
  if (graph == NULL) {
    fprintf(stderr, "Error while creating the graph\n");
    ESMFree(&epidemicESM);
    return 1;
  }
  VecFloat *fromPx = VecFloatCreate(2);
  VecFloat *toPx = VecFloatCreate(2);
  pix._rgba[0] = pix._rgba[1] = 155;
  TGAPencilSetColor(pen, &pix);
  for (int iSample = 0; iSample < nbSampleEpidemic - 1; ++iSample) {
    esmStat from = dataEpidemic[3 * iSample];
    esmEvt event = dataEpidemic[3 * iSample + 1];
    esmStat to = dataEpidemic[3 * iSample + 2];
    if (event > 1) {
      VecSet(fromPx, 0, (event - 1) * coeff[0]);
      VecSet(fromPx, 1, from * coeff[1]);
      VecSet(toPx, 0, event * coeff[0]);
      VecSet(toPx, 1, to * coeff[1]);
      TGADrawLine(graph, fromPx, toPx, pen);
    }
  }
  epidemicESM->_curState = stat_7;
  pix._rgba[0] = pix._rgba[1] = pix._rgba[2] = 0;
  TGAPencilSetColor(pen, &pix);
  for (int iWeek = 1; iWeek < 52; ++iWeek) {
    esmStat curStat = ESMGetStat(epidemicESM);
    if (curStat == ESM_STATNULL) {
      printf("Unexpected situation");
      iWeek = 53;
    } else {
      ESMStepByEvt(epidemicESM, iWeek);
      esmStat nextStat = ESMGetStat(epidemicESM);
      VecSet(fromPx, 0, (iWeek - 1) * coeff[0]);
      VecSet(fromPx, 1, curStat * coeff[1]);
      VecSet(toPx, 0, iWeek * coeff[0]);
      VecSet(toPx, 1, nextStat * coeff[1]);
      TGADrawLine(graph, fromPx, toPx, pen);
      printf("%d/%s, ", iWeek, strStatEpidemic[curStat]);
    }
  }
  printf("\n");
  TGAFilterGaussBlur(graph, 0.5, 2.0);
  TGASave(graph, (char*)"./graph.tga");
  TGAFree(&graph);
  // Forecast probability from week 48, 3 weeks ahead
  printf("Example of helping management of epidemic special plan.\n");
  printf("Check if (prob of cat. 8+) > 25%% in 3 weeks.\n");
  esmEvt fromWeek = 48;
  esmStat fromStat = stat_5;
  int thresholdCat = 8;
  float threshold = 0.25;
  int nbWeek = 3;
  bool flagAlert = false;
  // Create a clone and delete all the transitions out of the period 
  // to create a submodel of the epidemic during the relevant weeks
  EvtStatMac *clone = ESMClone(epidemicESM);
  for (int from = 0; from < clone->_nbStatus; ++from) {
    for (int event = 0; event < clone->_nbEvent; ++event) {
      for (int to = 0; to < clone->_nbStatus; ++to) {
        if (event < fromWeek || event > fromWeek + nbWeek) {
          ESMSetPToGivFromEvt(clone, from, event, to, 0.0);
        }
      }
    }
  }
  // Renormalize the ESM
  ESMNormalize(clone);
  // Get the probabilities for each category
  printf("At week %d with a current category of %s, %d weeks forecast:\n", 
    fromWeek, strStatEpidemic[fromStat], nbWeek);
  flagAlert = false;
  for (int to = 0; to < clone->_nbStatus; ++to) {
    float prob = 
      ESMGetPToGivFrom(clone, fromStat, to, nbWeek);
    if (prob > ESM_EPSILON) {
      printf("  to %s: %.3f\n", strStatEpidemic[to], prob);
      if (to >= thresholdCat && prob > threshold)
        flagAlert = true;
    }
  }
  if (flagAlert == false)
    printf("  -> No need to trigger the epidemic plan.\n");
  else
    printf("  -> Need to trigger the epidemic plan.\n");
  // One week later, if situation hasn't changed
  fromWeek += 1;
  // Create another clone and delete all the transitions out of the period 
  // to create a submodel of the epidemic during the relevant weeks
  ESMFree(&clone);
  clone = ESMClone(epidemicESM);
  for (int from = 0; from < clone->_nbStatus; ++from) {
    for (int event = 0; event < clone->_nbEvent; ++event) {
      for (int to = 0; to < clone->_nbStatus; ++to) {
        if (event < fromWeek || event > fromWeek + nbWeek) {
          ESMSetPToGivFromEvt(clone, from, event, to, 0.0);
        }
      }
    }
  }
  // Renormalize the ESM
  ESMNormalize(clone);
  // Get the probabilities for each category
  printf("Next week (%d) with category unchanged, %d weeks forecast:\n", 
    fromWeek, nbWeek);
  flagAlert = false;
  for (int to = 0; to < clone->_nbStatus; ++to) {
    float prob = 
      ESMGetPToGivFrom(clone, fromStat, to, nbWeek);
    if (prob > ESM_EPSILON) {
      printf("  to %s: %.3f\n", strStatEpidemic[to], prob);
      if (to >= thresholdCat && prob > threshold)
        flagAlert = true;
    }
  }
  if (flagAlert == false)
    printf("  -> No need to trigger the epidemic plan.\n");
  else
    printf("  -> Need to trigger the epidemic plan.\n");
  // One week later, if situation has deteriorated
  fromStat += 1;
  // Get the probabilities for each category
  printf("If the category had been %s:\n", strStatEpidemic[fromStat]);
  flagAlert = false;
  for (int to = 0; to < clone->_nbStatus; ++to) {
    float prob = 
      ESMGetPToGivFrom(clone, fromStat, to, nbWeek);
    if (prob > ESM_EPSILON) {
      printf("  to %s: %.3f\n", strStatEpidemic[to], prob);
      if (to >= thresholdCat && prob > threshold)
        flagAlert = true;
    }
  }
  if (flagAlert == false)
    printf("  -> No need to trigger the epidemic plan.\n");
  else
    printf("  -> Need to trigger the epidemic plan.\n");
  // Free memory
  ESMFree(&epidemicESM);
  ESMFree(&clone);
  VecFree(&dim);
  VecFree(&fromPx);
  VecFree(&toPx);

  return 0;
}
