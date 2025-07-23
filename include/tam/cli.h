#ifndef TAM_CLI_H__
#define TAM_CLI_H__

#include <optional>
#include <string>

namespace tam {

struct CliArgs {
    std::string Filename;
    bool Trace, Step, Help;
};

std::optional<CliArgs> parseCli(int Argc, const char **Argv);

} // namespace tam

#endif // TAM_CLI_H__
