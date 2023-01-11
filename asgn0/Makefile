CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra

all: hello 

hello: hello.o
	$(CC) hello.o -o hello

hello.o: hello.c
	$(CC) $(CFLAGS) -c hello.c
	
clean:
	rm -f hello hello.o
	

