#ifndef __NNDEP_DEPENDENCY_GRAPH_H__
#define __NNDEP_DEPENDENCY_GRAPH_H__

#include <vector>
#include <string>
#include <tuple>

class DependencyGraph
{
    public:
        DependencyGraph();
        DependencyGraph(const DependencyGraph& graph);
        ~DependencyGraph() {}

        void init();

        void add(const std::vector<int> & h, const std::vector<std::string> & l);
        void set(int k, int h, const std::string & l);

        const std::vector<int> get_head(int k) const;
        int get_head_num(int k);
        const std::vector<std::string>  get_label(int k);

        bool has_head(int k); //return if node k has any head
        bool has_head(int k, int h);//return if node k has head h
        const std::string & get_arc_label(int k, int h);//get the label of node k to head h

        int get_root();
        bool is_single_root();

        bool is_root(int k); // return if node k has head root(0)

        bool is_tree();

        bool move_proj_order (int k, int t, std::vector<int> & proj_order_reverse);
        bool cal_projective_order_search();//proj order by all search

        void cal_projective_order(
                std::vector<std::vector<int>>& rev_tree,
                int root,
                int& timestamp);

        void init_dynamic_projective_order();
        void cal_dynamic_projective_order(
                const DependencyGraph & graph,
                std::vector<int> dynamic_order);
      //  bool is_projective();

       void print();

   // private:
      //  bool visit_tree(int w);

    public:
        int n;
        int arc_n;//denote the arc number
        std::vector<std::vector<int>> heads;
        std::vector<std::vector<std::string>> labels;

        std::vector<int> proj_order;

        std::vector<int> dynamic_proj_order; //dynamic
        std::vector<int> dynamic_order_map; //dynamic order
        std::vector<int> dynamic_order_reverse;//dynamic
        int dynamic_order_len;
};

#endif
