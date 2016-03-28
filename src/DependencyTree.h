#ifndef __NNDEP_DEPENDENCY_TREE_H__
#define __NNDEP_DEPENDENCY_TREE_H__

#include <vector>
#include <string>
#include <tuple>

class DependencyTree
{
    public:
        DependencyTree();
        DependencyTree(const DependencyTree& tree);
        ~DependencyTree() {}

        void init();

        void add(int h, const std::string & l);
        void set(int k, int h, const std::string & l);

        int get_head(int k);
        const std::string & get_label(int k);
        int get_root();
        bool is_single_root();

        bool is_tree();
        bool is_projective();
        void cal_projective_order_and_mpc();
        void cal_projective_order(
                std::vector<std::vector<int>>& rev_tree,
                int root,
                int& timestamp);
        std::tuple<bool, int, int> cal_mpc(
                std::vector<std::vector<int>>& rev_tree,
                int root);

        void print();

    private:
        bool visit_tree(int w);

    public:
        int n;
        std::vector<int> heads;
        std::vector<std::string> labels;

        std::vector<int> proj_order; // projective word order
        // std::tuple<bool, int, int> mpc;
        std::vector<int> mpc;
        int counter; // for detecting projectivity
};

#endif
