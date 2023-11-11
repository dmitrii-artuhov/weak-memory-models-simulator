#pragma once

#include <iostream>
#include <string>

#include "ast/node.h"

namespace wmm_simulator {

class ConditionNode final : public AstNode {
public:
    ConditionNode(std::string&& register_name, std::string&& goto_label);
    void accept() override;
    ~ConditionNode() {
        // TODO: remove
        std::cout << "~ConditionNode()" << std::endl;
    }

private:
    std::string m_register_name;
    std::string m_goto_label;
};

}