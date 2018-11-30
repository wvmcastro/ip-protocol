output: xifconfig.c xifconfig.h communication.o ../my_interface.h ../definitions.h
	gcc -Wall -pedantic -std=c99 communication.o xifconfig.c -o xifconfig

communication.o: ../communication.h ../communication.c
	gcc -c -Wall -pedantic -std=c99 ../communication.c

clean:
	rm *.o xifconfig
