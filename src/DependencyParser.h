#ifndef __NNDEP_DEPENDENCY_PARSER_H__
#define __NNDEP_DEPENDENCY_PARSER_H__

#include <vector>
// #include <map>
#include <unordered_map>

#include "math/mat.h"
#include "Config.h"
#include "ParsingSystem.h"
#include "Classifier.h"
#include "Configuration.h"

class DependencyParser
{
    public:
        DependencyParser(const char * cfg_filename);
        DependencyParser(std::string& cfg_filename);
        ~DependencyParser(); // TODO

        void train(
                const char * train_file,
                const char * dev_file,
                const char * model_file,
                const char * embed_file,
                const char * premodel_file,
                int sub_sampling = -1);

        void train(
                std::string & train_file,
                std::string & dev_file,
                std::string & model_file,
                std::string & embed_file,
                std::string & premodel_file,
                int sub_sampling = -1);

        void finetune(
                const char * train_file, // target language
                const char * premodel_file,
                const char * model_file,
                const char * emb_file,
                int sub_sampling = -1);

        void finetune(
                std::string & train_file, // target language
                std::string & premodel_file,
                std::string & model_file,
                std::string & emb_file,
                int sub_sampling = -1);

        void extract_transition_sequence(std::string & train_file);
        void extract_transition_sequence(const char * train_file);

        /**
         * if re_precompute is true, then do precomputing
         *  based on test data, ignore that in the readin model
         */
        void test(
                const char * test_file,
                const char * output_file,
                bool re_precompute = false);

        void test(
                std::string & test_file,
                std::string & output_file,
                bool re_precompute = false);

        void gen_dictionaries_graph(
                std::vector<DependencySent> & sents,
                std::vector<DependencyGraph> & graphs);

        void collect_dynamic_features(
                std::vector<DependencySent> & sents,
                std::vector<DependencyGraph> & graphs);

        void setup_classifier_for_training(
                std::vector<DependencySent> & sents,
                std::vector<DependencyGraph> & graphs,
                const char * embed_file,
                const char * premodel_file);

        void read_embed_file(const char * embed_file);

        Dataset gen_train_samples_graph(
                std::vector<DependencySent> & sents,
                std::vector<DependencyGraph> & graphs);

        void scan_test_samples(
                std::vector<DependencySent> & sents,
                std::vector<DependencyGraph> & graphs,
                std::vector<int> & precompute_ids);

        void save_model(const char * filename);
        void save_model(const std::string & filename);

        void load_model(const char * filename, bool re_precompute = false);
        void load_model(const std::string & filename, bool re_precompute = false);

        void load_model_cl(const char * filename, const char * clemb);
        void load_model_cl(
                const std::string & filename,
                const std::string & clemb);

        void predict_graph(
                std::vector<DependencySent>& sents,
                std::vector<DependencyGraph>& graphs);
        void predict_graph(
                DependencySent& sent,
                DependencyGraph& graph);

        std::vector<int> get_features(Configuration& c);
        // Vec<int> get_features_array(Configuration& c);

        int get_word_id(const std::string & s);
        int get_pos_id(const std::string & s);
        int get_label_id(const std::string & s);

        int get_distance_id(const int & d);
        int get_valency_id(const std::string & v);

        int get_cluster_id(const std::string & c);

        void process_headless(Configuration& c);
        void process_headless_search_all(int k, std::vector<Snd_head>& cand_2nd_heads, Configuration& c, int dir);
        void get_best_label(Configuration c, std::string & opt_label, double & opt_score, int arc_dir); // arc_dir is the arc direction

    private:
        void generate_ids();

    private:
        std::vector<std::string> known_words;
        std::vector<std::string> known_poss;
        std::vector<std::string> known_labels;

        std::vector<int> known_distances;
        std::vector<std::string> known_valencies;
        std::vector<std::string> known_clusters;

        std::unordered_map<std::string, int> word_ids;
        std::unordered_map<std::string, int> pos_ids;
        std::unordered_map<std::string, int> label_ids;

        std::unordered_map<int, int> distance_ids;
        std::unordered_map<std::string, int> valency_ids;

        std::unordered_map<std::string, int> cluster_ids;

        std::vector<int> pre_computed_ids;
        NNClassifier * classifier;
        ParsingSystem * system;

        Mat<double> embeddings;
        std::unordered_map<std::string, int> embed_ids;

        Config config;
};

#endif
