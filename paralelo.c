/* paralelo.c (Gustavo e Tiago) */

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
int master = 0;
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
  int id; /* Identificador do processo */
  int n;  /* Numero de processos */
  int i, size;
  double *vet, valor, *vresp, resposta, tempo, a[GRAU + 1];
  int hostsize; /* Tamanho do nome do nodo */
  char hostname[MPI_MAX_PROCESSOR_NAME];
  MPI_Status status; /* Status de retorno */
  int tag = 0;

  MPI_Init(&argc, &argv);
  MPI_Get_processor_name(hostname, &hostsize);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &n);

  if (id == master)
  {
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

  // Fica trancado até todos chegarem nessa linha (chamarem o metodo)
  MPI_Barrier(MPI_COMM_WORLD);

  // Enviar o a para todos os slaves;
  MPI_Bcast(&a, GRAU, MPI_DOUBLE, master, MPI_COMM_WORLD);

  // INICIO - CALCULO
  int slv, first, sizeBySlave;
  for (size = TAM_INI; size <= TAM_MAX; size += TAM_INC)
  {
    if (id == master)
    {
      // Se for o master
      tempo = -MPI_Wtime();
      // Calcula o tamanho do chunk
      sizeBySlave = size / n;
      // Para cada slave
      for (slv = 1; slv < n; ++slv)
      {
        // Envia para o slave slv o pedaço para ele calcular
        first = (slv - 1) * sizeBySlave;
        MPI_Send(&first, 1, MPI_INT, slv, tag, MPI_COMM_WORLD);
        MPI_Send(&sizeBySlave, 1, MPI_INT, slv, tag, MPI_COMM_WORLD);
        MPI_Send(&x[first], sizeBySlave, MPI_DOUBLE, slv, tag, MPI_COMM_WORLD);
      }
      // Tivemos que separar em 2 for pois o MPI_Recv tranca a thread
      int slavesToGo = n;
      while (slavesToGo > 0)
      {
        // Recebe de algum slave o pedaço q ele calculou
        MPI_Recv(&first, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
        printf("descargo: %d\n", status.MPI_SOURCE);
        MPI_Recv(&sizeBySlave, 1, MPI_INT, status.MPI_SOURCE, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&y[first], sizeBySlave, MPI_DOUBLE, status.MPI_SOURCE, tag, MPI_COMM_WORLD, &status);
        --slavesToGo;
      }
      tempo += MPI_Wtime();
      printf("%d %lf\n", size, tempo);
    }
    else
    {
      // Se for algum slave:
      MPI_Recv(&first, 1, MPI_INT, master, tag, MPI_COMM_WORLD, &status);
      MPI_Recv(&sizeBySlave, 1, MPI_INT, master, tag, MPI_COMM_WORLD, &status);
      MPI_Recv(&x[0], 1, MPI_DOUBLE, master, tag, MPI_COMM_WORLD, &status);

      // Calcula
      for (i = 0; i < sizeBySlave; ++i)
        y[i] = polinomio(a, GRAU, x[i]);

      // Envia de volta
      MPI_Send(&first, 1, MPI_INT, master, tag, MPI_COMM_WORLD);
      MPI_Send(&sizeBySlave, 1, MPI_INT, master, tag, MPI_COMM_WORLD);
      MPI_Send(&y[0], sizeBySlave, MPI_DOUBLE, master, tag, MPI_COMM_WORLD);
    }
  }
  // FIM - CALCULO

  if (id == master)
  {
    /* Verificacao */
    for (i = 0; i < size; ++i)
    {
      if (y[i] != gabarito[i])
      {
        erro("verificacao falhou!");
      }
    }
  }
  MPI_Finalize();
  return 0;
}