#ifndef CBST_BST_H
#define CBST_BST_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h> 
 
typedef int (*bst_compare)(const void* a, const void* b);
typedef void (*bst_free)(void* p);

typedef struct bst bst_t;

typedef void (*bst_visit)(const void* key, void* value, void* user);

bst_t* bst_create(bst_compare cmp, bst_free free_key, bst_free free_val);
void bst_destroy(bst_t* bst);
void bst_clear(bst_t* bst);

bool bst_insert(bst_t* bst, void* key, void* value);
void* bst_get(const bst_t* bst, const void* key);
bool bst_contains(const bst_t* bst, const void* key);
bool bst_remove(bst_t* bst, const void* key);

void* bst_min_key(const bst_t* bst);
void* bst_max_key(const bst_t* bst);
void* bst_floor_key(const bst_t* bst, const void* key);
void* bst_ceiling_key(const bst_t* bst, const void* key);
void* bst_successor_key(const bst_t* bst, const void* key);
void* bst_predecessor_key(const bst_t* bst, const void* key);

size_t bst_size(const bst_t* bst);
int bst_height(const bst_t* bst);

void bst_traverse_inorder(const bst_t* bst, bst_visit visit, void* user);
void bst_traverse_preorder(const bst_t* bst, bst_visit visit, void* user);
void bst_traverse_postorder(const bst_t* bst, bst_visit visit, void* user);

typedef struct bst_iter bst_iter_t;
bst_iter_t* bst_iter_inorder_create(const bst_t* bst);
bool bst_iter_next(bst_iter_t* it, const void** out_key, void** out_value);
void bst_iter_destroy(bst_iter_t* it);

#endif
