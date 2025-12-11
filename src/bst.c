#include "bst.h"
#include <stdlib.h>

typedef struct node {
    void* key;
    void* value;
    struct node* left;
    struct node* right;
    struct node* parent;
} node_t;

struct bst {
    node_t* root;
    size_t size;
    bst_compare cmp;
    bst_free free_key;
    bst_free free_val;
};

static node_t* node_create(void* key, void* value) {
    node_t* n = (node_t*)malloc(sizeof(node_t));
    if (!n) return NULL;
    n->key = key;
    n->value = value;
    n->left = n->right = n->parent = NULL;
    return n;
}

static void free_node(struct bst* bst, node_t* n) {
    if (!n) return;
    if (bst->free_key) bst->free_key(n->key);
    if (bst->free_val) bst->free_val(n->value);
    free(n);
}

bst_t* bst_create(bst_compare cmp, bst_free free_key, bst_free free_val) {
    if (!cmp) return NULL;
    bst_t* bst = (bst_t*)malloc(sizeof(struct bst));
    if (!bst) return NULL;
    bst->root = NULL;
    bst->size = 0;
    bst->cmp = cmp;
    bst->free_key = free_key;
    bst->free_val = free_val;
    return bst;
}

static void clear_rec(struct bst* bst, node_t* n) {
    if (!n) return;
    clear_rec(bst, n->left);
    clear_rec(bst, n->right);
    free_node(bst, n);
}

void bst_destroy(bst_t* bst) {
    if (!bst) return;
    clear_rec(bst, bst->root);
    free(bst);
}

void bst_clear(bst_t* bst) {
    if (!bst) return;
    clear_rec(bst, bst->root);
    bst->root = NULL;
    bst->size = 0;
}

bool bst_insert(bst_t* bst, void* key, void* value) {
    if (!bst) return false;
    node_t* parent = NULL;
    node_t* cur = bst->root;
    while (cur) {
        parent = cur;
        int c = bst->cmp(key, cur->key);
        if (c == 0) {
            if (bst->free_val) bst->free_val(cur->value);
            cur->value = value;
            return true;
        } else if (c < 0) {
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    }
    node_t* n = node_create(key, value);
    if (!n) return false;
    n->parent = parent;
    if (!parent) {
        bst->root = n;
    } else if (bst->cmp(key, parent->key) < 0) {
        parent->left = n;
    } else {
        parent->right = n;
    }
    bst->size++;
    return true;
}

static node_t* find_node(const bst_t* bst, const void* key) {
    node_t* cur = bst->root;
    while (cur) {
        int c = bst->cmp(key, cur->key);
        if (c == 0) return cur;
        cur = (c < 0) ? cur->left : cur->right;
    }
    return NULL;
}

void* bst_get(const bst_t* bst, const void* key) {
    if (!bst) return NULL;
    node_t* n = find_node(bst, key);
    return n ? n->value : NULL;
}

bool bst_contains(const bst_t* bst, const void* key) {
    if (!bst) return false;
    return find_node(bst, key) != NULL;
}

static node_t* subtree_min(node_t* n) {
    if (!n) return NULL;
    while (n->left) n = n->left;
    return n;
}

static node_t* subtree_max(node_t* n) {
    if (!n) return NULL;
    while (n->right) n = n->right;
    return n;
}

static void transplant(struct bst* bst, node_t* u, node_t* v) {
    if (!u) return;
    if (!u->parent) {
        bst->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    if (v) v->parent = u->parent;
}

bool bst_remove(bst_t* bst, const void* key) {
    if (!bst) return false;
    node_t* z = find_node(bst, key);
    if (!z) return false;

    if (!z->left) {
        transplant(bst, z, z->right);
        free_node(bst, z);
    } else if (!z->right) {
        transplant(bst, z, z->left);
        free_node(bst, z);
    } else {
        node_t* y = subtree_min(z->right);
        if (y->parent != z) {
            transplant(bst, y, y->right);
            y->right = z->right;
            if (y->right) y->right->parent = y;
        }
        transplant(bst, z, y);
        y->left = z->left;
        if (y->left) y->left->parent = y;
        if (bst->free_key) bst->free_key(z->key);
        if (bst->free_val) bst->free_val(z->value);
        free(z);
    }
    bst->size--;
    return true;
}

void* bst_min_key(const bst_t* bst) {
    if (!bst || !bst->root) return NULL;
    node_t* m = subtree_min(bst->root);
    return m ? m->key : NULL;
}

void* bst_max_key(const bst_t* bst) {
    if (!bst || !bst->root) return NULL;
    node_t* m = subtree_max(bst->root);
    return m ? m->key : NULL;
}

void* bst_floor_key(const bst_t* bst, const void* key) {
    if (!bst || !bst->root) return NULL;
    node_t* cur = bst->root;
    node_t* floor = NULL;
    while (cur) {
        int c = bst->cmp(key, cur->key);
        if (c == 0) return cur->key;
        if (c < 0) {
            cur = cur->left;
        } else {
            floor = cur;
            cur = cur->right;
        }
    }
    return floor ? floor->key : NULL;
}

void* bst_ceiling_key(const bst_t* bst, const void* key) {
    if (!bst || !bst->root) return NULL;
    node_t* cur = bst->root;
    node_t* ceil = NULL;
    while (cur) {
        int c = bst->cmp(key, cur->key);
        if (c == 0) return cur->key;
        if (c < 0) {
            ceil = cur;
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    }
    return ceil ? ceil->key : NULL;
}

static node_t* successor_node(const bst_t* bst, const void* key) {
    node_t* n = find_node(bst, key);
    if (!n) return NULL;
    if (n->right) return subtree_min(n->right);
    node_t* p = n->parent;
    while (p && n == p->right) {
        n = p;
        p = p->parent;
    }
    return p;
}

static node_t* predecessor_node(const bst_t* bst, const void* key) {
    node_t* n = find_node(bst, key);
    if (!n) return NULL;
    if (n->left) return subtree_max(n->left);
    node_t* p = n->parent;
    while (p && n == p->left) {
        n = p;
        p = p->parent;
    }
    return p;
}

void* bst_successor_key(const bst_t* bst, const void* key) {
    node_t* s = successor_node(bst, key);
    return s ? s->key : NULL;
}

void* bst_predecessor_key(const bst_t* bst, const void* key) {
    node_t* p = predecessor_node(bst, key);
    return p ? p->key : NULL;
}

size_t bst_size(const bst_t* bst) {
    return bst ? bst->size : 0;
}

static int height_rec(const node_t* n) {
    if (!n) return -1;
    int hl = height_rec(n->left);
    int hr = height_rec(n->right);
    return (hl > hr ? hl : hr) + 1;
}

int bst_height(const bst_t* bst) {
    if (!bst) return -1;
    return height_rec(bst->root);
}

static void traverse_in(const node_t* n, bst_visit visit, void* user) {
    if (!n) return;
    traverse_in(n->left, visit, user);
    visit(n->key, (void*)n->value, user);
    traverse_in(n->right, visit, user);
}
static void traverse_pre(const node_t* n, bst_visit visit, void* user) {
    if (!n) return;
    visit(n->key, (void*)n->value, user);
    traverse_pre(n->left, visit, user);
    traverse_pre(n->right, visit, user);
}
static void traverse_post(const node_t* n, bst_visit visit, void* user) {
    if (!n) return;
    traverse_post(n->left, visit, user);
    traverse_post(n->right, visit, user);
    visit(n->key, (void*)n->value, user);
}

void bst_traverse_inorder(const bst_t* bst, bst_visit visit, void* user) {
    if (!bst || !visit) return;
    traverse_in(bst->root, visit, user);
}
void bst_traverse_preorder(const bst_t* bst, bst_visit visit, void* user) {
    if (!bst || !visit) return;
    traverse_pre(bst->root, visit, user);
}
void bst_traverse_postorder(const bst_t* bst, bst_visit visit, void* user) {
    if (!bst || !visit) return;
    traverse_post(bst->root, visit, user);
}

struct bst_iter {
    const bst_t* bst;
    const node_t* stack[64]; 
    int top;
    const node_t* current;
};

static void push(struct bst_iter* it, const node_t* n) {
    if (!n) return;
    if (it->top < (int)(sizeof(it->stack)/sizeof(it->stack[0]))) {
        it->stack[it->top++] = n;
    }
}
static const node_t* pop(struct bst_iter* it) {
    if (it->top == 0) return NULL;
    return it->stack[--it->top];
}

bst_iter_t* bst_iter_inorder_create(const bst_t* bst) {
    if (!bst) return NULL;
    struct bst_iter* it = (struct bst_iter*)malloc(sizeof(struct bst_iter));
    if (!it) return NULL;
    it->bst = bst;
    it->top = 0;
    it->current = bst->root;
    while (it->current) {
        push(it, it->current);
        it->current = it->current->left;
    }
    return (bst_iter_t*)it;
}

bool bst_iter_next(bst_iter_t* iter, const void** out_key, void** out_value) {
    if (!iter || !out_key || !out_value) return false;
    struct bst_iter* it = (struct bst_iter*)iter;
    const node_t* n = pop(it);
    if (!n) return false;
    *out_key = n->key;
    *out_value = n->value;

    const node_t* r = n->right;
    while (r) {
        push(it, r);
        r = r->left;
    }
    return true;
}

void bst_iter_destroy(bst_iter_t* iter) {
    free(iter);
}
