#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void merge(int* arr, int* temp, size_t left, size_t mid, size_t right) {
    size_t i = left, j = mid + 1, k = left;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }

    while (i <= mid) {
        temp[k++] = arr[i++];
    }

    while (j <= right) {
        temp[k++] = arr[j++];
    }

    for (i = left; i <= right; i++) {
        arr[i] = temp[i];
    }
}

void mergeSort(int* arr, int* temp, size_t left, size_t right) {
    if (left < right) {
        size_t mid = left + (right - left) / 2;

        mergeSort(arr, temp, left, mid);
        mergeSort(arr, temp, mid + 1, right);
        merge(arr, temp, left, mid, right);
    }
}

void chunkedMergeSort(FILE* inputFile, size_t chunkSize) {
    size_t totalElements = 0;
    fseek(inputFile, 0, SEEK_END);
    totalElements = ftell(inputFile) / sizeof(int);
    rewind(inputFile);

    int* chunk = (int*)malloc(chunkSize * sizeof(int));
    int* temp = (int*)malloc(chunkSize * sizeof(int));
    if (!chunk || !temp) {
        printf("Error allocating memory for chunks.\n");
        fclose(inputFile);
        free(chunk);
        free(temp);
        return;
    }

    size_t elementsRead = 0;
    size_t totalRead = 0;

    while ((elementsRead = fread(chunk, sizeof(int), chunkSize, inputFile)) > 0) {
        mergeSort(chunk, temp, 0, elementsRead - 1);
        totalRead += elementsRead;

        printf("\nOrdened Array: ");
        for (size_t i = 0; i < elementsRead; i++) {
            printf("%d ", chunk[i]);
        }
        printf("\n");
    }

    free(chunk);
    free(temp);
}

int main() {
    FILE *file = fopen("random_numbers.bin", "rb");
    if (!file) {
        printf("Error opening random_numbers.bin file\n");
    }

    int number;

    printf("Original Array: ");
    while (fread(&number, sizeof(int), 1, file) == 1) {
        printf("%d ", number);
    }

    printf("\n\n");
    printf("EOF");
    printf("\n\n");

    size_t chunkSize = 1000000;
    chunkedMergeSort(file, chunkSize);

    fclose(file);

    return 0;
}
