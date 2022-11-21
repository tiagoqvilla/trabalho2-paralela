#include <stdio.h>
#include <stdbool.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Comm parent, intercomm;
    const int slaves = 2;
    bool master;
    int data[slaves];
    int mydata;
    int i, rank, root;

    MPI_Init(&argc, &argv);
    MPI_Comm_get_parent(&parent);
    master = parent == MPI_COMM_NULL;

    if (master) {
        int errcodes[slaves];
        /* spawn the slaves */
        MPI_Comm_spawn(argv[0], MPI_ARGV_NULL, slaves, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercomm, errcodes);

        /* prepare data to be scattered to the slaves */
        for (i=0; i<slaves; i++) {
            data[i] = i;
        }

        /* here we assume there is only one master */
        root = MPI_ROOT;
    } else {
        intercomm = parent;
        root = 0;
    }

    /* scatter data from master to slaves */
    MPI_Scatter(data, 1, MPI_INT, &my-data, 1, MPI_INT, root, intercomm);

    if (!master) {
        /* slaves do their work */
        mydata = mydata + 1;
    }

    /* gather data from slaves to master */
    MPI_Gather(&mydata, 1, MPI_INT, data, 1, MPI_INT, root, intercomm);

    if (master) {
        int i;
        for (i=0; i<slaves; i++) {
            printf("Slave %d returned %d\n", i, data[i]);
        }
    }

    MPI_Comm_disconnect(&intercomm);

    MPI_Finalize();
    return 0;
}