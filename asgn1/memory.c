#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

//#include "memory_functions.h"

int main() {
    int infile;
    char one;
    char buffer[1024] = "";
    char *command;
    char *filename;

    while (read(STDIN_FILENO, &one, sizeof(one)) > 0) {
        if (one == '\n') {
            break;
        }
        strncat(buffer, &one, 1);
    }
    if (strlen(buffer) == 0) {
        return 1;
    }
    command = strdup(strtok(buffer, " "));
    filename = strdup(strtok(NULL, " "));
    if (strlen(filename) == 0 || strlen(filename) > PATH_MAX || strtok(NULL, " ") != NULL) {
        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
        return 1;
    }
    buffer[0] = '\0';
    if (!strcmp(command, "set")) {
        infile = open(filename, O_WRONLY | O_CREAT);
        if (infile > 0) {
            ftruncate(infile, 0);
        } else if (infile == -1) {
            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed\n"));
            return 1;
        }
        while (read(STDIN_FILENO, &one, sizeof(one)) > 0){
            strncat(buffer, &one, 1);
        }
        if (strlen(buffer) == 0) {
            ftruncate(infile, 0);
        } else {
            write(infile, buffer, strlen(buffer));
        }
        close(infile);
        write(STDOUT_FILENO, "OK\n", sizeof("OK"));
        return 0;
    } 
    else if (!strcmp(command, "get")) {
        infile = open(filename, O_RDONLY);
        if (infile == -1){
            write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
            return 1;
        }
        while (read(infile, &one, sizeof(one)) > 0) {
            strncat(buffer, &one, 1);
        }
        write(STDOUT_FILENO, buffer, strlen(buffer));
        return 0;
    } 
    else {
        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
        return 1;
    }
}

        /* tok = strtok(NULL, " "); //need to make it look for \n
        printf("%lu\n", strlen(tok));
        printf("%lu\n", sizeof(tok));
        write(STDOUT_FILENO, token, sizeof(token));*/
        //printf("%lu\n", strlen(input));
//printf("%lu\n", sizeof(input));
//printf("%s\n", buffer);*/
