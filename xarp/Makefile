output: linked_list.o xarp.h xarp.c communication.o ../definitions.h
	gcc -Wall -pedantic -std=c99 linked_list.o communication.o xarp.c -o xarp -pthread

linked_list.o: ../linked_list.c ../linked_list.h
		gcc -c ../linked_list.c

communication.o: ../communication.h ../communication.c
	gcc -c -Wall -pedantic -std=c99 ../communication.c

clean:
	rm *.o xarp
