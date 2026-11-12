#include <stdio.h>
#include <stdlib.h>
#include "list.h"

static int block_size(const block_t *block) {
    return block->end - block->start + 1;
}

static node_t *make_node(block_t *block) {
    node_t *node = malloc(sizeof(*node));
    if (node == NULL) {
        fprintf(stderr, "Error: unable to allocate list node\n");
        exit(EXIT_FAILURE);
    }
    node->blk = block;
    node->next = NULL;
    return node;
}

list_t *list_create(void) {
    list_t *list = malloc(sizeof(*list));
    if (list == NULL) exit(EXIT_FAILURE);
    list->head = NULL;
    return list;
}

void list_free(list_t *list) {
    if (list == NULL) return;
    node_t *current = list->head;
    while (current != NULL) {
        node_t *next = current->next;
        free(current->blk);
        free(current);
        current = next;
    }
    free(list);
}

int list_is_in_by_pid(const list_t *list, int pid) {
    for (node_t *current = list->head; current != NULL; current = current->next) {
        if (current->blk->pid == pid) return 1;
    }
    return 0;
}

block_t *list_remove_by_pid(list_t *list, int pid) {
    node_t **link = &list->head;
    while (*link != NULL && (*link)->blk->pid != pid) link = &(*link)->next;
    if (*link == NULL) return NULL;

    node_t *removed = *link;
    block_t *block = removed->blk;
    *link = removed->next;
    free(removed);
    return block;
}

block_t *list_take_first_fit(list_t *list, int requested_size) {
    node_t **link = &list->head;
    while (*link != NULL && block_size((*link)->blk) < requested_size) {
        link = &(*link)->next;
    }
    if (*link == NULL) return NULL;

    node_t *removed = *link;
    block_t *block = removed->blk;
    *link = removed->next;
    free(removed);
    return block;
}

void list_add_to_back(list_t *list, block_t *block) {
    node_t **link = &list->head;
    while (*link != NULL) link = &(*link)->next;
    *link = make_node(block);
}

void list_add_ascending_by_blocksize(list_t *list, block_t *block) {
    node_t **link = &list->head;
    while (*link != NULL && block_size((*link)->blk) <= block_size(block)) {
        link = &(*link)->next;
    }
    node_t *node = make_node(block);
    node->next = *link;
    *link = node;
}

void list_add_descending_by_blocksize(list_t *list, block_t *block) {
    node_t **link = &list->head;
    while (*link != NULL && block_size((*link)->blk) >= block_size(block)) {
        link = &(*link)->next;
    }
    node_t *node = make_node(block);
    node->next = *link;
    *link = node;
}

void list_add_ascending_by_address(list_t *list, block_t *block) {
    node_t **link = &list->head;
    while (*link != NULL && (*link)->blk->start <= block->start) {
        link = &(*link)->next;
    }
    node_t *node = make_node(block);
    node->next = *link;
    *link = node;
}

void list_coalese_nodes(list_t *list) {
    /* Sorting by address makes every pair of adjacent free blocks neighbors. */
    list_t sorted = {NULL};
    while (list->head != NULL) {
        node_t *node = list->head;
        list->head = node->next;
        node_t **link = &sorted.head;
        while (*link != NULL && (*link)->blk->start <= node->blk->start) {
            link = &(*link)->next;
        }
        node->next = *link;
        *link = node;
    }
    list->head = sorted.head;

    node_t *current = list->head;
    while (current != NULL && current->next != NULL) {
        if (current->blk->end + 1 == current->next->blk->start) {
            node_t *merged = current->next;
            current->blk->end = merged->blk->end;
            current->next = merged->next;
            free(merged->blk);
            free(merged);
        } else {
            current = current->next;
        }
    }
}

void print_list(const list_t *list, const char *title, int show_pid) {
    printf("%s:\n", title);
    int index = 0;
    for (node_t *current = list->head; current != NULL;
         current = current->next, ++index) {
        printf("Block %d:\t START: %d\t END: %d",
               index, current->blk->start, current->blk->end);
        if (show_pid) printf("\t PID: %d", current->blk->pid);
        putchar('\n');
    }
}
