#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <string_view>
#include <variant>
#include <vector>

// impl. by: https://www.codeproject.com/Tips/5261900/Cplusplus-Lightweight-Parsing-Command-Line-Argumen

namespace wmm_simulator::args {

template <class Options>
struct CommandOptions : Options {
    using PropertyType = std::variant<std::string Options::*, int Options::*, double Options::*, bool Options::*>;
    using Argument = std::pair<std::string, PropertyType>;

    ~CommandOptions() = default;

    Options parse(int argc, const char* argv[]) {
        std::vector<std::string_view> vargv(argv, argv + argc);
        for (int idx = 0; idx < argc; ++idx) {
            for (auto& cbk : callbacks) {
                cbk.second(idx, vargv);
            }
        }

        return static_cast<Options>(*this);
    }

    static std::unique_ptr<CommandOptions> create(std::initializer_list<Argument> args) {
        auto cmdOpts = std::unique_ptr<CommandOptions>(new CommandOptions());
        for (auto arg : args) {
            cmdOpts->register_callback(arg);
        }
        return cmdOpts;
    }

private:
    using callback_t = std::function<void(int, const std::vector<std::string_view>&)>;
    std::map<std::string, callback_t> callbacks;

    CommandOptions() = default;
    CommandOptions(const CommandOptions&) = delete;
    CommandOptions(CommandOptions&&) = delete;
    CommandOptions& operator=(const CommandOptions&) = delete;
    CommandOptions& operator=(CommandOptions&&) = delete;

    void register_callback(std::string name, PropertyType prop) {
        callbacks[name] =
            [this, name, prop](int idx, const std::vector<std::string_view>& argv) {
                if (argv[idx] == name) {
                    std::visit(
                        [this, idx, &argv](auto&& arg) {
                            if (idx < argv.size() - 1) {
                                std::stringstream value;
                                value << argv[idx + 1];
                                value >> this->*arg;
                            }
                        },
                        prop
                    );
                }
            };
    };

    void register_callback(Argument p) {
        return register_callback(p.first, p.second);
    }
};

}