# Compile files
add_library(serialization
        # Main Entities
        src/serialization/source.cpp
        include/serialization/ISerializable.h
        include/serialization/ISerializer.h
        include/serialization/SerializerAggregator.h
        # Custom Exceptions
        include/serialization/exceptions/SerializationException.h
        include/serialization/exceptions/DeserializationException.h
        include/serialization/exceptions/InterpretationException.h
        # Helpers
        include/serialization/helpers/TupleBuilder.h
        include/serialization/helpers/serializers/JsonStringSerializer.h
        include/serialization/helpers/type_converters/StringTypeConverter.h
        # Example Classes
        include/serialization/example/TestClass.h
        include/serialization/example/ImplicitTestClass.h
)

# Tests setup
if (BUILD_TESTING)
    add_executable(serialization_tests tests/main.cpp)
    target_link_libraries(serialization_tests PRIVATE GTest::gtest_main serialization)
    include(GoogleTest)
    gtest_discover_tests(serialization_tests)
endif()

# Expose settings
target_include_directories(serialization
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(serialization PUBLIC cxx_setup)
