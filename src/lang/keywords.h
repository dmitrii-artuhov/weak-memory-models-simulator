#pragma once

#include <string>

namespace wmm_simulator {

struct Keyword {
    inline static const std::string CONDITION = "if";
    inline static const std::string GOTO = "goto";
    inline static const std::string THREAD_GOTO = "thread_goto";
    
    inline static const std::string LOAD = "load";
    inline static const std::string STORE = "store";
    inline static const std::string CAS = "cas";
    inline static const std::string FAI = "fai";
    inline static const std::string FENCE = "fence";
    
    inline static const std::string SEQ_CST = "SEQ_CST";
    inline static const std::string REL = "REL";
    inline static const std::string ACQ = "ACQ";
    inline static const std::string REL_ACQ = "REL_ACQ";
    inline static const std::string RLX = "RLX";
};

}
