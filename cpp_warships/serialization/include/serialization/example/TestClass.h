#pragma once

#include <serialization/ISerializable.h>
#include <serialization/ISerializer.h>
#include <serialization/example/ImplicitTestClass.h>
#include <serialization/helpers/serializers/JsonStringSerializer.h>
#include <serialization/helpers/type_converters/StringTypeConverter.h>

#include <memory>

namespace cpp_warships::serialization::example {
    using namespace helpers::serializers;
    using namespace helpers::type_converters;

    inline char TestClassTypeName[] = "TestClass";

    class TestClassStringSerializer;

    class TestClass : public ISerializable<TestClassTypeName> {
    private:
        int intPrivateField = 0;
        std::string stringPrivateField = "default";

    public:
        friend TestClassStringSerializer;
        friend std::ostream& operator<<(std::ostream& stream, const TestClass& item);

        TestClass()
            : implicitClass({}) {};
        int intPublicField = 0;
        std::string stringPublicField = "default";
        ImplicitTestClass implicitClass;
    };

    class TestClassStringSerializer : public ISerializer<
                                          std::string,
                                          TestClass,
                                          TestClassTypeName,
                                          ImplicitTestClassStringSerializer> {
    public:
        using ISerializer::ISerializer;
        bool isRelated(std::string item) override {
            return JsonStringSerializer::isIncludeFields(
                       item,
                       "intPublicField",
                       "stringPublicField",
                       "intPrivateField",
                       "stringPrivateField"
                   ) &&
                   std::get<0>(childrenSerializers).isRelated(item);
        }

        std::string serialize(TestClass& item) override {
            auto implicitClassSerialized =
                std::get<0>(childrenSerializers).serialize(item.implicitClass);

            return JsonStringSerializer::serializeFields(
                {{"intPublicField", std::to_string(item.intPublicField)},
                 {"stringPublicField", item.stringPublicField},
                 {"intPrivateField", std::to_string(item.intPrivateField)},
                 {"stringPrivateField", item.stringPrivateField},
                 {"implicitClass", implicitClassSerialized}}
            );
        }

        TestClass deserialize(std::string data) override {
            TestClass restored;

            try {
                readOwnFields(restored, data);
                readNestedClass(restored, data);
            } catch (const std::exception& error) {
                std::cerr << "Deserialization interrupted: \n\t" << error.what() << std::endl;
                throw;
            }

            return restored;
        }

    private:
        static void readOwnFields(TestClass& restored, std::string& data) {
            JsonStringSerializer::setFieldValue<int>(
                &restored.intPublicField,
                data,
                "intPublicField",
                StringTypeConverter::stringToInt
            );
            JsonStringSerializer::setFieldValue(
                &restored.stringPublicField,
                data,
                "stringPublicField"
            );
            JsonStringSerializer::setFieldValue<int>(
                &restored.intPrivateField,
                data,
                "intPrivateField",
                StringTypeConverter::stringToInt
            );
            JsonStringSerializer::setFieldValue(
                &restored.stringPrivateField,
                data,
                "stringPrivateField"
            );
        }

        void readNestedClass(TestClass& restored, std::string& data) {
            const std::unique_ptr<std::string> field{
                JsonStringSerializer::extractFieldValue(data, "implicitClass", true)
            };
            if (field != nullptr) {
                restored.implicitClass = std::get<0>(childrenSerializers).deserialize(*field);
            }
        }
    };

    inline std::ostream& operator<<(std::ostream& stream, const TestClass& item) {
        stream << "TestClass intPublicField: " << item.intPublicField << std::endl;
        stream << "TestClass stringPublicField: " << item.stringPublicField << std::endl;
        stream << "TestClass intPrivateField: " << item.intPrivateField << std::endl;
        stream << "TestClass stringPrivateField: " << item.stringPrivateField << std::endl;
        stream << item.implicitClass << std::endl;

        return stream;
    }
}  // namespace cpp_warships::serialization::example