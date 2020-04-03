//Author Peter Adamson

#include <stdio.h>

#include <pthread.h>

#include <stdlib.h>

#include <time.h>

#include <semaphore.h>

//global variable declarations
int tobaccoOnTable; //0 if not on table, 1 if on table
int paperOnTable; //0 if not on table, 1 if on table
int matchesOnTable; //0 if not on table, 1 if on table
int tableFull; //0 if no items on table, 1 if items on table
int smokerNum;
pthread_mutex_t lock1;
sem_t waiter;

//function declarations
void * barman();
void * smoker();
int randInt(int min, int max);

int main(int argc, char ** argv) {
  //set up our end time condition
  time_t endAt;
  time_t start = time(NULL);
  time_t end = 100;
  endAt = start + end;

  int i;
  tobaccoOnTable = 0; //initalize on table value
  paperOnTable = 0; //initialize on table value
  matchesOnTable = 0; //initialize on table value
  //tableFull = 0;
  smokerNum = 1;
  int n = 3; //number of smokers
  sem_init( & waiter, 0, 0);

  //declare the barman and smokers
  pthread_t barmanThread;
  pthread_t smokerThread[n];

  //create the barman and smokers
  pthread_create( & barmanThread, NULL, barman, NULL);
  for (i = 0; i < n; i++) {
    pthread_create( & smokerThread[i], NULL, smoker, NULL);
  }

  //loop the main thread until timeout has been reached
  while (1) {
    if (start < endAt) { //we have not reached the timeout
    
      sleep(1);
      start = time(NULL);
    } else { //we have reached the timeout
    
      pthread_cancel(barmanThread); //cancel the TA
      for (i = 0; i < n; i++) {
        pthread_cancel(smokerThread[i]); //cancel the students
      }
      break;
    }
  }
}

void * barman() {
  int decision;
  while (1) {
    decision = randInt(1, 3);
    if (decision == 1) {
      pthread_mutex_lock( & lock1);
      tobaccoOnTable = 1;
      paperOnTable = 1;
      //tableFull = 1;
      printf("barman placed tobacco and paper on the table\n");
      pthread_mutex_unlock( & lock1);
      sem_wait( & waiter);
    } else if (decision == 2) {
      pthread_mutex_lock( & lock1);
      tobaccoOnTable = 1;
      matchesOnTable = 1;
      //tableFull = 1;
      printf("barman placed tobacco and matches on the table\n");
      pthread_mutex_unlock( & lock1);
      sem_wait( & waiter);
    } else if (decision == 3) {
      pthread_mutex_lock( & lock1);
      paperOnTable = 1;
      matchesOnTable = 1;
      //tableFull = 1;
      printf("barman placed paper and matches on the table\n");
      pthread_mutex_unlock( & lock1);
      sem_wait( & waiter);
    }
  }
}

void * smoker() {
  pthread_mutex_lock( & lock1);
  int smokerNumber = smokerNum;
  int hasTobacco = 0;
  int hasPaper = 0;
  int hasMatches = 0;
  int sleepTime;
  smokerNum = smokerNum + 1;
  pthread_mutex_unlock( & lock1);
  if (smokerNumber == 1) {//set this smoker to have tobacco
    hasTobacco = 1;
  } else if (smokerNumber == 2) {//set this smoker to have paper
    hasPaper = 1;
  } else if (smokerNumber == 3) {//set this smoker to have matches
    hasMatches = 1;
  }

  while (1) {
    if (hasTobacco == 1) {
      pthread_mutex_lock( & lock1);
      if (paperOnTable == 1 && matchesOnTable == 1) {
        paperOnTable = 0;
        matchesOnTable = 0;
        sem_post( & waiter);
        pthread_mutex_unlock( & lock1);
        printf("smoker number %d grabbed paper and matches off the table and began smoking\n", smokerNumber);
        sleepTime = randInt(5, 20);
        sleep(sleepTime);
      } else {
        pthread_mutex_unlock( & lock1);
      }
    } else if (hasPaper == 1) {
      pthread_mutex_lock( & lock1);
      if (tobaccoOnTable == 1 && matchesOnTable == 1) {
        tobaccoOnTable = 0;
        matchesOnTable = 0;
        sem_post( & waiter);
        pthread_mutex_unlock( & lock1);
        printf("smoker number %d grabbed tobacco and matches off the table and began smoking\n", smokerNumber);
        sleepTime = randInt(5, 20);
        sleep(sleepTime);
      } else {
        pthread_mutex_unlock( & lock1);
      }
    } else if (hasMatches == 1) {
      pthread_mutex_lock( & lock1);
      if (tobaccoOnTable == 1 && paperOnTable == 1) {
        tobaccoOnTable = 0;
        paperOnTable = 0;
        sem_post( & waiter);
        pthread_mutex_unlock( & lock1);
        printf("smoker number %d grabbed tobacco and papers off the table and began smoking\n", smokerNumber);
        sleepTime = randInt(5, 20);
        sleep(sleepTime);
      } else {
        pthread_mutex_unlock( & lock1);
      }
    }
  }
}

//generates a random integer between min and max (inclusively)
int randInt(int min, int max) {
  return rand() % (max + 1 - min) + min;
}