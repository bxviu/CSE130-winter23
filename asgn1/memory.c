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
        if (f[i] == '/') {
            return 1;
        }
    }
    return 0;
}

int cleanupReturn(int returnCode, char *fn, char *cmd, int infile) {
    if (fn)
        free(fn);
    if (cmd)
        free(cmd);
    if (infile > 0)
        close(infile);
    return returnCode;
}

/* void getCommand(char *command, char *buffer, char *token) {
    token = strtok(buffer, " ");
    command = strdup(token);
}
void getFilename(char *filename, char *buffer, char *token) {
    const char s[2] = "\n";
    token = strtok(NULL, s);
    filename = strdup(buffer);
}*/

int writeCommand(int infile, char *buffer, int bytes_written, int *bytes_read) {
    do {
        int bytes = write(infile, &buffer[bytes_written], *bytes_read - bytes_written);
        if (bytes < 0) {
            return 1;
        }
        bytes_written += bytes;
    } while (bytes_written < *bytes_read);
    return 0;
}

int checkEquality(char *buffer, char *command) {
    for (int i = 0; i < (int) strlen(command); i++) {
        if (buffer[i] != command[i]) {
            return 1;
        }
    }
    return 0;
}

int main() {
    int infile = -2;
    //int trunc = 0;
    //char one;
    char buffer[8192] = "";
    char *token = NULL;
    //char reade[8192] = "";
    char *command = NULL;
    char *filename = NULL;
    char buffy[8192];
    int bytes_read = 0;

    do {
        bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (bytes_read < 0) {
            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
            return cleanupReturn(1, filename, command, infile);
        } else if (bytes_read > 0) {
            if (infile != -2 && command && filename) {
                if (!strcmp(command, "set")) {
                    //int bytes_written = 0;
                    if (writeCommand(infile, buffer, 0, &bytes_read)) {
                        write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                        return cleanupReturn(1, filename, command, infile);
                    }
                }
                continue;
            }
            if (!command) {
                //getCommand(command, buffer, token);
                strcpy(buffy, buffer);
                token = strtok(buffy, " ");
                command = strdup(token);
            }
            if (!filename && command) {
                //getFilename(filename, buffer + 1, token);
                const char n[3] = " \n";
                //const char s[1] = " ";
                //printf("hi|%d", bytes_read);
                token = strtok(NULL, n);
                if (token == NULL || strlen(token) == 0 || strlen(token) > PATH_MAX
                    || nullTerminatorCheck(token)) {
                    write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
                    return cleanupReturn(1, filename, command, infile);
                }
                filename = strdup(token);
                /* printf("filename: %s|\n", filename);
                printf("command: %s|\n", command);
                printf("buffer: %s|\n", buffer);
                printf("token: %s|\n", token);
                printf("-n: %lu|\n", strlen(buffer));
                printf("c: %c|\n", buffer[strlen(buffer) - 1]);*/
                if (!strcmp(command, "set")) {
                    if (buffer[strlen(buffer) - 1] == '\n'
                        || buffer[strlen(command) + strlen(filename) + 1] == '\n') {
                        infile = open(filename, O_TRUNC | O_WRONLY | O_CREAT);
                        //printf("hi|%d", bytes_read);
                        if (infile == -1) {
                            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                            return cleanupReturn(1, filename, command, infile);
                        }
                        /* printf("buffy: %s|\n", buffy);
                        printf("buffer: %s|\n", buffer);
                        printf("token: %s|\n", token);*/
                        //buffer[strlen(buffer) - 1] = ' ';
                        //memmove(buffer, &buffer[strlen(filename)+1],strlen(buffer) - strlen(filename)-1);
                        //memcpy(buffer, &buffer[strlen(command) + strlen(filename) + 1], strlen(buffer));
                        //printf("buffer: %s|\n", buffer);
                        if (writeCommand(infile, buffer, strlen(command) + strlen(filename) + 2,
                                &bytes_read)) {
                            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                            return cleanupReturn(1, filename, command, infile);
                        }
                        /* printf("buffer: %s\n", buffer);
                        if (writeCommand(infile, buffer, strlen(command) + strlen(filename) + 1, &bytes_read)) {
                            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                            return cleanupReturn(1, filename, command, infile);
                        }
                        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
                        return cleanupReturn(1, filename, command, infile);*/
                    }
                } else if (!strcmp(command, "get")) {
                    if (strlen(buffer) != strlen(filename) + strlen(command) + 2) {
                        infile = -3; //extra spaces aafter filename
                    }
                }
            }
            /* printf("filename: %s\n", filename);
            printf("command: %s\n", command);
            printf("buffer: %s\n", buffer);*/
        }
    } while (bytes_read > 0);
    if (!command && !filename && strlen(buffer) == 0) {
        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
        return 1;
    }
    if (infile != -3 && command && !strcmp(command, "get")) {
        infile = open(filename, O_RDONLY);
        /* printf(
            "%lu, %lu, %lu, %lu", strlen(buffer), strlen(command), strlen(filename), strlen(token));
        printf("filename: %s|\n", filename);
        printf("command: %s|\n", command);
        printf("buffer: %s|\n", buffer);
        printf("token: %s|\n", token);
        printf("-n: %lu|\n", strlen(buffer));
        printf("c: %c|\n", buffer[strlen(buffer)-1]);*/
        if (infile == -1 || buffer[strlen(buffer) - 1] != '\n' || checkEquality(buffer, command)) {
            write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
            return cleanupReturn(1, filename, command, infile);
        }
        do {
            bytes_read = read(infile, buffer, sizeof(buffer));
            if (bytes_read < 0) {
                write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                return cleanupReturn(1, filename, command, infile);
            } else if (bytes_read > 0) {
                //int bytes_written = 0;
                if (writeCommand(STDOUT_FILENO, buffer, 0, &bytes_read)) {
                    write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                    return cleanupReturn(1, filename, command, infile);
                }
            }
        } while (bytes_read > 0);
        close(infile);
        return cleanupReturn(0, filename, command, infile);
    } else if (infile != -2 && !strcmp(command, "set")) {
        //close(infile);
        write(STDOUT_FILENO, "OK\n", sizeof("OK"));
        return cleanupReturn(0, filename, command, infile);
    }
    //if (infile != -2) {
    //    close(infile);
    //}
    write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
    return cleanupReturn(1, filename, command, infile);
}

/* do {
        bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer);
        if (bytes_read < 0) {
            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
            return cleanupReturn(1, filename, command);
        }
        else if (bytes_read > 0) {
            int bytes_written = 0;
            do {
                int bytes
                    = write(STDOUT_FILENO, buffer + bytes_written, bytes_read - bytes_written);
                if (bytes <= 0) {
                    write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                    return cleanupReturn(1, filename, command);
                }
                bytes_written += bytes;
            } while (bytes_written < bytes_read);
        }
    } while (bytes_read > 0);


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
    / printf("filename: %s", filename);
    printf("command: %s", command);
    printf("buffer: %s", buffer);/
    if (!command && !filename && strlen(buffer) == 0) {
        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
        return 1;
    }
    / for (int i = 0; i < (int) strlen(buffer); i++) {
        if (buffer[i] == ' ' && !command) {
            command = strdup(reade);
            reade[0] = '\0';
            continue;
        } else if ((buffer[i] == ' ' || buffer[i] == '\n') && command && !filename) {
            filename = strdup(reade);
            reade[0] = '\0';
        }
        strncat(reade, &buffer[i], 1);
    }/
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
    
}*/
