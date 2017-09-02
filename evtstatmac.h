// *************** EVTSTATMAC.H ***************

#ifndef EVTSTATMAC_H
#define EVTSTATMAC_H

// ================= Include =================

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

// ================= Define ==================

// Maximum number of samples when learning, to avoid overflow
#define ESM_NBMAXSAMPLE INT_MAX
// Null value for events and status
#define ESM_NULL -1
#define ESM_EVTNULL ESM_NULL
#define ESM_STATNULL ESM_NULL
// Precision
#define ESM_EPSILON 0.00000001
// Event type
#define esmEvt int
// State type
#define esmStat int

// ================= Data structure ===================

typedef struct EvtStatMac {
  // Number of possible statuss
  int _nbStatus;
  // Number of possible events
  int _nbEvent;
  // Number of learnt transitions for each status 
  int *_nbTransit;
  // Number of learnt occurence of each event for each status
  int *_nbEvt;
  // Number of learnt occurence of each status from each (status,event)
  int *_nbEvtToStat;
  // Probability of occurence of each event from each status
  float *_probEvt;
  // Probability of reaching each event from each (status,event)
  float *_probEvtToStat;
  // Initial status of the ESM
  esmStat _initState;
  // Current status of the ESM
  esmStat _curState;
  // Last occured event (updated by ESMStep...())
  esmEvt _lastEvt;
} EvtStatMac;

// ================ Functions declaration ====================

// Get a randomly choosen integer between 0 and 'nb' - 1 with a 
// probability for integer i to be choosen equal to 'prob[i]', 'prob' 
// must be normalized
// Return ESM_NULL if arguments are invalid
int ESMGetRnd(float *prob, int nb);

// Create, reset and init the ESM with 'nbState' possible status 
// and 'nbEvent' possible events, return NULL if the ESM couldn't 
// be created 
EvtStatMac* ESMCreate(esmStat initState, esmStat nbState, 
  esmEvt nbEvent);

// Clone a ESM, return NULL if the ESM couldn't be cloned
EvtStatMac* ESMClone(EvtStatMac *this);

// Load a ESM from a stream, return NULL if the ESM couldn't be loaded
EvtStatMac* ESMLoad(FILE *stream);

// Save a ESM to a stream, return false if the ESM couldn't be saved,
// true else
// Format:
// <_nbStatus>\n
// <_nbEvent>\n
// <_initState>\n
// for each status:
//   <_nbTransit[status]>\n
// for each status:
//   for each event:
//     <_nbEvt[status,event]>\n
// for each statusI:
//   for each event:
//     for each statusJ:
//       <_nbEvtToStat[statusI,event,statusJ]>\n
// for each status:
//   for each event:
//       <_probEvt[status,event]>\n
// for each statusI:
//   for each event:
//     for each statusJ:
//       <_probEvtToStat[statusI,event,statusJ]>\n
// <_curState>\n
// <_lastEvt>\n
bool ESMSave(EvtStatMac *this, FILE *stream);

// Init the ESM and reset all the probabilities of events to 
// 1.0/_nbEvent and all the probabilities of status to 1.0/_nbStatus
void ESMReset(EvtStatMac *this);

// Init the ESM and reset all the probabilities to 0.0
void ESMResetNull(EvtStatMac *this);

// Reset the current status to the initial status
void ESMInit(EvtStatMac *this);

// Normalize the probabilities
void ESMNormalize(EvtStatMac *this);

// Free the memory used by the ESM
void ESMFree(EvtStatMac **this);

// Print the ESM on 'stream'. If 'statLbl' and/or 'evtLbl' are not 
// null they are used as labels for status and events respectively 
// instead of printing their index
void ESMPrint(EvtStatMac *this, FILE *stream, char **statLbl, 
  char **evtLbl);

// Memorize one occurence of status 'to' after event 'event' from
// status 'from', and update all probabilities respectively
// ESMResetNull must be called before starting to learn
void ESMLearn(EvtStatMac *this, esmStat from, esmEvt event, esmStat to);

// Get the current status
// Return ESM_STATNULL if argument is invalid
esmStat ESMGetStat(EvtStatMac *this);

// Set the probability for 'event' to occur from status 'from'
void ESMSetPEvtGivFrom(EvtStatMac *this, esmStat from, esmEvt event, 
  float prob);

// Set the probability to be in status 'to' after 'event' occured 
// from status 'from'
void ESMSetPToGivFromEvt(EvtStatMac *this, esmStat from, esmEvt event, 
  esmStat to, float prob);

// Update the current status to a randomly choosen status from the
// current status
void ESMStep(EvtStatMac *this);

// Update the current status to a randomly choosen status given that 
// 'event' has occured from the current status
void ESMStepByEvt(EvtStatMac *this, esmEvt event);

// Get a randomly choosen event from the current status 
// Return ESM_EVTNULL if arguments are invalid
esmEvt ESMGetNextEvt(EvtStatMac *this);

// Get the last occured event 
// Return ESM_EVTNULL if arguments are invalid
esmEvt ESMGetLastEvt(EvtStatMac *this);

// Get a randomly choosen next status from the current status
// Return ESM_STATNULL if arguments are invalid
esmStat ESMGetNextStat(EvtStatMac *this);

// Get a randomly choosen next status given that 'event' has occured 
// from the current status
// Return ESM_STATNULL if arguments are invalid
esmStat ESMGetNextStatByEvt(EvtStatMac *this, esmEvt event);

// Get the probability that 'event' occurs from status 'from'
// Return 0.0 if arguments are invalid
float ESMGetPEvtGivFrom(EvtStatMac *this, esmStat from, esmEvt event);

// Get the probability that status 'to' occurs knowing 'event'
// occured after status 'from'
// Return 0.0 if arguments are invalid
float ESMGetPToGivFromEvt(EvtStatMac *this, esmStat from, esmEvt event, 
  esmStat to);

// Probability to ends to status 'to' after 'nbEvt' events starting 
// from status 'from'
// Return 0.0 if arguments are invalid
float ESMGetPToGivFrom(EvtStatMac *this, esmStat from, 
  esmStat to, int nbEvt);

// Probability to be in status 'status'
// Return 0.0 if arguments are invalid
float ESMGetPStatus(EvtStatMac *this, esmStat status);

// Probability that event 'event' occurs
// Return 0.0 if arguments are invalid
float ESMGetPEvent(EvtStatMac *this, esmEvt event);

// Probability that a transition from status 'from' through event 
// 'event' occurs
// Return 0.0 if arguments are invalid
float ESMGetPFromEvt(EvtStatMac *this, esmStat from, esmEvt event);

// Probability that a transition from status 'from' to status 'to' occurs
// Return 0.0 if arguments are invalid
float ESMGetPFromTo(EvtStatMac *this, esmStat from, esmStat to);

// Probability that a transition to status 'to' through event 'event' 
// occurs
// Return 0.0 if arguments are invalid
float ESMGetPEvtTo(EvtStatMac *this, esmEvt event, esmStat to);

// Probability that a transition from status 'from' through event 
// 'event' to status 'to' occurs
// Return 0.0 if arguments are invalid
float ESMGetPFromEvtTo(EvtStatMac *this, esmStat from, esmEvt event,
  esmStat to);

// Probability that a transition started from status 'from' knowing 
// that event 'event' occured
// Return 0.0 if arguments are invalid or P(event) = 0.0
float ESMGetPFromGivEvt(EvtStatMac *this, esmStat from, esmEvt event);

// Probability that a transition started from status 'from' knowing 
// that it has ended to status 'to'
// Return 0.0 if arguments are invalid or P(to) = 0.0
float ESMGetPFromGivTo(EvtStatMac *this, esmStat from, esmStat to);

// Probability that a transition ends to status 'to' knowing that 
// event 'event' occured
// Return 0.0 if arguments are invalid or P(event) = 0.0
float ESMGetPToGivEvt(EvtStatMac *this, esmEvt event, esmStat to);

// Probability that a transition through event 'event' occured
// knowing that it ended to status 'to'
// Return 0.0 if arguments are invalid or P(to) = 0.0
float ESMGetPEvtGivTo(EvtStatMac *this, esmEvt event, esmStat to);

// Probability that a transition started from status 'from' knowing 
// that 'event' occured and ended to status 'to'
// Return 0.0 if arguments are invalid or P(event, to) = 0.0
float ESMGetPFromGivEvtTo(EvtStatMac *this, esmStat from, esmEvt event,
  esmStat to);

// Probability that a transition through event 'event' occured knowing 
// that it started from status 'from' and ended to status 'to'
// Return 0.0 if arguments are invalid or P(from, to) = 0.0
float ESMGetPEvtGivFromTo(EvtStatMac *this, esmStat from, esmEvt event, 
  esmStat to);

#endif
