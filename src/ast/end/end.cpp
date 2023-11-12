#include "end.h"


namespace wmm_simulator {

EndNode::EndNode() {}

void EndNode::accept(const Visitor* visitor) const {
    visitor->visit(this);
    std::cout << "EndNode visist" << std::endl;
}

}