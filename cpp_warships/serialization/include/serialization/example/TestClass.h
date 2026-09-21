#pragma once

#include <serialization/example/ImplicitTestClass.h>
#include <serialization/ISerializer.h>
#include <serialization/ISerializable.h>
#include <serialization/helpers/serializers/JsonStringSerializer.h>
#include <serialization/helpers/type_converters/StringTypeConverter.h>

namespace cpp_warships::game_saves::examples {
    using namespace helpers::serializers;
    using namespace helpers::type_converters;

    // Test class serializer TName definition.
    inline char TestClassTypeName[] = "TestClass";

    // Forward declaration of TestClass and TestClassStringSerializer to avoid circular dependency.
    class TestClassStringSerializer;

    // Test class that implements ISerializable and contains both public and private fields.
    class TestClass : public ISerializable<TestClassTypeName> {
    private:
        int intPrivateField = 0;
        std::string stringPrivateField = "default";
    public:
        friend TestClassStringSerializer;
        friend std::ostream& operator<<(std::ostream& os, const TestClass& obj);

        TestClass() : implicitClass({}) {};
        int intPublicField = 0;
        std::string stringPublicField = "default";
        ImplicitTestClass implicitClass;
    };

    // Serializer for TestClass with std::string serialization implementation.
    class TestClassStringSerializer
        : public ISerializer<std::string, TestClass, TestClassTypeName, ImplicitTestClassStringSerializer>
    {
    public:
        using ISerializer::ISerializer;
        bool isRelated(std::string item) override {
            return JsonStringSerializer::isIncludeFields(
                item,
                "intPublicField",
                "stringPublicField",
                "intPrivateField",
                "stringPrivateField"
            ) && std::get<0>(childrenSerializers).isRelated(item);
        }

        std::string serialize(TestClass& item) override {
            auto implicitClassSerialized = std::get<0>(childrenSerializers).serialize(item.implicitClass);

            return JsonStringSerializer::serializeFields({
                { "intPublicField", std::to_string(item.intPublicField) },
                { "stringPublicField", item.stringPublicField },
                { "intPrivateField", std::to_string(item.intPrivateField) },
                { "stringPrivateField", item.stringPrivateField },
                { "implicitClass", implicitClassSerialized }
            });
        }

        TestClass deserialize(std::string data) override {
            auto* testClass = new TestClass();

            try {
                JsonStringSerializer::setFieldValue<int>(&testClass->intPublicField, data, "intPublicField", StringTypeConverter::stringToInt);
                JsonStringSerializer::setFieldValue(&testClass->stringPublicField, data, "stringPublicField");
                JsonStringSerializer::setFieldValue<int>(&testClass->intPrivateField, data, "intPrivateField", StringTypeConverter::stringToInt);
                JsonStringSerializer::setFieldValue(&testClass->stringPrivateField, data, "stringPrivateField");

                if (const auto field = JsonStringSerializer::extractFieldValue(data, "implicitClass", true)) {
                    testClass->implicitClass = std::get<0>(childrenSerializers).deserialize(*field);
                    delete field;
                }
            } catch (std::exception& e) {
                std::cerr << "Deserialization interrupted: \n\t" << e.what() << std::endl;

                delete testClass;
                throw e;
            }

            return *testClass;
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const TestClass& obj) {
        os << "TestClass intPublicField: " << obj.intPublicField << std::endl;
        os << "TestClass stringPublicField: " << obj.stringPublicField << std::endl;
        os << "TestClass intPrivateField: " << obj.intPrivateField << std::endl;
        os << "TestClass stringPrivateField: " << obj.stringPrivateField << std::endl;
        os << obj.implicitClass << std::endl;

        return os;
    }
} // namespace cpp_warships::game_saves::examples