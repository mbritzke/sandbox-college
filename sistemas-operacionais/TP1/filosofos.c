#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>

typedef struct
{
  int id_filosofo;
  int comeu;
  int pensou;
  int esperou;
} Relatorio;

pthread_mutex_t mutex1, mutex2, mutex3;
int final;
volatile int running = 1;
pthread_mutex_t *filosofos;
Relatorio *indices;

void pensando()
{
  sleep(5);
}

void comendo()
{
  sleep(2);
}

int setN(int n)
{
  final = n;
}

int getN()
{
  return final;
}

void *filosofo(void *indice)
{ //mando por parâmetro a identificação desse filosofo
  // o que um filosofo vai fazer?
  Relatorio *threadAtual = (Relatorio *)indice;
  int i = threadAtual->id_filosofo;

  while (1)
  {             // laço infinito
    pensando(); //penso
    threadAtual->pensou = threadAtual->pensou + 1;

    if (i == 0)
    { //tenho que pegar o meu garfo + o garfo de mim - 1. Se for zero, dá erro

      if (pthread_mutex_trylock(&filosofos[i]) == 0)
      { //tento pegar meu garfo

        if (pthread_mutex_trylock(&filosofos[getN()]) == 0)
        {            // tento pegar o garfo do meu amiguinho
          comendo(); //vou comer
          threadAtual->comeu = threadAtual->comeu + 1;
        }
        else
        { //não consegui pegar o garfo do amiguinho
          threadAtual->esperou = threadAtual->esperou + 1;
          pthread_mutex_unlock(&filosofos[i]); // liberei o meu garfo
          sleep(rand() % 4);                   // espero um numero aleatório de 1 a 3
        }
      }
      else
      { // caso não consegui pegar meu próprio garfo
        threadAtual->esperou = threadAtual->esperou + 1;
        sleep(rand() % 4);
      }
    }
    else
    { // se i não for zero

      if (pthread_mutex_trylock(&filosofos[i]) == 0)
      { // tento pegar meu garfo

        if (pthread_mutex_trylock(&filosofos[i - 1]) == 0)
        { // tento pegar o garfo do meu amiguinho
          comendo();
          threadAtual->comeu = threadAtual->comeu + 1;
        }
        else
        { // caso não consiga pegar o garfo do meu  amiguinho
          threadAtual->esperou = threadAtual->esperou + 1;
          pthread_mutex_unlock(&filosofos[i]);
          sleep(rand() % 4);
        }
      }
      else
      { // caso não consegui pegar meu próprio garfo

        threadAtual->esperou = threadAtual->esperou + 1;
        sleep(rand() % 4);
      }
    }
  }
}

int main(int argc, char **argv)
{
  pthread_t thread1;
  int n;
  int runtime;
  pthread_t *threads;
  Relatorio *indices;

  printf("Digite o numero de threads: ");
  scanf("%d", &n);
  printf("%d\n", n);
  setN(n);

  printf("Digite o tempo das threads: ");
  scanf("%d", &runtime);

  filosofos = (pthread_mutex_t *)malloc(n * sizeof(pthread_mutex_t));
  indices = (Relatorio *)malloc(n * sizeof(Relatorio));
  threads = (pthread_t *)malloc(n * sizeof(pthread_t));

  int i;

  for (i = 0; i <= n; i++)
  {

    pthread_mutex_init(&filosofos[i], NULL); // garfos disponíveis
  }

  for (i = 0; i <= n; i++)
  {

    indices[i].id_filosofo = i;
    pthread_create(&threads[i], NULL, filosofo, (void *)&indices[i]); // threads lançadas
  }

  sleep(runtime);
  running = 0;
  printf("\n");

  int j;

  for (j = 0; j <= n; j++)
  {

    printf("THREAD = %d \n", indices[j].id_filosofo);
    printf("COMI = %d \n", indices[j].comeu);
    printf("PENSEI = %d \n", indices[j].pensou);
    printf("TENTEI = %d \n", indices[j].esperou);
    printf("\n");
  }

  return 0;
}
