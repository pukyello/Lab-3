#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Tree {
    int data;
    Tree* left;
    Tree* right;
};

struct RBTree {
    int data;
    RBTree* left;
    RBTree* right;
    RBTree* parent;
    bool red;
};

struct Stack {
    Tree* tree;
    Stack* next;
};

struct RBStack {
    RBTree* tree;
    RBStack* next;
};

struct Queue {
    RBTree* tree;
    Queue* next;
};

void push(Stack*& top, Tree* tree) {
    Stack* new_node = new Stack;
    new_node->tree = tree;
    new_node->next = top;
    top = new_node;
}

Tree* pop(Stack*& top) {
    if (top == NULL) return NULL;
    Tree* result = top->tree;
    Stack* temp = top;
    top = top->next;
    delete temp;
    return result;
}

void pushRB(RBStack*& top, RBTree* tree) {
    RBStack* new_node = new RBStack;
    new_node->tree = tree;
    new_node->next = top;
    top = new_node;
}

RBTree* popRB(RBStack*& top) {
    if (top == NULL) return NULL;
    RBTree* result = top->tree;
    RBStack* temp = top;
    top = top->next;
    delete temp;
    return result;
}

void enqueue(Queue*& front, Queue*& rear, RBTree* tree) {
    Queue* new_node = new Queue;
    new_node->tree = tree;
    new_node->next = NULL;

    if (rear == NULL) {
        front = rear = new_node;
    }
    else {
        rear->next = new_node;
        rear = new_node;
    }
}

RBTree* dequeue(Queue*& front, Queue*& rear) {
    if (front == NULL) return NULL;

    RBTree* result = front->tree;
    Queue* temp = front;
    front = front->next;

    if (front == NULL) rear = NULL;

    delete temp;
    return result;
}

bool checkBrackets(string s) {
    int count = 0;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '(') count++;
        if (s[i] == ')') count--;
        if (count < 0) return false;
    }
    return count == 0;
}

Tree* parseTree(string s, int& pos, bool& valid) {
    while (pos < s.length() && s[pos] == ' ') pos++;

    if (pos >= s.length() || s[pos] != '(') {
        valid = false;
        return NULL;
    }
    pos++;

    while (pos < s.length() && s[pos] == ' ') pos++;

    if (pos >= s.length() || !(s[pos] == '-' || (s[pos] >= '0' && s[pos] <= '9'))) {
        valid = false;
        return NULL;
    }

    int num = 0;
    bool negative = false;

    if (s[pos] == '-') {
        negative = true;
        pos++;
    }

    while (pos < s.length() && s[pos] >= '0' && s[pos] <= '9') {
        num = num * 10 + (s[pos] - '0');
        pos++;
    }

    if (negative) num = -num;

    Tree* node = new Tree;
    node->data = num;
    node->left = NULL;
    node->right = NULL;

    int childCount = 0;
    while (pos < s.length() && s[pos] == ' ') pos++;

    while (pos < s.length() && s[pos] == '(') {
        if (childCount >= 2) {
            valid = false;
            return NULL;
        }
        if (childCount == 0) {
            node->left = parseTree(s, pos, valid);
            if (!valid) return NULL;
            childCount++;
        }
        else {
            node->right = parseTree(s, pos, valid);
            if (!valid) return NULL;
            childCount++;
        }
        while (pos < s.length() && s[pos] == ' ') pos++;
    }

    while (pos < s.length() && s[pos] == ' ') pos++;

    if (pos >= s.length() || s[pos] != ')') {
        valid = false;
        return NULL;
    }
    pos++;

    return node;
}

void preorderRecursive(Tree* root) {
    if (root == NULL) return;
    cout << root->data << " ";
    preorderRecursive(root->left);
    preorderRecursive(root->right);
}

void inorderRecursive(Tree* root) {
    if (root == NULL) return;
    inorderRecursive(root->left);
    cout << root->data << " ";
    inorderRecursive(root->right);
}

void postorderRecursive(Tree* root) {
    if (root == NULL) return;
    postorderRecursive(root->left);
    postorderRecursive(root->right);
    cout << root->data << " ";
}

void leftRotate(RBTree*& root, RBTree* x) {
    RBTree* y = x->right;
    x->right = y->left;

    if (y->left != NULL) y->left->parent = x;
    y->parent = x->parent;

    if (x->parent == NULL) root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void rightRotate(RBTree*& root, RBTree* y) {
    RBTree* x = y->left;
    y->left = x->right;

    if (x->right != NULL) x->right->parent = y;
    x->parent = y->parent;

    if (y->parent == NULL) root = x;
    else if (y == y->parent->right) y->parent->right = x;
    else y->parent->left = x;

    x->right = y;
    y->parent = x;
}

void fixTree(RBTree*& root, RBTree* node) {
    while (node != root && node->parent != NULL && node->parent->red) {
        if (node->parent == node->parent->parent->left) {
            RBTree* uncle = node->parent->parent->right;

            if (uncle != NULL && uncle->red) {
                node->parent->red = false;
                uncle->red = false;
                node->parent->parent->red = true;
                node = node->parent->parent;
            }
            else {
                if (node == node->parent->right) {
                    node = node->parent;
                    leftRotate(root, node);
                }
                node->parent->red = false;
                node->parent->parent->red = true;
                rightRotate(root, node->parent->parent);
            }
        }
        else {
            RBTree* uncle = node->parent->parent->left;

            if (uncle != NULL && uncle->red) {
                node->parent->red = false;
                uncle->red = false;
                node->parent->parent->red = true;
                node = node->parent->parent;
            }
            else {
                if (node == node->parent->left) {
                    node = node->parent;
                    rightRotate(root, node);
                }
                node->parent->red = false;
                node->parent->parent->red = true;
                leftRotate(root, node->parent->parent);
            }
        }
    }
    root->red = false;
}

RBTree* insertRB(RBTree* root, int value) {
    RBTree* new_node = new RBTree;
    new_node->data = value;
    new_node->red = true;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;

    if (root == NULL) {
        new_node->red = false;
        return new_node;
    }

    RBTree* parent = NULL;
    RBTree* current = root;

    while (current != NULL) {
        parent = current;
        if (value < current->data) current = current->left;
        else if (value > current->data) current = current->right;
        else {
            delete new_node;
            return root;
        }
    }

    new_node->parent = parent;
    if (value < parent->data) parent->left = new_node;
    else parent->right = new_node;

    if (new_node->parent->parent != NULL) {
        fixTree(root, new_node);
    }

    while (root->parent != NULL) {
        root = root->parent;
    }
    root->red = false;

    return root;
}

RBTree* findMin(RBTree* node) {
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

void transplant(RBTree*& root, RBTree* u, RBTree* v) {
    if (u->parent == NULL) {
        root = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else {
        u->parent->right = v;
    }
    if (v != NULL) {
        v->parent = u->parent;
    }
}

void fixDelete(RBTree*& root, RBTree* x) {
    while (x != root && (x == NULL || !x->red)) {
        if (x == x->parent->left) {
            RBTree* w = x->parent->right;

            if (w != NULL && w->red) {
                w->red = false;
                x->parent->red = true;
                leftRotate(root, x->parent);
                w = x->parent->right;
            }

            if (w != NULL &&
                (w->left == NULL || !w->left->red) &&
                (w->right == NULL || !w->right->red)) {
                if (w != NULL) w->red = true;
                x = x->parent;
            }
            else {
                if (w != NULL && (w->right == NULL || !w->right->red)) {
                    if (w->left != NULL) w->left->red = false;
                    if (w != NULL) w->red = true;
                    rightRotate(root, w);
                    w = x->parent->right;
                }

                if (w != NULL) {
                    w->red = x->parent->red;
                    x->parent->red = false;
                    if (w->right != NULL) w->right->red = false;
                    leftRotate(root, x->parent);
                }
                x = root;
            }
        }
        else {
            RBTree* w = x->parent->left;

            if (w != NULL && w->red) {
                w->red = false;
                x->parent->red = true;
                rightRotate(root, x->parent);
                w = x->parent->left;
            }

            if (w != NULL &&
                (w->right == NULL || !w->right->red) &&
                (w->left == NULL || !w->left->red)) {
                if (w != NULL) w->red = true;
                x = x->parent;
            }
            else {
                if (w != NULL && (w->left == NULL || !w->left->red)) {
                    if (w->right != NULL) w->right->red = false;
                    if (w != NULL) w->red = true;
                    leftRotate(root, w);
                    w = x->parent->left;
                }

                if (w != NULL) {
                    w->red = x->parent->red;
                    x->parent->red = false;
                    if (w->left != NULL) w->left->red = false;
                    rightRotate(root, x->parent);
                }
                x = root;
            }
        }
    }

    if (x != NULL) x->red = false;
}

RBTree* deleteRB(RBTree* root, int value) {
    if (root == NULL) return root;

    RBTree* node = root;
    RBTree* x = NULL;
    RBTree* y = NULL;
    bool yOriginalRed;

    while (node != NULL) {
        if (value == node->data) {
            break;
        }
        else if (value < node->data) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }

    if (node == NULL) return root;

    y = node;
    yOriginalRed = y->red;

    if (node->left == NULL) {
        x = node->right;
        transplant(root, node, node->right);
    }
    else if (node->right == NULL) {
        x = node->left;
        transplant(root, node, node->left);
    }
    else {
        y = findMin(node->right);
        yOriginalRed = y->red;
        x = y->right;

        if (y->parent == node) {
            if (x != NULL) x->parent = y;
        }
        else {
            transplant(root, y, y->right);
            y->right = node->right;
            if (y->right != NULL) y->right->parent = y;
        }

        transplant(root, node, y);
        y->left = node->left;
        if (y->left != NULL) y->left->parent = y;
        y->red = node->red;
    }

    delete node;

    if (!yOriginalRed) {
        if (x != NULL) {
            fixDelete(root, x);
        }
        else {
            RBTree* dummy = new RBTree;
            dummy->red = false;
            dummy->parent = (y->parent != NULL) ? y->parent : NULL;
            dummy->left = dummy->right = NULL;
            if (dummy->parent != NULL) {
                if (y->parent->left == NULL) dummy->parent->left = dummy;
                else if (y->parent->right == NULL) dummy->parent->right = dummy;
            }
            fixDelete(root, dummy);
            if (dummy->parent != NULL) {
                if (dummy->parent->left == dummy) dummy->parent->left = NULL;
                else if (dummy->parent->right == dummy) dummy->parent->right = NULL;
            }
            delete dummy;
        }
    }

    while (root != NULL && root->parent != NULL) {
        root = root->parent;
    }
    return root;
}

bool searchRB(RBTree* root, int value) {
    RBTree* current = root;
    while (current != NULL) {
        if (value == current->data) return true;
        else if (value < current->data) current = current->left;
        else current = current->right;
    }
    return false;
}

void BFS(RBTree* root) {
    if (root == NULL) return;

    Queue* front = NULL;
    Queue* rear = NULL;
    enqueue(front, rear, root);

    while (front != NULL) {
        RBTree* current = dequeue(front, rear);

        if (current->red) cout << current->data << "R ";
        else cout << current->data << "B ";

        if (current->left != NULL) enqueue(front, rear, current->left);
        if (current->right != NULL) enqueue(front, rear, current->right);
    }
}

void preorder(RBTree* root) {
    if (root == NULL) return;

    RBStack* stack = NULL;
    pushRB(stack, root);

    while (stack != NULL) {
        RBTree* current = popRB(stack);

        if (current->red) cout << current->data << "R ";
        else cout << current->data << "B ";

        if (current->right != NULL) pushRB(stack, current->right);
        if (current->left != NULL) pushRB(stack, current->left);
    }
}

void inorder(RBTree* root) {
    RBStack* stack = NULL;
    RBTree* current = root;

    while (current != NULL || stack != NULL) {
        while (current != NULL) {
            pushRB(stack, current);
            current = current->left;
        }

        current = popRB(stack);

        if (current->red) cout << current->data << "R ";
        else cout << current->data << "B ";

        current = current->right;
    }
}

void postorder(RBTree* root) {
    if (root == NULL) return;

    RBStack* s1 = NULL;
    RBStack* s2 = NULL;
    pushRB(s1, root);

    while (s1 != NULL) {
        RBTree* current = popRB(s1);
        pushRB(s2, current);

        if (current->left != NULL) pushRB(s1, current->left);
        if (current->right != NULL) pushRB(s1, current->right);
    }

    while (s2 != NULL) {
        RBTree* current = popRB(s2);

        if (current->red) cout << current->data << "R ";
        else cout << current->data << "B ";
    }
}

bool checkRBProperties(RBTree* root, int& blackCount, int currentBlack, bool& valid) {
    if (root == NULL) {
        if (blackCount == -1) blackCount = currentBlack;
        else if (currentBlack != blackCount) valid = false;
        return true;
    }

    if (root->red) {
        if ((root->left != NULL && root->left->red) ||
            (root->right != NULL && root->right->red)) {
            valid = false;
        }
    }

    int newBlack = currentBlack + (root->red ? 0 : 1);

    return checkRBProperties(root->left, blackCount, newBlack, valid) &&
        checkRBProperties(root->right, blackCount, newBlack, valid);
}

void validateRBTree(RBTree* root) {
    if (root == NULL) {
        cout << "Tree is empty" << endl;
        return;
    }

    if (root->red) {
        cout << "VIOLATION: Root is red!" << endl;
    }
    else {
        cout << "Property 1: Root is black - OK" << endl;
    }

    int blackCount = -1;
    bool valid = true;
    checkRBProperties(root, blackCount, 0, valid);

    if (valid) {
        cout << "Property 5: All paths have same number of black nodes (" << blackCount << ") - OK" << endl;
    }
    else {
        cout << "VIOLATION: Different number of black nodes in paths!" << endl;
    }

    valid = true;
    Queue* front = NULL;
    Queue* rear = NULL;
    enqueue(front, rear, root);

    while (front != NULL) {
        RBTree* current = dequeue(front, rear);

        if (current->red) {
            if ((current->left != NULL && current->left->red) ||
                (current->right != NULL && current->right->red)) {
                valid = false;
                cout << "VIOLATION: Red node " << current->data << " has red child!" << endl;
            }
        }

        if (current->left != NULL) enqueue(front, rear, current->left);
        if (current->right != NULL) enqueue(front, rear, current->right);
    }

    if (valid) {
        cout << "Property 3: Red nodes have only black children - OK" << endl;
    }
}

RBTree* makeRB(Tree* root) {
    if (root == NULL) return NULL;

    RBTree* rb_root = NULL;
    Stack* stack = NULL;
    push(stack, root);

    while (stack != NULL) {
        Tree* current = pop(stack);
        rb_root = insertRB(rb_root, current->data);

        if (current->right != NULL) push(stack, current->right);
        if (current->left != NULL) push(stack, current->left);
    }

    return rb_root;
}

void freeTree(Tree* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

void freeRBTree(RBTree* root) {
    if (root == NULL) return;
    freeRBTree(root->left);
    freeRBTree(root->right);
    delete root;
}

int main() {
    setlocale(LC_ALL, "Rus");
    cout << "=== Tree Parser and RB-Tree ===" << endl;

    ifstream file("mytree.txt");
    string input;
    if (file.is_open()) {
        getline(file, input);
        file.close();
        cout << "File read successfully" << endl;
    }
    else {
        input = "(5 (3 (2) (4)) (7 (6) (8)))";
        cout << "File not found, using default example: " << input << endl;
    }

    cout << "Input string: " << input << endl;

    if (!checkBrackets(input)) {
        cout << "Error: incorrect brackets" << endl;
        return 1;
    }

    int pos = 0;
    bool valid = true;
    Tree* my_tree = parseTree(input, pos, valid);

    if (!valid || my_tree == NULL) {
        cout << "Parsing error: not a binary tree or incorrect format" << endl;
        return 1;
    }

    cout << "\nRecursive traversals of the original tree:" << endl;
    cout << "Pre-order:  ";
    preorderRecursive(my_tree);
    cout << endl;

    cout << "In-order:   ";
    inorderRecursive(my_tree);
    cout << endl;

    cout << "Post-order: ";
    postorderRecursive(my_tree);
    cout << endl;

    cout << "\nBuilding Red-Black tree..." << endl;
    RBTree* rb_tree = makeRB(my_tree);

    cout << "\nRed-Black tree traversals:" << endl;
    cout << "1) BFS (breadth-first):      ";
    BFS(rb_tree);
    cout << endl;

    cout << "2) Pre-order:                ";
    preorder(rb_tree);
    cout << endl;

    cout << "3) In-order:                 ";
    inorder(rb_tree);
    cout << endl;

    cout << "4) Post-order:               ";
    postorder(rb_tree);
    cout << endl;

    cout << "\nValidating RB-tree properties..." << endl;
    validateRBTree(rb_tree);

    cout << "\nDemonstration of RB-tree operations:" << endl;
    if (my_tree) {
        int demoValue = my_tree->data;
        cout << "Search " << demoValue << ": " << (searchRB(rb_tree, demoValue) ? "found" : "not found") << endl;

        int testValue = 999;
        cout << "Search " << testValue << ": " << (searchRB(rb_tree, testValue) ? "found" : "not found") << endl;
    }

    cout << "\n=== Demonstration of deletion ===" << endl;
    if (rb_tree != NULL) {
        cout << "Original tree: ";
        BFS(rb_tree);
        cout << endl;

        int nodesToDelete[] = { 3, 7 };
        for (int i = 0; i < 2; i++) {
            int deleteValue = nodesToDelete[i];
            if (searchRB(rb_tree, deleteValue)) {
                cout << "\nDeleting node " << deleteValue << ":" << endl;
                rb_tree = deleteRB(rb_tree, deleteValue);
                cout << "Tree after deletion: ";
                BFS(rb_tree);
                cout << endl;

                cout << "Validation after deletion:" << endl;
                validateRBTree(rb_tree);
            }
            else {
                cout << "Node " << deleteValue << " not found for deletion" << endl;
            }
        }
    }

    cout << "\nNode:" << endl;
    cout << "NumberR - red nodes of the RB-tree" << endl;
    cout << "NumberB - black nodes of the RB-tree" << endl;

    freeTree(my_tree);
    freeRBTree(rb_tree);

    cout << "\nProgram completed. Press Enter to exit...";
    cin.get();

    return 0;
}
