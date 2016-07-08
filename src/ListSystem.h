#ifndef __NNDEP_LISTSYSTEM_H__
#define __NNDEP_LISTSYSTEM_H__

#include <vector>
#include "ParsingSystem.h"
#include "Configuration.h"

class ListSystem : public ParsingSystem
{
    public:
        ListSystem() { lang = "english"; }
        explicit ListSystem(std::vector<std::string>& ldict,
                              std::string& language,
                              bool is_labeled = true);
        ~ListSystem();

        void make_transitions();

        bool can_apply(Configuration& c, const std::string& t);

        void apply(Configuration& c, const std::string& t);

        const std::string get_oracle(
                Configuration& c,
                DependencyGraph& graph);

        bool is_oracle(
                Configuration& c,
                std::string& t,
                DependencyGraph& graph);

        bool can_process(DependencyGraph& graph);

        Configuration init_configuration(DependencySent & sent);

        bool is_terminal(Configuration& c);
};

#endif
