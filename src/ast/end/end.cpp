#include "end.h"


namespace wmm_simulator {

EndNode::EndNode() {}

void EndNode::accept() {
    std::cout << "EndNode visist" << std::endl;
}

}