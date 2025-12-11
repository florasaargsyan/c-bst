#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
static void free_str(void* p) { free(p); }
static void free_int(void* p) { free(p); }

static void print_pair(const void* key, void* value, void* user) {
    (void)user;
    printf("  %s => %d\n", (const char*)key, *(int*)value);  // Added indent for readability
}

int main(void) {
    bst_t* bst = bst_create(cmp_str, free_str, free_int);
    if (!bst) { 
        fprintf(stderr, "Failed to create BST\n"); 
        return 1; 
    }

    const char* keys[] = {"delta", "alpha", "charlie", "bravo", "echo"};
    int vals[] = {4, 1, 3, 2, 5};

    for (int i = 0; i < 5; ++i) {
        char* k = my_strdup(keys[i]);
        if (!k) { 
            fprintf(stderr, "Memory allocation failed for key %s\n", keys[i]); 
            bst_destroy(bst);  // Clean up before exit
            return 1; 
        }
        int* v = malloc(sizeof(int));
        if (!v) { 
            fprintf(stderr, "Memory allocation failed for value %d\n", vals[i]); 
            free(k); 
            bst_destroy(bst);  // Clean up before exit
            return 1; 
        }
        *v = vals[i];
        bst_insert(bst, k, v);
    }

    printf("BST Size: %zu, Height: %d\n\n", bst_size(bst), bst_height(bst));

    printf("In-order traversal (sorted):\n");
    bst_traverse_inorder(bst, print_pair, NULL);

    printf("\nPre-order traversal:\n");
    bst_traverse_preorder(bst, print_pair, NULL);

    printf("\nPost-order traversal:\n");
    bst_traverse_postorder(bst, print_pair, NULL);

    printf("\n--- Search Operations ---\n");
    printf("Min key: %s\n", (char*)bst_min_key(bst));
    printf("Max key: %s\n", (char*)bst_max_key(bst));
    printf("Floor of 'delta': %s\n", (char*)bst_floor_key(bst, "delta"));
    printf("Ceiling of 'bob': %s\n", (char*)bst_ceiling_key(bst, "bob"));
    
    printf("\n--- Removal Test ---\n");
    printf("Removing key 'charlie'...\n");
    if (bst_remove(bst, "charlie")) {
        printf("Successfully removed 'charlie'\n");
    }
    printf("In-order traversal after removal:\n");
    bst_traverse_inorder(bst, print_pair, NULL);

    printf("\n--- Final Statistics ---\n");
    printf("Final Size: %zu, Height: %d\n", bst_size(bst), bst_height(bst));

    bst_destroy(bst);
    printf("\nBST demonstration complete.\n");
    return 0;
}