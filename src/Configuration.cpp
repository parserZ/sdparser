#include "Configuration.h"
#include "Config.h"
#include "strutils.h"
#include <cassert>
#include <algorithm>

using namespace std;

Configuration::Configuration(Configuration& c)
{
    stack  = c.stack;
    buffer = c.buffer;
    pass_buffer =  c.pass_buffer;
    graph   = c.graph;
    sent   = c.sent;

    lvalency = c.lvalency;
    rvalency = c.rvalency;
}

Configuration::Configuration(DependencySent& s)
{
    init(s);
}

void Configuration::init(DependencySent& s)
{
    stack.clear();
    buffer.clear();
    pass_buffer.clear();
    snd_heads.clear();

    lvalency.clear();
    rvalency.clear();

    sent = s;
    for (int i = 1; i <= sent.n; ++i)
    {
        vector<int> head;
        vector<string> label;
        vector<Snd_head> snd_head;
        graph.add(head, label);
        buffer.push_back(i);
        snd_heads.push_back(snd_head);
    }

    lvalency.resize(sent.n + 1, 0);
    rvalency.resize(sent.n + 1, 0);

    stack.push_back(0);
}

void Configuration::reset(int k, int b)
{
    stack.clear();
    buffer.clear();
    pass_buffer.clear();
    buffer.push_back(b);
    stack.push_back(0);
    stack.push_back(k);    
}

bool Configuration::shift()
{
    int k = get_buffer(0);
    if (k == Config::NONEXIST)
        return false;
    while(!pass_buffer.empty()){
        int p = get_pass_buffer(0);
        pass_buffer.erase(pass_buffer.begin());
        stack.push_back(p);
    }
    buffer.erase(buffer.begin());
    stack.push_back(k);
    return true;
}

bool Configuration::pass()
{
    int w = get_stack(0);
    if (w == 0 || w == Config::NONEXIST) //can not pass root(0)
        return false;
    pass_buffer.insert(pass_buffer.begin(), w);
    remove_top_stack();
    return true;
}

bool Configuration::reduce()
{
    int w = get_stack(0);
    if (w == 0 || w == Config::NONEXIST) //can not reduce root(0)
        return false;
    remove_top_stack();
    return true;
}

bool Configuration::remove_top_stack()
{
    int n_stack = get_stack_size();
    if (n_stack < 1)
        return false;
    stack.erase(stack.begin() + stack.size() - 1);
    return true;
}

bool Configuration::remove_second_top_stack()
{
    int n_stack = get_stack_size();
    if (n_stack < 2)
        return false;
    stack.erase(stack.begin() + stack.size() - 2);
    return true;
}

int Configuration::get_stack_size()
{
    return stack.size();
}

int Configuration::get_buffer_size()
{
    return buffer.size();
}

int Configuration::get_pass_buffer_size()
{
    return pass_buffer.size();
}

int Configuration::get_sent_size()
{
    return sent.n;
}

void Configuration::save_2nd_head(std::string trans, int score) //node k is stored at k-1
{
    Snd_head snd_head;
    if (trans.length() < 4)
        return;
    snd_head.label = trans.substr(3, trans.length() - 4);
    snd_head.score = score;
    int k,h;
    if (startswith(trans, "L")){
        k = get_stack(0);
        h = get_buffer(0);
    }
    else if (startswith(trans, "R")){
        k = get_buffer(0);
        h = get_stack(0);
    }
    else{
        cerr << "error: save 2nd head:" << trans << endl;
    }
    snd_head.head = h;
  //  cerr << "save head :" << k << endl;
    snd_heads[k-1].push_back(snd_head);
}

bool Configuration::find_2nd_head(int k) //node k is stored at k-1
{
   // cerr << "head num:" << snd_heads[k-1].size() << endl;
    if (snd_heads[k-1].size() < 1)
        return false;
    Snd_head opt_head;
    opt_head.score = -_DBL_MAX;
    opt_head.head = -1;
    for (int i = 0; i < snd_heads[k-1].size(); i++){
        if (snd_heads[k-1][i].score > opt_head.score 
            && !has_path_to(k, snd_heads[k-1][i].head))
            opt_head = snd_heads[k-1][i];
    }
    if (opt_head.head != -1){
        add_arc(opt_head.head, k, opt_head.label);
        return true;
    }
    return false;
}

std::vector<int> Configuration::get_head(int k)
{
    return graph.get_head(k);
}

bool Configuration::is_root(int k)
{
    return graph.is_root(k);
}

bool Configuration::has_head(int k)
{
    return graph.has_head(k);
}

bool Configuration::has_head(int k, int h)//return if node k has head h
{
    return graph.has_head(k, h);
}

const std::vector<std::string>  Configuration::get_label(int k)
{
    return graph.get_label(k);
}

const string & Configuration::get_arc_label(int k, int h)
{
    return graph.get_arc_label(k, h);
}

/**
 * k starts from 0 (top-stack)
 */
int Configuration::get_stack(int k)
{
    int n_stack = get_stack_size();
    return (k >= 0 && k < n_stack)
                ? stack[n_stack - 1 - k]
                : Config::NONEXIST;
}

int Configuration::get_buffer(int k)
{
    int n_buffer = get_buffer_size();
    return (k >= 0 && k < n_buffer)
                ? buffer[k]
                : Config::NONEXIST;
}

int Configuration::get_pass_buffer(int k)
{
    int n_pass_buffer = get_pass_buffer_size();
    return (k >= 0 && k < n_pass_buffer)
                ? pass_buffer[k]
                : Config::NONEXIST;
}

std::vector<int> Configuration::get_dynamic_order()
{
    int n_buffer = get_buffer_size();
    int n_stack = get_stack_size();
   // cerr << "get order:" << n_buffer <<" "<<n_stack << endl;
    std::vector<int> dynamic_order;
    for (int i = n_stack-1; i >= 0; i--)
        dynamic_order.push_back(get_stack(i));
    for (int i = 0; i < n_buffer; i++)
        dynamic_order.push_back(get_buffer(i));
    return dynamic_order;
}

int Configuration::get_distance()
{
    // return abs(get_stack(0) - get_buffer(0));
    return encode_distance(get_stack(0), get_stack(1));
}

int Configuration::encode_distance(const int & h, const int & m)
{
    int diff;
    diff = h - m;
    assert(diff != 0);

    if (diff < 0) diff = -diff;
    if (diff > 10) diff = 6;
    else if (diff > 5) diff = 5;

    return diff;
}

string Configuration::encode_valency(const string & typ, const int & k)
{
    int v = k;
    if (v > 10) v = 6;
    else if (v > 5) v = 5;

    return typ + to_str(v);
}

/**
 * k starts from 0 (root)
 */
string Configuration::get_word(int k)
{
    if (k == 0)
        return Config::ROOT;
    else
        -- k;

    return (k < 0 || k >= sent.n)
                ? Config::NIL
                : sent.words[k];
}

/**
 * k starts from 0 (root)
 */
string Configuration::get_pos(int k)
{
    if (k == 0)
        return Config::ROOT;
    else
        -- k;

    return (k < 0 || k >= sent.n)
                ? Config::NIL
                : sent.poss[k];
}

string Configuration::get_cluster(int k)
{
    if (k == 0)
        return Config::ROOT;
    else
        -- k;

    return (k < 0 || k >= sent.n)
                ? Config::NIL
                : sent.clusters[k];
}

string Configuration::get_cluster_prefix(int k, int p)
{
    if (k == 0)
        return Config::ROOT;
    else
        -- k;

    return (k < 0 || k > sent.n)
                ? Config::NIL
                : get_brown_prefix(sent.clusters[k], p);
                // : ((sent.clusters[k] == Config::UNKNOWN)
                //         ? Config::UNKNOWN
                //         : get_brown_prefix(sent.clusters[k], p));
}

void Configuration::add_arc(int h, int m, const string & l)// h -> m
{
    graph.set(m, h, l);
}

string Configuration::get_lvalency(int k)
{
    if (k < 0 || k > graph.n)
        return Config::UNKNOWN;

    return encode_valency("L", lvalency[k]);
}

string Configuration::get_lvalency_fc(int k)
{
    if (k < 0 || k > graph.n)
        return Config::UNKNOWN;
        // return Config::NONEXIST;

    int cnt = 0;
    for (int i = 1; i < k; ++i){
        vector<int> h = graph.get_head(i);
        for (int j = 0; j < (int)h.size(); j++){
            if (h[j] == k)
                cnt += 1;
        }
    }
    return "L" + to_str(cnt);
}

string Configuration::get_rvalency(int k)
{
    if (k < 0 || k > graph.n)
        return Config::UNKNOWN;

    return encode_valency("R", rvalency[k]);
}

string Configuration::get_rvalency_fc(int k)
{
    if (k < 0 || k > graph.n)
        return Config::UNKNOWN;
        // return Config::NONEXIST;

    int cnt = 0;
    for (int i = graph.n; i > k; --i){
        vector<int> h = graph.get_head(i);
        for (int j = 0; j < (int)h.size(); j++){
            if (h[j] == k)
                cnt += 1;
        }
    }
    return "R" + to_str(cnt);
}

int Configuration::get_left_child(int k, int cnt)
{
    if (k < 0 || k > graph.n)
        return Config::NONEXIST;
    int c = 0;
    for (int i = 1; i < k; ++i){
        vector<int> h = graph.get_head(i);
        for (int j = 0; j < (int)h.size(); j++){
            if (h[j] == k)
                if ((++c) == cnt)
                    return i;
        }
    }
    return Config::NONEXIST;
}

int Configuration::get_left_child(int k)
{
    return get_left_child(k, 1);
}

int Configuration::get_right_child(int k, int cnt)
{
    if (k < 0 || k > graph.n)
        return Config::NONEXIST;

    int c = 0;
    for (int i = graph.n; i > k; --i){
        vector<int> h = graph.get_head(i);
        for (int j = 0; j < (int)h.size(); j++){
            if (h[j] == k)
                if ((++c) == cnt)
                    return i;
        }
    }
    return Config::NONEXIST;
}

int Configuration::get_right_child(int k)
{
    return get_right_child(k, 1);
}
//-------------------------get head------------------------
int Configuration::get_left_head(int k, int cnt)
{
    if (k < 0 || k > graph.n)
        return Config::NONEXIST;
    int c = 0;
    vector<int> h = graph.get_head(k);
    sort(h.begin(), h.end());
    if (h.size() >= cnt)
        return h[cnt-1];
    return Config::NONEXIST;
}

int Configuration::get_left_head(int k)
{
    return get_left_head(k, 1);
}

int Configuration::get_right_head(int k, int cnt)
{
    if (k < 0 || k > graph.n)
        return Config::NONEXIST;
    int c = 0;
    vector<int> h = graph.get_head(k);
    sort(h.begin(), h.end());
    if (h.size() >= cnt)
        return h[h.size() - cnt];
    return Config::NONEXIST;
}

int Configuration::get_right_head(int k)
{
    return get_right_head(k, 1);
}
//-----------------
bool Configuration::multi_head_in_buffer(int k, DependencyGraph& gold_graph)//check if k is a node with multihead in buffer
{
    vector<int> gold_head = gold_graph.get_head(k);
    if (gold_head.size()==1)
        return false;
    vector<int> head = graph.get_head(k);
    if (head.size()+1 >= gold_head.size())
        return false;
    for (int i = 0; i < gold_head.size(); i++){
        if (!graph.has_head(k, gold_head[i]))
        {
            int n_buffer = get_buffer_size();
            for (int j = 0; j < n_buffer; j++)
                if (get_buffer(j) == gold_head[i])
                    return true;
        }
    }
    return false;
}

bool Configuration::lack_head(int k, DependencyGraph& gold_graph) // return if add head of k is in graph
{
    vector<int> gh = gold_graph.get_head(k);
    vector<int> h = graph.get_head(k);
    if ((int)gh.size() > (int)h.size())
        return true;
    return false;
}

bool Configuration::has_other_head(int k, DependencyGraph& gold_graph) // return if all head of k except 1 is in graph 
{
    vector<int> gh = gold_graph.get_head(k);
    vector<int> h = graph.get_head(k);
    if ((int)gh.size() > (int)h.size()+1)
        return true;
    /*
    for (int j = 0; j < (int)gh.size(); j++){//every head in gold graph need to occur in graph
        bool find_flag = false;
        for (int m = 0; m < (int)h.size(); m++){
                if (h[m] == gh[j])
                    find_flag = true;
        }
        if (!find_flag)
            return true;
    }*/
    return false;
}

bool Configuration::has_path_to(int k, int h) //return if node k has path to head h
{    
    return search_path(k, h);
}

bool Configuration::search_path(int k, int h) // return if k has path to h
{
    std::vector<int> heads = graph.get_head(h);
    if (heads.size() == 0 
        || (heads.size() == 1 && heads[0] == Config::NONEXIST))
        return false;
    for (int i = 0; i < heads.size(); i++){
        if (heads[i] == k)
            return true;
        if (search_path(k, heads[i]))
            return true;
    }
    return false;
}

bool Configuration::has_other_child_in_stack(int k, DependencyGraph& gold_graph) // except top stack
{
    int n_stack = get_stack_size();
    for (int i = 1; i < n_stack; ++i){ //for every word in stack
        vector<int> gh = gold_graph.get_head(get_stack(i));
        vector<int> h = graph.get_head(get_stack(i));
        for (int j = 0; j < (int)gh.size(); j++){
            if (gh[j] == k){
                bool find_flag = false;
                for (int m =0; m < (int)h.size(); m++){
                    if (h[m] == k)
                        find_flag = true;
                }
                if (!find_flag)
                    return true;
            }
        }
    }
    return false;
}

bool Configuration::node_in_stack(int k)
{
    int n_stack = get_stack_size();
    for (int i = 1; i < n_stack; i++){
        int n = get_stack(i);
        if (n == k)
            return true;
    }
    return false;
}

bool Configuration::has_other_head_in_stack(int k, DependencyGraph& gold_graph) //except top stack  in stack[0]
{
    vector<int> gh = gold_graph.get_head(k);
    vector<int> h = graph.get_head(k);
    for (int i = 0; i < gh.size(); i++){
        if (node_in_stack(gh[i])){ // if the gold head is in stack
            bool find_flag = false;
            for (int j = 0; j < h.size(); j++)
                if (h[j] == gh[i])
                    find_flag = true;
            if (!find_flag)
                return true;
        }
    }
    return false;
}

bool Configuration::has_other_child(int k, DependencyGraph& gold_graph)
{
    for (int i = 1; i <= graph.n; ++i){
        vector<int> gh = gold_graph.get_head(i);
        vector<int> h = graph.get_head(i);
        for (int j = 0; j < (int)gh.size(); j++){
            if (gh[j] == k){
                bool find_flag = false;
                for (int m =0; m < (int)h.size(); m++){
                    if (h[m] == k)
                        find_flag = true;
                }
                if (!find_flag)
                    return true;
            }
        }
    }
    return false;
}

int Configuration::get_left_valency(int k)
{
    if (k < 0 || k >= graph.n)
        return Config::NONEXIST;
    int cnt = 0;
    for (int i = 0; i < k; ++k){
        vector<int> h = graph.get_head(i);
        for (int j = 0; j < (int)h.size(); j++){
            if (h[j] == k)
                ++ cnt;
        }
    }
    return cnt;
}

int Configuration::get_right_valency(int k)
{
    if (k < 0 || k >= graph.n)
        return Config::NONEXIST;
    int cnt = 0;
    for (int i = k + 1; i <= graph.n; ++k){
        vector<int> h = graph.get_head(i);
        for (int j = 0; j < (int)h.size(); j++){
            if (h[j] == k)
                ++ cnt;
        }
    }
    return cnt;
}

const DependencyGraph & Configuration::get_graph()
{
    return graph;
}

bool  Configuration::is_graph()
{
    int root_num = 0;
    for (int i = 1; i <= graph.n; i++){
        vector<int> h = graph.get_head(i);
        if (h.size() < 1){
            cerr << "headless node " <<endl;
            return false;
        }
        for (int j = 0; j < h.size(); j++)
            if (h[j]==0)
                root_num++;
    }
    if (root_num > 1)
        cerr << "more than 1 root " << endl;
    if (root_num == 0){
        cerr << "no root" << endl;
        for (int i = 1; i <= graph.n; i++){
            cerr << endl << "node:" <<i <<"head:";
            vector<int> h = graph.get_head(i);
            for (int j = 0; j < h.size(); j++){
                cerr<< h[j] <<" ";
            }
        }
    }
    return (root_num == 1);
}

string Configuration::info()
{
    string s = "[S]";
    for (int i = 0; i < get_stack_size(); ++i)
    {
        if (i > 0)
            s.append(",");
        s += to_str(stack[i]);
    }

    s.append("\n[B]");
    for (int i = 0; i < get_buffer_size(); ++i)
    {
        if (i > 0)
            s.append(",");
        s += to_str(buffer[i]);
    }

    s.append("\n[PB]");
    for (int i = 0; i < get_pass_buffer_size(); ++i)
    {
        if (i > 0)
            s.append(",");
        s += to_str(pass_buffer[i]);
    }

    /*
    s.append("\n[H]");
    for (int i = 1; i <= tree.n; ++i)
    {
        if (i > 1)
            s.append(",");
        s.append(to_str(get_head(i)))
         .append("(")
         .append(get_label(i))
         .append(")");
    }
    */

    return s;
}

