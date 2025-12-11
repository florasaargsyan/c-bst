#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bst.h"

static char* my_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* dup = malloc(len);
    if (dup) memcpy(dup, s, len);
    return dup;
}

static int cmp_str(const void* a, const void* b) { 
    return strcmp((const char*)a, (const char*)b); 
}
static int cmp_int(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}
static void free_str(void* p) { free(p); }
static void free_int(void* p) { free(p); }

int main(void) {
    int test_count = 0;
    printf("=== BST Comprehensive Test Suite ===\n\n");

    printf("Test %d: BST creation and destruction\n", ++test_count);
    bst_t* bst = bst_create(cmp_str, free_str, free_int);
    assert(bst != NULL);
    assert(bst_size(bst) == 0);
    assert(bst_height(bst) == -1);
    bst_destroy(bst);

    printf("Test %d: Single insertion and retrieval\n", ++test_count);
    bst = bst_create(cmp_str, free_str, free_int);
    char* k1 = my_strdup("alpha");
    int* v1 = malloc(sizeof(int)); 
    *v1 = 1;
    assert(bst_insert(bst, k1, v1));
    assert(bst_size(bst) == 1);
    assert(bst_height(bst) == 0);
    assert(bst_contains(bst, "alpha"));
    assert(*(int*)bst_get(bst, "alpha") == 1);
    bst_destroy(bst);

    printf("Test %d: Multiple insertions\n", ++test_count);
    bst = bst_create(cmp_str, free_str, free_int);
    const char* keys[] = {"delta", "beta", "alpha", "gamma", "epsilon"};
    int vals[] = {4, 2, 1, 3, 5};
    for (int i = 0; i < 5; ++i) {
        char* k = my_strdup(keys[i]);
        int* v = malloc(sizeof(int)); 
        *v = vals[i];
        assert(bst_insert(bst, k, v));
    }
    assert(bst_size(bst) == 5);
    for (int i = 0; i < 5; ++i) {
        assert(bst_contains(bst, keys[i]));
        assert(*(int*)bst_get(bst, keys[i]) == vals[i]);
    }

    printf("Test %d: Duplicate key insertion (update value)\n", ++test_count);
    int* v_new = malloc(sizeof(int)); 
    *v_new = 99;
    char* k_dup = my_strdup("alpha");
    assert(bst_insert(bst, k_dup, v_new));
    assert(bst_size(bst) == 5);
    assert(*(int*)bst_get(bst, "alpha") == 99);

    printf("Test %d: Min and Max keys\n", ++test_count);
    assert(strcmp((char*)bst_min_key(bst), "alpha") == 0);
    assert(strcmp((char*)bst_max_key(bst), "gamma") == 0);

    printf("Test %d: Floor and Ceiling operations\n", ++test_count);
    assert(strcmp((char*)bst_floor_key(bst, "delta"), "delta") == 0);
    assert(strcmp((char*)bst_floor_key(bst, "charlie"), "beta") == 0);
    assert(strcmp((char*)bst_ceiling_key(bst, "delta"), "delta") == 0);
    assert(strcmp((char*)bst_ceiling_key(bst, "charlie"), "delta") == 0);
    assert(bst_floor_key(bst, "aaa") == NULL);
    assert(bst_ceiling_key(bst, "zzz") == NULL);

    printf("Test %d: Successor and Predecessor\n", ++test_count);
    assert(strcmp((char*)bst_successor_key(bst, "alpha"), "beta") == 0);
    assert(strcmp((char*)bst_successor_key(bst, "epsilon"), "gamma") == 0);
    assert(bst_successor_key(bst, "gamma") == NULL);
    assert(strcmp((char*)bst_predecessor_key(bst, "gamma"), "epsilon") == 0);
    assert(strcmp((char*)bst_predecessor_key(bst, "beta"), "alpha") == 0);
    assert(bst_predecessor_key(bst, "alpha") == NULL);

    printf("Test %d: Remove leaf node\n", ++test_count);
    assert(bst_remove(bst, "alpha"));
    assert(!bst_contains(bst, "alpha"));
    assert(bst_size(bst) == 4);

    printf("Test %d: Remove node with one child\n", ++test_count);
    assert(bst_remove(bst, "gamma"));
    assert(!bst_contains(bst, "gamma"));
    assert(bst_size(bst) == 3);

    printf("Test %d: Remove node with two children\n", ++test_count);
    assert(bst_remove(bst, "delta"));
    assert(!bst_contains(bst, "delta"));
    assert(bst_size(bst) == 2);
    assert(bst_contains(bst, "beta"));
    assert(bst_contains(bst, "epsilon"));

    printf("Test %d: Remove non-existent key\n", ++test_count);
    assert(!bst_remove(bst, "nonexistent"));
    assert(bst_size(bst) == 2);

    printf("Test %d: Clear BST\n", ++test_count);
    bst_clear(bst);
    assert(bst_size(bst) == 0);
    assert(bst_height(bst) == -1);
    assert(!bst_contains(bst, "beta"));
    bst_destroy(bst);

    printf("Test %d: Iterator functionality\n", ++test_count);
    bst = bst_create(cmp_str, free_str, free_int);
    const char* iter_keys[] = {"charlie", "alpha", "bravo"};
    for (int i = 0; i < 3; ++i) {
        char* k = my_strdup(iter_keys[i]);
        int* v = malloc(sizeof(int)); *v = i;
        bst_insert(bst, k, v);
    }
    
    bst_iter_t* it = bst_iter_inorder_create(bst);
    assert(it != NULL);
    
    const void* key;
    void* value;
    int iter_count = 0;
    const char* expected[] = {"alpha", "bravo", "charlie"};
    while (bst_iter_next(it, &key, &value)) {
        assert(strcmp((const char*)key, expected[iter_count]) == 0);
        iter_count++;
    }
    assert(iter_count == 3);
    bst_iter_destroy(it);
    bst_destroy(bst);

    printf("Test %d: BST with integer keys\n", ++test_count);
    bst_t* int_bst = bst_create(cmp_int, free_int, free_int);
    int* keys_int[] = {malloc(sizeof(int)), malloc(sizeof(int)), malloc(sizeof(int))};
    *keys_int[0] = 10; *keys_int[1] = 5; *keys_int[2] = 15;
    for (int i = 0; i < 3; ++i) {
        int* v = malloc(sizeof(int)); 
        *v = i;
        bst_insert(int_bst, keys_int[i], v);
    }
    int search_key = 10;
    assert(bst_contains(int_bst, &search_key));
    assert(*(int*)bst_min_key(int_bst) == 5);
    assert(*(int*)bst_max_key(int_bst) == 15);
    bst_destroy(int_bst);

    printf("Test %d: Edge cases - NULL handling\n", ++test_count);
    assert(bst_create(NULL, NULL, NULL) == NULL);
    assert(bst_get(NULL, "key") == NULL);
    assert(!bst_contains(NULL, "key"));
    assert(bst_size(NULL) == 0);

    printf("\n=== All %d tests passed! ===\n", test_count);
    return 0;
}