#include "end.h"


namespace wmm_simulator {

EndNode::EndNode() {}

void EndNode::accept(Visitor* visitor, ProgramState* state) const {
    visitor->visit(this, state);
}

}