CC = clang
CFLAGS = -Wall -pedantic -Werror -Wextra 

all: queue.o

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c 

clean:
	rm -f queue.o 
	
format:
	clang-format -i *.[ch]
