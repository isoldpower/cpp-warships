#include <gtest/gtest.h>
#include <input_parser/builder/ConfigCommandBuilder.h>
#include <input_parser/builder/DefaultParameterBuilder.h>
#include <input_parser/builder/ParameterBuildDirector.h>
#include <input_parser/model/ParserCommandInfo.h>
#include <input_parser/model/ParserParameter.h>

#include <regex>
#include <string>

namespace cpp_warships::input_parser::builder {
    TEST(DefaultParameterBuilderTests, BuildsAParameterFromWhatItWasTold) {
        DefaultParameterBuilder builder;

        const model::ParserParameter parameter = builder.addFlag("--count")
                                                     .setDescription("how many")
                                                     .setNecessary(true)
                                                     .setValidator(std::regex{"^[0-9]+$"})
                                                     .build();

        EXPECT_TRUE(parameter.getIsFlagPresent("--count"));
        EXPECT_EQ(parameter.getDescription(), "how many");
        EXPECT_TRUE(parameter.getNecessary());
        EXPECT_TRUE(parameter.validate("12").first);
        EXPECT_FALSE(parameter.validate("abc").first);
    }

    TEST(DefaultParameterBuilderTests, TakesSeveralFlagsForOneParameter) {
        DefaultParameterBuilder builder;

        const model::ParserParameter parameter = builder.addFlag("--count").addFlag("-c").build();

        EXPECT_TRUE(parameter.getIsFlagPresent("--count"));
        EXPECT_TRUE(parameter.getIsFlagPresent("-c"));
    }

    TEST(DefaultParameterBuilderTests, BuildingTwiceGivesTheSameParameterAgain) {
        DefaultParameterBuilder builder;
        builder.addFlag("--count");

        EXPECT_TRUE(builder.build().getIsFlagPresent("--count"));
        EXPECT_TRUE(builder.build().getIsFlagPresent("--count"));
    }

    TEST(DefaultParameterBuilderTests, BuildingAndResettingStartsFresh) {
        DefaultParameterBuilder builder;
        builder.addFlag("--count").setNecessary(true).setDescription("how many");

        const model::ParserParameter first = builder.buildAndReset();
        const model::ParserParameter second = builder.build();

        EXPECT_TRUE(first.getIsFlagPresent("--count"));
        EXPECT_FALSE(second.getIsFlagPresent("--count"));
        EXPECT_FALSE(second.getNecessary());
        EXPECT_TRUE(second.getDescription().empty());
    }

    TEST(ParameterBuildDirectorTests, BuildsANecessaryParameter) {
        DefaultParameterBuilder builder;
        ParameterBuildDirector director{&builder};

        director.buildNecessary("--count", std::regex{"^[0-9]+$"});
        const model::ParserParameter parameter = builder.build();

        EXPECT_TRUE(parameter.getNecessary());
        EXPECT_TRUE(parameter.getIsFlagPresent("--count"));
        EXPECT_TRUE(parameter.validate("7").first);
    }

    TEST(ParameterBuildDirectorTests, BuildsAnUnnecessaryParameter) {
        DefaultParameterBuilder builder;
        ParameterBuildDirector director{&builder};

        director.buildUnnecessary("--verbose");
        const model::ParserParameter parameter = builder.build();

        EXPECT_FALSE(parameter.getNecessary());
        EXPECT_TRUE(parameter.getIsFlagPresent("--verbose"));
    }

    TEST(ParameterBuildDirectorTests, ResettingClearsTheBuilderBehindIt) {
        DefaultParameterBuilder builder;
        ParameterBuildDirector director{&builder};
        director.buildNecessary("--count");

        director.reset();

        EXPECT_FALSE(builder.build().getIsFlagPresent("--count"));
    }

    TEST(ConfigCommandBuilderTests, BuildsACommandFromWhatItWasTold) {
        ConfigCommandBuilder<void> builder;
        DefaultParameterBuilder parameterBuilder;

        const model::ParserCommandInfoConfig<void> config =
            builder.setDescription("does a thing")
                .addParameter(parameterBuilder.addFlag("--count").build())
                .setResolveAllFlags(true)
                .setCallback([](model::ParsedOptions) {})
                .build();
        const model::ParserCommandInfo<void> command{config};

        EXPECT_EQ(command.getDescription(), "does a thing");
        EXPECT_EQ(command.getParameters().size(), 1U);
        EXPECT_TRUE(command.getResolveAllFlags());
        EXPECT_NE(command.getExecutable(), nullptr);
    }

    TEST(ConfigCommandBuilderTests, KeepsTheErrorAndHelpCallbacks) {
        ConfigCommandBuilder<void> builder;

        const model::ParserCommandInfo<void> command{builder.setDescription("does a thing")
                                                         .setCallback([](model::ParsedOptions) {})
                                                         .setDisplayError([](model::ParsedOptions) {
                                                         })
                                                         .setPrintHelp([](model::ParsedOptions) {})
                                                         .build()};

        EXPECT_NE(command.getErrorDisplay(), nullptr);
        EXPECT_NE(command.getPrintHelp(), nullptr);
    }

    TEST(ConfigCommandBuilderTests, BuildingAndResettingStartsFresh) {
        ConfigCommandBuilder<void> builder;
        builder.setDescription("does a thing").setResolveAllFlags(true);

        const model::ParserCommandInfo<void> first{builder.buildAndReset()};
        const model::ParserCommandInfo<void> second{builder.build()};

        EXPECT_EQ(first.getDescription(), "does a thing");
        EXPECT_TRUE(first.getResolveAllFlags());
        EXPECT_TRUE(second.getDescription().empty());
        EXPECT_FALSE(second.getResolveAllFlags());
    }
}  // namespace cpp_warships::input_parser::builder
