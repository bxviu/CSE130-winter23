#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
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

int is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
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

int writeCommand(int infile, char *buffer, int bytes_written, int bytes_read) {
    do {
        int bytes = write(infile, &buffer[bytes_written], bytes_read - bytes_written);
        //write(STDERR_FILENO, "h", sizeof("h"));
        if (bytes < 0) {
            return 1;
        }
        bytes_written += bytes;
    } while (bytes_written < bytes_read);
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
    //char *token = NULL;
    //char reade[8192] = "";
    char *command = NULL;
    char *filename = NULL;
    char tmp[8192] = "";
    //char buffy[8192];
    int bytes_read = 0;

    /* while (read(STDIN_FILENO, &one, sizeof(one)) > 0) {
        if (!command && one == ' ') {
            command = strdup(buffer);
            buffer[0] = '\0';
            continue;
        } else if (!filename && command && (one == ' ' || one == '\n')) {
            filename = strdup(buffer);
            buffer[0] = '\0';
            break;
        }
        strncat(buffer, &one, 1);
    }*/
    //bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
    //write(STDERR_FILENO, buffer, sizeof(buffer));
    //for (int i = 0; i < bytes_read; i++) {
    //    //printf("char: %c|\n", buffer[i]);
    //    //write(STDERR_FILENO, buffer, sizeof(buffer));
    //    if ((buffer[i] == ' ') && !command) {
    //        char b[i - 1];
    //        //printf("%lu", strlen(buffer) - strlen(command) - 2);
    //        for (int j = 0; j < i; j++) {
    //            b[j] = buffer[j];
    //        }
    //        //printf("b: %s|\n", b);
    //        command = strdup(b);
    //    } else if ((buffer[i] == ' ' || buffer[i] == '\n') && command && !filename) {
    //        char b[i - strlen(command) - 1];
    //        //printf("%lu", strlen(buffer) - strlen(command) - 2);
    //        for (int j = 0; j < (int) (i - strlen(command) - 1); j++) {
    //            b[j] = buffer[strlen(command) + j + 1];
    //        }
    //        //printf("b: %s|\n", b);
    //        filename = strdup(b);
    //    }
    //}
    ///*printf("filename: %s|\n", filename);
    //printf("filenamel: %lu|\n", strlen(filename));

    //printf("command: %s|\n", command);
    //printf("command: %lu|\n", strlen(command));

    //printf("buffer: %s|\n", buffer);
    //printf("buffer: %lu|\n", strlen(buffer));*/
    //if (filename == NULL || strlen(filename) == 0 || strlen(filename) > PATH_MAX
    //                || nullTerminatorCheck(filename)) {
    //                write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
    //                return cleanupReturn(1, filename, command, infile);
    //            }
    //if (!strcmp(command, "set")) {
    //    if (1) {
    //        infile = open(filename, O_TRUNC | O_WRONLY | O_CREAT, 0666);
    //        //printf("hi|%d", bytes_read);
    //        if (infile == -1) {
    //            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
    //            return cleanupReturn(1, filename, command, infile);
    //        }
    //        //printf("filename: %s|\n", filename);
    //        //printf("command: %s|\n", command);
    //        //printf("buffer: %s|\n", buffer);
    //        if (writeCommand(infile, buffer, strlen(command) + strlen(filename) + 2,
    //                bytes_read)) {
    //            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
    //            return cleanupReturn(1, filename, command, infile);
    //        }
    //    }
    //}
    

    do {
        bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        int read_bytes = 0;
        if (bytes_read < 0) {
            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
            return cleanupReturn(1, filename, command, infile);
        } else if (bytes_read > 0) {
            if (infile > 0 && command && filename) {
                if (!strcmp(command, "set")) {
                    if (writeCommand(infile, buffer, 0, bytes_read)) {
                        write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                        return cleanupReturn(1, filename, command, infile);
                    }
                }
                continue;
            }
            if (!command) {
                //write(STDERR_FILENO, buffer, sizeof(buffer));
                for (int i = 0; i < bytes_read; i++) {
                    read_bytes++;
                    if (buffer[i] == ' ') {
                        command = strdup(tmp);
                        tmp[0] = '\0';
                        break;
                    }
                    strncat(tmp, &buffer[i], 1);
                    //printf("tmp: %s|\n", tmp);
                }
            }
            if (!filename && command) {
                //write(STDERR_FILENO, buffer, sizeof(buffer));
                for (int i = read_bytes; i < bytes_read; i++) {
                    read_bytes++;
                    //write(STDERR_FILENO, tmp, sizeof(tmp));
                    if (buffer[i] == ' ' || buffer[i] == '\n') {
                        filename = strdup(tmp);
                        tmp[0] = '\0';
                        infile = (buffer[i] == '\n') ? -2 : -3;
                        if (filename == NULL || strlen(filename) == 0 || strlen(filename) > PATH_MAX
                            || nullTerminatorCheck(filename)) { //|| !is_regular_file(filename)) {
                            infile = -3;
                            //write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
                            //return cleanupReturn(1, filename, command, infile);
                        }
                        //printf("filename: %s|\n", filename);
                        break;
                    }
                    strncat(tmp, &buffer[i], 1);
                }
                    //printf("tmp: %s|\n", tmp);
                

  /*                   printf("filename: %s|\n", filename);
                printf("command: %s|\n", command);
                printf("buffer: %s|\n", buffer);*/
                //printf("token: %s|\n", token);
                /*printf("-n: %lu|\n", strlen(buffer));
                printf("c: %c|\n", buffer[strlen(buffer) - 1]);*/
            }
            if (filename && command && !strcmp(command, "set")) {
                if (infile != -3) {
                    infile = open(filename, O_TRUNC | O_WRONLY | O_CREAT, 0666);
                    if (infile == -1) {
                        write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                        return cleanupReturn(1, filename, command, infile);
                    }
                    if (writeCommand(infile, buffer, read_bytes,
                            bytes_read)) {
                        write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                        return cleanupReturn(1, filename, command, infile);
                    }
                }
            } else if (filename && command && infile != -3 && !strcmp(command, "get")) {
                //printf("bytes_read: %d|\nread_bytes: %d|\n", bytes_read, read_bytes);
                if (bytes_read > read_bytes) { //strlen(buffer) != strlen(filename) + strlen(command) + 2) {
                    infile = -3; //extra spaces aafter filename
                }
            }
            
        }
    } while (bytes_read > 0);
    if (!command && !filename && strlen(buffer) == 0) {
        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
        return 1;
    }
    if (infile != -3 && command && !strcmp(command, "get")) {
        infile = open(filename, O_RDONLY, 0666);
        //printf("if: %d, filename: %s|%c|\n", infile, filename, filename[strlen(filename) - 1]);
        if (infile == -1 || (infile > 0 && !is_regular_file(filename))) {
            write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
            return cleanupReturn(1, filename, command, infile);
        }
        do {
            bytes_read = read(infile, buffer, sizeof(buffer));
            if (bytes_read < 0) {
                write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                return cleanupReturn(1, filename, command, infile);
            } else if (bytes_read > 0) {

                if (writeCommand(STDOUT_FILENO, buffer, 0, bytes_read)) {
                    write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                    return cleanupReturn(1, filename, command, infile);
                }
            }
        } while (bytes_read > 0);
        close(infile);
        return cleanupReturn(0, filename, command, infile);
    } else if (infile > 0 && !strcmp(command, "set")) {
        write(STDOUT_FILENO, "OK\n", sizeof("OK"));
        return cleanupReturn(0, filename, command, infile);
    }
    write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
    return cleanupReturn(1, filename, command, infile);
}


//    do {
//        bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
//        //write(STDOUT_FILENO, buffer, sizeof(buffer));
//        //printf("buffer: %s|\n", buffer);
//        //write(STDERR_FILENO, buffer, sizeof(buffer));
//        if (bytes_read < 0) {
//            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
//            return cleanupReturn(1, filename, command, infile);
//        } else if (bytes_read > 0) {
//            if (infile != -2 && command && filename) {
//                if (!strcmp(command, "set")) {
//                    //int bytes_written = 0;
//                    if (writeCommand(infile, buffer, 0, bytes_read)) {
//                        write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
//                        return cleanupReturn(1, filename, command, infile);
//                    }
//                }
//                continue;
//            }
//            if (!command) {
//                strcpy(buffy, buffer);
//                token = strtok(buffy, " ");
//                command = strdup(token);
//            }
//            if (!filename && command) {
//                const char n[3] = " \n";
//                //printf("hi|%d", bytes_read);
//                token = strtok(NULL, n);
//                 if (token == NULL || strlen(token) == 0 || strlen(token) > PATH_MAX
//                    || nullTerminatorCheck(token)) {
//                    write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
//                    return cleanupReturn(1, filename, command, infile);
//                }
//                filename = strdup(token);
//                //printf("command: %s|\n", command);
//                //write(STDERR_FILENO, command, sizeof(command));
//                //write(STDERR_FILENO, "\n", sizeof('\n'));
//                //write(STDERR_FILENO, buffer, sizeof(buffer));
//                //write(STDERR_FILENO, "\n", sizeof('\n'));
//                /* for (int i = strlen(command) + 1; i < (int) strlen(buffer); i++) {
//                    //printf("char: %c|\n", buffer[i]);
//                    //write(STDERR_FILENO, buffer, sizeof(buffer));
//                    if (buffer[i] == ' ' || buffer[i] == '\n') {
//                        char b[strlen(buffer) - strlen(command) - 2];
//                        //printf("%lu", strlen(buffer) - strlen(command) - 2);
//                        for (int j = 0; j < (int) (strlen(buffer) - strlen(command)-2); j++) {
//                            b[j] = buffer[strlen(command)+j+1];
//                        }
//                        //printf("b: %s|\n", b);
//                        filename = strdup(b);
//                    }
//                }*/
//                //command = "get";
//                //filename = "test.dat";
//                //printf("filename: %s|\n", filename);
//                /* if (filename == NULL || strlen(filename) == 0 || strlen(filename) > PATH_MAX
//                    || nullTerminatorCheck(filename)) {
//                    write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
//                    return cleanupReturn(1, filename, command, infile);
//                }*/
//
//                /* printf("filename: %s|\n", filename);
//                printf("command: %s|\n", command);
//                printf("buffer: %s|\n", buffer);
//                printf("token: %s|\n", token);
//                printf("-n: %lu|\n", strlen(buffer));
//                printf("c: %c|\n", buffer[strlen(buffer) - 1]);*/
//                if (!strcmp(command, "set")) {
//                    if (buffer[strlen(buffer) - 1] == '\n'
//                        || buffer[strlen(command) + strlen(filename) + 1] == '\n') {
//                        infile = open(filename, O_TRUNC | O_WRONLY | O_CREAT, 0666);
//                        //printf("hi|%d", bytes_read);
//                        if (infile == -1) {
//                            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
//                            return cleanupReturn(1, filename, command, infile);
//                        }
//                        /* printf("buffy: %s|\n", buffy);
//                        printf("buffer: %s|\n", buffer);
//                        printf("token: %s|\n", token);*/
//                        //buffer[strlen(buffer) - 1] = ' ';
//                        //memmove(buffer, &buffer[strlen(filename)+1],strlen(buffer) - strlen(filename)-1);
//                        //memcpy(buffer, &buffer[strlen(command) + strlen(filename) + 1], strlen(buffer));
//                        //printf("buffer: %s|\n", buffer);
//                        if (writeCommand(infile, buffer, strlen(command) + strlen(filename) + 2,
//                                bytes_read)) {
//                            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
//                            return cleanupReturn(1, filename, command, infile);
//                        }
//                        /* printf("buffer: %s\n", buffer);
//                        if (writeCommand(infile, buffer, strlen(command) + strlen(filename) + 1, &bytes_read)) {
//                            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
//                            return cleanupReturn(1, filename, command, infile);
//                        }
//                        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
//                        return cleanupReturn(1, filename, command, infile);*/
//                    }
//                } else if (!strcmp(command, "get")) {
//                    if (strlen(buffer) != strlen(filename) + strlen(command) + 2) {
//                        infile = -3; //extra spaces aafter filename
//                    }
//                }
//            }
//            /* printf("filename: %s\n", filename);
//            printf("command: %s\n", command);
//            printf("buffer: %s\n", buffer);*/
//        }
//    } while (bytes_read > 0);
//    if (!command && !filename && strlen(buffer) == 0) {
//        write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
//        return 1;
//    }
//    //write(STDERR_FILENO, "&adsadsa", sizeof("&adsadsa"));
//    //infile = -2;
//    if (infile != -3 && command && !strcmp(command, "get")) {
//        infile = open(filename, O_RDONLY, 0666);
//        /* printf(
//            "%lu, %lu, %lu, %lu", strlen(buffer), strlen(command), strlen(filename), strlen(token));
//        printf("filename: %s|\n", filename);
//        printf("command: %s|\n", command);
//        printf("buffer: %s|\n", buffer);
//        printf("token: %s|\n", token);
//        printf("-n: %lu|\n", strlen(buffer));
//        printf("c: %c|\n", buffer[strlen(buffer)-1]);*/
//         if (infile == -1 || buffer[strlen(buffer) - 1] != '\n'
//                 || checkEquality(buffer, command) //){
//            || (infile > 0 && !is_regular_file(filename))) {
//            write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
//            return cleanupReturn(1, filename, command, infile);
//        }
//        
//        do {
//            bytes_read = read(infile, buffer, sizeof(buffer));
//            if (bytes_read < 0) {
//                write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
//                return cleanupReturn(1, filename, command, infile);
//            } else if (bytes_read > 0) {
//                //int bytes_written = 0;
//                if (writeCommand(STDOUT_FILENO, buffer, 0, bytes_read)) {
//                    write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
//                    return cleanupReturn(1, filename, command, infile);
//                }
//            }
//        } while (bytes_read > 0);
//        close(infile);
//        return cleanupReturn(0, filename, command, infile);
//    } else if (infile != -2 && !strcmp(command, "set")) {
//        //close(infile);
//        write(STDOUT_FILENO, "OK\n", sizeof("OK"));
//        return cleanupReturn(0, filename, command, infile);
//    }
//    //if (infile != -2) {
//    //    close(infile);
//    //}
//    write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
//    return cleanupReturn(1, filename, command, infile);
//}

