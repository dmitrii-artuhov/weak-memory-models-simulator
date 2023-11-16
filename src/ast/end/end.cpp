#include "end.h"


namespace wmm_simulator {

EndNode::EndNode() {}

void EndNode::accept(Visitor* visitor) const {
    visitor->visit(this);
}

}