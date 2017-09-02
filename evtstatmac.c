// *************** EVTSTATMAC.C ***************

// ================= Include =================

#include "evtstatmac.h"

// ================= Define ==================

#define rnd() (double)(rand())/(float)(RAND_MAX)
#define ITRANSIT(s) (s)
#define IEVT(esm, s, e) ((s)*esm->_nbEvent+(e))
#define IEVTTOSTAT(esm, f, e, t) (IEVT(esm, f, e)*esm->_nbStatus+(t))

// ================ Functions implementation ==================

// Get a randomly choosen integer between 0 and 'nb' - 1 with a 
// probability for integer i to be choosen equal to 'prob[i]', 'prob' 
// must be normalized
// Return ESM_NULL if arguments are invalid
int ESMGetRnd(float *prob, int nb) {
  // Check arguments
  if (prob == NULL || nb <= 0) 
    return ESM_NULL;
  // Get a random value between 0 and 1
  float r = rnd();
  // Get the index 'ret' for which 
  // sum(prob[0..ret] <= r < sum(prob[0..ret+1]
  int ret = 0;
  float sum = 0.0;
  while (ret < nb && 
    !(sum >= r || r < sum + prob[ret])) {
    sum += prob[ret];
    ++ret;
  }
  if (ret < nb)
    return ret;
  else 
    return nb - 1;
}

// Create, reset and init the ESM with 'nbState' possible status 
// and 'nbEvent' possible events, return NULL if the ESM couldn't 
// be created 
EvtStatMac* ESMCreate(esmStat initState, esmStat nbState, 
  esmEvt nbEvent) {
  // Check arguments
  if (initState < 0 || initState >= nbState ||
    nbState <= 0 || nbEvent <= 0) 
    return NULL;
  // Allocate memory
  EvtStatMac *this = (EvtStatMac*)malloc(sizeof(EvtStatMac));
  if (this == NULL) return NULL;
  // Initialize properties with arguments
  this->_initState = initState;
  this->_nbStatus = nbState;
  this->_nbEvent = nbEvent;
  this->_lastEvt = ESM_EVTNULL;
  // Allocate memory for number of transition and probabilities
  this->_nbTransit = (int*)malloc(sizeof(int) * nbState);
  this->_nbEvt = (int*)malloc(sizeof(int) * nbState * nbEvent);
  this->_nbEvtToStat = 
    (int*)malloc(sizeof(int) * nbState * nbEvent * nbState);
  this->_probEvt = (float*)malloc(sizeof(float) * nbState * nbEvent);
  this->_probEvtToStat = 
    (float*)malloc(sizeof(float) * nbState * nbEvent * nbState);
  if (this->_nbTransit == NULL || this->_nbEvt == NULL || 
    this->_nbEvtToStat == NULL || this->_probEvt == NULL || 
    this->_probEvtToStat == NULL)
    ESMFree(&this);
  // Reset the number of transitions and probabilities
  ESMReset(this);
  // Return the new EvtStatMac
  return this;
}

// Clone a ESM, return NULL if the ESM couldn't be cloned
EvtStatMac* ESMClone(EvtStatMac *this) {
  // Check arguments
  if (this == NULL) 
    return NULL;
  // Create a EvtStatMac with the same arguments as the one in argument
  EvtStatMac *c = 
    ESMCreate(this->_initState, this->_nbStatus, this->_nbEvent);
  if (c != NULL) {
    // Copy the number of transition and probabilities
    memcpy(c->_nbTransit, this->_nbTransit, sizeof(int) * this->_nbStatus);
    memcpy(c->_nbEvt, this->_nbEvt, 
      sizeof(int) * this->_nbStatus * this->_nbEvent);
    memcpy(c->_nbEvtToStat, this->_nbEvtToStat, 
      sizeof(int) * this->_nbStatus * this->_nbEvent * this->_nbStatus);
    memcpy(c->_probEvt, this->_probEvt, 
      sizeof(int) * this->_nbStatus * this->_nbEvent);
    memcpy(c->_probEvtToStat, this->_probEvtToStat, 
      sizeof(int) * this->_nbStatus * this->_nbEvent * this->_nbStatus);
  }
  // Return the cloned EvtStatMac
  return c;
}

// Load a ESM from a stream, return NULL if the ESM couldn't be loaded
EvtStatMac* ESMLoad(FILE *stream) {
  // Check arguments
  if (stream == NULL) 
    return NULL;

  // Read the number of statuss and events and initial status
  int nbState;
  int nbEvent;
  int initState;
  if (fscanf(stream, "%d", &nbState) == EOF)
    return NULL;
  if (fscanf(stream, "%d", &nbEvent) == EOF)
    return NULL;
  if (fscanf(stream, "%d", &initState) == EOF)
    return NULL;
  // Create a EvtStatMac
  EvtStatMac *this = ESMCreate(initState, nbState, nbEvent);
  if (this == NULL)
    return NULL;
  // Read the values of all the properties of the EVM structure
  for (int iStat = 0; iStat < this->_nbStatus; ++iStat)
    if (fscanf(stream, "%d", 
      &(this->_nbTransit[ITRANSIT(iStat)])) == EOF) {
      ESMFree(&this);
      return NULL;
    }
  for (int iStat = 0; iStat < this->_nbStatus; ++iStat)
    for (int iEvt = 0; iEvt < this->_nbEvent; ++iEvt)
      if (fscanf(stream, "%d", 
        &(this->_nbEvt[IEVT(this, iStat, iEvt)])) == EOF) {
        ESMFree(&this);
        return NULL;
      }
  for (int iStat = 0; iStat < this->_nbStatus; ++iStat)
    for (int iEvt = 0; iEvt < this->_nbEvent; ++iEvt)
      for (int jStat = 0; jStat < this->_nbStatus; ++jStat)
        if (fscanf(stream, "%d", 
          &(this->_nbEvtToStat[
          IEVTTOSTAT(this, iStat, iEvt, jStat)])) == EOF) {
          ESMFree(&this);
          return NULL;
        }
  for (int iStat = 0; iStat < this->_nbStatus; ++iStat)
    for (int iEvt = 0; iEvt < this->_nbEvent; ++iEvt)
      if (fscanf(stream, "%f", 
        &(this->_probEvt[IEVT(this, iStat, iEvt)])) == EOF) {
        ESMFree(&this);
        return NULL;
      }
  for (int iStat = 0; iStat < this->_nbStatus; ++iStat)
    for (int iEvt = 0; iEvt < this->_nbEvent; ++iEvt)
      for (int jStat = 0; jStat < this->_nbStatus; ++jStat)
        if (fscanf(stream, "%f", 
          &(this->_probEvtToStat[
          IEVTTOSTAT(this, iStat, iEvt, jStat)])) == EOF) {
          ESMFree(&this);
          return NULL;
        }
  if (fscanf(stream, "%d", &(this->_curState)) == EOF) {
    ESMFree(&this);
    return NULL;
  }
  if (fscanf(stream, "%d", &(this->_lastEvt)) == EOF) {
    ESMFree(&this);
    return NULL;
  }
  // Return the cloned EvtStatMac
  return this;
}

// Save a ESM to a stream, return false if the ESM couldn't be saved,
// true else
bool ESMSave(EvtStatMac *this, FILE *stream) {
  // Check arguments
  if (this == NULL || stream == NULL) 
    return false;
  // Save the values of all the properties of the EVM structure
  if (fprintf(stream, "%d\n", this->_nbStatus) < 0)
    return false;
  if (fprintf(stream, "%d\n", this->_nbEvent) < 0)
    return false;
  if (fprintf(stream, "%d\n", this->_initState) < 0)
    return false;
  for (int iStat = 0; iStat < this->_nbStatus; ++iStat)
    if (fprintf(stream, "%d\n", this->_nbTransit[ITRANSIT(iStat)]) < 0)
      return false;
  for (int iStat = 0; iStat < this->_nbStatus; ++iStat)
    for (int iEvt = 0; iEvt < this->_nbEvent; ++iEvt)
      if (fprintf(stream, "%d\n", 
        this->_nbEvt[IEVT(this, iStat, iEvt)]) < 0)
        return false;
  for (int iStat = 0; iStat < this->_nbStatus; ++iStat)
    for (int iEvt = 0; iEvt < this->_nbEvent; ++iEvt)
      for (int jStat = 0; jStat < this->_nbStatus; ++jStat)
        if (fprintf(stream, "%d\n", 
          this->_nbEvtToStat[IEVTTOSTAT(this, iStat, iEvt, jStat)]) < 0)
          return false;
  for (int iStat = 0; iStat < this->_nbStatus; ++iStat)
    for (int iEvt = 0; iEvt < this->_nbEvent; ++iEvt)
      if (fprintf(stream, "%f\n", 
        this->_probEvt[IEVT(this, iStat, iEvt)]) < 0)
        return false;
  for (int iStat = 0; iStat < this->_nbStatus; ++iStat)
    for (int iEvt = 0; iEvt < this->_nbEvent; ++iEvt)
      for (int jStat = 0; jStat < this->_nbStatus; ++jStat)
        if (fprintf(stream, "%f\n", 
          this->_probEvtToStat[IEVTTOSTAT(
          this, iStat, iEvt, jStat)]) < 0)
          return false;
  if (fprintf(stream, "%d\n", this->_curState) < 0)
    return false;
  if (fprintf(stream, "%d\n", this->_lastEvt) < 0)
    return false;
  // Return true to signify success
  return true;
}

// Init the ESM and reset all the probabilities of events to 
// 1.0/_nbEvent and all the probabilities of status to 1.0/_nbStatus
void ESMReset(EvtStatMac *this) {
  // Check arguments
  if (this == NULL) 
    return;
  // Init the status
  ESMInit(this);
  // For each possible status 'from'
  for (esmStat iStat = this->_nbStatus; iStat--;) {
    // Reset the number of transition from status 'from'
    this->_nbTransit[ITRANSIT(iStat)] = 0;
    // For each possible event
    for (esmEvt iEvt = this->_nbEvent; iEvt--;) {
      // Reset the number of transition ('from', 'event')
      this->_nbEvt[IEVT(this, iStat, iEvt)] = 0;
      // Reset the probability of transition ('from', 'event') to 
      // 1/_nbEvent
      this->_probEvt[IEVT(this, iStat, iEvt)] = 
        1.0 / (float)(this->_nbEvent);
      // For each possible status 'to'
      for (esmStat jStat = this->_nbStatus; jStat--;) {
        // Reset the number of transition ('from', 'event', 'to')
        this->_nbEvtToStat[IEVTTOSTAT(this, iStat, iEvt, jStat)] = 0;
        // Reset the probability of transition ('from', 'event', 'to') 
        // to 1/_nbStatus
        this->_probEvtToStat[IEVTTOSTAT(this, iStat, iEvt, jStat)] = 
          1.0 / (float)(this->_nbStatus);
      }
    }
  }
}

// Init the ESM and reset all the probabilities to 0.0
void ESMResetNull(EvtStatMac *this) {
  // Check arguments
  if (this == NULL) 
    return;
  // Init the status
  ESMInit(this);
  // For each possible status 'from'
  for (esmStat iStat = this->_nbStatus; iStat--;) {
    // Reset the number of transition from status 'from'
    this->_nbTransit[ITRANSIT(iStat)] = 0;
    // For each possible event
    for (esmEvt iEvt = this->_nbEvent; iEvt--;) {
      // Reset the number of event from status 'from'
      this->_nbEvt[IEVT(this, iStat, iEvt)] = 0;
      // Reset the probqbility of event 'event' from status 'from'
      this->_probEvt[IEVT(this, iStat, iEvt)] = 0.0;
      // For each possible status 'to'
      for (esmStat jStat = this->_nbStatus; jStat--;) {
        // Reset the number of transition ('from', 'event', 'to')
        this->_nbEvtToStat[IEVTTOSTAT(this, iStat, iEvt, jStat)] = 0;
        // Reset the probability of transition ('from', 'event', 'to')
        this->_probEvtToStat[IEVTTOSTAT(this, iStat, iEvt, jStat)] = 0.0;
      }
    }
  }
}

// Reset the current status to the initial status
void ESMInit(EvtStatMac *this) {
  // Check arguments
  if (this == NULL) 
    return;
  // Set the current status to the initial status
  this->_curState = this->_initState;  
}

// Normalize the probabilities
void ESMNormalize(EvtStatMac *this) {
  // Check arguments
  if (this == NULL) 
    return;
  // For each possible status 'from'
  for (esmStat iStat = this->_nbStatus; iStat--;) {
    // variable to memorize the sum of probabilities
    float sum = 0.0;
    // For each possible event
    for (esmEvt iEvt = this->_nbEvent; iEvt--;) {
      // Reset the sum of probabilities
      sum = 0.0;
      // For each possible status 'to'
      for (esmStat jStat = this->_nbStatus; jStat--;)
        // Sum the probability of ('from', 'event', 'to')
        sum += 
          this->_probEvtToStat[IEVTTOSTAT(this, iStat, iEvt, jStat)];
      // If the sum is not null 
      if (sum > ESM_EPSILON)
        // For each status 'to'
        for (esmStat jStat = this->_nbStatus; jStat--;)
          // Normalize the probability of ('from', 'event', 'to') by 
          // dividing by the sum of probabilities
          this->_probEvtToStat[IEVTTOSTAT(this, iStat, iEvt, jStat)] /=
            sum;
      // Else, the sum of probabilities is null
      else
        // It means this event never occurs,
        // set its probability to 0
        this->_probEvt[IEVT(this, iStat, iEvt)] = 0.0;
    }
    // Reset the sum of probability
    sum = 0.0;
    // For each possible event
    for (esmEvt iEvt = this->_nbEvent; iEvt--;)
      // Sum the probability of this event
      sum += this->_probEvt[IEVT(this, iStat, iEvt)];
    // If the sum is not null
    if (sum > ESM_EPSILON)
      // For each possible event
      for (esmEvt iEvt = this->_nbEvent; iEvt--;)
        // Normalize the probability of ('from', 'event') by dividing 
        // by the sum of probabilities
        this->_probEvt[IEVT(this, iStat, iEvt)] /= sum;
  }
}

// Free the memory used by the ESM
void ESMFree(EvtStatMac **this) {
  // Check arguments
  if (this == NULL || *this == NULL) 
    return;
  // Free the memory
  if ((*this)->_nbTransit != NULL) {
    free((*this)->_nbTransit);
    (*this)->_nbTransit = NULL;
  }
  if ((*this)->_nbEvt != NULL) {
    free((*this)->_nbEvt);
    (*this)->_nbEvt = NULL;
  }
  if ((*this)->_nbEvtToStat != NULL) {
    free((*this)->_nbEvtToStat);
    (*this)->_nbEvtToStat = NULL;
  }
  if ((*this)->_probEvt != NULL) {
    free((*this)->_probEvt);
    (*this)->_probEvt = NULL;
  }
  if ((*this)->_probEvtToStat != NULL) {
    free((*this)->_probEvtToStat);
    (*this)->_probEvtToStat = NULL;
  }
  free(*this);
  *this = NULL;
}

// Print the ESM on 'stream'. If 'statLbl' and/or 'evtLbl' are not 
// null they are used as labels for status and events respectively 
// instead of printing their index
void ESMPrint(EvtStatMac *this, FILE *stream, char **statLbl, 
  char **evtLbl) {
  // Check arguments
  if (this == NULL || stream == NULL) 
    return;
  // If there is no labels for the status
  if (statLbl == NULL)
    // Print the index of the current status
    fprintf(stream, "current status: %d\n", this->_curState);
  // Else, if the current status has a valid index
  else if (this->_curState >= 0 && this->_curState < this->_nbStatus)
    // Print the label of the current status
    fprintf(stream, "current status: '%s'\n", statLbl[this->_curState]);
  // Else, the current status is not a valid index
  else 
    // Print the current status
    fprintf(stream, "current status: (null, %d)\n", this->_curState);
  // For each possible status
  for (int from = 0; from < this->_nbStatus; ++from) {
    // If there is no labels for the status
    if (statLbl == NULL)
      // Print the index of the status 'from'
      fprintf(stream, "from status #%d:\n", from);
    // Else, if the status 'from' has a valid index
    else if (from >= 0 && from < this->_nbStatus)
      // Print the label of the status 'from'
      fprintf(stream, "from status '%s'\n", statLbl[from]);
    // Else, the status 'from' is not a valid index
    else 
      // Print the status 'from'
      fprintf(stream, "from status (null, %d)\n", from);
    // Flag to memorize if there is occurences from the status 'from'
    bool flagOccurence = false;
    // For each possible event
    for (int event = 0; event < this->_nbEvent; ++event) {
      // Get the probability of this event from the status 'from'
      float p = ESMGetPEvtGivFrom(this, from, event);
      // If the probability is not null
      if (p > ESM_EPSILON) {
        // Memorize that there is occurence from the status 'from'
        flagOccurence = true;
        // If there is no label for the events
        if (evtLbl == NULL)
          // Print the index of the event
          fprintf(stream, "  by event #%d (%f):\n", event, p);
        // Else, if the index of the event is valid
        else if (event >= 0 && event < this->_nbEvent)
          // Print the label of the event
          fprintf(stream, "  by event '%s' (%f):\n", evtLbl[event], p);
        // Else, the index of the event is not valid
        else
          // Print the event
          fprintf(stream, "  by event (null, %d) (%f):\n", event, p);
        // For each status 'to'
        for (int to = 0; to < this->_nbStatus; ++to) {
          // Get the probability of ('from', 'event', 'to')
          float q = ESMGetPToGivFromEvt(this, from, event, to);
          // If the probability is not null
          if (q > ESM_EPSILON) {
            // If there is no label for the status
            if (statLbl == NULL)
              // Print the index of the status 'to'
              fprintf(stream, "    -> status #%d (%f)\n", to, q);
            // Else, if the index for the status 'to' is valid
            else if (to >= 0 && to < this->_nbStatus)
              // Print the label of the status 'to'
              fprintf(stream, "    -> status '%s' (%f)\n", 
                statLbl[to], q);
            // Else, the index of the status is not valid
            else
              // Print the the status 'to'
              fprintf(stream, "    -> status (null, %d) (%f)\n", 
                to, q);
          }
        }
      }
    }
    // If there was no transit from the status 'from'
    if (flagOccurence == false)
      // Print an appropriate message
      fprintf(stream, "  no transit from this status\n");
  }
}

// Memorize one occurence of status 'to' after event 'event' from
// status 'from', and update all probabilities respectively
// ESMResetNull must be called before starting to learn
void ESMLearn(EvtStatMac *this, esmStat from, esmEvt event, 
  esmStat to) {
  // Check arguments
  if (this == NULL || event < 0 || event >= this->_nbEvent ||
    from < 0 || from >= this->_nbStatus ||
    to < 0 || to >= this->_nbStatus)
    return;
  // If we haven't reached the maximum number of samples
  if (this->_nbTransit[ITRANSIT(from)] < ESM_NBMAXSAMPLE) {
    // Update the number of transition for the status 'from'
    ++(this->_nbTransit[ITRANSIT(from)]);
    // Update the number of events for the status 'from'
    ++(this->_nbEvt[IEVT(this, from, event)]);
    // Update the number of transition for ('from', 'event');
    ++(this->_nbEvtToStat[IEVTTOSTAT(this, from, event, to)]);
    // Calculate the new probability for ('from', 'event')
    float p = (float)(this->_nbEvt[IEVT(this, from, event)]) / 
      (float)(this->_nbTransit[ITRANSIT(from)]);
    // Set the probability
    ESMSetPEvtGivFrom(this, from, event, p);
    // Calculate the new probability for ('from', 'event', 'to')
    float q = 
      (float)(this->_nbEvtToStat[IEVTTOSTAT(this, from, event, to)]) / 
      (float)(this->_nbEvt[IEVT(this, from, event)]);
    // Set the probability
    ESMSetPToGivFromEvt(this, from, event, to, q);
    // Keep the probabilities normalized
    ESMNormalize(this);
  // Else, if we have reached the maximum number of samples
  } else {
    // Calculate the new probability for ('from', 'event')
    float p = (ESMGetPEvtGivFrom(this, from, event) * 
      (float)(this->_nbTransit[ITRANSIT(from)]) + 1.0) / 
      ((float)(this->_nbTransit[ITRANSIT(from)]) + 1.0);
    // Set the probability
    ESMSetPEvtGivFrom(this, from, event, p);
    // Calculate the new probability for ('from', 'event', 'to')
    float q = (ESMGetPToGivFromEvt(this, from, event, to) * 
      (float)(this->_nbEvt[IEVT(this, from, event)]) + 1.0) / 
      ((float)(this->_nbEvt[IEVT(this, from, event)]) + 1.0);
    // Set the probability
    ESMSetPToGivFromEvt(this, from, event, to, q);
    // Keep the probabilities normalized
    ESMNormalize(this);
    // Correct the number of events for the status 'from'
    // to keep _nbEvt coherent with probability for ('from', 'event')
    this->_nbEvt[IEVT(this, from, event)] = 
      (int)round(p * (float)(this->_nbTransit[ITRANSIT(from)]));
    // Correct the number of events for the status 'from'
    // to keep _nbEvtToStat coherent with _nbEvt and 
    // probability for ('from', 'event', 'to')
    this->_nbEvtToStat[IEVTTOSTAT(this, from, event, to)] = 
      (int)round(q * (float)(this->_nbEvt[IEVT(this, from, event)]));
  }
}

// Get the current status
// Return ESM_STATNULL if argument is invalid
esmStat ESMGetStat(EvtStatMac *this) {
  // Check arguments
  if (this == NULL) 
    return ESM_STATNULL;
  // Return the current status
  return this->_curState;  
}

// Set the probability for 'event' to occur from status 'from'
void ESMSetPEvtGivFrom(EvtStatMac *this, esmStat from, esmEvt event, 
  float prob) {
  // Check arguments
  if (this == NULL || event < 0 || event >= this->_nbEvent ||
    from < 0 || from >= this->_nbStatus || prob < 0.0) 
    return;
  // Set the probability
  this->_probEvt[IEVT(this, from, event)] = prob;
}

// Set the probability to be in status 'to' after 'event' occured 
// from status 'from'
void ESMSetPToGivFromEvt(EvtStatMac *this, esmStat from, esmEvt event, 
  esmStat to, float prob) {
  // Check arguments
  if (this == NULL || event < 0 || event >= this->_nbEvent ||
    from < 0 || from >= this->_nbStatus ||
    to < 0 || to >= this->_nbStatus || prob < 0.0) 
    return;
  // Set the probability
  this->_probEvtToStat[IEVTTOSTAT(this, from, event, to)] = prob;
}

// Update the current status to a randomly choosen status from the
// current status
void ESMStep(EvtStatMac *this) {
  // Check arguments
  if (this == NULL) 
    return;
  // Set the current status to a randomly choosen status from the 
  // current status via a randomly choosen event according to 
  // memorized probabilities
  this->_lastEvt = ESMGetNextEvt(this);
  this->_curState = ESMGetNextStatByEvt(this, this->_lastEvt);
}

// Update the current status to a randomly choosen status given that 
// 'event' has occured from the current status
void ESMStepByEvt(EvtStatMac *this, int event) {
  // Check arguments
  if (this == NULL || event < 0 || event >= this->_nbEvent) 
    return;
  // Set the current status to a randomly choosen status from the 
  // current status via the requested event according to 
  // memorized probabilities
  this->_curState = ESMGetNextStatByEvt(this, event);
  this->_lastEvt = event;
}

// Get a randomly choosen event from the current status 
// Return ESM_EVTNULL if arguments are invalid
esmEvt ESMGetNextEvt(EvtStatMac *this) {
  // Check arguments
  if (this == NULL || this->_curState < 0 || 
    this->_curState >= this->_nbStatus) 
    return ESM_EVTNULL;
  // Return a randomly choosen event according to memorized 
  // probabilities of events from the current status
  return ESMGetRnd(this->_probEvt + IEVT(this, this->_curState, 0), 
    this->_nbEvent);
}

// Get the last occured event 
// Return ESM_EVTNULL if arguments are invalid
esmEvt ESMGetLastEvt(EvtStatMac *this) {
  // Check arguments
  if (this == NULL) 
    return ESM_EVTNULL;
  // Return the last event
  return this->_lastEvt;
}

// Get a randomly choosen next status from the current status
// Return ESM_STATNULL if arguments are invalid
esmStat ESMGetNextStat(EvtStatMac *this) {
  // Check arguments
  if (this == NULL) return ESM_STATNULL;
  // Return a randomly choosen status from the current status via 
  // a randomly choosen event according to memorized probabilities
  return ESMGetNextStatByEvt(this, ESMGetNextEvt(this));
}

// Get a randomly choosen next status given that 'event' has occured 
// from the current status
// Return ESM_STATNULL if arguments are invalid
esmStat ESMGetNextStatByEvt(EvtStatMac *this, esmEvt event) {
  // Check arguments
  if (this == NULL || event < 0 || event >= this->_nbEvent ||
    this->_curState < 0 || this->_curState >= this->_nbStatus) 
    return ESM_STATNULL;
  // Return a randomly choosen status according to memorized 
  // probabilities of status from the current status via the requested
  // event
  return ESMGetRnd( 
    this->_probEvtToStat + 
    IEVTTOSTAT(this, this->_curState, event, 0), 
    this->_nbStatus);
}

// Get the probability that 'event' will occur from status 'from'
// Return 0.0 if arguments are invalid
float ESMGetPEvtGivFrom(EvtStatMac *this, esmStat from, 
  esmEvt event) {
  // Check arguments
  if (this == NULL || event < 0 || event >= this->_nbEvent ||
    from < 0 || from >= this->_nbStatus) 
    return 0.0;
  // Return the memorized probability
  return this->_probEvt[IEVT(this, from, event)];
}

// Get the probability that status 'to' occured knowing 'event'
// occured after status 'from'
// Return 0.0 if arguments are invalid
float ESMGetPToGivFromEvt(EvtStatMac *this, esmStat from, esmEvt event, 
  esmStat to) {
  // Check arguments
  if (this == NULL || event < 0 || event >= this->_nbEvent ||
    from < 0 || from >= this->_nbStatus ||
    to < 0 || to >= this->_nbStatus) 
    return 0.0;
  // Return the memorized probability
  return this->_probEvtToStat[IEVTTOSTAT(this, from, event, to)];  
}

// Recursive function for ESMGetProbStatByStat
float ESMGetPToGivFromRec(EvtStatMac *this, esmStat from, esmStat to, 
  int nbEvt) {
  // Variable to memorize the probability
  double prob = 0.0;
  // If we have reach the requested number of event and we are at the
  // requested 'from' status
  if (from == to && nbEvt == 0) {
    // Return a probability of 1.0, which will sum up the probability 
    // down to this point
    prob = 1.0;
  // Else, if we haven't reach yet the requested number of event
  // keep checking the possible transition leading to the status 'to'
  } else if (nbEvt > 0) {
    // For each possible status
    for (esmStat iStat = this->_nbStatus; iStat--;) {
      // For each possible event
      for (esmEvt iEvt = this->_nbEvent; iEvt--;) {
        // Get the probability for the transition (iStat, iEvt, to)
        float p = this->_probEvt[IEVT(this, iStat, iEvt)] * 
          this->_probEvtToStat[IEVTTOSTAT(this, iStat, iEvt, to)];
        // If the probability for this transition is not null
        if (p > ESM_EPSILON) {
          // Go back one more event seaching for transitions going 
          // to iStat, multiply by the probability of these transition
          // and add to the total probability
          p *= ESMGetPToGivFromRec(this, from, iStat, nbEvt - 1);
          prob += p;
        }
      }
    }
  }
  // Return the result probability
  return prob;
}

// Probability to ends to status 'to' after 'nbEvt' events starting 
// from status 'from'
// Return 0.0 if arguments are invalid
float ESMGetPToGivFrom(EvtStatMac *this, esmStat from, esmStat to, 
  int nbEvt) {
  // Check arguments
  if (this == NULL || from < 0 || from >= this->_nbStatus ||
    to < 0 || to >= this->_nbStatus) 
    return 0.0;
  // Start the recursion
  double p = ESMGetPToGivFromRec(this, from, to, nbEvt);
  // Return the probability
  return p;
}

// Probability to be in status 'status'
// Return 0.0 if arguments are invalid
float ESMGetPStatus(EvtStatMac *this, esmStat status) {
  // Check arguments
  if (this == NULL || status < 0 || status >= this->_nbStatus) 
    return 0.0;
  // Variable to memorize the delta between probability for ending
  // condition of the loop
  float dprob = 0.0;
  // Allocate memory for the calculated probabilities of each status
  float *prior = (float*)malloc(sizeof(float) * this->_nbStatus);
  if (prior == NULL) return 0.0;
  // Initialize the probability of each status to 1/_nbStatus
  // (consider all status as equally probable by defaut) 
  for (int iStat = this->_nbStatus; iStat--;)
    prior[iStat] = 1.0 / (float)(this->_nbStatus);
  // Allocate memory for the new calculated probability of each status
  float *nprior = (float*)malloc(sizeof(float) * this->_nbStatus);
  if (nprior == NULL) {
    free(prior);
    return 0.0;
  }
  // Security to avoid infinite loop
  int nbMaxIter = 10000;
  // Loop to calculate the limit
  do {
    // For each possible status 'to'
    for (int iStat = this->_nbStatus; iStat--;) {
      // Set the new probability of this status to 0.0
      nprior[iStat] = 0.0;
      // For each possible status 'from'
      for (int jStat = this->_nbStatus; jStat--;)
        // Sum the probability of transition from 'from' to 'to'
        // multiplied by the probability of 'from'
        nprior[iStat] += prior[jStat] * 
          ESMGetPToGivFrom(this, jStat, iStat, 1);
    }
    // For each possible status
    for (int iStat = this->_nbStatus; iStat--;) {
      // If it's the requested status
      if (iStat == status)
        // Memorize the delta between current probability and new
        // probability
        dprob = fabs(nprior[iStat] - prior[iStat]);
      // Update the probability of this status with the newly calculated
      // probability
      prior[iStat] = nprior[iStat];
    }
    --nbMaxIter;
  // Loop until the probability of the requested status is not stable
  } while (dprob > ESM_EPSILON && nbMaxIter > 0);
  // Memorize the probability of the requested status
  float prob = prior[status];
  // Free memory
  free(prior);
  free(nprior);
  // Return the result probability
  return prob;
}

// Probability that event 'event' occurs
// Return 0.0 if arguments are invalid
float ESMGetPEvent(EvtStatMac *this, esmEvt event) {
  // Check arguments
  if (this == NULL || event < 0 || event >= this->_nbEvent) 
    return 0.0;
  // Variable to memorize the result probability
  float prob = 0.0;
  // For each possible status
  for (int iStat = this->_nbStatus; iStat--;)
    // Sum the probability of the event frm this status multiplied
    // by the probability of this status
    prob += ESMGetPStatus(this, iStat) * 
      ESMGetPEvtGivFrom(this, iStat, event);
  // Return the result probability
  return prob;
}

// Probability that a transition from status 'from' through event 
// 'event' occurs
// Return 0.0 if arguments are invalid
float ESMGetPFromEvt(EvtStatMac *this, esmStat from, esmEvt event) {
  // Check arguments
  if (this == NULL || from < 0 || from >= this->_nbStatus ||
    event < 0 || event >= this->_nbEvent) 
    return 0.0;
  return ESMGetPEvtGivFrom(this, from, event) * 
    ESMGetPStatus(this, from);
}

// Probability that a transition from status 'from' to status 'to' occurs
// Return 0.0 if arguments are invalid
float ESMGetPFromTo(EvtStatMac *this, esmStat from, esmStat to) {
  // Check arguments
  if (this == NULL || from < 0 || from >= this->_nbStatus ||
    to < 0 || to >= this->_nbStatus) 
    return 0.0;
  return ESMGetPToGivFrom(this,from, to, 1) * 
    ESMGetPStatus(this, from);
}

// Probability that a transition to status 'to' through event 'event' 
// occurs
// Return 0.0 if arguments are invalid
float ESMGetPEvtTo(EvtStatMac *this, esmEvt event, esmStat to) {
  // Check arguments
  if (this == NULL || event < 0 || event >= this->_nbEvent ||
    to < 0 || to >= this->_nbStatus) 
    return 0.0;
  float p = 0.0;
  for (int iStat = this->_nbStatus; iStat--;)
    p += ESMGetPToGivFromEvt(this, iStat, event, to) * 
      ESMGetPEvtGivFrom(this, iStat, event) * 
      ESMGetPStatus(this, iStat);
  return p;
}

// Probability that a transition from status 'from' through event 
// 'event' to status 'to' occurs
// Return 0.0 if arguments are invalid
float ESMGetPFromEvtTo(EvtStatMac *this, esmStat from, esmEvt event,
  esmStat to) {
  // Check arguments
  if (this == NULL || from < 0 || from >= this->_nbStatus ||
    event < 0 || event >= this->_nbEvent ||
    to < 0 || to >= this->_nbStatus) 
    return 0.0;
  return ESMGetPStatus(this, from) * 
    ESMGetPEvtGivFrom(this, from, event) *
    ESMGetPToGivFromEvt(this, from, event, to);
}

// Probability that a transition started from status 'from' knowing 
// that event 'event' occured
// Return 0.0 if arguments are invalid or P(event) = 0.0 
float ESMGetPFromGivEvt(EvtStatMac *this, esmStat from, esmEvt event) {
  // Check arguments
  if (this == NULL || from < 0 || from >= this->_nbStatus ||
    event < 0 || event >= this->_nbEvent) 
    return 0.0;
  float d = ESMGetPEvent(this, event);
  if (d > ESM_EPSILON)
    return ESMGetPEvtGivFrom(this, from, event) * 
      ESMGetPStatus(this, from) / d;
  else
    return 0.0;
}

// Probability that a transition started from status 'from' knowing 
// that it has ended to status 'to'
// Return 0.0 if arguments are invalid or P(to) = 0.0
float ESMGetPFromGivTo(EvtStatMac *this, esmStat from, esmStat to) {
  // Check arguments
  if (this == NULL || from < 0 || from >= this->_nbStatus ||
    to < 0 || to >= this->_nbStatus) 
    return 0.0;
  float d = ESMGetPStatus(this, to);
  if (d > ESM_EPSILON)
    return ESMGetPToGivFrom(this, from, to, 1) * 
      ESMGetPStatus(this, from) / d;
  else
    return 0.0;
}

// Probability that a transition ends to status 'to' knowing that 
// event 'event' occured
// Return 0.0 if arguments are invalid or P(event) = 0.0
float ESMGetPToGivEvt(EvtStatMac *this, esmEvt event, esmStat to) {
  // Check arguments
  if (this == NULL || event < 0 || event >= this->_nbEvent ||
    to < 0 || to >= this->_nbStatus) 
    return 0.0;
  float d = ESMGetPEvent(this, event);
  if (d > ESM_EPSILON)
    return ESMGetPEvtTo(this, event, to) / d;
  else
    return 0.0;
}

// Probability that a transition through event 'event' occured knowing that it ended to status 'to'
// Return 0.0 if arguments are invalid or P(to) = 0.0
float ESMGetPEvtGivTo(EvtStatMac *this, esmEvt event, esmStat to) {
  // Check arguments
  if (this == NULL || event < 0 || event >= this->_nbEvent ||
    to < 0 || to >= this->_nbStatus) 
    return 0.0;
  float d = ESMGetPStatus(this, to);
  if (d > ESM_EPSILON)
    return ESMGetPEvtTo(this, event, to) / d;
  else
    return 0.0;
}

// Probability that a transition started from status 'from' knowing 
// that 'event' occured and ended to status 'to'
// Return 0.0 if arguments are invalid or P(event, to) = 0.0
float ESMGetPFromGivEvtTo(EvtStatMac *this, esmStat from, esmEvt event,
  esmStat to) {
  // Check arguments
  if (this == NULL || from < 0 || from >= this->_nbStatus ||
    event < 0 || event >= this->_nbEvent ||
    to < 0 || to >= this->_nbStatus) 
    return 0.0;
  float d = ESMGetPEvtTo(this, event, to);
  if (d > ESM_EPSILON)
    return ESMGetPFromEvtTo(this, from, event, to) / d;
  else
    return 0.0;
}

// Probability that a transition through event 'event' occured knowing 
// that it started from status 'from' and ended to status 'to'
// Return 0.0 if arguments are invalid or P(from, to) = 0.0
float ESMGetPEvtGivFromTo(EvtStatMac *this, esmStat from, 
  esmEvt event, esmStat to) {
  // Check arguments
  if (this == NULL || event < 0 || event >= this->_nbEvent ||
    from < 0 || from >= this->_nbStatus ||
    to < 0 || to >= this->_nbStatus) 
    return 0.0;
  float d = ESMGetPFromTo(this, from, to);
  if (d > ESM_EPSILON)
    return ESMGetPFromEvtTo(this, from, event, to) / d;
  else
    return 0.0;
}

