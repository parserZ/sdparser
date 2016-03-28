#include <iostream>
#include <fstream>
#include <vector>

#include "DependencyTree.h"
#include "DependencySent.h"
#include "Util.h"

using namespace std;

void test_nonproj_oracle(int argc, char** argv)
{
    vector<DependencyTree> trees;
    vector<DependencySent> sents;

    string train_file = argv[1];
    Util::load_conll_file(train_file.c_str(), sents, trees, true);

    for (auto iter = trees.begin(); iter != trees.end(); ++ iter)
    {
        iter->cal_projective_order_and_mpc();
        iter->print();
    }
}

void test_multiroot(int argc, char** argv)
{
    vector<DependencyTree> trees;
    vector<DependencySent> sents;

    string train_file = argv[1];
    Util::load_conll_file(train_file.c_str(), sents, trees, true);

    int num_single_root = 0;
    int num_projective  = 0;
    int num_multi_root_proj = 0;

    for (auto iter = trees.begin(); iter != trees.end(); ++ iter)
    {
        if (!iter->is_single_root() && iter->is_projective())
        {
            cerr << "## multi-root but projective ##" << endl;
            iter->print();
            num_multi_root_proj += 1;
        }

        if (iter->is_single_root()) num_single_root += 1;
        if (iter->is_projective())  num_projective  += 1;
    }

    cout << "Total trees #:" << trees.size()    << endl;
    cout << "Single root #:" << num_single_root << endl;
    cout << "Projective #:"  << num_projective  << endl;
    cout << "Multiple root & Projective #:" << num_multi_root_proj << endl;
}

void gen_badcase(int argc, char** argv)
{
    vector<DependencyTree> trees;
    vector<DependencySent> sents;

    string train_file = argv[1];
    Util::load_conll_file(train_file.c_str(), sents, trees, true);

    // for ()
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cerr << "Arguments: [filename]" << endl;
        exit(-1);
    }

    test_nonproj_oracle(argc, argv);
    // test_multiroot(argc, argv);

    return 0;
}

