#include <gtest/gtest.h>

#include <iostream>
#include <option_parser.hpp>
#include <string>
#include <vector>

TEST(Option, NoNames)
{
    EXPECT_THROW(
        Option<int>({}, OptionArg::NONE, "invalid option with no name"),
        std::invalid_argument);
}

TEST(Option, InvalidName)
{
    EXPECT_THROW(Option<int>({"valid", "[(*)]invalid"}, OptionArg::NONE,
                             "invalid option with no name"),
                 std::invalid_argument);
}

TEST(Parse, ShortName)
{
    char *argv[] = {(char *)"program.exe", (char *)"-x", (char *)"10", NULL};
    int argc = sizeof(argv) / sizeof(char *) - 1;

    OptionParser op;
    Option<int> option = Option<int>({"x"}, OptionArg::REQ, "desc");
    op.AddOption(&option);
    op.Parse(argc, argv);
    
    EXPECT_EQ(option.Value(), 10);
}

TEST(Parse, ShortNameBind)
{
    char *argv[] = {(char *)"program.exe", (char *)"-x", (char *)"10", NULL};
    int argc = sizeof(argv) / sizeof(char *) - 1;

    int x = 0;
    OptionParser op;
    Option<int> option = Option<int>({"x"}, OptionArg::REQ, "desc").Bind(&x);
    op.AddOption(&option);
    op.Parse(argc, argv);

    EXPECT_EQ(x, 10);
}

TEST(Parse, LongName)
{
    char *argv[] = {(char *)"program.exe", (char *)"--percent", (char *)"0.5", NULL};
    int argc = sizeof(argv) / sizeof(char *) - 1;

    OptionParser op;
    Option<float> option = Option<float>({"percent"}, OptionArg::REQ, "desc");
    op.AddOption(&option);
    op.Parse(argc, argv);

    EXPECT_EQ(option.Value(), 0.5);
}