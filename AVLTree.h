#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <memory>
#include <exception>
#include <iostream>
#include <functional>
#include <utility>

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

template <class T, class Key, class Compare = std::less<Key>>
class AVLTree {
public:
    struct KeyValuePair {
        Key key_m;
        T data_m;
    };

private:
    struct Node {
        T data_m;
        Key key_m;
        int height_m;
        std::unique_ptr<Node> left_m;
        std::unique_ptr<Node> right_m;

        // הפתרון: מקבלים T לפי ערך ומעבירים אותו (Move) פנימה כדי למנוע העתקה של שולחנות
        Node(const Key& k, T d) :
            data_m(std::move(d)), key_m(k), height_m(0), left_m(nullptr), right_m(nullptr) {}
    };

    std::unique_ptr<Node> root_m;
    int size_m;
    Compare comp_m;

    int getHeight(const std::unique_ptr<Node>& node) const {
        return node ? node->height_m : -1;
    }

    int getBalanceFactor(const std::unique_ptr<Node>& node) const {
        if (!node) return 0;
        return getHeight(node->left_m) - getHeight(node->right_m);
    }

    void updateHeight(const std::unique_ptr<Node>& node) {
        if (node) {
            int leftHeight = getHeight(node->left_m);
            int rightHeight = getHeight(node->right_m);
            node->height_m = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
        }
    }

    std::unique_ptr<Node> rightRotate(std::unique_ptr<Node> y) {
        std::unique_ptr<Node> x = std::move(y->left_m);
        std::unique_ptr<Node> T2 = std::move(x->right_m);
        y->left_m = std::move(T2);
        x->right_m = std::move(y);
        updateHeight(x->right_m);
        updateHeight(x);
        return x;
    }

    std::unique_ptr<Node> leftRotate(std::unique_ptr<Node> x) {
        std::unique_ptr<Node> y = std::move(x->right_m);
        std::unique_ptr<Node> T2 = std::move(y->left_m);
        x->right_m = std::move(T2);
        y->left_m = std::move(x);
        updateHeight(y->left_m);
        updateHeight(y);
        return y;
    }

    // הפתרון: שרשור של std::move כדי למנוע יצירת עותקים לא חוקיים בדרך לעלה
    std::unique_ptr<Node> insertNode(std::unique_ptr<Node> node, const Key& key, T data) {
        if (!node) {
            return std::unique_ptr<Node>(new Node(key, std::move(data)));
        }
        if (comp_m(key, node->key_m)) {
            node->left_m = insertNode(std::move(node->left_m), key, std::move(data));
        } else if (comp_m(node->key_m, key)) {
            node->right_m = insertNode(std::move(node->right_m), key, std::move(data));
        } else {
            throw ElementAlreadyExistsException();
        }

        updateHeight(node);
        int balance = getBalanceFactor(node);

        if (balance > 1 && comp_m(key, node->left_m->key_m))
            return rightRotate(std::move(node));
        if (balance < -1 && comp_m(node->right_m->key_m, key))
            return leftRotate(std::move(node));
        if (balance > 1 && comp_m(node->left_m->key_m, key)) {
            node->left_m = leftRotate(std::move(node->left_m));
            return rightRotate(std::move(node));
        }
        if (balance < -1 && comp_m(key, node->right_m->key_m)) {
            node->right_m = rightRotate(std::move(node->right_m));
            return leftRotate(std::move(node));
        }
        return node;
    }

    std::unique_ptr<Node> removeNode(std::unique_ptr<Node> node, const Key& key) {
        if (!node) throw ElementNotFoundException();

        if (comp_m(key, node->key_m)) {
            node->left_m = removeNode(std::move(node->left_m), key);
        } else if (comp_m(node->key_m, key)) {
            node->right_m = removeNode(std::move(node->right_m), key);
        } else {
            if (!node->left_m || !node->right_m) {
                std::unique_ptr<Node> temp = node->left_m ? std::move(node->left_m) : std::move(node->right_m);
                if (!temp) {
                    node.reset();
                    return nullptr;
                } else {
                    node = std::move(temp);
                }
            } else {
                Node* temp = node->right_m.get();
                while (temp->left_m) temp = temp->left_m.get();

                node->key_m = std::move(temp->key_m);
                node->data_m = std::move(temp->data_m);
                node->right_m = removeNode(std::move(node->right_m), node->key_m);
            }
        }

        if (!node) return node;

        updateHeight(node);
        int balance = getBalanceFactor(node);

        if (balance > 1 && getBalanceFactor(node->left_m) >= 0)
            return rightRotate(std::move(node));
        if (balance > 1 && getBalanceFactor(node->left_m) < 0) {
            node->left_m = leftRotate(std::move(node->left_m));
            return rightRotate(std::move(node));
        }
        if (balance < -1 && getBalanceFactor(node->right_m) <= 0)
            return leftRotate(std::move(node));
        if (balance < -1 && getBalanceFactor(node->right_m) > 0) {
            node->right_m = rightRotate(std::move(node->right_m));
            return leftRotate(std::move(node));
        }
        return node;
    }

    T& findNode(Node* node, const Key& key) {
        if (!node) throw ElementNotFoundException();
        if (comp_m(key, node->key_m)) return findNode(node->left_m.get(), key);
        else if (comp_m(node->key_m, key)) return findNode(node->right_m.get(), key);
        else return node->data_m;
    }

    void storeInArrayHelper(Node* node, KeyValuePair* arr, int& index) {
        if (!node) return;
        storeInArrayHelper(node->left_m.get(), arr, index);
        arr[index].key_m = node->key_m;
        arr[index].data_m = node->data_m;
        index++;
        storeInArrayHelper(node->right_m.get(), arr, index);
    }

    std::unique_ptr<Node> buildFromArrayHelper(KeyValuePair* arr, int start, int end) {
        if (start > end) return nullptr;
        int mid = start + (end - start) / 2;
        auto node = std::unique_ptr<Node>(new Node(arr[mid].key_m, arr[mid].data_m));
        node->left_m = buildFromArrayHelper(arr, start, mid - 1);
        node->right_m = buildFromArrayHelper(arr, mid + 1, end);
        updateHeight(node);
        return node;
    }

public:
    AVLTree() : root_m(nullptr), size_m(0) {}
    ~AVLTree() = default;

    AVLTree(AVLTree&&) = default;
    AVLTree& operator=(AVLTree&&) = default;

    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    int getSize() const { return size_m; }
    bool isEmpty() const { return size_m == 0; }

    void insert(const Key& key, T data) {
        root_m = insertNode(std::move(root_m), key, std::move(data));
        size_m++;
    }

    void remove(const Key& key) {
        root_m = removeNode(std::move(root_m), key);
        size_m--;
    }

    T& find(const Key& key) {
        return findNode(root_m.get(), key);
    }

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

    void storeInArray(KeyValuePair* arr) {
        int index = 0;
        storeInArrayHelper(root_m.get(), arr, index);
    }

    void buildFromArray(KeyValuePair* arr, int size) {
        root_m = buildFromArrayHelper(arr, 0, size - 1);
        size_m = size;
    }

    void clear() {
        root_m.reset();
        size_m = 0;
    }
};

#endif // AVL_TREE_H