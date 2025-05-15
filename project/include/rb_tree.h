
#ifndef RB_TREE_H
#define RB_TREE_H

#include "process.h"

#define RED 0
#define BLACK 1

enum rbtraversal {
    PREORDER,
    INORDER,
    POSTORDER
};

struct RBNode {
    Process process;
    bool is_red;
    RBNode* parent;
    RBNode* left;
    RBNode* right;
    
    RBNode(Process p) : process(p), is_red(true), parent(nullptr), left(nullptr), right(nullptr) {}
};

class RBTree {
private:
    RBNode* nil;
    RBNode* root;
    
    // Helper functions for balancing
    void rotateLeft(RBNode* x);
    void rotateRight(RBNode* y);
    void fixInsert(RBNode* node);
    void fixDelete(RBNode* node);
    
    // Helper for deletion
    void transplant(RBNode* u, RBNode* v);
    RBNode* minimum(RBNode* node);
    
    // Utility functions
    void destroyTree(RBNode* node);
    void printInorder(RBNode* node, int depth);
    void applyInorder(RBNode* node, int (*func)(Process&, void*), void* cookie);

public:
    RBTree();
    ~RBTree();
    
    // Core operations
    void insert(Process p);
    Process findMin();  // Leftmost node (smallest vruntime)
    bool remove(int pid);  // Remove process by pid
    RBNode* search(int pid);  // Find node by pid
    RBNode* searchHelper(RBNode* node, int pid);
    
    // Tree properties
    bool isEmpty();
    
    // Debug functions
    void print();
    int apply(int (*func)(Process&, void*), void* cookie);
    
    // Prevent copying
    RBTree(const RBTree&) = delete;
    RBTree& operator=(const RBTree&) = delete;
};

#endif // RB_TREE_H