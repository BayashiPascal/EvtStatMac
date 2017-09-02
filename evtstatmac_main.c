#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "evtstatmac.h"

#define rnd() (double)(rand())/(float)(RAND_MAX)

#define NBMAXSTAT 5
#define NBMAXEVT 5
#define NBSAMPLE 1000000
#define EPSILON 0.0001

typedef enum {
  stat_0, stat_1, stat_2, stat_3, stat_4
} statusTest;
typedef enum {
  evt_0, evt_1, evt_2, evt_3, evt_4
} evtTest;
char *strStat[NBMAXSTAT] = {(char*)"stat0", (char*)"stat1", 
    (char*)"stat2", (char*)"stat3", (char*)"stat4"};
char *strEvt[NBMAXEVT] = {(char*)"evt0", (char*)"evt1", (char*)"evt2", (char*)"evt3", (char*)"evt4"};

// Structure to memorize the sample data

struct Sample {
  esmStat _from;
  esmEvt _evt;
  esmStat _to;
};

// Functions used to probabilities from statistics in sampled ata

float CountFrom(struct Sample *sample, esmStat from) {
  float ret = 0.0;
  for (int iSample = NBSAMPLE; iSample--;) {
    if (sample[iSample]._from == from)
      ret += 1.0;
  }
  return ret;
}

float CountEvt(struct Sample *sample, esmEvt evt) {
  float ret = 0.0;
  for (int iSample = NBSAMPLE; iSample--;)
    if (sample[iSample]._evt == evt)
      ret += 1.0;
  return ret;
}

float CountTo(struct Sample *sample, esmStat to) {
  float ret = 0.0;
  for (int iSample = NBSAMPLE; iSample--;)
    if (sample[iSample]._to == to)
      ret += 1.0;
  return ret;
}

float CountFromEvt(struct Sample *sample, 
  esmStat from, esmEvt evt) {
  float ret = 0.0;
  for (int iSample = NBSAMPLE; iSample--;)
    if (sample[iSample]._from == from &&
      sample[iSample]._evt == evt)
      ret += 1.0;
  return ret;
}

float CountEvtTo(struct Sample *sample, 
  esmEvt evt, esmStat to) {
  float ret = 0.0;
  for (int iSample = NBSAMPLE; iSample--;)
    if (sample[iSample]._evt == evt &&
      sample[iSample]._to == to)
      ret += 1.0;
  return ret;
}

float CountFromTo(struct Sample *sample, 
  esmStat from, esmStat to) {
  float ret = 0.0;
  for (int iSample = NBSAMPLE; iSample--;)
    if (sample[iSample]._from == from &&
      sample[iSample]._to == to)
      ret += 1.0;
  return ret;
}

float CountFromEvtTo(struct Sample *sample, 
  esmStat from, esmEvt evt, esmStat to) {
  float ret = 0.0;
  for (int iSample = NBSAMPLE; iSample--;)
    if (sample[iSample]._from == from &&
      sample[iSample]._evt == evt &&
      sample[iSample]._to == to)
      ret += 1.0;
  return ret;
}

void Check(float p, float q, char *flagOK, float *nbErr, 
  float *sumErr, float *maxErr) {
  if (fabs(p - q) > EPSILON) {
    printf("NOK !! %f", fabs(p - q));
    *flagOK = 0;
    *nbErr += 1.0;
    *sumErr += fabs(p - q);
    *maxErr = (fabs(p - q) > *maxErr ? fabs(p - q) : *maxErr);
  } else
    printf("OK");
  printf("\n");
}

float ProbFromTo(struct Sample *sample, 
  esmStat from, esmStat to, int nb) {
  float nbFrom = 0.0;
  float nbMatch = 0.0;
  for (int iSample = 0; iSample < NBSAMPLE - (nb - 1); ++iSample) {
    if (sample[iSample]._from == from) {
      nbFrom += 1.0;
      if (sample[iSample + nb - 1]._to == to) {
        nbMatch += 1.0;
      }
    }
  }
  if (nbFrom > 0.0)
    return nbMatch / nbFrom;
  else
    return 0.0;
}

int main(int argc, char **argv) {
  srandom(time(NULL));
  
  // Create a EvtStatMac
  int nbStat = (int)round(rnd() * (float)(NBMAXSTAT - 2)) + 2;
  int nbEvt = (int)round(rnd() * (float)(NBMAXEVT - 1)) + 1;
  printf("nbStat: %d, nbEvt: %d\n", nbStat, nbEvt);
  EvtStatMac *esmA = ESMCreate(stat_0, nbStat, nbEvt); 
  if (esmA == NULL) {
    fprintf(stderr, "can't create esmA\n");
    return 1;
  }
  // Print the EvtStatMac
  printf("Default ESM:\n");
  ESMPrint(esmA, stdout, strStat, strEvt);
  // Set random behaviour
  for (int iStat = nbStat; iStat--;)
    for (int iEvt = nbEvt; iEvt--;)
      ESMSetPEvtGivFrom(esmA, iStat, iEvt, rnd());
  for (int iStat = nbStat; iStat--;)
    for (int iEvt = nbEvt; iEvt--;)
      for (int jStat = nbStat; jStat--;)
        ESMSetPToGivFromEvt(esmA, iStat, iEvt, jStat, rnd());
  ESMNormalize(esmA);
  // Print the EvtStatMac
  printf("Random ESM:\n");
  ESMPrint(esmA, stdout, strStat, strEvt);
  // Few steps
  printf("Run few steps:\n");
  for (int iStep = 0; iStep < 5; ++iStep) {
    printf("%d: %s->", iStep, strStat[ESMGetStat(esmA)]);
    ESMStep(esmA);
    printf("%s->%s\n", strEvt[ESMGetLastEvt(esmA)], 
      strStat[ESMGetStat(esmA)]);
  }
  printf("Reinit and run few steps:\n");
  ESMInit(esmA);
  for (int iStep = 0; iStep < 5; ++iStep) {
    printf("%d: %s->", iStep, strStat[ESMGetStat(esmA)]);
    esmEvt evt = ESMGetNextEvt(esmA);
    ESMStepByEvt(esmA, evt);
    printf("%s->%s\n", strEvt[evt], 
      strStat[ESMGetStat(esmA)]);
  }
  // Create samples
  printf("Create sample data from ESM\n");
  struct Sample *samples = 
    (struct Sample*)malloc(sizeof(struct Sample) * NBSAMPLE);
  if (samples == NULL) {
    fprintf(stderr, "can't allocate sample\n");
    return 1;
  }
  for (int iSample = 0; iSample < NBSAMPLE; ++iSample) {
    samples[iSample]._from = ESMGetStat(esmA);
    ESMStep(esmA);
    samples[iSample]._evt = esmA->_lastEvt;
    samples[iSample]._to = ESMGetStat(esmA);
  }
  // Create another EvvtStatMac
  printf("Create another ESM\n");
  EvtStatMac *esmB = ESMCreate(esmA->_initState, nbStat, nbEvt); 
  if (esmB == NULL) {
    fprintf(stderr, "can't create esmB\n");
    return 1;
  }
  // Learn its behaviour from the samples (must reset before learning)
  printf("Learn its behaviour from sample data\n");
  ESMResetNull(esmB);
  for (int iSample = 0; iSample < NBSAMPLE; ++iSample)
    ESMLearn(esmB, samples[iSample]._from, 
      samples[iSample]._evt, samples[iSample]._to);
  // Save the EvtStatMac, and load it into another and compare
  printf("Save the learnt ESM to ESM.txt\n");
  FILE *stream = fopen("./ESM.txt", "w");
  if (ESMSave(esmB, stream) == false) {
    printf("Couldn't save the ESM\n");
    ESMFree(&esmA);
    ESMFree(&esmB);
    return 1;
  }
  fclose(stream);
  printf("Load ESM.txt into another ESM\n");
  stream = fopen("./ESM.txt", "r");
  EvtStatMac *esmLoad = ESMLoad(stream);
  if (esmLoad == NULL) {
    printf("Couldn't load the ESM\n");
    ESMFree(&esmA);
    ESMFree(&esmB);
    return 1;
  }
  fclose(stream);
  printf("Check the saved and loaded ESMs\n");
  char flagProbEvtOk = 1;
  for (int i = nbStat * nbEvt; i--;) {
    if (fabs(esmB->_probEvt[i] - esmLoad->_probEvt[i]) > EPSILON)
      flagProbEvtOk = 0;
  }
  char flagProbEvtToStatOk = 1;
  for (int i = nbStat * nbEvt * nbStat; i--;) {
    if (fabs(esmB->_probEvtToStat[i] - 
      esmLoad->_probEvtToStat[i]) > EPSILON)
      flagProbEvtToStatOk = 0;
  }
  if (esmB->_nbStatus != esmLoad->_nbStatus ||
    esmB->_nbEvent != esmLoad->_nbEvent ||
    esmB->_initState != esmLoad->_initState ||
    esmB->_curState != esmLoad->_curState ||
    esmB->_lastEvt != esmLoad->_lastEvt ||
    memcmp(esmB->_nbTransit, esmLoad->_nbTransit, 
      sizeof(int) * nbStat) != 0 ||
    memcmp(esmB->_nbEvt, esmLoad->_nbEvt, 
      sizeof(int) * nbStat * nbEvt) != 0 ||
    memcmp(esmB->_nbEvtToStat, esmLoad->_nbEvtToStat, 
      sizeof(int) * nbStat * nbEvt * nbStat) != 0 ||
    flagProbEvtOk == 0 || flagProbEvtToStatOk == 0) {
    printf("Loaded ESM doesn't match original one\n");
    ESMPrint(esmB, stdout, strStat, strEvt);
    ESMPrint(esmLoad, stdout, strStat, strEvt);
    ESMFree(&esmA);
    ESMFree(&esmB);
    ESMFree(&esmLoad);
    return 1;
  } else {
    printf("Loaded ESM matches original one\n");
  }
  ESMFree(&esmLoad);
  // Clone the ESM and check it with original
  EvtStatMac *esmCloned = ESMClone(esmB);
  if (esmCloned == NULL) {
    ESMFree(&esmA);
    ESMFree(&esmB);
    return 1;
  }
  flagProbEvtOk = 1;
  for (int i = nbStat * nbEvt; i--;) {
    if (fabs(esmB->_probEvt[i] - esmCloned->_probEvt[i]) > EPSILON)
      flagProbEvtOk = 0;
  }
  flagProbEvtToStatOk = 1;
  for (int i = nbStat * nbEvt * nbStat; i--;) {
    if (fabs(esmB->_probEvtToStat[i] - 
      esmCloned->_probEvtToStat[i]) > EPSILON)
      flagProbEvtToStatOk = 0;
  }
  if (esmB->_nbStatus != esmCloned->_nbStatus ||
    esmB->_nbEvent != esmCloned->_nbEvent ||
    esmB->_initState != esmCloned->_initState ||
    esmB->_curState != esmCloned->_curState ||
    esmB->_lastEvt != esmCloned->_lastEvt ||
    memcmp(esmB->_nbTransit, esmCloned->_nbTransit, 
      sizeof(int) * nbStat) != 0 ||
    memcmp(esmB->_nbEvt, esmCloned->_nbEvt, 
      sizeof(int) * nbStat * nbEvt) != 0 ||
    memcmp(esmB->_nbEvtToStat, esmCloned->_nbEvtToStat, 
      sizeof(int) * nbStat * nbEvt * nbStat) != 0 ||
    flagProbEvtOk == 0 || flagProbEvtToStatOk == 0) {
    printf("Cloned ESM doesn't match original one\n");
    ESMPrint(esmB, stdout, strStat, strEvt);
    ESMPrint(esmCloned, stdout, strStat, strEvt);
    ESMFree(&esmA);
    ESMFree(&esmB);
    ESMFree(&esmCloned);
    return 1;
  } else {
    printf("Cloned ESM matches original one\n");
  }
  ESMFree(&esmCloned);  
  // Compare all calculated probabilities with statistics from
  // samples
  printf("Compare all probabilities\n");
  printf("Calculated / sample statistics, check precision: %f\n",
    EPSILON);
  char flagOK = 1;
  float nbErr = 0.0;
  float sumErr = 0.0;
  float maxErr = 0.0;
  for (int evt = 0; evt < nbEvt; ++evt) {
    for (int from = 0; from < nbStat; ++from) {
      float p = ESMGetPEvtGivFrom(esmB, from, evt);
      float q = CountFromEvt(samples, from, evt) / 
        CountFrom(samples, from);
      printf("P(evt:%s|from:%s) = %.4f / %.4f ", 
        strEvt[evt], strStat[from], p, q);
      Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
    }
  }
  for (int to = 0; to < nbStat; ++to) {
    for (int from = 0; from < nbStat; ++from) {
      for (int evt = 0; evt < nbEvt; ++evt) {
        float p = ESMGetPToGivFromEvt(esmB, from, evt, to);
        float q = CountFromEvtTo(samples, from, evt, to) / 
          CountFromEvt(samples, from, evt);
        printf("P(to:%s|from:%s,evt:%s) = %.4f / %.4f ", 
          strStat[to], strStat[from], strEvt[evt], p, q);
        Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
      }
    }
  }
  for (int to = 0; to < nbStat; ++to) {
    for (int from = 0; from < nbStat; ++from) {
      float p = ESMGetPToGivFrom(esmB, from, to, 1);
      float q = CountFromTo(samples, from, to) / 
        CountFrom(samples, from);
      printf("P1(to:%s|from:%s) = %.4f / %.4f ", 
        strStat[to], strStat[from], p, q);
      Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
    }
  }
  for (int from = 0; from < nbStat; ++from) {
    float p = ESMGetPStatus(esmB, from);
    float q = CountFrom(samples, from) / (float)NBSAMPLE;
    printf("P(%s) = %.4f / %.4f ", strStat[from], p, q);
    Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
  }
  for (int evt = 0; evt < nbEvt; ++evt) {
    float p = ESMGetPEvent(esmB, evt);
    float q = CountEvt(samples, evt) / (float)NBSAMPLE;
    printf("P(%s) = %.4f / %.4f ", strEvt[evt], p, q);
    Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
  }
  for (int from = 0; from < nbStat; ++from) {
    for (int evt = 0; evt < nbEvt; ++evt) {
      float p = ESMGetPFromEvt(esmB, from, evt);
      float q = CountFromEvt(samples, from, evt) / (float)NBSAMPLE;
      printf("P(from:%s, evt:%s) = %.4f / %.4f ", 
        strStat[from], strEvt[evt], p, q);
      Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
    }
  }
  for (int from = 0; from < nbStat; ++from) {
    for (int to = 0; to < nbStat; ++to) {
      float p = ESMGetPFromTo(esmB, from, to);
      float q = CountFromTo(samples, from, to) / (float)NBSAMPLE;
      printf("P(from:%s, to:%s) = %.4f / %.4f ", 
        strStat[from], strStat[to], p, q);
      Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
    }
  }
  for (int evt = 0; evt < nbEvt; ++evt) {
    for (int to = 0; to < nbStat; ++to) {
      float p = ESMGetPEvtTo(esmB, evt, to);
      float q = CountEvtTo(samples, evt, to) / (float)NBSAMPLE;
      printf("P(evt:%s, to:%s) = %.4f / %.4f ", 
        strEvt[evt], strStat[to], p, q);
      Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
    }
  }
  for (int from = 0; from < nbStat; ++from) {
    for (int evt = 0; evt < nbEvt; ++evt) {
      for (int to = 0; to < nbStat; ++to) {
        float p = ESMGetPFromEvtTo(esmB, from, evt, to);
        float q = CountFromEvtTo(samples, from, evt, to) / 
          (float)NBSAMPLE;
        printf("P(from:%s, evt:%s, to:%s) = %.4f / %.4f ", 
          strStat[from], strEvt[evt], strStat[to], p, q);
        Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
      }
    }
  }
  for (int from = 0; from < nbStat; ++from) {
    for (int evt = 0; evt < nbEvt; ++evt) {
      float p = ESMGetPFromGivEvt(esmB, from, evt);
      float q = CountFromEvt(samples, from, evt) / 
        CountEvt(samples, evt);
      printf("P(from:%s|evt:%s) = %.4f / %.4f ", 
        strStat[from], strEvt[evt], p, q);
      Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
    }
  }
  for (int from = 0; from < nbStat; ++from) {
    for (int to = 0; to < nbStat; ++to) {
      float p = ESMGetPFromGivTo(esmB, from, to);
      float q = CountFromTo(samples, from, to) / 
        CountTo(samples, to);
      printf("P(from:%s|to:%s) = %.4f / %.4f ", 
        strStat[from], strStat[to], p, q);
      Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
    }
  }
  for (int to = 0; to < nbStat; ++to) {
    for (int evt = 0; evt < nbEvt; ++evt) {
      float p = ESMGetPToGivEvt(esmB, evt, to);
      float q = CountEvtTo(samples, evt, to) / 
        CountEvt(samples, evt);
      printf("P(to:%s|evt:%s) = %.4f / %.4f ", 
        strEvt[evt], strStat[to], p, q);
      Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
    }
  }
  for (int evt = 0; evt < nbEvt; ++evt) {
    for (int to = 0; to < nbStat; ++to) {
      float p = ESMGetPEvtGivTo(esmB, evt, to);
      float q = CountEvtTo(samples, evt, to) / 
        CountTo(samples, to);
      printf("P(evt:%s|to:%s) = %.4f / %.4f ", 
        strStat[to], strEvt[evt], p, q);
      Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
    }
  }
  for (int from = 0; from < nbStat; ++from) {
    for (int evt = 0; evt < nbEvt; ++evt) {
      for (int to = 0; to < nbStat; ++to) {
        float p = ESMGetPFromGivEvtTo(esmB, from, evt, to);
        float q = CountFromEvtTo(samples, from, evt, to) / 
          CountEvtTo(samples, evt, to);
        printf("P(from:%s|evt:%s, to:%s) = %.4f / %.4f ", 
          strStat[from], strEvt[evt], strStat[to], p, q);
        Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
      }
    }
  }
  for (int evt = 0; evt < nbEvt; ++evt) {
    for (int from = 0; from < nbStat; ++from) {
      for (int to = 0; to < nbStat; ++to) {
        float p = ESMGetPEvtGivFromTo(esmB, from, evt, to);
        float q = CountFromEvtTo(samples, from, evt, to) / 
          CountFromTo(samples, from, to);
        printf("P(evt:%s|from:%s, to:%s) = %.4f / %.4f ", 
          strEvt[evt], strStat[from], strStat[to], p, q);
        Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
      }
    }
  }
  if (flagOK == 1)
    printf("All correct\n");
  else {
    printf("Not all correct\n");
    printf("Nb error: %d\n", (int)nbErr);
    printf("Max error: %f\n", maxErr);
    printf("Avg error: %f\n", (sumErr / nbErr));
  }
  for (int iStep = 2; iStep < 7; ++iStep) {
    flagOK = 1;
    nbErr = 0.0;
    sumErr = 0.0;
    maxErr = 0.0;
    for (int to = 0; to < nbStat; ++to) {
      for (int from = 0; from < nbStat; ++from) {
        float p = ESMGetPToGivFrom(esmB, from, to, iStep);
        float q = ProbFromTo(samples, from, to, iStep);
        printf("P%d(to:%s|from:%s) = %.4f / %.4f ", iStep,
          strStat[to], strStat[from], p, q);
        Check(p, q, &flagOK, &nbErr, &sumErr, &maxErr);
      }
    }
    if (flagOK == 1)
      printf("All correct\n");
    else {
      printf("Not all correct\n");
      printf("Nb error: %d\n", (int)nbErr);
      printf("Max error: %f\n", maxErr);
      printf("Avg error: %f\n", (sumErr / nbErr));
    }
  }
  // Free memory
  ESMFree(&esmA);
  ESMFree(&esmB);
  free(samples);
  
  return 0;
}
