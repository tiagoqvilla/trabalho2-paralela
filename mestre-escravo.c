// MestreEscravo.c (Roland Teodorowitsch; 17 out. 2019)
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char* argv[]) {
  int i, id, n, message;
  MPI_Status status;

  MPI_Init(&argc , &argv); // funcao que inicializa o MPI, todo o codigo paralelo estah abaixo
  MPI_Comm_rank(MPI_COMM_WORLD, &id); // pega pega o numero do processo atual (rank)
  MPI_Comm_size(MPI_COMM_WORLD, &n);  // pega informacao do numero de processos (quantidade total)
  if ( id == 0 ) {  // MESTRE
     int *bag = (int *) malloc(sizeof(int)*(n-1));
     if (bag==NULL) {
        MPI_Finalize();
        return 1;
     }
     // Inicializacao do conjunto de trabalho
     for ( i=0 ; i < n-1; bag[i] = 10*(++i));
     printf("Estado inicial:");
     for ( i=0; i<n-1; ++i)
         printf(" %d", bag[i]);
     printf("\n\n");
     // Envio das mensagens para os escravos
     for ( i=1 ; i < n ; i++) {
         message = bag[i-1];
         MPI_Send(&message, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
     }
     // Recebimento das respostas
     for ( i=1 ; i < n ; i++) {
         MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
         printf("Mestre [%d/%d] recebeu %d\n", id, n, message);
         bag[status.MPI_SOURCE-1] = message;
     }
     printf("\nResultado final:");
     for ( i=0; i<n-1; ++i)
         printf(" %d", bag[i]);
     printf("\n");
     free((void *)bag);
  }
  else {  // ESCRAVO
     // Recebe mensagem
     MPI_Recv(&message, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
     printf("Escravo [%d/%d] recebeu %d\n", id, n, message);
     // Retorna resposta para o mestre
     message = id;
     MPI_Send(&message, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
  }
  MPI_Finalize();
  return 0;
}