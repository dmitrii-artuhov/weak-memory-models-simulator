#include "thread-goto.h"


namespace wmm_simulator {

ThreadGotoNode::ThreadGotoNode(std::string thread_goto_label): m_thread_goto_label(std::move(thread_goto_label)) {}

void ThreadGotoNode::accept(Visitor* visitor, ProgramState* state) const {
    visitor->visit(this, state);
}


const std::string_view ThreadGotoNode::get_thread_start_label() const {
    return std::string_view(m_thread_goto_label);
}

}