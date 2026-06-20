#include <string>
#include <iostream>

#include <cpp_warships/game_saves/include/example/TestClass.h>
#include <cpp_warships/game_saves/include/example/ImplicitTestClass.h>
#include <cpp_warships/game_saves/include/SerializerAggregator.h>

using namespace cpp_warships::game_saves;

int main() {
    examples::TestClass test_class;
    test_class.stringPublicField = "changed";

    SerializerAggregator<std::string> test_serializer;
    test_serializer.setSerializers(
        new examples::TestClassStringSerializer{},
        new examples::ImplicitTestClassStringSerializer{}
    );

    test_class.implicitClass.stringPublicField = "changed-public";

    auto serialized_data = test_serializer.serialize(test_class);
    auto serialized_implicit_data = test_serializer.serialize(test_class.implicitClass);

    std::cout << "1) Serialized data: " << serialized_data << std::endl;
    std::cout << "2) Serialized implicit data: " << serialized_implicit_data << std::endl;
    std::cout << "---- SERIALIZATION FINISHED ----" << std::endl << std::endl;

    std::cout << "---- DESERIALIZATION RESULT ----" << std::endl;
    auto deserialized_test_class = test_serializer.deserialize<examples::TestClass>(serialized_data);
    std::cout << deserialized_test_class << std::endl;

    return 0;
}
