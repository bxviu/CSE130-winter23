#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <stdio.h>
//#include "memory_functions.h"
#define MAX_LENGTH = 128;

int main() {
    int infile;
    //char buffer[1]; //(MAX_LENGTH * 8)];
    char one;
    char token[1024] = "";
    while (read(STDIN_FILENO, &one, sizeof(one)) > 0) {
        if (one == '\n') {
            break;
        }
        strncat(token, &one, 1);
        //printf("%s\n", token);
    }
    //printf("%lu\n", strlen(token));
    //printf("%lu\n", sizeof(token));
    char *tok;
    //printf("%s\n", token);//set hello.txt\nhilo
    tok = strtok(token, " ");
    //printf("%s\n", token);
    //printf("%s\n", buffer);
    if (!strcmp(tok, "set")) {
        tok = strtok(NULL, " "); //need to make it look for \n
        //printf("%lu\n", strlen(tok));
        //printf("%lu\n", sizeof(tok));
        //write(STDOUT_FILENO, tok, sizeof(tok));
        infile = open(tok, O_WRONLY | O_CREAT);
        if (infile > 0) {
            ftruncate(infile, 0);
        }
        token[0] = '\0';
        while (read(STDIN_FILENO, &one, sizeof(one)) > 0){
            strncat(token, &one, 1);
            /* if (one == '\n') {
                break;
            }*/
            //printf("%s\n", token);
        }
        write(infile, token, strlen(token));
        //char input[strlen(token)];
        //strcpy(input, token);
        //printf("%lu\n", strlen(input));
        //printf("%lu\n", sizeof(input));
        close(infile);
        write(STDOUT_FILENO, "OK\n", sizeof("OK\n"));
        return 0;
    } 
    else if (!strcmp(token, "get")) {
        /* token = strtok(NULL, "\n");
        if (!token) {
            write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command\n"));
            return 1;
        }
        infile = open(token, O_RDONLY);
        if (infile == -1 || strtok(NULL, "") != NULL) {
            write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command\n"));
            return 1;
        }
        read(infile, buffer, sizeof(buffer));
        write(STDOUT_FILENO, buffer, sizeof(buffer));
        return 0;*/
    } 
    else {
        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command\n"));
        //printf("Invalid Command\n");
        return 1;
    }
    //free(buffer);
}
