#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define MAX_MSG_SIZE 256
#define ACK_VAL 0xFF

int child(int writeFd, int readFd);
int parent(int writeFd, int readFd, const char *message, char msgSize);
int myRead(int fd, char *buf, size_t size);
int myWrite(int fd, const char *buf, char size);
int packageRead(int fd, char *buf);
int packageWrite(int fd, const char *buf, char size);
int childError();
int parentError();
