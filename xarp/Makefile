output: arp_linked_list.o xarp.h xarp.c communication.o ../definitions.h
	gcc -Wall -pedantic -std=c99 arp_linked_list.o communication.o xarp.c -o xarp -pthread

arp_linked_list.o: ../arp_linked_list.c ../arp_linked_list.h
		gcc -c ../arp_linked_list.c

communication.o: ../communication.h ../communication.c
	gcc -c -Wall -pedantic -std=c99 ../communication.c

clean:
	rm *.o xarp
