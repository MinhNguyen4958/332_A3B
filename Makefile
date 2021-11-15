C = gcc
CFLAGS = -Wall -g

.PHONY: clean
pet_testing: pet_testing.o petgroomsynch.o
	${C} ${CLFAGS} -o pet_testing pet_testing.o petgroomsynch.o

pet_testing.o: pet_testing.c petgroomsynch.h

clean:
	rm -f *.o pet_testing