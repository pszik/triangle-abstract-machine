#ifndef TAM_CLI_H__
#define TAM_CLI_H__

#include <string>

namespace tam {

struct CliArgs {
    std::string Filename;
    bool Trace, Step;
};

CliArgs *parseCli(int Argc, const char **Argv);

} // namespace tam

#endif // TAM_CLI_H__
