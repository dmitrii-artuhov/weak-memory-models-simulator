#pragma once

#pragma once

#include <iostream>
#include <sstream>
#include <unordered_map>

#include "storage-subsystem/storage-subsystem.h"

namespace wmm_simulator {

class SRAStorageSubsystem : public StorageSubsystem {
public:
    SRAStorageSubsystem();
    SRAStorageSubsystem(const SRAStorageSubsystem& other);

    int read(
        int thread_id,
        const std::string& location_name,
        MemoryOrder memory_order
    ) override;
    void write(
        int thread_id,
        const std::string& location_name,
        int value,
        MemoryOrder memory_order
    ) override;
    void fence(
        int thread_id,
        MemoryOrder memory_order
    ) override;
    
    StorageSubsystem* make_copy() const override;
    std::vector <std::unique_ptr<StorageSubsystem>> get_eps_transitions(int thread_id) const;
    const std::vector <std::string_view> get_increase_locations(int thread_id) const;
    void increase_location_local_timestamp(int thread_id, const std::string& location_name);
    bool has_eps_transitions(int thread_id) const override;
    std::string get_printable_state() override;
    void prepare_thread(int thread_id) override;

private:
    struct ThreadView {
        ThreadView() = default;
        ThreadView(const std::string& loc, int value) {
            view[loc] = value;
        }

        ThreadView(const ThreadView& other) {
            view = other.view;
        }

        std::string get_printable_state() const {
            std::stringstream ss;
            for (auto& [ loc, timestamp ] : view) {
                ss << loc << " @ " << timestamp << std::endl;
            }
            return ss.str();
        }

        void set_timestamp(const std::string& loc, int timestamp) {
            view[loc] = timestamp;
        }

        const int get_timestamp(const std::string& loc) const {
            if (!view.count(loc)) return 0;
            return view.at(loc);
        }

        std::map<std::string, int> view; // { location, timestamp }
    };

    struct Message {
        std::string location;
        int value;
        MemoryOrder memory_order;
        ThreadView view;
    };

    std::vector<Message> m_memory;
    std::map<int, ThreadView> m_thread_views; // { thread id, { location, timestamp } }
    std::map<std::string, int> m_location_timestamps; // { location, globally max timestamp }
};

}