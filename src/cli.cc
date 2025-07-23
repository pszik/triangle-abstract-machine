#include <cstring>
#include <optional>
#include <tam/cli.h>

namespace tam {

static std::optional<CliArgs> parse1(const char **Argv);
static std::optional<CliArgs> parse2(const char **Argv);
static std::optional<CliArgs> parse3(const char **Argv);

std::optional<CliArgs> parseCli(int Argc, const char **Argv) {
    switch (Argc) {
    case 2:
        return parse1(Argv + 1);
        break;
    case 3:
        return parse2(Argv + 1);
        break;
    case 4:
        return parse3(Argv + 1);
        break;
    default:
        return {};
    }
}

static inline bool isTraceArg(const char *Arg) {
    return (strcmp(Arg, "-t") == 0 || strcmp(Arg, "--trace") == 0);
}

static inline bool isStepArg(const char *Arg) {
    return (strcmp(Arg, "-s") == 0 || strcmp(Arg, "--step") == 0);
}

static std::optional<CliArgs> parse1(const char **Argv) {
    if (isTraceArg(Argv[0]) || isStepArg(Argv[0])) {
        return {};
    }
    if (strcmp(Argv[0], "-h") == 0 || strcmp(Argv[0], "--help") == 0) {
        return CliArgs{"", false, false, true};
    }
    return CliArgs{std::string(Argv[0]), false, false, false};
}

static std::optional<CliArgs> parse2(const char **Argv) {
    if (isTraceArg(Argv[0])) {
        return CliArgs{std::string(Argv[1]), true, false, false};
    } else if (isStepArg(Argv[0])) {
        return CliArgs{std::string(Argv[1]), false, true, false};
    } else if (strcmp(Argv[0], "-ts") == 0 || strcmp(Argv[0], "-st") == 0) {
        return CliArgs{std::string(Argv[2]), true, true, false};
    } else {
        return {};
    }
}

static std::optional<CliArgs> parse3(const char **Argv) {
    if ((isTraceArg(Argv[0]) && isStepArg(Argv[1])) ||
        (isStepArg(Argv[0]) && isTraceArg(Argv[1]))) {
        return CliArgs{std::string(Argv[2]), true, true, false};
    } else {
        return {};
    }
}

} // namespace tam
