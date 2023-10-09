CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -pedantic -g -O

csim: main.o cache.o
	$(CC) -o main.o cache.o

main.o: main.cpp cache.h
	$(CC) -c main.cpp cache.h $(CFLAGS)

cache.o: cache.cpp cache.h
	$(CC) -c cache.cpp cache.h $(CFLAGS)

clean:
	rm -f *.o csim



