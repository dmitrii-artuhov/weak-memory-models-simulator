#include "goto.h"


namespace wmm_simulator {

GotoNode::GotoNode(std::string goto_label): m_goto_label(std::move(goto_label)) {}

void GotoNode::accept(Visitor* visitor, ProgramState* state) const {
    visitor->visit(this, state);
}

const std::string_view GotoNode::get_goto_label() const {
    return std::string_view(m_goto_label);
}

}