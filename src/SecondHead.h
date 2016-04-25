#ifndef __NNDEP_SNDHEAD_H__
#define __NNDEP_SNDHEAD_H__

#include <string>

class Snd_head
{
    public:
        int head = -1;
        double score = -100000;
        std::string label;
};

#endif
