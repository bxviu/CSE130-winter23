#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

//#include "memory_functions.h"

int nullTerminatorCheck(char *f) {
    for (int i = 0; i < (int) strlen(f); i++) {
        if (i + 1 < (int) strlen(f) && f[i] == '\\' && f[i + 1] == '0') {
            return 1;
        }
    }
    return 0;
}

int cleanupReturn(int returnCode, char *fn, char *cmd) {
    if (fn)
        free(fn);
    if (cmd)
        free(cmd);
    return returnCode;
}

int main() {
    int infile;
    char one;
    char buffer[1024] = "";
    //char *token;
    char reade[1024] = "";
    char *command = NULL;
    char *filename = NULL;

    while (read(STDIN_FILENO, &one, sizeof(one)) > 0) {
        strncat(buffer, &one, 1);
    }
    if (strlen(buffer) == 0) {
        return 1;
    }
    for (int i = 0; i < (int) strlen(buffer); i++) {
        if (buffer[i] == ' ' && !command) {
            command = strdup(reade);
            reade[0] = '\0';
            if (i + 1 < (int) strlen(buffer))
                i++;
        } else if ((buffer[i] == ' ' || buffer[i] == '\n') && command && !filename) {
            filename = strdup(reade);
            reade[0] = '\0';
            //printf("fn: %s\n", filename);
            //if (i+1 < (int)strlen(buffer)) i++;
        }
        //printf("re: %s\n", reade);
        strncat(reade, &buffer[i], 1);
    }
    if (!filename) {
        filename = strdup(reade);
        reade[0] = '\0';
    }
    //printf("token: %s", token);
    if (strlen(filename) == 0 || strlen(filename) > PATH_MAX || nullTerminatorCheck(filename)) {
        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
        return cleanupReturn(1, filename, command);
    }
    /* printf("filename: %s", filename);
    printf("command: %s", command);
    printf("token: %s", token);*/
    buffer[0] = '\0';
    if (!strcmp(command, "set")) {
        if (reade[0] != '\n') {
            write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
            return cleanupReturn(1, filename, command);
        }
        memmove(reade, reade + 1, strlen(reade));
        infile = open(filename, O_WRONLY | O_CREAT);
        if (infile > 0) {
            ftruncate(infile, 0);
        } else if (infile == -1) {
            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
            return cleanupReturn(1, filename, command);
        }
        write(infile, reade, strlen(reade));
        close(infile);
        write(STDOUT_FILENO, "OK\n", sizeof("OK"));
        return cleanupReturn(0, filename, command);
    } else if (!strcmp(command, "get")) {
        //if ((reade[0] != '\n' && strlen(reade) > 0) || (reade[0] == '\n' && strlen(reade) > 1)) { //if both "get foo.txt" and "get foo.txt\n" allowed
        if (reade[0] != '\n'
            || (reade[0] == '\n' && strlen(reade) > 1)) { //if only "get foo.txt" allowed
            write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
            return cleanupReturn(1, filename, command);
        }
        infile = open(filename, O_RDONLY);
        if (infile == -1) {
            write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
            return cleanupReturn(1, filename, command);
        }
        while (read(infile, &one, sizeof(one)) > 0) {
            strncat(buffer, &one, 1);
        }
        write(STDOUT_FILENO, buffer, strlen(buffer));
        return cleanupReturn(0, filename, command);
    } else {
        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
        return cleanupReturn(1, filename, command);
    }
}
