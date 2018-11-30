output: linked_list.o ipd.o communication.o arp_protocol.o ../definitions.h ../my_interface.h protocol_headers.h
	gcc -Wall -pedantic -std=c99 linked_list.o ipd.o communication.o arp_protocol.o -o ipd -pthread

linked_list.o: ../linked_list.c ../linked_list.h
	gcc -c ../linked_list.c

ipd.o: ipd.c
	gcc -c ipd.c

communication.o: ../communication.c ../communication.h
	gcc -c ../communication.c

arp_protocol.o: arp_protocol.h arp_protocol.c protocol_headers.h
	gcc -c -Wall -pedantic -std=c99 arp_protocol.c

clean:
	rm *.o ipd
