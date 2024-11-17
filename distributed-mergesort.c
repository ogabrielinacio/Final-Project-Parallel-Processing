#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void merge(int *a, int *b, int l, int m, int r) {
    int h, i, j, k;
    h = l;
    i = l;
    j = m + 1;

    while((h <= m) && (j <= r)) {
        if(a[h] <= a[j]) {
            b[i] = a[h];
            h++;
        } else {
            b[i] = a[j];
            j++;
        }
        i++;
    }

    if(m < h) {
        for(k = j; k <= r; k++) {
            b[i] = a[k];
            i++;
        }
    } else {
        for(k = h; k <= m; k++) {
            b[i] = a[k];
            i++;
        }
    }

    for(k = l; k <= r; k++) a[k] = b[k];
}

void mergeSort(int *a, int *b, int l, int r) {
    int m;
    if(l < r) {
        m = (l + r) / 2;
        mergeSort(a, b, l, m);
        mergeSort(a, b, m + 1, r);
        merge(a, b, l, m, r);
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

       // Broadcasts the total size of the array to all processes
       MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

       // Calculates the size of the sub-array
       sub_size = n / size;

       // Each process allocates space for its sub-array
       sub_array = (int *)malloc(sub_size * sizeof(int));

       // Distributes the original array among the processes
       MPI_Scatter(array, sub_size, MPI_INT, sub_array, sub_size, MPI_INT, 0, MPI_COMM_WORLD);

       // Each process sorts its part
       int *temp_array = (int *)malloc(sub_size * sizeof(int));
       mergeSort(sub_array, temp_array, 0, sub_size - 1);

       // Gathers the sorted parts
       MPI_Gather(sub_array, sub_size, MPI_INT, array, sub_size, MPI_INT, 0, MPI_COMM_WORLD);

       if (rank == 0) {
           // Process 0 performs the final merge
           int *final_array = (int *)malloc(n * sizeof(int));
           mergeSort(array, final_array, 0, n - 1);

           printf("\nOrdened Array: ");
           for (int i = 0; i < n; i++) {
               printf("%d ", array[i]);
           }
           printf("\n");

           free(final_array);
           free(array);
       }

       free(sub_array);
       free(temp_array);

       MPI_Finalize();
       return 0;
}
