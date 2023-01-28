CC = clang
CFLAGS = -Wall -pedantic -Werror -Wextra 

all: httpserver

httpserver: httpserver.o requestparser.o asgn2_helper_funcs.a 
	$(CC) httpserver.o requestparser.o -o httpserver asgn2_helper_funcs.a

httpserver.o: httpserver.c httpserver.h
	$(CC) $(CFLAGS) -c httpserver.c 

requestparser.o: requestparser.c httpserver.h
	$(CC) $(CFLAGS) -c requestparser.c 
	
clean:
	rm -f httpserver httpserver.o
	
format:
	clang-format -i *.[ch]
