#ifndef LIST_H
#define LIST_H

typedef struct block {
    int pid;
    int start;
    int end;
} block_t;

typedef struct node {
    block_t *blk;
    struct node *next;
} node_t;

typedef struct list {
    node_t *head;
} list_t;

list_t *list_create(void);
void list_free(list_t *list);
int list_is_in_by_pid(const list_t *list, int pid);
block_t *list_remove_by_pid(list_t *list, int pid);
block_t *list_take_first_fit(list_t *list, int requested_size);
void list_add_to_back(list_t *list, block_t *block);
void list_add_ascending_by_blocksize(list_t *list, block_t *block);
void list_add_descending_by_blocksize(list_t *list, block_t *block);
void list_add_ascending_by_address(list_t *list, block_t *block);
void list_coalese_nodes(list_t *list);
void print_list(const list_t *list, const char *title, int show_pid);

#endif
