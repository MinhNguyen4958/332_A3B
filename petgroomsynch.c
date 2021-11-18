#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "petgroomsynch.h"

// threshold for numbers of cats/dogs can be scheduled to a grooming station continuously while the other is waiting 
#define SCHEDTHRESHOLD 3

// a makeshift semaphore of avaiable stations
int available_stations;

pthread_mutex_t stationLock;
pthread_cond_t condStation;

// any array of pets that are currently in a grooming station
pet_t* petLogs;

// the amount of wait for dogs/cats
int DogsWaiting;
int CatsWaiting;

// the number of dogs/cats scheduled to a station
int DogsSched;
int CatsSched;

// number of cats in grooming stations
int numCats;
// number of dogs in grooming stations
int numDogs;

int petgroom_init(int numstations) {
    available_stations = numstations;

    if (pthread_mutex_init(&stationLock, NULL) != 0) {
        return -1;
    }
     if (pthread_cond_init(&condStation, NULL) != 0) {
        return -1;
     }
    
    petLogs = (pet_t*) malloc(numstations * sizeof(pet_t));
    
    numCats = 0;
    numDogs = 0;

    DogsWaiting = 0;
    CatsWaiting = 0;

    DogsSched = 0;
    CatsSched = 0;

    return 0;
}

int newpet(pet_t pet) {

    if (pthread_mutex_lock(&stationLock) != 0) {
        return -1;
    }

    if (pet == other) {
        // other just needs to wait for an available station
        while (available_stations == 0) {
            printf("other is waiting for a station\n");
            if (pthread_cond_wait(&condStation, &stationLock) != 0) {
                return -1;
            }
        }
    } else if (pet == cat) {
        // while there are no available stations, CatsSched reached threshold, and there are still dogs being groomed, cats wait
        while (!(available_stations > 0 && CatsSched < SCHEDTHRESHOLD && numDogs == 0)) {
            CatsWaiting++; // increment the wait counter by 1
            printf("cat waiting counter: %d | dog waiting counter: %d\n", CatsWaiting, DogsWaiting);
            if (pthread_cond_wait(&condStation, &stationLock) != 0) {
                return -1;
            }
        }
    } else if (pet == dog) {
        // while there are no available stations, DogsSched reached threshold, and there are still cats being groomed, dogs wait
        while (!(available_stations > 0 && DogsSched < SCHEDTHRESHOLD && numCats == 0)) {
            DogsWaiting++; // increment the wait counter by 1
            printf("cat waiting counter: %d | dog waiting counter: %d\n", CatsWaiting, DogsWaiting);
            if (pthread_cond_wait(&condStation, &stationLock) != 0) {
                return -1;
            }
        }
    }

    if (pet == cat) { 
        numCats++; // increase the number of cats being groomed
        CatsSched++; // increase the number of cats being scheduled
    } else if (pet == dog) {
        numDogs++; // increase the number of dogs being groomed
        DogsSched++; // increase the number of dogs being groomed
    }

    // if the cat/dog has been scheduled, reset the wait counter
    if (pet == cat && CatsWaiting > 0) {
        CatsWaiting = 0;
    } else if (pet == dog && DogsWaiting > 0) {
        DogsWaiting = 0;
    }

    int counter = 0;
    // search through the logs for an available station
    while (petLogs[counter] != 0) { 
        counter++;
    }
    petLogs[counter] = pet;

    // decrement the sema by 1
    available_stations -= 1;

    char* pet_type;
    if (pet == cat) {
        pet_type = "cat";
    } else if (pet == dog) {
        pet_type = "dog";
    } else if (pet == other) {
        pet_type = "other";
    }

    printf("%s is being groomed\n", pet_type);
    printf("%d dogs and %d cats\n", numDogs, numCats);

    if (pthread_mutex_unlock(&stationLock) != 0) {
        return -1;
    }
    return 0;
  }

int petdone(pet_t pet) {
    if (pthread_mutex_lock(&stationLock) != 0) {
        return -1;
    }

    int counter = 0;
    while (petLogs[counter] != pet) {
        counter++;
    }
    // pet is found, remove the pet from log
    petLogs[counter] = 0;

    // increase sema by 1
    available_stations++;

    //decrement the number of cats/dogs being groomed
    if (pet == cat) {
        numCats--;
    } else if (pet == dog) {
        numDogs--;
    }

    // For dogs, when there are no cats waiting and threshold has been reached, reset sched counter
    if (CatsWaiting == 0 && DogsSched == SCHEDTHRESHOLD) {
        DogsSched = 0;
    // For cats, when there are no dogs waiting and threshold has been reached, reset sched counter
    } else if (DogsWaiting == 0 && CatsSched == SCHEDTHRESHOLD) {
        CatsSched = 0;
    }

    char* pet_type;
    if (pet == cat) {
        pet_type = "cat";
    } else if (pet == dog) {
        pet_type = "dog";
    } else {
        pet_type = "other";
    }

    printf("%s has done being groomed\n", pet_type);
    printf("%d dogs and %d cats\n", numDogs, numCats);

    if (pthread_cond_broadcast(&condStation) != 0) {
        return -1;
    }
    if (pthread_mutex_unlock(&stationLock) != 0) {
        return -1;
    }
    return 0;
}

int petgroom_done() {
    // clean up all variables
    available_stations = 0;
    if (pthread_mutex_destroy(&stationLock) != 0) {
        return -1;
    }
    if (pthread_cond_destroy(&condStation) != 0) {
        return -1;
    }
    free(petLogs);

    numCats = 0;
    numDogs = 0;

    DogsWaiting = 0;
    CatsWaiting = 0;

    DogsSched = 0;
    CatsSched = 0;
    return 0;
}

void* grooming(void* aPet) {
    pet_t pet = *(pet_t*) aPet;
    int arrivalTime = (rand() % 10) + 1; // arrival time between 1 to 10
    
    char* pet_type;
    
    if (pet == cat) {
        pet_type = "cat";
    } else if (pet == dog) {
        pet_type = "dog";
    } else if (pet == other) {
        pet_type = "other";
    }

    printf("%s will arrive a grooming station after %d seconds\n", pet_type, arrivalTime);
    sleep(arrivalTime);
    newpet(pet);
    sleep(6); // grooming for 7 seconds
    petdone(pet);
    free(aPet);
}