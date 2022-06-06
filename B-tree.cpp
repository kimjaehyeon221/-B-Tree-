//알고리즘 2분반 과제 4 - 32200983 김재현 
//[B - Tree 알고리즘 구현하기]
//C++ 또는 Java 언어로 구현하되 소스 코드는 Github에 올리고 링크를 제출할 것.
//Minimum degree t = 5, 7, 9 값에 대해 0 - 9999 사이의 데이터 1000개를 삽입하고 500개를 삭제하는 예제를 실행하여 실행 시간을 분석할 것.

#include <iostream>
#include <stdio.h>
#include <stdlib.h> //srand, rand를 사용하기 위한 헤더파일
#include <time.h> // time을 사용하기 위한 헤더파일
using namespace std;

class Tree {
    int* keys;
    int t;
    Tree** C;
    int n;
    bool leaf_node;

public:
    Tree(int _t, bool _leaf_node);

    void traverse();

    int findKey(int k);
    void insert_more(int k);
    void splitChild(int i, Tree* y);
    void Deletenode(int k);
    void remove_leaf_node(int idx);
    void remove_nonleaf_node(int idx);
    int get_pre(int idx);
    int get_suc(int idx);
    void fill(int idx);
    void borrow_prev(int idx);
    void borrow_next(int idx);
    void merge(int idx);
    friend class BTree;
};

class BTree {
    Tree* root;
    int t;

public:
    BTree(int _t) {
        root = NULL;
        t = _t;
    }

    void traverse() {
        if (root != NULL)
            root->traverse();
    }

    void insertion(int k);

    void Deletenode(int k);
};

// B tree node
Tree::Tree(int t1, bool leaf_node1) {
    t = t1;
    leaf_node = leaf_node1;

    keys = new int[2 * t - 1];
    C = new Tree * [2 * t];

    n = 0;
}

// Find the key
int Tree::findKey(int k) {
    int idx = 0;
    while (idx < n && keys[idx] < k)
        ++idx;
    return idx;
}

// Deletenode operation
void Tree::Deletenode(int k) {
    int idx = findKey(k);

    if (idx < n && keys[idx] == k) {
        if (leaf_node)
            remove_leaf_node(idx);
        else
            remove_nonleaf_node(idx);
    }
    else {
        if (leaf_node) {
            //cout << "The key " << k << " is does not exist in the tree\n";
            return;
        }

        bool flag = ((idx == n) ? true : false);

        if (C[idx]->n < t)
            fill(idx);

        if (flag && idx > n)
            C[idx - 1]->Deletenode(k);
        else
            C[idx]->Deletenode(k);
    }
    return;
}

// Remove from the leaf_node
void Tree::remove_leaf_node(int idx) {
    for (int i = idx + 1; i < n; ++i)
        keys[i - 1] = keys[i];

    n--;

    return;
}

// Delete from non leaf_node node
void Tree::remove_nonleaf_node(int idx) {
    int k = keys[idx];

    if (C[idx]->n >= t) {
        int pred = get_pre(idx);
        keys[idx] = pred;
        C[idx]->Deletenode(pred);
    }

    else if (C[idx + 1]->n >= t) {
        int succ = get_suc(idx);
        keys[idx] = succ;
        C[idx + 1]->Deletenode(succ);
    }

    else {
        merge(idx);
        C[idx]->Deletenode(k);
    }
    return;
}

int Tree::get_pre(int idx) {
    Tree* cur = C[idx];
    while (!cur->leaf_node)
        cur = cur->C[cur->n];

    return cur->keys[cur->n - 1];
}

int Tree::get_suc(int idx) {
    Tree* cur = C[idx + 1];
    while (!cur->leaf_node)
        cur = cur->C[0];

    return cur->keys[0];
}

void Tree::fill(int idx) {
    if (idx != 0 && C[idx - 1]->n >= t)
        borrow_prev(idx);

    else if (idx != n && C[idx + 1]->n >= t)
        borrow_next(idx);

    else {
        if (idx != n)
            merge(idx);
        else
            merge(idx - 1);
    }
    return;
}

// Borrow from previous
void Tree::borrow_prev(int idx) {
    Tree* child = C[idx];
    Tree* sibling = C[idx - 1];

    for (int i = child->n - 1; i >= 0; i--)
        child->keys[i + 1] = child->keys[i];

    if (!child->leaf_node) {
        for (int i = child->n; i >= 0; i--)
            child->C[i + 1] = child->C[i];
    }

    child->keys[0] = keys[idx - 1];

    if (!child->leaf_node)
        child->C[0] = sibling->C[sibling->n];

    keys[idx - 1] = sibling->keys[sibling->n - 1];

    child->n += 1;
    sibling->n -= 1;

    return;
}

// Borrow from the next
void Tree::borrow_next(int idx) {
    Tree* child = C[idx];
    Tree* sibling = C[idx + 1];

    child->keys[(child->n)] = keys[idx];

    if (!(child->leaf_node))
        child->C[(child->n) + 1] = sibling->C[0];

    keys[idx] = sibling->keys[0];

    for (int i = 1; i < sibling->n; ++i)
        sibling->keys[i - 1] = sibling->keys[i];

    if (!sibling->leaf_node) {
        for (int i = 1; i <= sibling->n; ++i)
            sibling->C[i - 1] = sibling->C[i];
    }

    child->n += 1;
    sibling->n -= 1;

    return;
}

// Merge
void Tree::merge(int idx) {
    Tree* child = C[idx];
    Tree* sibling = C[idx + 1];

    child->keys[t - 1] = keys[idx];

    for (int i = 0; i < sibling->n; ++i)
        child->keys[i + t] = sibling->keys[i];

    if (!child->leaf_node) {
        for (int i = 0; i <= sibling->n; ++i)
            child->C[i + t] = sibling->C[i];
    }

    for (int i = idx + 1; i < n; ++i)
        keys[i - 1] = keys[i];

    for (int i = idx + 2; i <= n; ++i)
        C[i - 1] = C[i];

    child->n += sibling->n + 1;
    n--;

    delete (sibling);
    return;
}

// Insertion operation
void BTree::insertion(int k) {
    if (root == NULL) {
        root = new Tree(t, true);
        root->keys[0] = k;
        root->n = 1;
    }
    else {
        if (root->n == 2 * t - 1) {
            Tree* s = new Tree(t, false);

            s->C[0] = root;

            s->splitChild(0, root);

            int i = 0;
            if (s->keys[0] < k)
                i++;
            s->C[i]->insert_more(k);

            root = s;
        }
        else
            root->insert_more(k);
    }
}

// Insertion non full
void Tree::insert_more(int k) {
    int i = n - 1;

    if (leaf_node == true) {
        while (i >= 0 && keys[i] > k) {
            keys[i + 1] = keys[i];
            i --;
        }

        keys[i + 1] = k;
        n = n + 1;
    }
    else {
        while (i >= 0 && keys[i] > k)
            i--;

        if (C[i + 1]->n == 2 * t - 1) {
            splitChild(i + 1, C[i + 1]);

            if (keys[i + 1] < k)
                i++;
        }
        C[i + 1]->insert_more(k);
    }
}

// Split child
void Tree::splitChild(int i, Tree* y) {
    Tree* z = new Tree(y->t, y->leaf_node);
    z->n = t - 1;

    for (int j = 0; j < t - 1; j++)
        z->keys[j] = y->keys[j + t];

    if (y->leaf_node == false) {
        for (int j = 0; j < t; j++)
            z->C[j] = y->C[j + t];
    }

    y->n = t - 1;

    for (int j = n; j >= i + 1; j--)
        C[j + 1] = C[j];

    C[i + 1] = z;

    for (int j = n - 1; j >= i; j--)
        keys[j + 1] = keys[j];

    keys[i] = y->keys[t - 1];

    n = n + 1;
}

// Traverse
void Tree::traverse() {
    int i;
    for (i = 0; i < n; i++) {
        if (leaf_node == false)
            C[i]->traverse();
        cout << "  " << keys[i];
    }

    if (leaf_node == false)
        C[i]->traverse();
}

// Delete Operation
void BTree::Deletenode(int k) {
    if (!root) {
        cout << "The tree is empty\n";
        return;
    }

    root->Deletenode(k);

    if (root->n == 0) {
        Tree* tmp = root;
        if (root->leaf_node)
            root = NULL;
        else
            root = root->C[0];

        delete tmp;
    }
    return;
}

int main() {

    srand(time(NULL)); // 난수 초기화


    ///Minimum degree t = 5
    cout << "[Case 1 : Minimum degree t = 5]" << endl;
    clock_t start, end;
    double result;

    start = clock(); // 수행 시간 측정 시작

    BTree t(5);


    //0 - 9999 사이의 데이터 1000개를 삽입
    for (int i = 0; i < 1000; i++) {
        t.insertion(i);
    }

    cout << "0 - 9999 사이의 데이터 1000개를 삽입한 B - tree : ";
    t.traverse();


    //500개를 삭제
    for (int i = 0; i < 500; i++) {
        int rn = rand() % 1000;
        t.Deletenode(rn);
    }

    cout << "\n500개를 삭제를 삭제한 B - tree : ";
    t.traverse();

    end = clock(); //시간 측정 끝
    result = (double)(end - start);

    //실행 시간을 분석할 것.
    cout << "\n" << "result : " << ((result) / CLOCKS_PER_SEC) << " seconds" << endl;
    cout << endl;

    //Minimum degree t = 7
    cout << "[Case 2 : Minimum degree t = 7]" << endl;
    clock_t start1, end1;
    double result1;

    start1 = clock(); // 수행 시간 측정 시작

    BTree a(7);


    //0 - 9999 사이의 데이터 1000개를 삽입
    for (int i = 0; i < 1000; i++) {
        a.insertion(i);
    }

    cout << "0 - 9999 사이의 데이터 1000개를 삽입한 B - tree : ";
    a.traverse();


    //500개를 삭제
    for (int i = 0; i < 500; i++) {
        int rn = rand() % 1000;
        a.Deletenode(rn);
    }

    cout << "\n500개를 삭제를 삭제한 B - tree : ";
    a.traverse();

    end1 = clock(); //시간 측정 끝
    result1 = (double)(end1 - start1);

    //실행 시간을 분석할 것.
    cout << "\n" << "result : " << ((result1) / CLOCKS_PER_SEC) << " seconds" << endl;
    cout << endl;

    //Minimum degree t = 9
    cout << "[Case 3 : Minimum degree t = 9]" << endl;
    clock_t start2, end2;
    double result2;

    start2 = clock(); // 수행 시간 측정 시작

    BTree b(9);


    //0 - 9999 사이의 데이터 1000개를 삽입
    for (int i = 0; i < 1000; i++) {
        b.insertion(i);
    }

    cout << "0 - 9999 사이의 데이터 1000개를 삽입한 B - tree : ";
    b.traverse();


    //500개를 삭제
    for (int i = 0; i < 500; i++) {
        int rn = rand() % 1000;
        b.Deletenode(rn);
    }

    cout << "\n500개를 삭제를 삭제한 B - tree : ";
    b.traverse();

    end2 = clock(); //시간 측정 끝
    result2 = (double)(end2 - start2);

    //실행 시간을 분석할 것.
    cout << "\n" << "result : " << ((result2) / CLOCKS_PER_SEC) << " seconds" << endl;
    cout << endl;

}