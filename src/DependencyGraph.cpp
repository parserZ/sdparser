#include "DependencyGraph.h"
#include "Config.h"

#include <iostream>

using namespace std;

DependencyGraph::DependencyGraph()
{
    init();
}

void DependencyGraph::init()
{
    n = 0;
    heads.clear();
    labels.clear();
    std::vector<int> h;
    std::vector<std::string> l;
    h.push_back(Config::NONEXIST);
    l.push_back(Config::UNKNOWN);
    heads.push_back(h);
    labels.push_back(l);
}

DependencyGraph::DependencyGraph(const DependencyGraph& graph)
{
    n = graph.n;
    heads = graph.heads;
    labels = graph.labels;
}

void DependencyGraph::add(const std::vector<int> & h, const std::vector<std::string> & l)
{
    ++n;
    arc_n += (int)h.size();
    heads.push_back(h);
    labels.push_back(l);
}

void DependencyGraph::set(int k, int h, const std::string & l)
{
    arc_n += 1;
    heads[k].push_back(h);
    labels[k].push_back(l);
}

const vector<int> DependencyGraph::get_head(int k) const
{
    if (k <= 0 || k > n){
        std::vector<int> h;
        h.push_back(Config::NONEXIST);
        return h;
    }
    return heads[k];
}

int DependencyGraph::get_head_num(int k)
{
    if (k <= 0 || k > n)
        return Config::NONEXIST;
    return heads[k].size();
}

bool DependencyGraph::has_head(int k)
{
    return (heads[k].size() > 0);
}

bool DependencyGraph::has_head(int k, int h)
{
    if (k <= 0 || k > n)
        return false;
    std::vector<int> head = get_head(k);
    for (int i = 0; i < (int)head.size(); i++){
        if (head[i] == h)
            return true;
    }
    return false;
}

const std::vector<std::string> DependencyGraph::get_label(int k)
{
    if (k <= 0 || k > n){
        std::vector<std::string> l;
        l.push_back(Config::NIL);
        return l;
    }
    return labels[k];   
}

const string & DependencyGraph::get_arc_label(int k, int h) // get label between node k and its head h
{
    if (k <= 0 || k > n)
        return Config::NIL;
    for (int i = 0; i < (int)heads[k].size(); i++){
        if (heads[k][i] == h){
            return labels[k][i];
        }
    }
    return Config::NIL;
}

int DependencyGraph::get_root()
{
    for (int k = 1; k <= n; ++k){
        vector<int> heads = get_head(k);
        for (int j = 0; j < (int)heads.size(); j++){
            if (heads[j] == 0)
                return k;
        }
    }
    return 0; // non tree
}

bool DependencyGraph::is_root(int k)
{
    if (k <=0 || k > n)
        return false;
    for(int i = 0; i < heads[k].size(); i++)
        if (heads[k][i] == 0)
            return true;
    return false;
}

bool DependencyGraph::is_single_root()
{
    int roots = 0;
    for (int k = 1; k <= n; ++k){
        vector<int> heads = get_head(k);
        for (int j = 0; j < (int)heads.size(); j++){
            if (heads[j] == 0)
                roots += 1;
        }
    }
    return (roots == 1);
}

/**
 * Checking if the tree is legal, O(n)
 */
 /// to be done adding DAG 
bool DependencyGraph::is_tree()
{
    vector<int> visited;
    vector<vector<int>> down_graph;
    for (int i = 0; i<=n ; ++i)
    {
        vector<int> h;
        down_graph.push_back(h);
    }
    for (int k = 1; k <= n ; ++k)
    {
        vector<int> heads = get_head(k);
        if ((int)heads.size() > 1)
            return false;
        for (int j = 0; j < (int)heads.size(); j++){
            down_graph[heads[j]].push_back(k);
        }
        visited.push_back(-1);
    }
    return true;

    /*
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
    return true;*/
}

/*
bool DependencyGraph::is_projective()
{
    if (!is_tree())
        return false;

    counter = -1;
    return visit_tree(0);
}

bool DependencyGraph::visit_tree(int w)
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
}*/

void DependencyGraph::print()
{
    for (int i = 0; i <= n; ++i){
        cerr << i << ":" ;
        vector<int> h = get_head(i);
        for(int j = 0; j < h.size(); j++)
             cerr<< h[j]<<"-" << get_arc_label(i, h[j]) << " ";
         cerr<<endl;
    }
    cerr << endl;
}

