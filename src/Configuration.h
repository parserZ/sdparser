#ifndef __NNDEP_CONFIGURATION_H__
#define __NNDEP_CONFIGURATION_H__

#include <vector>

//#include "DependencyTree.h"
#include "DependencyGraph.h"
#include "DependencySent.h"
#include "SecondHead.h"

class Configuration
{
    public:
        Configuration() {}
        Configuration(Configuration& c);
        Configuration(DependencySent& s);
        ~Configuration() {}

        void init(DependencySent& s);

        void reset(int k, int b); // k in stack top , b in buffer next

        // shift element from pass_buffer and buffer[0] to queue
        bool shift();

        //move the top element of stack to pass buffer
        bool pass();

        //reduce the top element of stack
        bool reduce();

        // remove top elements in stack
        bool remove_top_stack();

        // remove second top elements in stack
        bool remove_second_top_stack();

        int get_stack_size();

        int get_buffer_size();

        int get_pass_buffer_size();

        int get_sent_size();

        void save_2nd_head(std::string trans, int score);
        bool find_2nd_head(int k);

        std::vector<int> get_head(int k);

        bool has_head(int k); // return if node k has any head
        bool has_head(int k, int h);//return if node k has head h
        
        const std::vector<std::string>  get_label(int k);

        const std::string & get_arc_label(int k, int h);//get the label of node k to head h

        int get_stack(int k);

        int get_buffer(int k);

        int get_pass_buffer(int k);

        std::vector<int> get_dynamic_order();

        int get_distance();

        std::string get_lvalency(int k);
        std::string get_lvalency_fc(int k); // for debug

        std::string get_rvalency(int k);
        std::string get_rvalency_fc(int k); // for debug

        std::string get_word(int k);

        std::string get_pos(int k);

        std::string get_cluster(int k);

        std::string get_cluster_prefix(int k, int p);

        void add_arc(int h, int m, const std::string & l);

        int get_left_child(int k, int cnt);

        int get_left_child(int k);

        int get_right_child(int k, int cnt);

        int get_right_child(int k);

        int get_left_head(int k, int cnt);

        int get_left_head(int k);

        int get_right_head(int k, int cnt);

        int get_right_head(int k);

        bool lack_head(int k, DependencyGraph& gold_graph);

        bool is_root(int k); // return if node k has head 0(root)

        bool has_path_to(int k, int h); //return if node k has path to head h

        bool search_path(int k, int h); // return if k has path to h

        bool has_other_child_in_stack(int k, DependencyGraph& gold_graph);

        bool node_in_stack(int k);
        
        bool has_other_head_in_stack(int k, DependencyGraph& gold_graph);

        bool has_other_child(int k, DependencyGraph& gold_graph);

        bool has_other_head(int k, DependencyGraph& gold_graph);

        bool multi_head_in_buffer(int k, DependencyGraph& gold_graph);

        int get_left_valency(int k);

        int get_right_valency(int k);

        const DependencyGraph & get_graph();

        std::string info();

        bool is_graph();

    private:
        int encode_distance(const int & h, const int & m);
        std::string encode_valency(const std::string & typ, const int & k);

    public:
        // can not find define?
        double _DBL_MAX = 100000;
        std::vector<std::vector<Snd_head>> snd_heads; //length = n, begin at 0, id 0 represent first word
        /**
         * Not sure which one of [vector/list]
         *  is more efficient. try vector first.
         */
        std::vector<int> stack;
        std::vector<int> buffer;
        std::vector<int> pass_buffer;//store the passed word

        //DependencyTree tree;
        DependencyGraph graph;
        DependencySent sent;

        std::vector<int> lvalency;
        std::vector<int> rvalency;
};

#endif
