#ifndef __PETGROOMSYNCH_H__
#define __PETGROOMSYNCH_H__

#include <stdlib.h>
// declare
typedef enum {none = 0, cat = 1, dog = 2, other = 3} pet_t;

// initialize needed variables
int petgroom_init(int numstations);

// new pet needing to be groomed
int newpet(pet_t pet);

// pet is done being groomed
int petdone(pet_t pet);

// clean up all variables' values
int petgroom_done();

// grooming a given pet
void* grooming(void* aPet);

#endif                                  