#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define FIRST_FIT 1
#define BEST_FIT 2
#define WORST_FIT 3

static int get_policy(const char *argument) {
    if (!strcmp(argument, "-F") || !strcmp(argument, "-FIFO")) return FIRST_FIT;
    if (!strcmp(argument, "-B") || !strcmp(argument, "-BESTFIT")) return BEST_FIT;
    if (!strcmp(argument, "-W") || !strcmp(argument, "-WORSTFIT")) return WORST_FIT;
    return 0;
}

static void add_free_block(list_t *free_list, block_t *block, int policy) {
    block->pid = 0;
    if (policy == FIRST_FIT) list_add_to_back(free_list, block);
    else if (policy == BEST_FIT) list_add_ascending_by_blocksize(free_list, block);
    else list_add_descending_by_blocksize(free_list, block);
}

void allocate_memory(list_t *free_list, list_t *allocated_list,
                     int pid, int requested_size, int policy) {
    if (requested_size <= 0 || list_is_in_by_pid(allocated_list, pid)) {
        printf("Error: Not Enough Memory\n");
        return;
    }

    block_t *available = list_take_first_fit(free_list, requested_size);
    if (available == NULL) {
        printf("Error: Not Enough Memory\n");
        return;
    }

    int original_end = available->end;
    available->pid = pid;
    available->end = available->start + requested_size - 1;
    list_add_ascending_by_address(allocated_list, available);

    /* Return any unused portion of the selected block to the free list. */
    if (available->end < original_end) {
        block_t *fragment = malloc(sizeof(*fragment));
        if (fragment == NULL) {
            fprintf(stderr, "Error: Memory Allocation %d blocks\n", requested_size);
            exit(EXIT_FAILURE);
        }
        fragment->start = available->end + 1;
        fragment->end = original_end;
        add_free_block(free_list, fragment, policy);
    }
}

void deallocate_memory(list_t *allocated_list, list_t *free_list,
                       int pid, int policy) {
    block_t *released = list_remove_by_pid(allocated_list, pid);
    if (released != NULL) add_free_block(free_list, released, policy);
}

void coalese_memory(list_t *free_list) {
    list_coalese_nodes(free_list);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("usage: ./mmu <input file> -{F | B | W } \n"
               "(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
        return EXIT_FAILURE;
    }

    int policy = get_policy(argv[2]);
    if (policy == 0) {
        printf("usage: ./mmu <input file> -{F | B | W } \n"
               "(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
        return EXIT_FAILURE;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        fprintf(stderr, "Error: Invalid filepath\n");
        return EXIT_FAILURE;
    }

    int partition_size;
    if (fscanf(input, "%d", &partition_size) != 1 || partition_size <= 0) {
        fclose(input);
        return EXIT_FAILURE;
    }

    printf("PARTITION_SIZE = %d\n", partition_size);
    list_t *free_list = list_create();
    list_t *allocated_list = list_create();
    block_t *initial = malloc(sizeof(*initial));
    if (initial == NULL) return EXIT_FAILURE;
    initial->pid = 0;
    initial->start = 0;
    initial->end = partition_size - 1;
    list_add_to_back(free_list, initial);

    int pid, requested_size;
    while (fscanf(input, "%d %d", &pid, &requested_size) == 2) {
        printf("************************\n");
        if (pid == -99999) {
            printf("COALESCE/COMPACT\n");
            coalese_memory(free_list);
        } else if (pid > 0) {
            printf("ALLOCATE: %d FROM PID: %d\n", requested_size, pid);
            allocate_memory(free_list, allocated_list, pid, requested_size, policy);
        } else {
            printf("DEALLOCATE MEM: PID %d\n", abs(pid));
            deallocate_memory(allocated_list, free_list, abs(pid), policy);
        }
        printf("************************\n");
        print_list(free_list, "Free Memory", 0);
        printf("\n");
        print_list(allocated_list, "Allocated Memory", 1);
        printf("\n\n");
    }

    fclose(input);
    list_free(free_list);
    list_free(allocated_list);
    return EXIT_SUCCESS;
}
