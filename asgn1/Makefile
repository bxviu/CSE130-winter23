CC = clang
CFLAGS = -Wall -pedantic -Werror -Wextra -g

all: memory 

memory: memory.o
	$(CC) memory.o -o memory

memory.o: memory.c
	$(CC) $(CFLAGS) -c memory.c
	
clean:
	rm -f memory memory.o
	
format:
	clang-format -i *.[ch]
