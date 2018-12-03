output: xroute.o communication.o ip_linked_list.o
	gcc -Wall -pedantic -std=c99 communication.o ip_linked_list.o xroute.o -o xroute -pthread

xroute.o: xroute.h xroute.c ../definitions.h communication.o
	gcc -c -Wall -pedantic -std=c99 xroute.c

communication.o: ../communication.h ../communication.o
	gcc -c -Wall -pedantic -std=c99 ../communication.c

ip_linked_list.o: ../definitions.h ../ip_linked_list.h ../ip_linked_list.c
	gcc -c -Wall -pedantic -std=c99 ../ip_linked_list.c

clean:
	rm *.o xroute
