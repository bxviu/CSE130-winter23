CC = clang
CFLAGS = -Wall -pedantic -Werror -Wextra

all: httpserver

httpserver: httpserver.o requestparser.o requestmethods.o serverresponse.o asgn2_helper_funcs.a 
	$(CC) httpserver.o requestparser.o requestmethods.o serverresponse.o -o httpserver asgn2_helper_funcs.a

httpserver.o: httpserver.c httpserver.h
	$(CC) $(CFLAGS) -c httpserver.c 

requestparser.o: requestparser.c httpserver.h
	$(CC) $(CFLAGS) -c requestparser.c 

requestmethods.o: requestmethods.c httpserver.h
	$(CC) $(CFLAGS) -c requestmethods.c 
	
serverresponse.o: serverresponse.c httpserver.h
	$(CC) $(CFLAGS) -c serverresponse.c 
	
clean:
	rm -f httpserver httpserver.o requestparser.o requestmethods.o serverresponse.o
	
format:
	clang-format -i *.[ch]
