#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FRAMES 100
#define MAX_INPUT_SIZE 1000

int main(int argc, char *argv[]) {
    // Check if correct number of arguments provided
    if (argc != 4) {
        printf("Usage: %s <filename> <algorithm type> <number of frames>\n", argv[0]);
        exit(1);
    }

    // Open the input file
    FILE *input_file = fopen(argv[1], "r");

    // Check if input file was opened successfully
    if (input_file == NULL) {
        printf("Error: Unable to open input file.\n");
        exit(1);
    }

    // Parse input arguments
    int algorithm_type = atoi(argv[2]);
    int num_frames = atoi(argv[3]);

    // Create arrays for the page table, frame table, and reference bit table
    int page_table[MAX_FRAMES];
    int frame_table[MAX_FRAMES];
    int reference_bit_table[MAX_FRAMES];

    // Initialize all arrays to zero
    memset(page_table, 0, sizeof(page_table));
    memset(frame_table, 0, sizeof(frame_table));
    memset(reference_bit_table, 0, sizeof(reference_bit_table));

    // Initialize variables for tracking disk reads and writes
    int num_reads = 0;
    int num_writes = 0;

    // Initialize variables for parsing input file
    char input[MAX_INPUT_SIZE];
    char *token;
    int virtual_address, operation_type;

    // Initialize variables for implementing FIFO algorithm
    int fifo_queue[MAX_FRAMES];
    int queue_front = 0;
    int queue_back = 0;

    // Initialize variables for implementing LRU algorithm
    int lru_timestamp[MAX_FRAMES];
    int timestamp_counter = 0;

    // Loop through input file
    while (fgets(input, MAX_INPUT_SIZE, input_file) != NULL) {
        // Parse virtual address and operation type from input line
        token = strtok(input, " ");
        virtual_address = atoi(token);
        token = strtok(NULL, " ");
        operation_type = atoi(token);

        // Check if page is already in memory
        int page_found = 0;
        int page_index;
        for (int i = 0; i < num_frames; i++) {
            if (page_table[i] == virtual_address) {
                page_found = 1;
                page_index = i;
                break;
            }
        }

        // If page is not in memory, choose a page to evict using the selected algorithm
        if (!page_found) {
            // Increment disk reads counter
            num_reads++;

            // Find an empty frame if available
            int empty_frame_index = -1;
            for (int i = 0; i < num_frames; i++) {
                if (frame_table[i] == 0) {
                    empty_frame_index = i;
                    break;
                }
            }

            // If no empty frames available, use selected algorithm to choose a page to evict
            if (empty_frame_index == -1) {
                int evicted_page_index = -1;
                if (algorithm_type == 1) { // FIFO algorithm
                    evicted_page_index = fifo_queue[queue_front];
                    queue_front = (queue_front + 1) % num_frames;
                } else if (algorithm_type == 2) { // LRU algorithm
                    int oldest_timestamp = timestamp_counter;
                    for (int i = 0; i < num_frames; i++) {
                        if (lru_timestamp[i] < oldest_timestamp){
                            oldest_timestamp = lru_timestamp[i];
                            evicted_page_index = i;
                        }
                    }
                  }
                             // Increment disk writes counter
            num_writes++;

            // Remove evicted page from page table and frame table
            int evicted_page = page_table[evicted_page_index];
            page_table[evicted_page_index] = 0;
            frame_table[evicted_page_index] = 0;

            // If evicted page was dirty, write it back to disk
            if (reference_bit_table[evicted_page_index] == 1) {
                num_writes++;
            }

            // Add new page to page table and frame table
            page_table[empty_frame_index] = virtual_address;
            frame_table[empty_frame_index] = 1;
            reference_bit_table[empty_frame_index] = operation_type;

            // Update FIFO queue or LRU timestamp for new page
            if (algorithm_type == 1) { // FIFO algorithm
                fifo_queue[queue_back] = empty_frame_index;
                queue_back = (queue_back + 1) % num_frames;
            } else if (algorithm_type == 2) { // LRU algorithm
                lru_timestamp[empty_frame_index] = ++timestamp_counter;
            }
        } else { // If an empty frame is available, add new page to page table and frame table
            page_table[empty_frame_index] = virtual_address;
            frame_table[empty_frame_index] = 1;
            reference_bit_table[empty_frame_index] = operation_type;

            // Update FIFO queue or LRU timestamp for new page
            if (algorithm_type == 1) { // FIFO algorithm
                fifo_queue[queue_back] = empty_frame_index;
                queue_back = (queue_back + 1) % num_frames;
            } else if (algorithm_type == 2) { // LRU algorithm
                lru_timestamp[empty_frame_index] = ++timestamp_counter;
            }
        }
    } else { // If page is already in memory, update its reference bit and LRU timestamp (if using LRU algorithm)
        reference_bit_table[page_index] = operation_type;
        if (algorithm_type == 2) { // LRU algorithm
            lru_timestamp[page_index] = ++timestamp_counter;
        }
    }
}

// Print number of disk reads and writes
printf("Disk Reads: %d\n", num_reads);
printf("Disk Writes: %d\n", num_writes);

// Print contents of frames at the end of the simulation
printf("Frame Contents: ");
for (int i = 0; i < num_frames; i++) {
    if (frame_table[i] == 1) {
        printf("%d ", page_table[i]);
    } else {
        printf("Empty ");
    }
}
printf("\n");

// Close input file
fclose(input_file);

return 0;
 
                  
