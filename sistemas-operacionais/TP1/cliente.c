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
    sem_t *shm2; // semaforo
    char *aux;

    key = 5678;
    key2 = 5879;

    if ((shmid = shmget(key, SHMSZ, 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }

    if ((shmid2 = shmget(key2, sizeof(sem_t), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget uahsuahsuah");
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

    char *mensagem_bonita;

    printf("Digite a mensagem que deseja transmitir: ");
    scanf("%s", mensagem_bonita);

    while (*mensagem_bonita != '\0')
    {
        *shm = *mensagem_bonita;
        mensagem_bonita++;
        shm++;
    }

    sem_post(&(*shm2));
    exit(0);
}
