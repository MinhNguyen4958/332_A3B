#include <stdio.h>
#include <pthread.h>

#include "petgroomsynch.h"

#define CUSTOMERS 8
#define STATIONS 3
int main()
{
    // test case 1: 6 dogs 2 cats needs being groomed
    printf("Test 1: 6 dogs 2 cats needs being groomed with a threshold of 3 pets\n");

    // initizalize the grooming facility 
    petgroom_init(STATIONS);
    pthread_t customers_Test1[CUSTOMERS];
    pet_t petsTest1[] = {dog, dog, cat, dog, dog, dog, cat, dog};
    printf("The facility currently has [%d] stations and [%d] customers\n", STATIONS, CUSTOMERS);
    for (int i = 0; i < CUSTOMERS; i++) {
        pet_t* aPet = malloc(sizeof(pet_t));
        *aPet = petsTest1[i];
        if (pthread_create(&customers_Test1[i], NULL, &grooming, aPet) != 0) {
            perror("failed to create thread\n");
        }
    }
    // join a thread
    for (int i = 0; i < CUSTOMERS; i++) {
        if (pthread_join(customers_Test1[i], NULL) != 0) {
            perror("Failed to join thread\n");
        }
    }
    petgroom_done();

    // test case 2: a steady stream of dogs, no cats
    // Reason: to test DogsSched reset when there are no cats in the queue
    printf("\nTest 2: a steady stream of dogs, no cats\n");
    petgroom_init(STATIONS);
    pthread_t customers_Test2[CUSTOMERS];
    pet_t petsTest2[] = {dog, dog, dog, dog, dog, other, dog, dog};
    printf("The facility currently has [%d] stations and [%d] customers\n", STATIONS, CUSTOMERS);
    for (int i = 0; i < CUSTOMERS; i++) {
        pet_t* aPet = malloc(sizeof(pet_t));
        *aPet = petsTest2[i];
        if (pthread_create(&customers_Test2[i], NULL, &grooming, aPet) != 0) {
            perror("failed to create thread\n");
        }
    }

    // join a thread
    for (int i = 0; i < CUSTOMERS; i++) {
        if (pthread_join(customers_Test1[i], NULL) != 0) {
            perror("Failed to join thread\n");
        }
    }
    petgroom_done();

    // test case 3: a steady stream of cats, no dogs
    // Reason: to test CatsSched variable reset when there are no dogs in the queue
    printf("\nTest 3: a steady stream of cats, no dogs\n");
    petgroom_init(STATIONS);
    pthread_t customers_Test3[CUSTOMERS];
    pet_t petsTest3[] = {cat, cat, cat, cat, cat, cat, cat, cat};
    printf("The facility currently has [%d] stations and [%d] customers\n", STATIONS, CUSTOMERS);
    for (int i = 0; i < CUSTOMERS; i++) {
        pet_t* aPet = malloc(sizeof(pet_t));
        *aPet = petsTest3[i];
        if (pthread_create(&customers_Test3[i], NULL, &grooming, aPet) != 0) {
            perror("failed to create thread\n");
        }
    }

    // join a thread
    for (int i = 0; i < CUSTOMERS; i++) {
        if (pthread_join(customers_Test1[i], NULL) != 0) {
            perror("Failed to join thread\n");
        }
    }
    petgroom_done();
    return 0;
}