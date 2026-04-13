#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// BST node
typedef struct bst_node {
    uint32_t value;
    struct bst_node *left;
    struct bst_node *right;
} bst_node_t;

// function declarations
bst_node_t *bst_create_node(uint32_t value);
bst_node_t *bst_insert(bst_node_t *root, uint32_t value);
bst_node_t *bst_search(bst_node_t *root, uint32_t value);
bst_node_t *bst_find_min(bst_node_t *root);
void bst_inorder(bst_node_t *root);
void bst_free(bst_node_t *root);
int bst_height(bst_node_t *root);

int main(void)
{
    bst_node_t *root = NULL;

    printf("=== insert values ===\n");
    uint32_t values[] = {50, 30, 70, 20, 40, 60, 80};
    int len = 7;
    for (int i = 0; i < len; i++) {
        root = bst_insert(root, values[i]);
        printf("inserted %u\n", values[i]);
    }

    printf("\n=== inorder traversal (sorted) ===\n");
    bst_inorder(root);
    printf("\n");

    printf("\n=== search ===\n");
    uint32_t targets[] = {40, 99};
    for (int i = 0; i < 2; i++) {
        bst_node_t *found = bst_search(root, targets[i]);
        if(found) {
            printf("found %u\n", found->value);
        } else {
            printf("%u not found\n", targets[i]);
        }
    }

    printf("\n=== min value ===\n");
    bst_node_t *min = bst_find_min(root);
    if (min) printf("min = %u\n", min->value);

    printf("\n=== height ===\n");
    printf("height = %d\n", bst_height(root));

    bst_free(root);
    root = NULL;

    return 0;
}

bst_node_t *bst_create_node(uint32_t value)
{
    bst_node_t *n = malloc(sizeof(bst_node_t));
    if (n == NULL) return NULL;
    n->value = value;
    n->left = NULL;
    n->right = NULL;
    return n;
}

bst_node_t *bst_insert(bst_node_t *root, uint32_t value)
{
    if (root == NULL) return bst_create_node(value);
    if (value < root->value) {
        root->left = bst_insert(root->left, value);
    } else if (value > root->value) {
        root->right = bst_insert(root->right, value);
    }
    return root;
}

bst_node_t *bst_search(bst_node_t *root, uint32_t value)
{
    if (root == NULL) return NULL;
    if (value == root->value) return root;
    if (value < root->value) return bst_search(root->left, value);
    return  bst_search(root->right, value);
}

bst_node_t *bst_find_min(bst_node_t *root)
{
    if (root == NULL) return NULL;
    if (root->left == NULL) return root;
    return bst_find_min(root->left);
}

void bst_inorder(bst_node_t *root)
{
    if (root == NULL) return;
    bst_inorder(root->left);
    printf("%u ", root->value);
    bst_inorder(root->right);
}

void bst_free(bst_node_t *root)
{
    if (root == NULL) return;
    bst_free(root->left);
    bst_free(root->right);
    free(root);
}

int bst_height(bst_node_t *root)
{
    if (root == NULL) return 0;
    int left_h = bst_height(root->left);
    int right_h = bst_height(root->right);
    return 1 + (left_h > right_h ? left_h : right_h);
}
