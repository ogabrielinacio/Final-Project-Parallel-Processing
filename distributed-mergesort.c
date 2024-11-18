#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void merge(int *a, int *b, int l, int m, int r) {
    int h, i, j, k;
    h = l;
    i = l;
    j = m + 1;

    while ((h <= m) && (j <= r)) {
        if (a[h] <= a[j]) {
            b[i] = a[h];
            h++;
        } else {
            b[i] = a[j];
            j++;
        }
        i++;
    }

    if (m < h) {
        for (k = j; k <= r; k++) {
            b[i] = a[k];
            i++;
        }
    } else {
        for (k = h; k <= m; k++) {
            b[i] = a[k];
            i++;
        }
    }

    for (k = l; k <= r; k++) a[k] = b[k];
}

void mergeSort(int *a, int *b, int l, int r) {
    int m;
    if (l < r) {
        m = (l + r) / 2;
        mergeSort(a, b, l, m);
        mergeSort(a, b, m + 1, r);
        merge(a, b, l, m, r);
    }
}

void hierarchicalMerge(int *array, int n, int rank, int size) {
    int step = 1;

    while (step < size) {
        if (rank % (2 * step) == 0) {
            if (rank + step < size) {
                int recv_size;
                MPI_Recv(&recv_size, 1, MPI_INT, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int *recv_array = (int *)malloc(recv_size * sizeof(int));
                MPI_Recv(recv_array, recv_size, MPI_INT, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int *temp_array = (int *)malloc((n + recv_size) * sizeof(int));
                merge(array, temp_array, 0, n - 1, n + recv_size - 1);
                free(array);

                array = temp_array;
                n += recv_size;
            }
        } else {
            int parent = rank - step;
            MPI_Send(&n, 1, MPI_INT, parent, 0, MPI_COMM_WORLD);
            MPI_Send(array, n, MPI_INT, parent, 0, MPI_COMM_WORLD);
            break;
        }
        step *= 2;
    }

    if (rank == 0) {
        printf("\nOrdened Array: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n;              // Total number of elements
    int *array = NULL;  // Main array
    int *sub_array;     // Sub-array for each process
    int sub_size;       // Size of the sub-array

    if (rank == 0) {
        FILE *file = fopen("random_numbers.bin", "rb");
        if (!file) {
            printf("Error opening random_numbers.bin file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        n = file_size / sizeof(int);
        fseek(file, 0, SEEK_SET);

        array = (int *)malloc(file_size);
        if (!array) {
            printf("Error allocating memory\n");
            fclose(file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fread(array, sizeof(int), n, file);
        fclose(file);

        printf("Original Array: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", array[i]);
        }
        printf("\n\n");
        printf("EOF");
        printf("\n\n");
    }

    // Broadcast the total size of the array to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Adjust sub_size to handle remainder
    sub_size = n / size;
    if (rank < n % size) {
        sub_size++;
    }

    // Allocate space for sub-array
    sub_array = (int *)malloc(sub_size * sizeof(int));

    // Scatter the array among processes using MPI_Scatterv
    int *displs = (int *)malloc(size * sizeof(int));
    int *send_counts = (int *)malloc(size * sizeof(int));

    if (rank == 0) {
        int offset = 0;
        for (int i = 0; i < size; i++) {
            send_counts[i] = (n / size) + (i < n % size ? 1 : 0);
            displs[i] = offset;
            offset += send_counts[i];
        }
    }

    MPI_Scatterv(array, send_counts, displs, MPI_INT, sub_array, sub_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Sort each process's part
    int *temp_array = (int *)malloc(sub_size * sizeof(int));
    mergeSort(sub_array, temp_array, 0, sub_size - 1);

    // Perform hierarchical merge
    hierarchicalMerge(sub_array, sub_size, rank, size);

    free(sub_array);
    free(temp_array);

    if (rank == 0) {
        free(array);
    }

    MPI_Finalize();
    return 0;
}
