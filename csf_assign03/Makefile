CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -pedantic -g -O

csim: main.o cache.o
	$(CC) -o csim main.o cache.o

main.o: main.cpp cache.h
	$(CC) -$(CFLAGS) -c main.cpp

cache.o: cache.cpp cache.h
	$(CC) $(CFLAGS) -c cache.cpp

clean:
	rm -f *.o csim
