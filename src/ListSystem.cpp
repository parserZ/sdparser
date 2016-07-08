#include "ListSystem.h"
#include "strutils.h"
#include "Config.h"

#include <vector>
#include <map>
using namespace std;

ListSystem::ListSystem(vector<string>& ldict, string& language, bool is_labeled)
{
    lang = language;

    eval_lang = "english";

    labeled = is_labeled;

    labels = ldict;
    root_label = labels[labels.size() - 1];
    make_transitions();

    cerr << Config::SEPERATOR << endl
         << "#Transitions: " << transitions.size() << endl
         << "#Labels:      " << labels.size()      << endl
         << "ROOT LABEL:   " << root_label         << endl
         << "Labeled:      " << labeled            << endl;
}

ListSystem::~ListSystem()
{
}

void ListSystem::make_transitions()
{
    for (size_t i = 0; i < labels.size() - 1; ++i)
        transitions.push_back("LP(" + labels[i] + ")"); // left pop
    for (size_t i = 0; i < labels.size(); ++i)
        transitions.push_back("RA(" + labels[i] + ")"); // right arc
    for (size_t i = 0; i < labels.size() - 1; ++i)
        transitions.push_back("LA(" + labels[i] + ")"); // left arc

    transitions.push_back("NS"); // no shift
    transitions.push_back("NP"); // no pass

    cerr << "Transition types:" << endl;
    for (size_t i = 0; i < transitions.size(); ++i)
        cerr << transitions[i] << ", ";
    cerr << endl;
}   

bool ListSystem::can_apply(Configuration& c, const string& t)
{
    int n_stack = c.get_stack_size();
    int n_buffer = c.get_buffer_size();
    int w = c.get_stack(0);
    int b = c.get_buffer(0);
    std::vector<int> head_w = c.get_head(w);
    std::vector<int> head_b = c.get_head(b);
    int w_head = head_w.size();
    int b_head = head_b.size();
    root_label = labels[labels.size() - 1];

    if (startswith(t, "LA") || startswith(t, "LP"))
        return (w > 0 && b > 0 && !c.has_path_to(w, b) && !c.is_root(w));
    else if (startswith(t, "RA")){
        string l = t.substr(3, t.length() - 4);
        if (w == 0)
            return (l == root_label && !c.graph.is_single_root() && b_head == 0);
        else
            return (l != root_label && b > 0 && w >0 && !c.has_path_to(b, w));
    }
    else if (t == "NS")
        return (n_buffer > 0);
    else if (t == "NP") // can not pass root(0)
        return (n_stack > 1 && n_buffer > 0);
}

void ListSystem::apply(Configuration& c, const string& t)
{
    int b = c.get_buffer(0);
    int w = c.get_stack(0);

    // Left Pop
    if (startswith(t, "LP"))
    {
        c.add_arc(b, w, t.substr(3, t.length() - 4));
        c.reduce();
        c.lvalency[b] += 1;
        c.rhvalency[w] += 1;
    }
    // Left Arc
    else if (startswith(t, "LA"))
    {
        c.add_arc(b, w, t.substr(3, t.length() - 4));
        c.pass();
        c.lvalency[b] += 1;
        c.rhvalency[w] += 1;
    }
    // Right Arc
    else if (startswith(t, "RA"))
    {
        c.add_arc(w, b, t.substr(3, t.length() - 4));
        c.pass();
        c.rvalency[w] += 1;
        c.lhvalency[b] += 1;
    }
    // No Shift
    else if (t == "NS")
    {
        c.shift();
    }
    // No Pass
    else if (t == "NP")
    {
        c.pass();
    }
}

const string ListSystem::get_oracle(
        Configuration& c,
        DependencyGraph& graph)
{
    int w = c.get_stack(0);
    int b = c.get_buffer(0);

    if (w > 0 && graph.has_head(w, b) // w <- b
        && !c.has_path_to(w, b)) //no cycle
    {
        if ( !c.has_other_child(w, graph)
            && !c.has_other_head(w, graph)){
            return "LP(" + graph.get_arc_label(w, b) + ")";
        }
        else{ //has other child or head
            return "LA(" + graph.get_arc_label(w, b) + ")";
        }
    }
    else if ( w >= 0 && graph.has_head(b, w) //right arc w -> b
            && !c.has_path_to(b, w))
    {
        return "RA(" + graph.get_arc_label(b, w) + ")";
    }
    else if (( !c.has_other_child_in_stack(b, graph)
            && !c.has_other_head_in_stack(b, graph) 
            && !c.buffer.empty()) 
            || w == Config::NONEXIST)
        return "NS";
    else if ( w > 0)
        return "NP";
    else
        return "-E-";
}

bool ListSystem::is_oracle(
        Configuration& c,
        string& t,
        DependencyGraph& graph)
{
    // unused so far
    return true;
}

bool ListSystem::can_process(DependencyGraph& graph)
{
    // can process any kinds of trees
    return true;
}

Configuration ListSystem::init_configuration(DependencySent& sent)
{
    Configuration c(sent);
    return c;
}

bool ListSystem::is_terminal(Configuration& c)
{
    return (c.get_buffer_size() == 0);
}