#pragma once

#include <iostream>
#include <string>

#include "ast/node.h"
#include "visitor/visitor.h"

namespace wmm_simulator {

class ThreadGotoNode final : public AstNode {
public:
    ThreadGotoNode(std::string thread_goto_label);
    void accept(Visitor* visitor) const override;
    ~ThreadGotoNode() {
        // TODO: remove
        std::cout << "~ThreadGotoNode()" << std::endl;
    }

    const std::string_view get_thread_start_label() const;

private:
    std::string m_thread_goto_label;
};

}