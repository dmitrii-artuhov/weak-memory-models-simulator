#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "ast/node.h"
#include "lang/memory-order.h"

namespace wmm_simulator {

class CasNode final : public AstNode {
public:
    CasNode(
        MemoryOrder memory_order,
        std::string location_name,
        std::string register_expected,
        std::string register_desired
    );
    void accept(Visitor* visitor) const override;

    MemoryOrder get_memory_order() const;
    const std::string_view get_location_name() const;
    const std::string_view get_expected_register() const;
    const std::string_view get_desired_register() const;

private:
    MemoryOrder m_memory_order;
    std::string m_location_name;
    std::string m_register_expected;
    std::string m_register_desired;
};


}