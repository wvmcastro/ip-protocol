output: arp_linked_list.o ip_linked_list.o ipd.o xroute_server.o communication.o arp_protocol.o ../definitions.h ../my_interface.h protocol_headers.h xifconfig_server.o xarp_server.o ethernet_protocol.o
	gcc -Wall -pedantic -std=c99 ethernet_protocol.o ip_linked_list.o arp_linked_list.o xroute_server.o ipd.o communication.o xifconfig_server.o xarp_server.o arp_protocol.o -o ipd -pthread

ipd.o: ipd.c
	gcc -c ipd.c -pthread

xifconfig_server.o: xifconfig_server.h arp_linked_list.o ../my_interface.h ../definitions.h ../communication.o
	gcc -c -Wall -pedantic -std=c99 xifconfig_server.c -pthread

xarp_server.o: xarp_server.h xarp_server.c arp_linked_list.o ../my_interface.h ../communication.h arp_protocol.o
	gcc -c -Wall -pedantic xarp_server.c -pthread

xroute_server.o: xroute_server.h xroute_server.c ip_linked_list.o communication.o
	gcc -c -Wall -pedantic xroute_server.c

arp_protocol.o: arp_protocol.h arp_protocol.c protocol_headers.h ../my_interface.h ../definitions.h communication.o ethernet_protocol.o
	gcc -c -Wall -pedantic -std=c99 arp_protocol.c

ip_linked_list.o: ../ip_linked_list.c ../ip_linked_list.h ../definitions.h
	gcc -c ../ip_linked_list.c -pthread

arp_linked_list.o: ../arp_linked_list.c ../arp_linked_list.h ../definitions.h
	gcc -c ../arp_linked_list.c -pthread

communication.o: ../communication.c ../communication.h
	gcc -c ../communication.c

ethernet_protocol.o: ethernet_protocol.h ethernet_protocol.c
	gcc -c ethernet_protocol.c

clean:
	rm *.o ipd
