#include "ArcEager.h"
#include "strutils.h"
#include "Config.h"

#include <vector>
#include <map>
using namespace std;

ArcEager::ArcEager(vector<string>& ldict, string& language, bool is_labeled)
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

ArcEager::~ArcEager()
{
}

void ArcEager::make_transitions()
{
    for (size_t i = 0; i < labels.size() - 1; ++i)
        transitions.push_back("LR(" + labels[i] + ")"); // left reduce
    for (size_t i = 0; i < labels.size(); ++i)
        transitions.push_back("RS(" + labels[i] + ")"); // right shift
    for (size_t i = 0; i < labels.size() - 1; ++i)
        transitions.push_back("LP(" + labels[i] + ")"); // left pass
    for (size_t i = 0; i < labels.size() - 1; ++i)
        transitions.push_back("RP(" + labels[i] + ")"); // right pass

    transitions.push_back("NS"); // no shift
    transitions.push_back("NR"); // no reduce
    transitions.push_back("NP"); // no pass

    cerr << "Transition types:" << endl;
    for (size_t i = 0; i < transitions.size(); ++i)
        cerr << transitions[i] << ", ";
    cerr << endl;
}   

bool ArcEager::can_apply(Configuration& c, const string& t)
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

    if (startswith(t, "LR") || startswith(t, "LP"))
        return (w > 0 && b > 0 && !c.has_path_to(w, b) && !c.is_root(w));
    else if (startswith(t, "RS") || startswith(t, "RP")){
        string l = t.substr(3, t.length() - 4);
        if (w == 0)
            return (l == root_label && !c.graph.is_single_root() && b_head == 0);
        else
            return (l != root_label && b > 0 && w >0 && !c.has_path_to(b, w));
    }
    else if (t == "NS")
        return (n_buffer > 0);
    else if (t == "NR") // w has head
        return (n_stack > 1 && w_head > 0);
    else if (t == "NP") // can not pass root(0)
        return (n_stack > 1 && n_buffer > 0);
}

void ArcEager::apply(Configuration& c, const string& t)
{
    int b = c.get_buffer(0);
    int w = c.get_stack(0);

    // Left Reduce
    if (startswith(t, "LR"))
    {
        c.add_arc(b, w, t.substr(3, t.length() - 4));
        c.reduce();
    }
    // Right Reduce
    else if (startswith(t, "RS"))
    {
        c.add_arc(w, b, t.substr(3, t.length() - 4));
        c.shift();
    }
    // Left Attach
    else if (startswith(t, "LP"))
    {
        c.add_arc(b, w, t.substr(3, t.length() - 4));
        c.pass();
    }
    // Right Attach
    else if (startswith(t, "RP"))
    {
        c.add_arc(w, b, t.substr(3, t.length() - 4));
        c.pass();
    }
    // No Shift
    else if (t == "NS")
    {
        c.shift();
    }
    // No Reduce
    else if (t == "NR")
    {
        c.reduce();
    }
    // No Pass
    else if (t == "NP")
    {
        c.pass();
    }
}

const string ArcEager::get_oracle(
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
            return "LR(" + graph.get_arc_label(w, b) + ")";
        }
        else{ //has other child or head
            return "LP(" + graph.get_arc_label(w, b) + ")";
        }
    }
    else if ( w >= 0 && graph.has_head(b, w) //right arc w -> b
            && !c.has_path_to(b, w))
    {
            if ( !c.has_other_child_in_stack(b, graph)
                && !c.has_other_head_in_stack(b, graph)){
                return "RS(" + graph.get_arc_label(b, w) + ")";
            }
            else if (w > 0){
                return "RP(" + graph.get_arc_label(b, w) + ")";
            }
    }
    else if ( !c.has_other_child_in_stack(b, graph)
            && !c.has_other_head_in_stack(b, graph) 
            && !c.buffer.empty())
        return "NS";
    else if ( !c.has_other_child(w, graph)
            && !c.lack_head(w, graph))
        return "NR";
    else if ( w > 0)
        return "NP";
    else
        return "-E-";
}

bool ArcEager::is_oracle(
        Configuration& c,
        string& t,
        DependencyGraph& graph)
{
    // unused so far
    return true;
}

bool ArcEager::can_process(DependencyGraph& graph)
{
    // can process any kinds of trees
    return true;
}

Configuration ArcEager::init_configuration(DependencySent& sent)
{
    Configuration c(sent);
    return c;
}

bool ArcEager::is_terminal(Configuration& c)
{
    return (c.get_buffer_size() == 0);
}