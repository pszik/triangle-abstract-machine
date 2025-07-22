#include <cstring>
#include <string>

static char **parseFilename(char **Argv, std::string *Filename);
static char **parseTrace(char **Argv, bool *Trace, bool *Step);
static char **parseTrace1(char **Argv, bool *Trace);
static char **parseTrace2(char **Argv, bool *Step);
static char **parseStep(char **Argv, bool *Trace, bool *Step);
static char **parseStep1(char **Argv, bool *Trace);
static char **parseStep2(char **Argv, bool *Step);

void parseCli(char **Argv, std::string *Filename, bool *Trace, bool *Step) {
    if (strcmp(Argv[0], "-t") == 0 || strcmp(Argv[0], "--trace") == 0) {
        Argv = parseTrace(Argv, Trace, Step);
    } else if (strcmp(Argv[0], "-t") == 0 || strcmp(Argv[0], "--trace") == 0) {
        Argv = parseStep(Argv, Trace, Step);
    }
    parseFilename(Argv, Filename);
}

static char **parseFilename(char **Argv, std::string *Filename) {
    *Filename = std::string(Argv[0]);
    return Argv + 1;
}

static char **parseTrace(char **Argv, bool *Trace, bool *Step) {
    Argv = parseTrace1(Argv, Trace);
    return parseTrace2(Argv, Step);
}

static char **parseTrace1(char **Argv, bool *Trace) {
    *Trace = (strcmp(Argv[0], "-t") == 0 || strcmp(Argv[0], "--trace") == 0);
    return Argv + 1;
}

static char **parseTrace2(char **Argv, bool *Step) {
    if (strcmp(Argv[0], "-s") == 0 || strcmp(Argv[0], "--step") == 0) {
        return parseStep1(Argv, Step);
    }
    return Argv;
}

static char **parseStep(char **Argv, bool *Trace, bool *Step) {
    Argv = parseStep1(Argv, Step);
    return parseStep2(Argv, Trace);
}

static char **parseStep1(char **Argv, bool *Step) {
    *Step = (strcmp(Argv[0], "-s") == 0 || strcmp(Argv[0], "--step") == 0);
    return Argv + 1;
}

static char **parseStep2(char **Argv, bool *Trace) {
    if (strcmp(Argv[0], "-t") == 0 || strcmp(Argv[0], "--trace") == 0) {
        return parseTrace1(Argv, Trace);
    }
    return Argv;
}
