#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define SHMSZ 27

int main()
{
    int shmid, shmid2;
    key_t key, key2;
    char *s, *shm;
    int c;
    sem_t *shm2;
    char *aux;

    key = 5678;
    key2 = 5879;

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }

    if ((shmid2 = shmget(key2, sizeof(sem_t), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget ");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) < 0)
    {
        perror("shmat");
        exit(1);
    }

    if ((shm2 = (sem_t *)shmat(shmid2, NULL, 0)) < 0)
    {
        perror("shmat");
        exit(1);
    }

    sem_init(&(*shm2), 1, 0);

    s = shm;

    for (c = 0; c <= 200; c++)
    {
        *s++ = ' ';
    }

    *s = (long)NULL;

    while (1)
    {
        sem_wait(&(*shm2));
        for (s = shm; *s != (long)NULL; s++)
        {
            putchar(*s);
        }
        putchar('\n');
    }

    exit(0);
}
