/* Sequencial.c (Roland Teodorowitsch) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

/* CONSTANTES */
#define GRAU 400
#define TAM_INI 1000000
#define TAM_INC 1000000
#define TAM_MAX 10000000

/* VARIAVEIS GLOBAIS */
double x[TAM_MAX], y[TAM_MAX], gabarito[TAM_MAX];

/* PROTOTIPOS */
double polinomio(double v[], int grau, double x);
void erro(char *msg_erro);

double polinomio(double a[], int grau, double x)
{
  int i;
  double res = a[0], pot = x;
  for (i = 1; i <= grau; ++i)
  {
    res += a[i] * pot;
    pot = pot * x;
  }
  return res;
}

void erro(char *msg_erro)
{
  fprintf(stderr, "ERRO: %s\n", msg_erro);
  MPI_Finalize();
  exit(1);
}

int main(int argc, char **argv)
{
  int master;
  int id; /* Identificador do processo */
  int n;  /* Numero de processos */
  int i, size;
  double *vet, valor, *vresp, resposta, tempo, a[GRAU + 1];
  int hostsize; /* Tamanho do nome do nodo */
  char hostname[MPI_MAX_PROCESSOR_NAME];
  MPI_Status status; /* Status de retorno */

  MPI_Init(&argc, &argv);
  MPI_Get_processor_name(hostname, &hostsize);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &n);

  if (id == master) {
    /* Gera os coeficientes do polinomio */
    #pragma omp parallel for
    for (i = 0; i <= GRAU; ++i)
      a[i] = (i % 3 == 0) ? -1.0 : 1.0;

    /* Preenche vetores para o gabarito*/
    #pragma omp parallel for
    for (i = 0; i < TAM_MAX; ++i)
    {
      x[i] = 0.1 + 0.1 * (double)i / TAM_MAX;
      gabarito[i] = polinomio(a, GRAU, x[i]);
    }
  }
  
    // MPI_Scatter(globaldata, 1, MPI_INT, &localdata, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // printf("Processor %d has data %d\n", rank, localdata);
    // localdata *= 2;
    // printf("Processor %d doubling the data, now has %d\n", rank, localdata);

    // MPI_Gather(&localdata, 1, MPI_INT, globaldata, 1, MPI_INT, 0, MPI_COMM_WORLD);


  if (id == master)
  {
    /* Gera tabela com tamanhos e tempos */
    // for (size = TAM_INI; size <= TAM_MAX; size += TAM_INC)
    // MPI_Bcast(&a, TAM_INC, MPI_Double, master, MPI_COMM_WORLD);
    // {
      // CALCULAR!!!
      // BCAST -> ENVIAR PARA N
      // {
      //  - inicio
      //  - tamanho
      //  - CHUNK c tam valores
      // }

    // }
      // espera em um laço por N respostas
      // while(1 || 1 == 1) {
      // MPI_RECV
      // Recebeu N (Processos) valores, BREAK;
      // }

      // ou sla, montar o vetor y

      /* Mostra tempo */
      // printf("%d %lf\n", size, tempo);
      
      /* Verificacao */
      // for (i = 0; i < size; ++i)
      // {
      //   if (y[i] != gabarito[i])
      //   {
      //     erro("verificacao falhou!");
      //   }
      // }
  }
  else {
    // Slave
    // Vai receber
    // - Inicio
    // - tamanho
    // - CHUNK = n valores X[]*
    // RECV
    // MPI_Bcast(&a)
    // CALCULA

    // SEND
    // - inicio
    // - tamanho
    // - chunk y

    /* Calcula (Sequencial)*/
    // tempo = -MPI_Wtime();
    // for (i = 0; i < size; ++i)
    //   y[i] = polinomio(a, GRAU, x[i]);
    // tempo += MPI_Wtime();

    // MPI_Bcast(a, tamA, MPI_Float, 0 (root), MPI_COMM_WORLD)

    // DEPOIS DE CALCULAR, MPI_SEND() para o ROOT o Array Calculado
  }
  MPI_Finalize();
  return 0;
}