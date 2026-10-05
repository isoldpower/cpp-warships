#include <gtest/gtest.h>
#include <input_parser/model/ParserCommandInfo.h>
#include <input_parser/model/ParserParameter.h>

#include <regex>
#include <string>
#include <utility>
#include <vector>

namespace cpp_warships::input_parser::model {
    namespace {
        ParserParameter digitsParameter(bool necessary = false) {
            return ParserParameter{
                {"--count", "-c"},
                std::regex{"^[0-9]+$"},
                "how many",
                necessary
            };
        }
    }  // namespace

    TEST(ParserParameterTests, DefaultsToNothingRequired) {
        const ParserParameter parameter;

        EXPECT_FALSE(parameter.getNecessary());
        EXPECT_TRUE(parameter.getFlags().empty());
        EXPECT_TRUE(parameter.getDescription().empty());
    }

    TEST(ParserParameterTests, KeepsWhatItWasBuiltWith) {
        const ParserParameter parameter = digitsParameter(true);

        EXPECT_TRUE(parameter.getNecessary());
        EXPECT_EQ(parameter.getDescription(), "how many");
        EXPECT_EQ(parameter.getFlags().size(), 2U);
    }

    TEST(ParserParameterTests, RecognisesEachOfItsFlags) {
        const ParserParameter parameter = digitsParameter();

        EXPECT_TRUE(parameter.getIsFlagPresent("--count"));
        EXPECT_TRUE(parameter.getIsFlagPresent("-c"));
        EXPECT_FALSE(parameter.getIsFlagPresent("--other"));
    }

    TEST(ParserParameterTests, AcceptsAValueMatchingItsValidator) {
        const ParserParameter parameter = digitsParameter();

        const std::pair<bool, std::string> result = parameter.validate("42");

        EXPECT_TRUE(result.first);
        EXPECT_EQ(result.second, "42");
    }

    TEST(ParserParameterTests, RejectsAValueThatDoesNotMatch) {
        const ParserParameter parameter = digitsParameter();

        EXPECT_FALSE(parameter.validate("not a number").first);
        EXPECT_FALSE(parameter.validate("").first);
    }

    TEST(ParserParameterTests, HandsTheValueBackWhetherItMatchesOrNot) {
        const ParserParameter parameter = digitsParameter();

        EXPECT_EQ(parameter.validate("nope").second, "nope");
    }

    TEST(ParserCommandInfoTests, KeepsWhatItWasConfiguredWith) {
        const ParserCommandInfoConfig<void> config{
            "does a thing",
            {digitsParameter()},
            [](ParsedOptions) {}
        };
        const ParserCommandInfo<void> command{config};

        EXPECT_EQ(command.getDescription(), "does a thing");
        EXPECT_EQ(command.getParameters().size(), 1U);
        EXPECT_FALSE(command.getResolveAllFlags());
    }

    TEST(ParserCommandInfoTests, TheShortConfigurationLeavesTheExtrasEmpty) {
        const ParserCommandInfo<void> command{
            ParserCommandInfoConfig<void>{"does a thing", {}, [](ParsedOptions) {}}
        };

        EXPECT_EQ(command.getErrorDisplay(), nullptr);
        EXPECT_EQ(command.getPrintHelp(), nullptr);
    }

    TEST(ParserCommandInfoTests, TheFullConfigurationKeepsEveryCallback) {
        bool isErrorShown = false;
        bool isHelpShown = false;
        const ParserCommandInfoConfig<void> config{
            "does a thing",
            {},
            [](ParsedOptions) {},
            [&isErrorShown](ParsedOptions) { isErrorShown = true; },
            true,
            [&isHelpShown](ParsedOptions) { isHelpShown = true; }
        };
        const ParserCommandInfo<void> command{config};

        EXPECT_TRUE(command.getResolveAllFlags());
        ASSERT_NE(command.getErrorDisplay(), nullptr);
        ASSERT_NE(command.getPrintHelp(), nullptr);

        command.getErrorDisplay()({});
        command.getPrintHelp()({});

        EXPECT_TRUE(isErrorShown);
        EXPECT_TRUE(isHelpShown);
    }

    TEST(ParserCommandInfoTests, HandsBackAnExecutableThatCanBeCalled) {
        int calledWith = 0;
        const ParserCommandInfo<int> command{
            ParserCommandInfoConfig<int>{"counts", {}, [&calledWith](ParsedOptions options) {
                                             calledWith = static_cast<int>(options.size());
                                             return calledWith;
                                         }}
        };

        EXPECT_EQ(command.getExecutable()({{"a", "1"}, {"b", "2"}}), 2);
        EXPECT_EQ(calledWith, 2);
    }
}  // namespace cpp_warships::input_parser::model
