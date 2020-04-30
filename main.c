/*
* Description: A rudimentary pseudo shell, read and parse limited commands
*       Including two methods of using:
*       1. File mode: reads commands from input file
*       2. Interactive mode: reads commands from stdin
*
* Author: Yuhan Shi
*
* Date: 04/10/2020
*
* Notes:
* 1. Included command.h
* 2. Support commands:
*    Command     #args     Example             Desciption
*    --------    -----     ------------        -----------------------------------------------------
*    ls          0         ls                  Unix ls command. Will show the names of files and
*                                              folders of the current directory.
*
*    pwd         0         pwd                 Unix pwd command. Will show the current directory.
*
*    mkdir       1         mkdir <name>        Unix mkdir command. Will create a directory with the
*                                              filename specified by: <name>.
*
*    cd          1         cd directory        Unix cd command. Will change directory based on
*                                              the destination specified in the argument.
*
*    cp          2         cp <src> <dst>      Only for copying files. Copy one file from the path
*                                              specified by <src> to the location specified by <dst>.
*
*    mv          2         mv <src> <dst>      Only for moving files(cut-paste). Move a file from the
*                                              the location specified by <src> to the location
*                                              specified by <dst>.
*
*    rm          1         rm <filename>       Only for removing files (not a directory). Will remove
*                                              the specified file from the current directory.
*
*    cat         1         cat <filename>      Displays a file's content on the console
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "command.h"
/*---------------------------------------------------------------------------*/

/*--------------------------Function Declaration-----------------------------*/

void error_handler(char** cmd);
void commands_handler(char** cmd, int counter, char* buffer);

/*---------------------------------------------------------------------------*/


void error_handler(char** cmd) {
    /* error handler for wrong commands */
    if (strcmp(cmd[0], cmd[1]) == 0) {
        fprintf(stderr, "Error! Incorrect syntax. No control code found.\n");
    } else {
        fprintf(stderr, "Error! Unsupported parameters for command: %s\n", cmd[0]);
    }
}

void commands_handler(char** cmd, int counter, char* buffer) {
    /* handler for a set of commands
     * #args: | ls    0 | pwd 0 | cp 2 | mv  2 |
     *        | mkdir 1 | cd  1 | rm 1 | cat 1 |
     */

    /* Tokenize the input string */
    char *rem;
    char *token;
    char *saveptr = buffer;

    while((token = strtok_r(saveptr, ";\n", &saveptr))) {
        if (strcmp(token, " ") == 0) {
            fprintf(stderr, "Error! Unrecognized command: %s\n", token);
            break;
        }
        counter = 0;
        /* Save each tokenized cmd and params into cmd */
        rem = token;

        char *param;
        while ((param = strtok_r(rem, " ", &rem))) {
            cmd[counter] = param;
            counter++;
        }

        if (strcmp(cmd[0], "ls") == 0) {
            if (counter != 1) {
                error_handler(cmd);
                break;
            }
            listDir();
        }

        else if (strcmp(cmd[0], "pwd") == 0) {
            if (counter != 1) {
                error_handler(cmd);
                break;
            }
            showCurrentDir();
        }

        else if (strcmp(cmd[0], "mkdir") == 0) {
            if (counter > 2) {
                error_handler(cmd);
                break;
            }
            if (counter < 2) {
                fprintf(stderr, "Usage: %s <name>\n", cmd[0]);
                break;
            }
            char* dirName = cmd[1];
            makeDir(dirName);
        }

        else if (strcmp(cmd[0], "cd") == 0) {
            if (counter > 2) {
                error_handler(cmd);
                break;
            }
            if (counter < 2) {
                fprintf(stderr, "Usage: %s directory\n", cmd[0]);
                break;
            }
            char* dirName = cmd[1];
            changeDir(dirName);
        }

        else if (strcmp(cmd[0], "cp") == 0) {
            if (counter > 3) {
                error_handler(cmd);
                break;
            }
            if (counter < 3) {
                fprintf(stderr, "Usage: %s <src> <dst>\n", cmd[0]);
                break;
            }
            char *sourcePath = cmd[1];
            char *destinationPath = cmd[2];
            copyFile(sourcePath,destinationPath);
        }

        else if (strcmp(cmd[0], "mv") == 0) {
            if (counter > 3) {
                error_handler(cmd);
                break;
            }
            if (counter < 3) {
                fprintf(stderr, "Usage: %s <src> <dst>\n", cmd[0]);
                break;
            }
            char *sourcePath = cmd[1];
            char *destinationPath = cmd[2];
            moveFile(sourcePath,destinationPath);
        }

        else if (strcmp(cmd[0], "rm") == 0) {
            if (counter > 2) {
                error_handler(cmd);
                break;
            }
            if (counter < 2) {
                fprintf(stderr, "Usage: %s <filename>\n", cmd[0]);
                break;
            }
            char *filename = cmd[1];
            deleteFile(filename);
        }

        else if (strcmp(cmd[0], "cat") == 0) {
            if (counter > 2) {
                error_handler(cmd);
                break;
            }
            if (counter < 2) {
                fprintf(stderr, "Usage: %s <filename>\n", cmd[0]);
                break;
            }
            char *filename = cmd[1];
            displayFile(filename);
        }

        else {
            fprintf(stderr, "Error! Unrecognized command: %s\n", cmd[0]);
            break;
        }
    }
    saveptr = NULL;
    rem = NULL;
    token = NULL;
}

int main(int argc, char const *argv[]) {
    char *exitcmd = "exit\n";
    char *buffer;
    char **cmd;
    size_t bufsize = 256;
    FILE *fin = stdin;
    int counter;
    int filename = 1;

    // Malloc spaces
    buffer = (char *)malloc(bufsize * sizeof(char));
  	if (buffer == NULL) {
		fprintf(stderr, "Unable to allocate buffer\n");
		exit(1);
  	}

    cmd = (char **)malloc(bufsize * sizeof(char *));
    if (cmd == NULL) {
		fprintf(stderr, "Unable to allocate space for commands\n");
		exit(1);
  	}

    /* File mode */
    if ((argc == 3) && (strcmp(argv[1], "-f") == 0)) {
        filename = 0;

        fin = fopen(argv[2], "r");
        if (fin == NULL) {
            fprintf(stderr, "Unable to open file: %s\n", argv[2]);
            exit(EXIT_FAILURE);
        }
        freopen("output.txt", "w", stdout);
    }

    /* Explaing usage */
    else if (argc != 1 && argc != 3) {
        fprintf(stderr, "Interactive mode usage: %s\n", argv[0]);
        fprintf(stderr, "File mode usage: %s -f <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Interactive mode */
    while (1) {
        counter = 0;
        if (filename) {
            printf(">>> ");
        }
        /* Break the loop if reach the end of input file */
        if (getline(&buffer, &bufsize, fin) == EOF)
            break;

        /* Break the loop if the user entered <exit> */
		if (strcmp(buffer, exitcmd) == 0) {
            break;
		}
        /* Parse commands */
        else {
            commands_handler(cmd, counter, buffer);
        }
    }

    /*Free the allocated memory*/
    exitcmd = NULL;
    free(buffer);
    free(cmd);
    /*Close the files been opened*/
    fclose(fin);
    fclose(stdout);
    return 0;
}
