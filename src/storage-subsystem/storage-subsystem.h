#pragma once

#include <string>
#include <unordered_map>

#include "lang/memory-order.h"


namespace wmm_simulator {

class StorageSubsystem {
public:
    virtual ~StorageSubsystem() {}
    virtual int read(
        int thread_id,
        std::string_view location_name,
        MemoryOrder memory_order
    ) = 0;
    virtual void write(
        int thread_id,
        std::string_view location_name,
        int value,
        MemoryOrder memory_order
    ) = 0;
    virtual void fence(
        int thread_id,
        MemoryOrder memory_order
    ) = 0;
    virtual std::unordered_map<std::string, int> get_storage() = 0;
};

}