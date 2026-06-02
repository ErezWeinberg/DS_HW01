#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <memory>
#include <exception>
#include <iostream>
#include <cassert>
#include <functional>

// Exception classes for tree operations
class ElementNotFoundException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Element not found in the tree.";
    }
};

class ElementAlreadyExistsException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Element already exists in the tree.";
    }
};

/**
 * Generic AVL Tree implementation.
 * T: The data type to store.
 * Key: The key type to sort by.
 * Compare: The comparison functor.
 */
template <class T, class Key, class Compare = std::less<Key>>
class AVLTree {
private:
    struct Node {
        T data_m;
        Key key_m;
        int height_m;
        std::unique_ptr<Node> left_m;
        std::unique_ptr<Node> right_m;

        Node(const Key& k, const T& d) : 
            data_m(d), key_m(k), height_m(0), left_m(nullptr), right_m(nullptr) {}
    };

    std::unique_ptr<Node> root_m;
    int size_m;
    Compare comp_m;

    // Helper functions for internal tree manipulation
    int getHeight(const std::unique_ptr<Node>& node) const {
        return node ? node->height_m : -1;
    }

    int getBalanceFactor(const std::unique_ptr<Node>& node) const {
        if (!node) {
            return 0;
        }
        return getHeight(node->left_m) - getHeight(node->right_m);
    }

    void updateHeight(const std::unique_ptr<Node>& node) {
        if (node) {
            int leftHeight = getHeight(node->left_m);
            int rightHeight = getHeight(node->right_m);
            node->height_m = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
        }
    }

    // Perform a right rotation
    std::unique_ptr<Node> rightRotate(std::unique_ptr<Node> y) {
        std::unique_ptr<Node> x = std::move(y->left_m);
        std::unique_ptr<Node> T2 = std::move(x->right_m);

        y->left_m = std::move(T2);
        x->right_m = std::move(y);

        updateHeight(x->right_m);
        updateHeight(x);

        return x;
    }

    // Perform a left rotation
    std::unique_ptr<Node> leftRotate(std::unique_ptr<Node> x) {
        std::unique_ptr<Node> y = std::move(x->right_m);
        std::unique_ptr<Node> T2 = std::move(y->left_m);

        x->right_m = std::move(T2);
        y->left_m = std::move(x);

        updateHeight(y->left_m);
        updateHeight(y);

        return y;
    }

    // Recursive insert helper
    std::unique_ptr<Node> insertNode(std::unique_ptr<Node> node, const Key& key, const T& data) {
        if (!node) {
            return std::make_unique<Node>(key, data);
        }

        if (comp_m(key, node->key_m)) {
            node->left_m = insertNode(std::move(node->left_m), key, data);
        } else if (comp_m(node->key_m, key)) {
            node->right_m = insertNode(std::move(node->right_m), key, data);
        } else {
            // Key already exists
            throw ElementAlreadyExistsException();
        }

        updateHeight(node);
        int balance = getBalanceFactor(node);

        // Left Left Case
        if (balance > 1 && comp_m(key, node->left_m->key_m)) {
            return rightRotate(std::move(node));
        }
        // Right Right Case
        if (balance < -1 && comp_m(node->right_m->key_m, key)) {
            return leftRotate(std::move(node));
        }
        // Left Right Case
        if (balance > 1 && comp_m(node->left_m->key_m, key)) {
            node->left_m = leftRotate(std::move(node->left_m));
            return rightRotate(std::move(node));
        }
        // Right Left Case
        if (balance < -1 && comp_m(key, node->right_m->key_m)) {
            node->right_m = rightRotate(std::move(node->right_m));
            return leftRotate(std::move(node));
        }

        return node;
    }
    // Recursive remove helper
    std::unique_ptr<Node> removeNode(std::unique_ptr<Node> node, const Key& key) {
        if (!node) {
            throw ElementNotFoundException();
        }

        if (comp_m(key, node->key_m)) {
            node->left_m = removeNode(std::move(node->left_m), key);
        } else if (comp_m(node->key_m, key)) {
            node->right_m = removeNode(std::move(node->right_m), key);
        } else {
            // Node found!
            if (!node->left_m || !node->right_m) {
                std::unique_ptr<Node> temp = node->left_m ? std::move(node->left_m) : std::move(node->right_m);
                if (!temp) {
                    node.reset();
                    return nullptr;
                } else {
                    node = std::move(temp);
                }
            } else {
                // Node with two children. Get inorder successor
                Node* temp = node->right_m.get();
                while (temp->left_m) {
                    temp = temp->left_m.get();
                }
                // Copy the successor's data to this node
                node->key_m = temp->key_m;
                node->data_m = temp->data_m;
                // Remove the successor
                node->right_m = removeNode(std::move(node->right_m), temp->key_m);
            }
        }

        if (!node) {
            return node;
        }

        // Update height and rebalance
        updateHeight(node);
        int balance = getBalanceFactor(node);

        if (balance > 1 && getBalanceFactor(node->left_m) >= 0) {
            return rightRotate(std::move(node));
        }
        if (balance > 1 && getBalanceFactor(node->left_m) < 0) {
            node->left_m = leftRotate(std::move(node->left_m));
            return rightRotate(std::move(node));
        }
        if (balance < -1 && getBalanceFactor(node->right_m) <= 0) {
            return leftRotate(std::move(node));
        }
        if (balance < -1 && getBalanceFactor(node->right_m) > 0) {
            node->right_m = rightRotate(std::move(node->right_m));
            return leftRotate(std::move(node));
        }

        return node;
    }

    // Helper for fast search
    T& findNode(Node* node, const Key& key) {
        if (!node) {
            throw ElementNotFoundException();
        }
        if (comp_m(key, node->key_m)) {
            return findNode(node->left_m.get(), key);
        } else if (comp_m(node->key_m, key)) {
            return findNode(node->right_m.get(), key);
        } else {
            return node->data_m;
        }
    }
    
public:
    AVLTree() : root_m(nullptr), size_m(0) {}
    
    // Rule of Zero: unique_ptr handles all memory cleanup automatically!
    ~AVLTree() = default;

    // Prevent copy and assignment to avoid accidental deep copies or leaks
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    int getSize() const {
        return size_m;
    }

    bool isEmpty() const {
        return size_m == 0;
    }
    // insert function
    void insert(const Key& key, const T& data) {
        root_m = insertNode(std::move(root_m), key, data);
        size_m++;
    }
    // Public remove function
    void remove(const Key& key) {
        root_m = removeNode(std::move(root_m), key);
        size_m--;
    }

    // Public find function
    T& find(const Key& key) {
        return findNode(root_m.get(), key);
    }
    // Find the in-order predecessor for a given key
    T* findPredecessor(const Key& key) {
        Node* current = root_m.get();
        Node* predecessor = nullptr;

        while (current) {
            if (comp_m(current->key_m, key)) {
                predecessor = current;
                current = current->right_m.get();
            } else {
                current = current->left_m.get();
            }
        }

        return predecessor ? &(predecessor->data_m) : nullptr;
    }
    // Interface to be implemented
    void insert(const Key& key, const T& data);
    void remove(const Key& key);
    T& find(const Key& key);
};

#endif // AVL_TREE_H