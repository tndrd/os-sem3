#include "pipe2.h"

int myRead(int fd, char *buf, size_t size)
{
  assert(fd);
  assert(buf);

  int total = 0;
  int ret = 0;

  do
  {
    ret = read(fd, buf + total, size - total);

    if (ret < 0)
      return -1;

    total += ret;
  } while (total < size);

  return total;
}

int myWrite(int fd, const char *buf, char size)
{
  assert(fd);
  assert(buf);

  int total = 0;
  int ret = 0;

  do
  {
    ret = write(fd, buf + total, size - total);

    if (ret < 0)
      return -1;

    total += ret;
  } while (total < size);

  return total;
}

int packageRead(int fd, char *buf)
{
  char size = 0;

  if (myRead(fd, &size, 1) < 0)
    return -1;

  if (myRead(fd, buf, size) < 0)
    return -1;

  return size;
}

int packageWrite(int fd, const char *buf, char size)
{
  if (myWrite(fd, &size, 1) < 0)
    return -1;

  if (myWrite(fd, buf, size) < 0)
    return -1;

  return size;
}

int childError() {
  printf("Child error: %s\n", strerror(errno));
  return -1;
}


int parentError() {
  printf("Parent error: %s\n", strerror(errno));
  return -1;
}

int child(int writeFd, int readFd)
{
  assert(readFd >= 0);

  char buf[MAX_MSG_SIZE + 1];
  char ack[] = {ACK_VAL};

  if(packageRead(readFd, buf) < 0)
    return childError();
  
  printf("Child heard: %s\n", buf);

  if(packageWrite(writeFd, ack, sizeof(ack)) < 0)
    return childError();

  return 0;
}

int parent(int writeFd, int readFd, const char *message, char msgSize)
{
  assert(writeFd >= 0);
  assert(msgSize <= MAX_MSG_SIZE);

  unsigned char ack = 0;

  if(packageWrite(writeFd, message, msgSize) < 0)
    return parentError();
  
  printf("Parent sent: %s\n", message);  
  
  if(packageRead(readFd, &ack) < 0)
    return parentError();

  if (ack != ACK_VAL) {
    printf("Parent error: invalid ack %u\n", ack);
    return -1;
  }

  printf("Parent recieved correct ack from child\n");
  return 0;
}

int main()
{
  const char *message = "Hello child!";

  int fds[2] = {-1, -1};

  if (pipe(fds) < 0)
  {
    printf("Failed to create pipe: %s\n", strerror(errno));
    return 1;
  }

  int readFd = fds[0];
  int writeFd = fds[1];

  pid_t pid = fork();

  if (pid < 0)
  {
    printf("Failed to fork: %s\n", strerror(errno));
    return 1;
  }

  if (pid == 0)
  { // Child
    //close(writeFd);
    return child(writeFd, readFd);
  }

  else
  { // Parent
    //close(readFd);
    parent(writeFd, readFd, message, strnlen(message, MAX_MSG_SIZE));

    int status = 0;

    return 0;
  }
}
