#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    int* L = (int*)malloc(n1 * sizeof(int));
    int* R = (int*)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
    }
    for (int j = 0; j < n2; j++) {
        R[j] = arr[mid + 1 + j];
    }

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

void mergeSort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {

    FILE *file = fopen("random_numbers.bin", "rb");
    if (!file) {
        printf("Error opening random_numbers.bin file\n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long  file_size = ftell(file);
    size_t arr_size = file_size / sizeof(int);
    fseek(file, 0, SEEK_SET);

    int *arr = (int *)malloc(file_size);
    if (!arr) {
        printf("Error allocating memory\n");
        fclose(file);
        return 1;
    }

    fread(arr, sizeof(int), arr_size, file);
    fclose(file);

    printf("Original Array: \n");
    printArray(arr, arr_size);

    printf("\n\n");
    printf("EOF");
    printf("\n\n");

    mergeSort(arr, 0, arr_size - 1);

    printf("\nOrdened Array: \n");
    printArray(arr, arr_size);
    return 0;
}
