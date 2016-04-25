#include "AttachSystem.h"
#include "strutils.h"
#include "Config.h"

#include <vector>
#include <map>
using namespace std;

AttachSystem::AttachSystem(vector<string>& ldict, string& language, bool is_labeled)
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

AttachSystem::~AttachSystem()
{
}

void AttachSystem::make_transitions()
{
    for (size_t i = 0; i < labels.size(); ++i)
        transitions.push_back("LR(" + labels[i] + ")"); // left reduce
    for (size_t i = 0; i < labels.size(); ++i)
        transitions.push_back("RR(" + labels[i] + ")"); // right reduce
    for (size_t i = 0; i < labels.size(); ++i)
        transitions.push_back("LA(" + labels[i] + ")"); // left attach, not pop stack
    for (size_t i = 0; i < labels.size(); ++i)
        transitions.push_back("RA(" + labels[i] + ")"); // right attach, not pop stack

    transitions.push_back("E"); // Exchange/Swap
    transitions.push_back("S"); // shift

    cerr << "Transition types:" << endl;
    for (size_t i = 0; i < transitions.size(); ++i)
        cerr << transitions[i] << ", ";
    cerr << endl;
}

bool AttachSystem::can_apply(Configuration& c, const string& t)
{
    int n_stack = c.get_stack_size();
    int n_buffer = c.get_buffer_size();
    int w1 = c.get_stack(1);
    int w2 = c.get_stack(0);

    if (startswith(t, "LR") || startswith(t, "RR"))
    {
        string label = t.substr(3, t.length() - 4);
        int h = startswith(t, "LR") ? c.get_stack(0) : c.get_stack(1);

        if (h < 0)
            return false;
        if (labeled)
        {
            // set<string> punc_tags = get_punctuation_tags();
            // if (punc_tags.find(c.get_pos(h)) != punc_tags.end()) return false;
            if (h == 0 && !(label == root_label))   return false;

            // When training parsers for multi-rooted data, comment the following lines
            if (h > 0 && (label == root_label))     return false;
            //     cerr << "h > 0 and label == root_label" << endl;
        }
    }
    else if (t == "E")
    {
        if (n_stack < 2) return false;
        if (w1 > w2) return false; // not in original order
        if (w1 == 0) return false; // leftmost word is ROOT
    }
    else if (t == "S")
        return n_buffer > 0;

    if (startswith(t, "LR"))
        return (n_stack > 2 && !c.has_head(w1, w2));
    else if (startswith(t, "RR"))
        // single root
        return (n_stack > 2 && !c.has_head(w2, w1)) || (n_stack == 2 && n_buffer == 0);
    else if (startswith(t,"LA"))
        return (n_stack > 2 && !c.has_head(w1, w2));
    else if (startswith(t,"RA"))//can not be root situation
        return (n_stack > 2 && !c.has_head(w2, w1));
    else
        return n_buffer > 0; // shift
}

void AttachSystem::apply(Configuration& c, const string& t)
{
    int w1 = c.get_stack(1);
    int w2 = c.get_stack(0);

    // Left Reduce
    if (startswith(t, "LR"))
    {
        c.add_arc(w2, w1, t.substr(3, t.length() - 4));
        c.remove_second_top_stack();
        c.lvalency[w2] += 1;
    }
    // Right Reduce
    else if (startswith(t, "RR"))
    {
        c.add_arc(w1, w2, t.substr(3, t.length() - 4));
        c.remove_top_stack();
        c.rvalency[w1] += 1;
    }
    // Left Attach
    else if (startswith(t, "LA"))
    {
        c.add_arc(w2, w1, t.substr(3, t.length() - 4));
    }
    // Right Attach
    else if (startswith(t, "RA"))
    {
        c.add_arc(w1, w2, t.substr(3, t.length() - 4));
    }
    // Exchange/Swap
    else if (t == "E")
    {
        c.swap();
    }
    // Shift
    else
        c.shift();
}

const string AttachSystem::get_oracle(
        Configuration& c,
        DependencyGraph& graph)
{
    int w1 = c.get_stack(1);
    int w2 = c.get_stack(0);
    int b1 = c.get_buffer(0);

    int k = c.buffer.empty() ? -1 : c.buffer.back();
    //cerr << c.has_head(w1, w2) << c.has_other_child(w1, graph) << c.has_other_head(w1, graph) << endl;
   //cerr << "w1:" << w1 << " w2:" << w2 << " b1:" << b1 << endl;
  //  cerr << graph.dynamic_proj_order[w1] <<" " << graph.dynamic_proj_order[w2] <<" " << graph.dynamic_proj_order[b1]<< endl;
    if (w1 > 0
            && graph.has_head(w1, w2)
            && !c.has_head(w1, w2)
            && !c.has_other_child(w1, graph)
            && !c.has_other_head(w1, graph))
        return "LR(" + graph.get_arc_label(w1, w2) + ")";
    else if (w1 >= 0
            && graph.has_head(w2, w1)
            && !c.has_head(w2, w1)
            && !c.has_other_child(w2, graph)
            && !c.has_other_head(w2, graph))
        return "RR(" + graph.get_arc_label(w2, w1) + ")";
    else if (w1 > 0
            && graph.has_head(w1, w2)
            && !c.has_head(w1, w2)
            //&& !c.has_other_child(w1, graph)
            && c.has_other_head(w1, graph))
        return "LA(" + graph.get_arc_label(w1, w2) + ")";
    else if (w1 > 0
            && graph.has_head(w2, w1)
            && !c.has_head(w2, w1)
            //&& !c.has_other_child(w2, graph)
            && c.has_other_head(w2, graph))
        return "RA(" + graph.get_arc_label(w2, w1) + ")";
    /*else if (w1 < w2 && w1!=0
        && !(graph.has_head(w2, b1) && !c.has_other_child(w2, graph) && !c.has_other_head(w2, graph)) 
        && !(graph.has_head(b1, w2) && !c.has_other_child(b1, graph) && !c.has_other_head(b1, graph)) 
        && (k == -1 || graph.has_head(w1, b1) || graph.has_head(b1, w1)))*/
    else if (w1 !=0 && w1 < w2 && 
            (graph.dynamic_proj_order[w1] > graph.dynamic_proj_order[w2] 
            || k == -1
              ))
        return "E";
    else if(!c.buffer.empty())
        return "S";
    else
        return "-N-";
}

bool AttachSystem::is_oracle(
        Configuration& c,
        string& t,
        DependencyGraph& graph)
{
    // unused so far
    return true;
}

bool AttachSystem::can_process(DependencyGraph& graph)
{
    // can process any kinds of trees
    return true;
}

Configuration AttachSystem::init_configuration(DependencySent& sent)
{
    Configuration c(sent);
    return c;
}

bool AttachSystem::is_terminal(Configuration& c)
{
    return c.get_stack_size() == 1 &&
        c.get_buffer_size() == 0;
}

