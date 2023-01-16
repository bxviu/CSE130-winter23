#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
//#include "memory_functions.h"
#define MAX_LENGTH = 128;

int main() {
    int infile;
    char buffer[512]; //(MAX_LENGTH * 8)];
    char *token;
    while (read(STDIN_FILENO, buffer, sizeof(buffer)) > 0) {
        //printf("%s\n", buffer);//set hello.txt\nhilo
        token = strtok(buffer, " ");
        //printf("%s\n", token);
        if (!strcmp(token, "set")) {
            token = strtok(NULL, "\\n");
            //printf("%s\n", token);
            //token = "helo.txt";
            /* if (!token) {
                write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command\n"));
                return 1;
            }
            if (sizeof(token) / sizeof(token[0] > PATH_MAX)){//|| find) {
                write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command\n"));
                return 1;
            }*/
            infile = open(token, O_WRONLY | O_CREAT);
            do {
                token = strtok(NULL, "\n");
                //printf("%s\n", token);
                //write(infile, buffer, sizeof(buffer));
            } while (token);
            write(STDOUT_FILENO, "OK", sizeof("OK"));
            //return 0;
        } 
        else if (!strcmp(token, "get")) {
            token = strtok(NULL, "\n");
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
            return 0;
        } 
        else {
            write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command\n"));
            //printf("Invalid Command\n");
            return 1;
        }
    }
    //free(buffer);
}
