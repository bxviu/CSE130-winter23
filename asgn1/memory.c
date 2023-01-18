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

/* void updateCommandAndFilename(char *command, char *filename, char *buffer) {
    for (int i = 0; i < (int) strlen(buffer); i++) {
        if (buffer[i] == ' ' && !command) {
            command = strdup(reade);
            reade[0] = '\0';
            continue;
            //if (i + 1 < (int) strlen(buffer))
            //i++;
        } else if ((buffer[i] == ' ' || buffer[i] == '\n') && command && !filename) {
            filename = strdup(reade);
            reade[0] = '\0';

            //printf("fn: %s\n", filename);
            //if (i+1 < (int)strlen(buffer)) i++;
        }
        //printf("re: %s\n", reade);
        strncat(reade, &buffer[i], 1);
    }
}*/

int main() {
    int infile = -2;
    int trunc = 0;
    char one;
    char buffer[8192] = "";
    //char *token;
    //char reade[8192] = "";
    char *command = NULL;
    char *filename = NULL;

    while (read(STDIN_FILENO, &one, sizeof(one)) > 0) {
        //if (sizeof(buffer) == strlen(buffer)){ //&& !command && !filename) {
        //    updateCommandAndFilename(command,filename);
        //    buffer[0] = '\0';
        //printf("%lu | %lu | %s\n", sizeof(buffer), strlen(buffer), &one);
        //}
        if (!command && one == ' ') {
            command = strdup(buffer);
            buffer[0] = '\0';
            continue;
        } else if (!filename && command && (one == ' ' || one == '\n')) {
            filename = strdup(buffer);
            buffer[0] = '\0';
            //continue;
        }
        if (sizeof(buffer)-1 == strlen(buffer) && filename && !strcmp(command, "set")) {
            if (infile == -2 && buffer[0] != '\n') {
                write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
                return cleanupReturn(1, filename, command);
            }
            if (infile == -2) {
                memmove(buffer, buffer + 1, strlen(buffer));
            }
            if (infile == -2) {
                infile = open(filename, O_WRONLY | O_CREAT);
            }
            if (!trunc) {
                ftruncate(infile, 0);
                trunc = 1;
            }
            if (infile == -1) {
                write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                return cleanupReturn(1, filename, command);
            }
            write(infile, buffer, strlen(buffer));
            //close(infile);
            //write(STDOUT_FILENO, "OK\n", sizeof("OK"));
            buffer[0] = '\0';
        }
        else if (sizeof(buffer)-1 == strlen(buffer) && filename) {
            buffer[0] = '\0';
        }
        strncat(buffer, &one, 1);
    }
    /* printf("filename: %s", filename);
    printf("command: %s", command);
    printf("buffer: %s", buffer);*/
    if (!command && !filename && strlen(buffer) == 0) {
        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
        return 1;
    }
    /* for (int i = 0; i < (int) strlen(buffer); i++) {
        if (buffer[i] == ' ' && !command) {
            command = strdup(reade);
            reade[0] = '\0';
            continue;
        } else if ((buffer[i] == ' ' || buffer[i] == '\n') && command && !filename) {
            filename = strdup(reade);
            reade[0] = '\0';
        }
        strncat(reade, &buffer[i], 1);
    }*/
    if (!command) {
        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
        return cleanupReturn(1, filename, command);
    }
    if (!filename) {
        filename = strdup(buffer);
        //buffer[0] = '\0';
        //filename = strdup(reade);
        //reade[0] = '\0';
    }
    //printf("len: %lu | %s", strlen(filename), filename);
    if (strlen(filename) == 0 || strlen(filename) > PATH_MAX || nullTerminatorCheck(filename)) {
        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
        return cleanupReturn(1, filename, command);
    }
    if (!strcmp(command, "set")) {
        if (infile == -2 && buffer[0] != '\n') {
            write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
            return cleanupReturn(1, filename, command);
        }
        if (infile == -2){
            memmove(buffer, buffer + 1, strlen(buffer));
        }
        if (infile == -2) {
            infile = open(filename, O_TRUNC | O_WRONLY | O_CREAT);
        }
        if (infile == -1) {
            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
            return cleanupReturn(1, filename, command);
        }
        write(infile, buffer, strlen(buffer));
        close(infile);
        write(STDOUT_FILENO, "OK\n", sizeof("OK"));
        return cleanupReturn(0, filename, command);
    } else if (!strcmp(command, "get")) {
        //printf("buffer: %s", buffer);
        //if ((reade[0] != '\n' && strlen(reade) > 0) || (reade[0] == '\n' && strlen(reade) > 1)) { //if both "get foo.txt" and "get foo.txt\n" allowed
        if (infile == -2 && (buffer[0] != '\n'
            || (buffer[0] == '\n' && strlen(buffer) > 1))) { //if only "get foo.txt" allowed
            write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
            return cleanupReturn(1, filename, command);
        }
        buffer[0] = '\0';
        infile = open(filename, O_RDONLY);
        if (infile == -1) {
            write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
            return cleanupReturn(1, filename, command);
        }
        while (read(infile, &one, sizeof(one)) > 0) {
            strncat(buffer, &one, 1);
            if (sizeof(buffer) - 1 == strlen(buffer)) {
                write(STDOUT_FILENO, buffer, strlen(buffer));
                buffer[0] = '\0';
            }
        }
        write(STDOUT_FILENO, buffer, strlen(buffer));
        return cleanupReturn(0, filename, command);
    } else {
        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
        return cleanupReturn(1, filename, command);
    }
    /* printf("filename: %s", filename);
    printf("command: %s", command);
    printf("token: %s", token);*/
    /* buffer[0] = '\0';
    if (!strcmp(command, "set")) {
        if (reade[0] != '\n') {
            write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
            return cleanupReturn(1, filename, command);
        }
        memmove(reade, reade + 1, strlen(reade));
        infile = open(filename, O_TRUNC | O_WRONLY | O_CREAT);
        if (infile == -1) {
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
    }*/
}
