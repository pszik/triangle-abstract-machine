#include <optional>

#include "tam/cli.h"

#include <gtest/gtest.h>

TEST(CliTests, ParseHelp) {
    const char* argv[] = {"-h"};
    std::optional<CliArgs> args = ParseCli(1, argv);
    ASSERT_TRUE(args);
    ASSERT_FALSE(args->error);
    ASSERT_TRUE(args->help);
}

TEST(CliTests, ParseFnameOk) {
    const char* argv[] = {"test.tam"};
    std::optional<CliArgs> args = ParseCli(1, argv);
    ASSERT_TRUE(args);
    ASSERT_FALSE(args->help || args->trace || args->step || args->error);
    ASSERT_EQ("test.tam", args->filename);
}

TEST(CliTests, ParseTraceWithFnameOk) {
    const char* argv[] = {"-t", "test.tam"};
    std::optional<CliArgs> args = ParseCli(2, argv);
    ASSERT_TRUE(args);
    ASSERT_FALSE(args->help || args->step || args->error);
    ASSERT_EQ(0, *args->trace);
    ASSERT_EQ("test.tam", args->filename);
}

TEST(CliTests, ParseTraceWithTraceLvl) {
    const char* argv[] = {"-t", "2", "test.tam"};
    std::optional<CliArgs> args = ParseCli(3, argv);
    ASSERT_TRUE(args);
    ASSERT_FALSE(args->help || args->step || args->error);
    ASSERT_EQ(2, *args->trace);
    ASSERT_EQ("test.tam", args->filename);
}

TEST(CliTests, ParseTraceNoFilenameFail) {
    const char* argv[] = {"-t"};
    std::optional<CliArgs> args = ParseCli(1, argv);
    ASSERT_FALSE(args);
}

TEST(CliTests, ParseTraceStepOk) {
    const char* argv[] = {"-t", "-s", "test.tam"};
    std::optional<CliArgs> args = ParseCli(3, argv);
    ASSERT_TRUE(args);
    ASSERT_FALSE(args->help);
    ASSERT_EQ(0, *args->trace);
    ASSERT_TRUE(args->step);
    ASSERT_EQ("test.tam", args->filename);
}
