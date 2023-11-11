#pragma once

namespace wmm_simulator {

enum class MemoryOrder {
    SEQUENTIALLY_CONSISTENT,
    RELEASE,
    ACQUIRE,
    RELEASE_ACQUIRE,
    RELAXED
};

}