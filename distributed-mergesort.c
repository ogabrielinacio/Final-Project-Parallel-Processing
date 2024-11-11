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

    //  logic for parallel sorting here...
    
    MPI_Finalize();
    return 0;
}

