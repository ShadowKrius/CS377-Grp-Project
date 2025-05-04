
#include "rb_tree.h"
#include <iostream>
#include <iomanip>

RBTree::RBTree() {
    // Create nil node
    nil = new RBNode(Process());
    nil->is_red = false;
    nil->left = nil->right = nil->parent = nil;
    
    // Create root
    root = nil;
}

RBTree::~RBTree() {
    destroyTree(root);
    delete nil;
}

void RBTree::rotateLeft(RBNode* x) {
    RBNode* y = x->right;
    
    // Turn y's left subtree into x's right subtree
    x->right = y->left;
    if (y->left != nil) {
        y->left->parent = x;
    }
    
    // Link x's parent to y
    y->parent = x->parent;
    if (x->parent == nil) {
        root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    
    // Put x on y's left
    y->left = x;
    x->parent = y;
}

void RBTree::rotateRight(RBNode* y) {
    RBNode* x = y->left;
    
    // Turn x's right subtree into y's left subtree
    y->left = x->right;
    if (x->right != nil) {
        x->right->parent = y;
    }
    
    // Link y's parent to x
    x->parent = y->parent;
    if (y->parent == nil) {
        root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    
    // Put y on x's right
    x->right = y;
    y->parent = x;
}

void RBTree::insert(Process p) {
    RBNode* z = new RBNode(p);
    RBNode* y = nil;
    RBNode* x = root;
    
    z->left = nil;
    z->right = nil;
    
    // Standard BST insertion
    while (x != nil) {
        y = x;
        if (z->process.vruntime < x->process.vruntime) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    
    z->parent = y;
    if (y == nil) {
        root = z;  // Tree was empty
    } else if (z->process.vruntime < y->process.vruntime) {
        y->left = z;
    } else {
        y->right = z;
    }
    
    z->is_red = true;
    fixInsert(z);
}

void RBTree::fixInsert(RBNode* z) {
    while (z->parent->is_red) {
        if (z->parent == z->parent->parent->left) {
            RBNode* y = z->parent->parent->right;
            if (y->is_red) {
                // Case 1: Uncle is red
                z->parent->is_red = false;
                y->is_red = false;
                z->parent->parent->is_red = true;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    // Case 2: Uncle is black, z is right child
                    z = z->parent;
                    rotateLeft(z);
                }
                // Case 3: Uncle is black, z is left child
                z->parent->is_red = false;
                z->parent->parent->is_red = true;
                rotateRight(z->parent->parent);
            }
        } else {
            RBNode* y = z->parent->parent->left;
            if (y->is_red) {
                // Mirror Case 1
                z->parent->is_red = false;
                y->is_red = false;
                z->parent->parent->is_red = true;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    // Mirror Case 2
                    z = z->parent;
                    rotateRight(z);
                }
                // Mirror Case 3
                z->parent->is_red = false;
                z->parent->parent->is_red = true;
                rotateLeft(z->parent->parent);
            }
        }
    }
    root->is_red = false;
}

Process RBTree::findMin() {
    if (root == nil) {
        return Process();  // Return empty process if tree is empty
    }
    
    RBNode* current = root;
    while (current->left != nil) {
        current = current->left;
    }
    
    return current->process;
}

RBNode* RBTree::search(int pid) {
    RBNode* current = root;
    
    while (current != nil) {
        if (pid == current->process.pid) {
            return current;
        } else if (pid < current->process.pid) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    return nil;
}

void RBTree::transplant(RBNode* u, RBNode* v) {
    if (u->parent == nil) {
        root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

RBNode* RBTree::minimum(RBNode* node) {
    while (node->left != nil) {
        node = node->left;
    }
    return node;
}

bool RBTree::remove(int pid) {
    RBNode* z = search(pid);
    if (z == nil) {
        return false;  // Process not found
    }
    
    RBNode* y = z;
    RBNode* x;
    bool y_original_is_red = y->is_red;
    
    if (z->left == nil) {
        x = z->right;
        transplant(z, z->right);
    } else if (z->right == nil) {
        x = z->left;
        transplant(z, z->left);
    } else {
        y = minimum(z->right);
        y_original_is_red = y->is_red;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->is_red = z->is_red;
    }
    
    delete z;
    
    if (!y_original_is_red) {
        fixDelete(x);
    }
    
    return true;
}

void RBTree::fixDelete(RBNode* x) {
    while (x != root && !x->is_red) {
        if (x == x->parent->left) {
            RBNode* w = x->parent->right;
            if (w->is_red) {
                // Case 1: x's sibling w is red
                w->is_red = false;
                x->parent->is_red = true;
                rotateLeft(x->parent);
                w = x->parent->right;
            }
            if (!w->left->is_red && !w->right->is_red) {
                // Case 2: x's sibling w is black, and both of w's children are black
                w->is_red = true;
                x = x->parent;
            } else {
                if (!w->right->is_red) {
                    // Case 3: x's sibling w is black, w's left child is red, w's right child is black
                    w->left->is_red = false;
                    w->is_red = true;
                    rotateRight(w);
                    w = x->parent->right;
                }
                // Case 4: x's sibling w is black, and w's right child is red
                w->is_red = x->parent->is_red;
                x->parent->is_red = false;
                w->right->is_red = false;
                rotateLeft(x->parent);
                x = root;
            }
        } else {
            // Mirror cases
            RBNode* w = x->parent->left;
            if (w->is_red) {
                w->is_red = false;
                x->parent->is_red = true;
                rotateRight(x->parent);
                w = x->parent->left;
            }
            if (!w->right->is_red && !w->left->is_red) {
                w->is_red = true;
                x = x->parent;
            } else {
                if (!w->left->is_red) {
                    w->right->is_red = false;
                    w->is_red = true;
                    rotateLeft(w);
                    w = x->parent->left;
                }
                w->is_red = x->parent->is_red;
                x->parent->is_red = false;
                w->left->is_red = false;
                rotateRight(x->parent);
                x = root;
            }
        }
    }
    x->is_red = false;
}

bool RBTree::isEmpty() {
    return root == nil;
}

void RBTree::destroyTree(RBNode* node) {
    if (node != nil) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

void RBTree::print() {
    if (root == nil) {
        std::cout << "Tree is empty" << std::endl;
        return;
    }
    
    std::cout << "Red-Black Tree (inorder):" << std::endl;
    printInorder(root, 0);
    std::cout << std::endl;
}

void RBTree::printInorder(RBNode* node, int depth) {
    if (node != nil) {
        printInorder(node->right, depth + 1);
        
        std::cout << std::setw(4 * depth) << "";
        std::cout << "PID: " << node->process.pid 
                  << " VT: " << node->process.vruntime 
                  << " (" << (node->is_red ? "RED" : "BLACK") << ")" << std::endl;
        
        printInorder(node->left, depth + 1);
    }
}

int RBTree::apply(int (*func)(Process&, void*), void* cookie) {
    if (root == nil) {
        return 0;
    }
    
    applyInorder(root, func, cookie);
    return 0;
}

void RBTree::applyInorder(RBNode* node, int (*func)(Process&, void*), void* cookie) {
    if (node != nil) {
        applyInorder(node->left, func, cookie);
        func(node->process, cookie);
        applyInorder(node->right, func, cookie);
    }
}