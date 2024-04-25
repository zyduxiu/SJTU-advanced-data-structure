//
// Red Black Tree Definition
//

#ifndef RED_BLACK_TREE_RBTREE_H
#define RED_BLACK_TREE_RBTREE_H
#include "tree.hpp"
enum Color {RED, BLACK, DOUBLE_BLACK};

struct Node
{
    int data;
    int color;
    Node *left, *right, *parent;

    explicit Node(int);
    ~Node()
    {
        // 递归地删除左子树和右子树
//        delete left;
//        delete right;
//
//        // 注意：通常我们不会在Node的析构函数中删除parent，因为parent指针是父节点拥有的，
//        // 而不是当前节点。如果父节点被删除，它的析构函数将负责删除其指向的所有子节点。
//        // 但在某些特殊情况下，如果Node确实拥有parent的所有权，那么这里也需要删除parent。
//
//        // 将指针设置为nullptr是一个好习惯，可以防止悬挂指针
//        left = nullptr;
//        right = nullptr;
//        parent = nullptr;
    }
};

class RBTree:public Tree
{
private:
    Node *root;
protected:
    void rotateLeft(Node *&);
    void rotateRight(Node *&);
    void fixInsertRBTree(Node *&);
    void fixDeleteRBTree(Node *&);
    void inorderBST(Node *&);
    void preorderBST(Node *&);
    int getColor(Node *&);
    void setColor(Node *&, int);
    Node *minValueNode(Node *&);
    Node *maxValueNode(Node *&);
    Node* insertBST(Node *&, Node *&);
    Node* deleteBST(Node *&, int);
    int getBlackHeight(Node *);
public:
    ~RBTree(){delete root;};
    RBTree();
    Node* findHelper(Node* node, const int& value) {
        if (node == nullptr || node->data == value) {
            return node;
        }

        if (value < node->data) {
            return findHelper(node->left, value);
        } else {
            return findHelper(node->right, value);
        }
    }
    void insert(int);
    void remove(int);
    bool find(int key){
        return findHelper(root, key) != nullptr;
    }

    unsigned int size(){return 0;};
    unsigned int height(){return 0;};

    void inorder();
    void preorder();
};


#endif //RED_BLACK_TREE_RBTREE_H