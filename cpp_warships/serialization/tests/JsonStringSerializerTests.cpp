#include <gtest/gtest.h>
#include <serialization/helpers/serializers/JsonStringSerializer.h>
#include <serialization/helpers/type_converters/StringTypeConverter.h>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace cpp_warships::serialization::helpers {
    namespace {
        using serializers::JsonStringSerializer;

        /** @brief Takes ownership of what extractFieldValue hands back. */
        std::unique_ptr<std::string> owned(std::string* extracted) {
            return std::unique_ptr<std::string>{extracted};
        }
    }  // namespace

    TEST(JsonStringSerializerTests, WritesFieldsInAJsonLikeShape) {
        const std::string written =
            JsonStringSerializer::serializeFields({{"onlyField", "a value"}});

        EXPECT_NE(written.find("onlyField: a value"), std::string::npos);
        EXPECT_EQ(written.front(), '{');
        EXPECT_NE(written.find("};"), std::string::npos);
    }

    TEST(JsonStringSerializerTests, WritesEvenAnEmptySetOfFields) {
        const std::string written = JsonStringSerializer::serializeFields({});

        EXPECT_EQ(written, "{\n};\n");
    }

    TEST(JsonStringSerializerTests, ReadsBackAFieldItWrote) {
        std::string written = JsonStringSerializer::serializeFields({{"name", "a value"}});

        const auto value = owned(JsonStringSerializer::extractFieldValue(written, "name"));

        ASSERT_NE(value, nullptr);
        EXPECT_EQ(*value, "a value");
    }

    TEST(JsonStringSerializerTests, FindsEachOfSeveralFields) {
        std::string written =
            JsonStringSerializer::serializeFields({{"first", "one"}, {"second", "two"}});

        const auto first = owned(JsonStringSerializer::extractFieldValue(written, "first"));
        const auto second = owned(JsonStringSerializer::extractFieldValue(written, "second"));

        ASSERT_NE(first, nullptr);
        ASSERT_NE(second, nullptr);
        EXPECT_EQ(*first, "one");
        EXPECT_EQ(*second, "two");
    }

    TEST(JsonStringSerializerTests, ReadsTheOuterFieldWhenANestedOneSharesItsName) {
        std::string written = "{\nnested: {\nname: inner value;\n};\nname: outer value;\n};\n";

        const auto value = owned(JsonStringSerializer::extractFieldValue(written, "name"));

        ASSERT_NE(value, nullptr);
        EXPECT_EQ(*value, "outer value");
    }

    TEST(JsonStringSerializerTests, ReadsOnlyAFieldWithExactlyTheNameAsked) {
        std::string written = "{\nsurname: wrong;\nname: right;\n};\n";

        const auto value = owned(JsonStringSerializer::extractFieldValue(written, "name"));

        ASSERT_NE(value, nullptr);
        EXPECT_EQ(*value, "right");
    }

    TEST(JsonStringSerializerTests, SetsAFieldThatIsThere) {
        std::string written = JsonStringSerializer::serializeFields({{"name", "a value"}});
        std::string field;

        EXPECT_TRUE(JsonStringSerializer::setFieldValue(&field, written, "name"));

        EXPECT_EQ(field, "a value");
    }

    TEST(JsonStringSerializerTests, ConvertsAFieldOnTheWayIn) {
        std::string written = JsonStringSerializer::serializeFields({{"count", "42"}});
        int count = 0;

        EXPECT_TRUE(
            JsonStringSerializer::setFieldValue<int>(
                &count,
                written,
                "count",
                type_converters::StringTypeConverter::stringToInt
            )
        );

        EXPECT_EQ(count, 42);
    }

    TEST(JsonStringSerializerTests, RecognisesWhenEveryNamedFieldIsPresent) {
        const std::string written =
            JsonStringSerializer::serializeFields({{"first", "one"}, {"second", "two"}});

        EXPECT_TRUE(JsonStringSerializer::isIncludeFields(written, "first", "second"));
        EXPECT_FALSE(JsonStringSerializer::isIncludeFields(written, "first", "third"));
    }

    TEST(StringTypeConverterTests, ReadsAWholeNumber) {
        EXPECT_EQ(type_converters::StringTypeConverter::stringToInt("42"), 42);
        EXPECT_EQ(type_converters::StringTypeConverter::stringToInt("-7"), -7);
    }

    TEST(StringTypeConverterTests, ReadsADecimalNumber) {
        EXPECT_FLOAT_EQ(type_converters::StringTypeConverter::stringToFloat("2.5"), 2.5F);
        EXPECT_FLOAT_EQ(type_converters::StringTypeConverter::stringToFloat("-0.5"), -0.5F);
    }

    TEST(StringTypeConverterTests, RefusesSomethingThatIsNotANumber) {
        EXPECT_THROW(
            (void)type_converters::StringTypeConverter::stringToInt("not a number"),
            std::runtime_error
        );
        EXPECT_THROW(
            (void)type_converters::StringTypeConverter::stringToFloat("not a number"),
            std::runtime_error
        );
    }

    TEST(StringTypeConverterTests, RefusesAnEmptyString) {
        EXPECT_THROW(
            (void)type_converters::StringTypeConverter::stringToInt(""),
            std::runtime_error
        );
    }
}  // namespace cpp_warships::serialization::helpers
