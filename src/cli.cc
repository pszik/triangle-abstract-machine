#include <cstring>
#include <tam/cli.h>

namespace tam {

static CliArgs *parse1(const char **Argv);
static CliArgs *parse2(const char **Argv);
static CliArgs *parse3(const char **Argv);

CliArgs *parseCli(int Argc, const char **Argv) {
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
        return nullptr;
    }
}

static bool isTraceArg(const char *Arg) {
    return (strcmp(Arg, "-t") == 0 || strcmp(Arg, "--trace") == 0);
}

static bool isStepArg(const char *Arg) {
    return (strcmp(Arg, "-s") == 0 || strcmp(Arg, "--step") == 0);
}

static CliArgs *parse1(const char **Argv) {
    if (isTraceArg(Argv[0]) || isStepArg(Argv[0])) {
        return nullptr;
    }
    return new CliArgs{std::string(Argv[0]), false, false};
}

static CliArgs *parse2(const char **Argv) {
    if (isTraceArg(Argv[0])) {
        return new CliArgs{std::string(Argv[1]), true, false};
    } else if (isStepArg(Argv[0])) {
        return new CliArgs{std::string(Argv[1]), false, true};
    } else if (strcmp(Argv[0], "-ts") == 0 || strcmp(Argv[0], "-st") == 0) {
        return new CliArgs{std::string(Argv[2]), true, true};
    } else {
        return nullptr;
    }
}

static CliArgs *parse3(const char **Argv) {
    if ((isTraceArg(Argv[0]) && isStepArg(Argv[1])) ||
        (isStepArg(Argv[0]) && isTraceArg(Argv[1]))) {
        return new CliArgs{std::string(Argv[2]), true, true};
    } else {
        return nullptr;
    }
}

} // namespace tam
