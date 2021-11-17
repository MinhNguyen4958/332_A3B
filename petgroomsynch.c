#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "petgroomsynch.h"


// a makeshift semaphore of avaiable stations
int available_stations;

int total_stations;
pthread_mutex_t stationLock;
pthread_cond_t condStation;

// any array of pets that are currently in a grooming station
pet_t* petLogs;

// number of petgroom_init() calls

int initCalled = 0;

int maxWait = 2;
int waitCounter = 0;

// number of cats in grooming stations
int numCats;
// number of dogs in grooming stations
int numDogs;


void waitStation(char* pet_type) {
    printf("%s is waiting for a station\n", pet_type);
    waitCounter++;
    pthread_cond_wait(&condStation, &stationLock);
}
int petgroom_init(int numstations) {
    initCalled++;

    // if petgroom_init is called more than once without calling petgroom_done first, return failure
    if (initCalled == 2) {
        printf("init() has already been initialized!\n");
        return -1;
    }

    available_stations = numstations;
    total_stations = numstations;

    pthread_mutex_init(&stationLock, NULL);
    pthread_cond_init(&condStation, NULL);
    
    petLogs = (pet_t*) malloc(numstations * sizeof(pet_t));
    
    numCats = 0;
    numDogs = 0;

    return 0;
}

int newpet(pet_t pet) {
    pthread_mutex_lock(&stationLock);
    char* pet_type;
    
    if (pet == cat) {
        pet_type = "cat";
    } else if (pet == dog) {
        pet_type = "dog";
    } else if (pet == other) {
        pet_type = "random pet";
    }

    if (pet == other) {
        while (available_stations == 0) {
            waitStation(pet_type);
        }
    } else if (pet == cat) {
        while (numDogs > 0 || available_stations == 0)  {
            waitStation(pet_type);
        }
    } else {
        while (numCats > 0 || available_stations == 0) {
            waitStation(pet_type);
        }
    }

    waitCounter = 0;
    int counter = 0;
    // search through the logs for an available station
    while (petLogs[counter] != none) { 
        counter++;
    }

    petLogs[counter] = pet;
    // decrement the sema by 1

    available_stations -= 1;

    if (pet == cat) { 
        numCats++;
    } else if (pet == dog) {
        numDogs++;
    }

    printf("%s has entered grooming station [%d]\n", pet_type, counter);
    pthread_mutex_unlock(&stationLock);
    return 0;
  }

int petdone(pet_t pet) {
    pthread_mutex_lock(&stationLock);
    char* pet_type;
    
    if (pet == cat) {
        pet_type = "cat";
    } else if (pet == dog) {
        pet_type = "dog";
    } else {
        pet_type = "random pet";
    }
    int counter = 0;
    while (petLogs[counter] != pet) {
        counter++;
    }
    // pet is found, remove the pet from log
    petLogs[counter] = none;
    printf("%s has done being groomed at station [%d]\n", pet_type, counter);

    available_stations++;
    if (pet == cat) {
        numCats--;
    } else if (pet == dog) {
        numDogs--;
    }

    pthread_cond_broadcast(&condStation);
    pthread_mutex_unlock(&stationLock);
    return 0;
}

int petgroom_done() {

    // clean up all variables
    available_stations = 0;
    initCalled--;
    pthread_mutex_destroy(&stationLock);
    pthread_cond_destroy(&condStation);
    free(petLogs);
    return 0;
}

void* grooming(void* aPet) {
    pet_t pet = *(pet_t*) aPet;
    newpet(pet);
    sleep(5);
    petdone(pet);
    free(aPet);
}