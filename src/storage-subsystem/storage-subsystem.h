#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>

#include "lang/memory-order.h"

namespace wmm_simulator {

class StorageSubsystem {
public:
    virtual ~StorageSubsystem() {}
    virtual int read(
        int thread_id,
        const std::string& location_name,
        MemoryOrder memory_order
    ) = 0;
    virtual void write(
        int thread_id,
        const std::string& location_name,
        int value,
        MemoryOrder memory_order
    ) = 0;
    virtual void fence(
        int thread_id,
        MemoryOrder memory_order
    ) = 0;
    virtual void finish() {}
    virtual StorageSubsystem* make_copy() const = 0;
    virtual std::vector <std::unique_ptr<StorageSubsystem>> get_eps_transitions(int thread_id) const { return {}; }
    virtual bool has_eps_transitions(int thread_id) const { return false; }
    virtual std::string get_printable_state() = 0;
    virtual void prepare_thread(int thread_id) {} // required to let memory know that such thread already exists (important for SRA)
};

}