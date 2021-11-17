#include <stdio.h>
#include <pthread.h>

#include "petgroomsynch.h"

#define CUSTOMERS 5
#define STATIONS 3
int main()
{
    // initizalize the grooming facility 
    petgroom_init(STATIONS);
    pthread_t customers_list[CUSTOMERS];
    pet_t pets[] = {dog, dog, dog, dog, cat};
    printf("The facility currently has [%d] stations and [%d] customers\n", STATIONS, CUSTOMERS);
    for (int i = 0; i < CUSTOMERS; i++) {
        pet_t* aPet = malloc(sizeof(pet_t));
        *aPet = pets[i];
        if (pthread_create(&customers_list[i], NULL, &grooming, aPet) != 0) {
            perror("failed to create thread\n");
        }
    }

    for (int i = 0; i < CUSTOMERS; i++) {
        if (pthread_join(customers_list[i], NULL) != 0) {
            perror("Failed to join thread\n");
        }
    }
    return 0;
}