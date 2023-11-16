#pragma once

#include <string>

#include "lang/memory-order.h"


namespace wmm_simulator {

class StorageSubsystem {
public:
    virtual ~StorageSubsystem() {}
    virtual int read(std::string_view location_name, MemoryOrder memory_order) = 0;
    virtual void write(std::string_view location_name, int value, MemoryOrder memory_order) = 0;
    virtual int read_modify_write(std::string_view location_name, int new_value, MemoryOrder memory_order) = 0;
    virtual void fence(MemoryOrder memory_order) = 0;
};

}