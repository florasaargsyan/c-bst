# C-BST

A simple, unbalanced Binary Search Tree implementation in C99.

## What is this?

C-BST is a straightforward binary search tree library that stores key-value pairs. It works with any data type through function pointers - you provide comparison and memory management functions, and the tree handles the rest.

## Binary Search Tree Basics

A BST organizes data where each node has up to two children. The rule is simple: left children are smaller, right children are larger.

```
         delta(4)
        /        \
     beta(2)   gamma(3)
      /           \
  alpha(1)     epsilon(5)
```

This structure makes searching efficient - each comparison eliminates roughly half the remaining data, giving O(log n) average performance.

## How Insertion Works

Let's insert: delta, beta, alpha, gamma, epsilon

```
Insert "delta":
    delta

Insert "beta" (beta < delta):
    delta
    /
  beta

Insert "alpha" (alpha < beta):
      delta
      /
    beta
    /
  alpha

Insert "gamma" (gamma > delta):
      delta
      /    \
    beta   gamma
    /
  alpha

Insert "epsilon" (epsilon > delta, but < gamma):
        delta
       /     \
     beta    gamma
     /       /
  alpha  epsilon
```

In-order traversal (left-root-right) gives sorted output: alpha, beta, delta, epsilon, gamma.

## Features

- Works with any data type via function pointers
- Supports min, max, floor, ceiling, successor, predecessor operations
- Three traversal modes: in-order, pre-order, post-order
- Non-recursive iterator for memory-efficient traversal
- Pure C99 with no external dependencies
- Comprehensive test suite included

## Quick Start

```bash
git clone https://github.com/florasaargsyan/c-bst.git

cd c-bst
make
./tests/test_bst.exe    # Run tests
./examples/main.exe     # Run example
```

## Basic Usage

**1. Define a comparison function:**

```c
int cmp_str(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}
```

Return negative if `a < b`, zero if equal, positive if `a > b`.

**2. Create and use the tree:**

```c
bst_t* tree = bst_create(cmp_str, free, free);
bst_insert(tree, strdup("apple"), strdup("red"));
bst_insert(tree, strdup("banana"), strdup("yellow"));

char* color = (char*)bst_get(tree, "apple");
printf("apple is %s\n", color);  // apple is red

bst_destroy(tree);  // Frees all keys and values
```

## Core Operations

**Insert:** Adds a key-value pair. If the key already exists, the value is updated.

**Get:** Retrieves the value for a key. Returns NULL if not found.

**Contains:** Checks if a key exists without retrieving the value.

**Remove:** Deletes a key from the tree. Three cases:
- Leaf node: simply remove it
- One child: replace node with its child
- Two children: replace with in-order successor, then remove the successor

**Size:** Returns the number of nodes in the tree.

**Height:** Returns the longest path from root to leaf (-1 for an empty tree).

## Advanced Queries

**Min/Max:** The leftmost node is minimum, rightmost is maximum.

**Floor:** Largest key less than or equal to the given key.

**Ceiling:** Smallest key greater than or equal to the given key.

Example: Tree contains "alpha", "bravo", "delta", "echo"
- Floor of "charlie" → "bravo" (largest ≤ "charlie")
- Ceiling of "charlie" → "delta" (smallest ≥ "charlie")

**Successor:** Next larger key in sorted order.

**Predecessor:** Next smaller key in sorted order.

## Traversals

```
       delta
      /     \
   beta    gamma
    /       /
 alpha  epsilon
```

**In-order (Left-Root-Right):** alpha, beta, delta, epsilon, gamma  
Returns elements in sorted order. Most common traversal.

**Pre-order (Root-Left-Right):** delta, beta, alpha, gamma, epsilon  
Useful for copying the tree structure.

**Post-order (Left-Right-Root):** alpha, epsilon, gamma, beta, delta  
Useful for safe deletion (children freed before parents).

## Iterator

Walk through the tree without recursion:

```c
bst_iter_t* it = bst_iter_inorder_create(tree);
const void* key;
void* value;

while (bst_iter_next(it, &key, &value)) {
    printf("%s => %s\n", (const char*)key, (char*)value);
}

bst_iter_destroy(it);
```

The iterator maintains an internal stack and yields elements in sorted order.

## Performance

**Average case:** O(log n) for insert, search, delete  
**Worst case:** O(n) when the tree becomes unbalanced

The tree becomes unbalanced when you insert sorted data:

```
Balanced:                   Unbalanced (sorted insert):
    4                           1
   / \                           \
  2   6                           2
 / \ / \                           \
1  3 5  7                           3
                                     \
Height: log(n)                        4
Fast: O(log n)                         \
                                        5
                                    
                                    Height: n
                                    Slow: O(n)
```

**To avoid this:** Insert data in random order when possible.

## When to Use This

**Good fit:**
- You need sorted key-value storage
- Data arrives in random or semi-random order
- You need range queries (floor, ceiling, successor)
- Simple, readable code is important

**Not a good fit:**
- Data arrives pre-sorted (tree will degrade to a linked list)
- You need thread-safe operations
- You require guaranteed O(log n) worst-case performance

## Examples

### String Dictionary

```c
int cmp_str(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}

bst_t* dict = bst_create(cmp_str, free, free);
bst_insert(dict, strdup("cat"), strdup("meow"));
bst_insert(dict, strdup("dog"), strdup("woof"));

printf("cat says %s\n", (char*)bst_get(dict, "cat"));

bst_destroy(dict);
```

### Integer Keys

```c
int cmp_int(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

bst_t* tree = bst_create(cmp_int, free, free);

int* key = malloc(sizeof(int)); *key = 42;
int* val = malloc(sizeof(int)); *val = 100;
bst_insert(tree, key, val);

int search = 42;
if (bst_contains(tree, &search)) {
    printf("found 42\n");
}

bst_destroy(tree);
```

### Traversal with Visitor Function

```c
void print_entry(const void* key, void* value, void* user_data) {
    printf("%s => %d\n", (const char*)key, *(int*)value);
}

bst_traverse_inorder(tree, print_entry, NULL);
```

The `user_data` parameter lets you pass additional context to your visitor function.

## Memory Management

The tree owns the keys and values you pass to `bst_insert()`. When you call `bst_destroy()` or `bst_remove()`, it uses the free functions you provided during creation.

```c
bst_t* tree = bst_create(cmp_str, free, free);  // Will call free() on keys and values
```

If you manage memory yourself, pass NULL:

```c
bst_t* tree = bst_create(cmp_str, NULL, NULL);  // No automatic freeing
```

**Important caveat:** When inserting a duplicate key, the old value is freed but the new key pointer is leaked. Workaround:

```c
char* k = strdup("existing_key");
if (bst_contains(tree, "existing_key")) {
    free(k);  // Key exists, free the duplicate
} else {
    bst_insert(tree, k, new_value);
}
```

## Building

```bash
make              # Build library and programs
make clean        # Remove build artifacts
make test         # Build and run tests
make example      # Build and run example
```

Compiler flags: `-std=c99 -O2 -Wall -Wextra -Wpedantic`

## Integration

**Using the static library:**
```bash
gcc your_code.c libcbst.a -Iinclude -o your_program
```

**Compiling source directly:**
```bash
gcc your_code.c src/bst.c -Iinclude -o your_program
```

## Testing

Run the test suite with `make test && ./tests/test_bst.exe`

The suite includes 15 tests covering:
- Tree creation and destruction
- Insertion (single, multiple, duplicates)
- All query operations (min, max, floor, ceiling, etc.)
- Removal (leaf nodes, one child, two children)
- Iterator functionality
- Edge cases and NULL handling
- Both string and integer keys

Expected output:
```
=== BST Comprehensive Test Suite ===

Test 1: BST creation and destruction
Test 2: Single insertion and retrieval
...
Test 15: Edge cases - NULL handling

=== All 15 tests passed! ===
```

## Project Structure

```
c-bst/
├── include/bst.h       # Public API
├── src/bst.c           # Implementation
├── examples/main.c     # Usage examples
├── tests/test_bst.c    # Test suite
├── Makefile            # Build configuration
└── README.md           # This file
```

## API Reference

**Tree Management**  
`bst_create(cmp, free_key, free_val)` - Create a new tree  
`bst_destroy(bst)` - Free all nodes and the tree  
`bst_clear(bst)` - Remove all nodes, keep the tree structure

**Basic Operations**  
`bst_insert(bst, key, value)` - Insert or update a key-value pair  
`bst_get(bst, key)` - Get value for key  
`bst_contains(bst, key)` - Check if key exists  
`bst_remove(bst, key)` - Remove key from tree

**Queries**  
`bst_min_key(bst)` - Get minimum key  
`bst_max_key(bst)` - Get maximum key  
`bst_floor_key(bst, key)` - Largest key ≤ given key  
`bst_ceiling_key(bst, key)` - Smallest key ≥ given key  
`bst_successor_key(bst, key)` - Next larger key  
`bst_predecessor_key(bst, key)` - Next smaller key

**Properties**  
`bst_size(bst)` - Number of nodes  
`bst_height(bst)` - Tree height

**Traversals**  
`bst_traverse_inorder(bst, visit, user_data)` - Visit in sorted order  
`bst_traverse_preorder(bst, visit, user_data)` - Visit root first  
`bst_traverse_postorder(bst, visit, user_data)` - Visit root last

**Iterator**  
`bst_iter_inorder_create(bst)` - Create iterator  
`bst_iter_next(it, key, value)` - Get next element  
`bst_iter_destroy(it)` - Free iterator

See `include/bst.h` for detailed function signatures and documentation.

## Limitations

- Not thread-safe (use your own synchronization if needed)
- No automatic balancing (can degrade to O(n) with sorted data)
- Duplicate keys update values instead of creating new nodes
- Iterator limited to tree depth of 64 (configurable in source)
- No built-in serialization

## Requirements

- C99 or later
- No external dependencies
- Works with GCC, Clang, MSVC
- Tested on Windows, Linux, macOS

## License

MIT License - free to use in any project.

## Author

Flora Sargsyan  
GitHub: [@florasaargsyan](https://github.com/florasaargsyan)