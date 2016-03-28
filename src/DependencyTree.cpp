#include "DependencyTree.h"
#include "Config.h"

#include <iostream>

using namespace std;

DependencyTree::DependencyTree()
{
    init();
}

void DependencyTree::init()
{
    n = 0;
    heads.clear();
    labels.clear();
    proj_order.clear();
    mpc.clear();

    heads.push_back(Config::NONEXIST);
    labels.push_back(Config::UNKNOWN);
}

DependencyTree::DependencyTree(const DependencyTree& tree)
{
    n = tree.n;
    heads = tree.heads;
    labels = tree.labels;
    proj_order = tree.proj_order;
    mpc = tree.mpc;
}

void DependencyTree::add(int h, const string & l)
{
    ++n;
    heads.push_back(h);
    labels.push_back(l);
}

void DependencyTree::set(int k, int h, const string & l)
{
    heads[k] = h;
    labels[k] = l;
}

int DependencyTree::get_head(int k)
{
    if (k <= 0 || k > n)
        return Config::NONEXIST;
    return heads[k];
}

const string & DependencyTree::get_label(int k)
{
    if (k <= 0 || k > n)
        return Config::NIL;
    return labels[k];
}

int DependencyTree::get_root()
{
    for (int k = 1; k <= n; ++k)
        if (get_head(k) == 0)
            return k;
    return 0; // non tree
}

bool DependencyTree::is_single_root()
{
    int roots = 0;
    for (int k = 1; k <= n; ++k)
        if (get_head(k) == 0)
            roots += 1;
    return (roots == 1);
}

/**
 * Checking if the tree is legal, O(n)
 */
bool DependencyTree::is_tree()
{
    vector<int> h;
    h.push_back(-1);
    for (int k = 1; k <= n; ++k)
    {
        if (get_head(k) < 0 || get_head(k) > n)
            return false;
        h.push_back(-1);
    }
    for (int k = 1; k <= n; ++k)
    {
        int i = k;
        while (i > 0)
        {
            if (h[i] >= 0 && h[i] < k)
                break;
            if (h[i] == k)
                return false;
            h[i] = k;
            i = get_head(i);
        }
    }
    return true;
}

bool DependencyTree::is_projective()
{
    if (!is_tree())
        return false;

    counter = -1;
    return visit_tree(0);
}

bool DependencyTree::visit_tree(int w)
{
    for (int k = 1; k < w; ++k)
        if (get_head(k) == w && visit_tree(k) == false)
            return false;
    counter += 1;
    if (w != counter)
        return false;
    for (int k = w + 1; k <= n; ++k)
        if (get_head(k) == w && visit_tree(k) == false)
            return false;
    return true;
}

void DependencyTree::cal_projective_order_and_mpc()
{
    vector<vector<int>> rev_tree(n+1);
    int root = 0;

    for (size_t i = 1; i <= n; ++i)
    {
        int head = heads[i];
        // if (head == 0) {
        //     if (root != 0)
        //         cerr << "there should be only one root!" << endl;
        //     root = i;
        // }
        rev_tree[head].push_back(i);
    }

    int timestamp = 1;
    for (int i = 0; i <= n; ++i)
    {
        proj_order.push_back(0); // initialization
        mpc.push_back(0); // initialization
    }

    // cerr << "root = " << root << endl;
    cal_projective_order(rev_tree, root, timestamp);
    cal_mpc(rev_tree, root);
}

void DependencyTree::cal_projective_order(
        vector<vector<int>>& rev_tree,
        int root,
        int& timestamp)
{
    const vector<int>& children = rev_tree[root];
    if (children.size() == 0)
    {
        proj_order[root] = timestamp;
        timestamp += 1;
        return;
    }

    size_t i;
    for (i = 0; i < children.size() && children[i] < root; ++i)
    {
        int child = children[i];
        cal_projective_order(rev_tree, child, timestamp);
    }

    proj_order[root] = timestamp;
    timestamp += 1;

    for (; i < children.size(); ++i)
    {
        int child = children[i];
        cal_projective_order(rev_tree, child, timestamp);
    }
}

tuple<bool, int, int> DependencyTree::cal_mpc(
        vector<vector<int>>& rev_tree,
        int root)
{
    const vector<int>& children = rev_tree[root];
    if (children.size() == 0)
    {
        mpc[root] = root;
        return make_tuple(true, root, root);
    }

    int left = root, right = root;
    bool overall = true;

    int pivot = -1;
    for (pivot = 0; pivot < children.size() && children[pivot] < root; ++pivot);

    for (int i = pivot - 1; i >= 0; --i)
    {
        int child = children[i];
        tuple<bool, int, int> result = cal_mpc(rev_tree, child);
        overall = overall && get<0>(result);
        if (get<0>(result) == true && get<2>(result) + 1 == left)
            left = get<1>(result);
        else
            overall = false;
    }

    for (int i = pivot; i < children.size(); ++i)
    {
        int child = children[i];
        tuple<bool, int, int> result = cal_mpc(rev_tree, child);
        overall = overall && get<0>(result);
        if (get<0>(result) == true && right + 1 == get<1>(result))
            right = get<2>(result);
        else
            overall = false;
    }

    for (int i = left; i <= right; ++i)
    {
        mpc[i] = root;
    }

    return make_tuple(overall, left, right);
}

/*
void DependencyTree::cal_projective_order()
{
    map<int, vector<int>> head2child;
    vector<int> childs0; // children of ROOT

    for (size_t i = 1; i < n; ++i)
    {
        h = heads[i];
        if (head2child.find(h) != head2child.end())
            head2child[h].push_back(i);
        else
        {
            vector<int> childs;
            childs.push_back(i);
            head2child.insert(pair<int, vector<int>>(h, childs));
        }
        if (h == 0)
            childs0.push_back(i);
    }

    proj_order = get_subtree(head2child, 0, childs0);
}

vector<int> DependencyTree::get_subtree(
        map<int, vector<int>>& head2child,
        int head,
        vector<int>& childs)
{
    vector<int> seq;
    if (childs.size() == 0) seq.push_back(head);
    else
    {
        for (size_t i = 0; i < childs.size(); ++i)
        {
            int m = childs[i];
            if (head < m && seq.find(head) == seq.end() && head != 0)
                seq.push_back(head);

            if (head2child.find(m) == head2child.end())
            {
                seq.push_back(m);
                if (m == head - 1 && seq.find(head) == seq.end())
                    seq.push_back(head);
            }
            else
            {
                vector<int> subseq = get_subtree(head2child, m, head2child[m]);
                seq.insert(seq.end(), subseq.begin(), subseq.end());
            }
        }
    }
    if (seq.find(head) == seq.end() && head != 0)
        seq.push_back(head);

    return seq;
}
*/

void DependencyTree::print()
{
    for (int i = 0; i <= n; ++i)
        cerr << i << "-" << get_head(i)
             << "-" << get_label(i) << " | ";
    cerr << endl;

    for (int i = 0; i <= n; ++i)
        cerr << proj_order[i] << " ";
    cerr << endl;

    for (int i = 0; i <= n; ++i)
        cerr << mpc[i] << " ";
    cerr << endl;
}

