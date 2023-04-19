#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FRAMES 1000

int frames[MAX_FRAMES];

int fifo(int* pages, int num_pages, int num_frames) {
    int disk_reads = 0, disk_writes = 0;
    int frame_index = 0;
    int page_fault = 0;
    for (int i = 0; i < num_pages; i++) {
        int page = pages[i];
        int found = 0;
        for (int j = 0; j < num_frames; j++) {
            if (frames[j] == page) {
                found = 1;
                break;
            }
        }
        if (!found) {
            page_fault++;
            if (frame_index < num_frames) {
                frames[frame_index++] = page;
                disk_reads++;
            } else {
                frames[0] = page;
                disk_reads++;
                disk_writes++;
            }
        }
    }
    return disk_reads + disk_writes;
}

int lru(int* pages, int num_pages, int num_frames) {
    int disk_reads = 0, disk_writes = 0;
    int page_fault = 0;
    int* lru = (int*)malloc(num_frames * sizeof(int));
    memset(lru, 0, num_frames * sizeof(int));
    for (int i = 0; i < num_pages; i++) {
        int page = pages[i];
        int found = 0;
        for (int j = 0; j < num_frames; j++) {
            if (frames[j] == page) {
                found = 1;
                lru[j] = i;
                break;
            }
        }
        if (!found) {
            page_fault++;
            int min_lru = lru[0];
            int min_lru_index = 0;
            for (int j = 1; j < num_frames; j++) {
                if (lru[j] < min_lru) {
                    min_lru = lru[j];
                    min_lru_index = j;
                }
            }
            frames[min_lru_index] = page;
            lru[min_lru_index] = i;
            disk_reads++;
            disk_writes++;
        }
    }
    free(lru);
    return disk_reads + disk_writes;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Usage: %s <file name> <algorithm type (0 for FIFO, 1 for LRU)> <number of frames>\n", argv[0]);
        return 1;
    }
    char* file_name = argv[1];
    int algorithm_type = atoi(argv[2]);
    int num_frames = atoi(argv[3]);
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        printf("Unable to open file %s\n", file_name);
        return 1;
    }
    int num_pages = 0;
    int* pages = (int*)malloc(MAX_FRAMES * sizeof(int));
    int page, operation;
    while (fscanf(file, "%d %d", &page, &operation) != EOF) {
        pages[num_pages++] = page;
    }
    fclose(file);
    int disk_io;
    if (algorithm_type == 0) {
        disk_io = fifo(pages, num_pages, num_frames);
    } else if (algorithm_type == 1) {
        disk_io = lru(pages, num_pages, num_frames);
    } else {
        printf("Invalid algorithm type. Must be 0 (FIFO) or 1 (LRU)\n");
        return 1;
    }
    printf("Disk Reads: %d\n", disk_io);
    printf("Disk Writes: %d\n", disk_io);
    printf("Frames: ");
    for (int i = 0; i < num_frames; i++) {
        if (frames[i] == 0) {
            printf("_ ");
        } else {
            printf("%d ", frames[i]);
        }
    }
    printf("\n");
    free(pages);
    return 0;
}
   
