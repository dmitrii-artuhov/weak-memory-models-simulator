#include "thread-goto.h"


namespace wmm_simulator {

ThreadGotoNode::ThreadGotoNode(std::string thread_goto_label): m_thread_goto_label(std::move(thread_goto_label)) {}

void ThreadGotoNode::accept(const Visitor* visitor) const {
    visitor->visit(this);
    std::cout << "ThreadGotoNode visist" << std::endl;
}

}