#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

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

//https://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
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
    if (infile > 2)
        close(infile);
    return returnCode;
}

int writeCommand(int infile, char *buffer, int bytes_written, int bytes_read) {
    do {
        int bytes = write(infile, &buffer[bytes_written], bytes_read - bytes_written);
        if (bytes < 0) {
            return 1;
        }
        bytes_written += bytes;
    } while (bytes_written < bytes_read);
    return 0;
}

int main() {
    int infile = -2;
    char buffer[8192] = "";
    char tmp[8192] = "";
    char *command = NULL;
    char *filename = NULL;
    int bytes_read = 0;
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
                for (int i = 0; i < bytes_read; i++) {
                    read_bytes++;
                    if (buffer[i] == ' ') {
                        command = strdup(tmp);
                        tmp[0] = '\0';
                        break;
                    }
                    strncat(tmp, &buffer[i], 1);
                }
            }
            if (!filename && command) {
                for (int i = read_bytes; i < bytes_read; i++) {
                    read_bytes++;
                    if (buffer[i] == ' ' || buffer[i] == '\n') {
                        filename = strdup(tmp);
                        tmp[0] = '\0';
                        infile = (buffer[i] == '\n') ? -2 : -3;
                        if (filename == NULL || strlen(filename) == 0 || strlen(filename) > PATH_MAX
                            || nullTerminatorCheck(filename)) {
                            infile = -3;
                        }
                        break;
                    }
                    strncat(tmp, &buffer[i], 1);
                }
            }
            if (filename && command && !strcmp(command, "set")) {
                if (infile != -3) {
                    infile = open(filename, O_TRUNC | O_WRONLY | O_CREAT, 0666);
                    if (infile == -1) {
                        write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                        return cleanupReturn(1, filename, command, infile);
                    }
                    if (writeCommand(infile, buffer, read_bytes, bytes_read)) {
                        write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
                        return cleanupReturn(1, filename, command, infile);
                    }
                }
            } else if (filename && command && infile != -3 && !strcmp(command, "get")) {
                if (bytes_read > read_bytes) { //extra stuff after filename
                    infile = -3;
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
        return cleanupReturn(0, filename, command, infile);
    } else if (infile > 0 && !strcmp(command, "set")) {
        write(STDOUT_FILENO, "OK\n", sizeof("OK"));
        return cleanupReturn(0, filename, command, infile);
    }
    write(STDERR_FILENO, "Invalid Command\n", sizeof("Invalid Command"));
    return cleanupReturn(1, filename, command, infile);
}
