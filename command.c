/*
* Description: Implementation of commands by using system calls
* 
* Author: Yuhan Shi
*
* Date: Apr 8, 2020
* 
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "command.h"
/*---------------------------------------------------------------------------*/

/*--------------------------Function Declaration-----------------------------*/
void listDir();                                          // ls
void showCurrentDir();                                   // pwd
void makeDir(char *dirName);                             // mkdir
void changeDir(char *dirName);                           // cd
void copyFile(char *sourcePath, char *destinationPath);  // cp
void moveFile(char *sourcePath, char *destinationPath);  // mv
void deleteFile(char *filename);                         //rm
void displayFile(char *filename);                        // cat
/*---------------------------------------------------------------------------*/

void listDir()  /*for the ls command*/
{
    char* buf = NULL;
    char* cwd = NULL;
    size_t bufsize = 256;
    size_t count;
    buf = (char *)malloc(bufsize * sizeof(char));
    DIR *dir;
    struct dirent *sd;

    /* Get the current directory */
    cwd = getcwd(buf, bufsize);
    if (cwd == NULL) {
        write(1, "Error! Unable to get current directory.\n", strlen("Error! Unable to get current directory.\n"));
        return;
    }
    /* Open the dir using opendir() */
    dir = opendir(cwd);
    if (dir == NULL) {
        write(1, "Error! Unable to open directory.\n", strlen("Error! Unable to open directory.\n"));
        return;
    }
    /* use a while loop to read the dir */
    while ((sd = readdir(dir)) != NULL) {
        count = strlen(sd->d_name);
        write(1, sd->d_name, count);
        write(1, "\n", 2);
    }
    cwd = NULL;
    closedir(dir);
    free(buf);
}

void showCurrentDir()  /*for the pwd command*/
{
    size_t bufsize = 256;
    char* buf = (char *)malloc(bufsize * sizeof(char));
    char* cwd = NULL;

    // Get the current directory
    cwd = getcwd(buf, bufsize);
    if (cwd == NULL) {
        // int len = strlen("Error! Unable to get current directory.\n");
        // write(1, "Error! Unable to get current directory.\n", len);
        perror("Error!");
        return;
    }
    write(1, cwd, strlen(cwd));
    write(1, "\n", 2);

    free(buf);
    cwd = NULL;
}

void makeDir(char *dirName) /*for the mkdir command*/
{
    int check;
    check = mkdir(dirName, 0700);
    if (check == -1) {
        // int len = strlen("Error! Unable to create directory.\n");
        // write(1, "Error! Unable to create directory.\n", len);
        perror("Error! Create new directory failed");
        return;
    }
}

void changeDir(char *dirName) /*for the cd command*/
{
    int check;
    check = chdir(dirName);
    if (check == -1) {
        perror("Error! Change directory failed");
    }
}

void copyFile(char *sourcePath, char *destinationPath) /*for the cp command*/
{
    int fs, fd, nread;
    char buf[1];
    size_t bufsize = 32;
    DIR *dst;
    dst = opendir(destinationPath);
    char *destinationfile;
    destinationfile = (char *)malloc(bufsize * sizeof(char));
    /* Make a copy of destinationPath, so not affect the original pointer */
    strcpy(destinationfile, destinationPath);

    char *filename;
    fs = open(sourcePath, O_RDONLY);
    if (fs < 0) {
        perror("Error! Copy files failed");
        return;
    }
    /* Check if destinationPath is a directory */
    if (dst != NULL) {
        if (strstr(sourcePath, "/") != NULL) {
            filename = strrchr(sourcePath, '/');
            strcat(destinationfile, filename);
        }
        else {
            filename = sourcePath;
            if (strstr(destinationPath, "/") == NULL)
                strcat(destinationfile, "/");
            strcat(destinationfile, filename);
        }
        closedir(dst);
    }

    fd = open(destinationfile, O_WRONLY | O_CREAT, 0700);

    if (fd < 0) {
        perror("Error! Copy files failed");
        return;
    }

    while((nread = read(fs, buf, 1)) > 0) {
        write(fd, buf, 1);
    }

    filename = NULL;
    free(destinationfile);

    close(fs);
    close(fd);
}

void moveFile(char *sourcePath, char *destinationPath) /*for the mv command*/
{
    copyFile(sourcePath, destinationPath);
    deleteFile(sourcePath);
}

void deleteFile(char *filename) /*for the rm command*/
{
    DIR *dir = opendir(filename);
    if (dir != NULL) {
        int len = strlen("Error! Cannot remove directory.\n");
        write(1, "Error! Cannot remove directory.\n", len);
        closedir(dir);
        return;
    }
    int check;
    check = remove(filename);
    if (check == -1) {
        perror("Error! File cannot be removed");
        return;
    }
}

void displayFile(char *filename) /*for the cat command*/
{
    int fs, nread;
    char buf[1];
    fs = open(filename, O_RDONLY);
    if (fs < 0) {
        perror("Error! Display file failed");
        return;
    }
    while ((nread = read(fs, buf, 1)) > 0) {
        write(1, buf, strlen(buf));
    }
    write(1, "\n", 1);
    close(fs);
}
