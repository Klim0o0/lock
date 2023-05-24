#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

static int LOCKS_COUNT = 0;


void kill(int sig) {
    int fileDescriptor = open("stat", O_CREAT | O_WRONLY | O_APPEND, 0640);
    lseek(fileDescriptor, 0, SEEK_END);
    char statString[50];
    snprintf(statString, 50, "PID=%d; LOCKS_COUNT=%d\n", getpid(), LOCKS_COUNT);
    write(fileDescriptor, statString, strlen(statString));
    close(fileDescriptor);
    exit(0);
}

bool unlock(char *lockFile, char *pidBuffer) {
    int pidBufferLen = strlen(pidBuffer);
    char lockFilePidBuffer[pidBufferLen];

    int fileDescriptor = open(lockFile, O_RDONLY, 0640);

    read(fileDescriptor, lockFilePidBuffer, pidBufferLen);

    int readedPid = atoi(lockFilePidBuffer);
    if (readedPid != getpid())
        return false;
    close(fileDescriptor);

    if (remove(lockFile) != 0)
        return false;
    return true;
}

void fakeAction() {
    sleep(1);
}

void lock(char *lockFile, char *pidBuffer) {
    int fileDescriptor = -1;
    while (fileDescriptor == -1)
        fileDescriptor = open(lockFile, O_CREAT | O_EXCL | O_RDWR, 0640);


    write(fileDescriptor, pidBuffer, strlen(pidBuffer));
    close(fileDescriptor);
}


int main(int argc, char *argv[]) {
    signal(SIGINT, kill);

    if (argv[1] == NULL) {
        printf("ERROR: no file name argument");
        return 1;
    }

    char *file = argv[1];
    char pidBuffer[10];
    sprintf(pidBuffer, "%d", getpid());
    char *lockFile = strcat(file, ".lck");

    while (true) {
        lock(lockFile, pidBuffer);
        fakeAction();
        bool isOk = unlock(lockFile, pidBuffer);

        if (!isOk) {
            printf("ERROR: unlock failed");
            return 1;
        }
        LOCKS_COUNT++;
    }

    return 0;
}

