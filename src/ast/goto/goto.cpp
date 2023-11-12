#include "goto.h"


namespace wmm_simulator {

GotoNode::GotoNode(std::string goto_label): m_goto_label(std::move(goto_label)) {}

void GotoNode::accept(const Visitor* visitor) const {
    visitor->visit(this);
    std::cout << "GotoNode visist" << std::endl;
}

}