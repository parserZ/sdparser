#ifndef __NNDEP_NONPROJ_NIVRE_H__
#define __NNDEP_NONPROJ_NIVRE_H__

#include <vector>
#include "ParsingSystem.h"
#include "Configuration.h"

class NonProjNivre : public ParsingSystem
{
    public:
        NonProjNivre() { lang = "english"; }
        explicit NonProjNivre(std::vector<std::string>& ldict,
                              std::string& language,
                              bool is_labeled = true);
        ~NonProjNivre();

        void make_transitions();

        bool can_apply(Configuration& c, const std::string& t);

        void apply(Configuration& c, const std::string& t);

        const std::string get_oracle(
                Configuration& c,
                DependencyTree& tree);

        bool is_oracle(
                Configuration& c,
                std::string& t,
                DependencyTree& tree);

        bool can_process(DependencyTree& tree);

        Configuration init_configuration(DependencySent& sent);

        bool is_terminal(Configuration& c);
};

#endif
