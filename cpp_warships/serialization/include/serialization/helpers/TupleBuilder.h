#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace cpp_warships::serialization::helpers {
    template <typename T, typename... Ts>
    struct is_one_of;

    template <typename... TChildren>
    struct TupleBuilder;

    /** @brief Helper type trait to check if a type T is in a parameter pack Ts.. @tparam T The
     * type to check. */
    template <typename T, typename... Ts>
    struct is_one_of : std::disjunction<std::is_same<T, Ts>...> {};

    /** @brief Helper to create a tuple with filtered or default-constructed elements @tparam
     * TChildren The types to include in the final output tuple. */
    template <typename... TChildren>
    struct TupleBuilder {
        /** @brief Unified entrypoint to building tuple. */
        template <typename... Arguments>
        static std::tuple<TChildren...> build(Arguments&&... arguments) {
            return buildInOrder(
                std::index_sequence_for<TChildren...>{},
                std::forward<Arguments>(arguments)...
            );
        }

    private:
        /** @brief Helper to find the first argument of type T @tparam T The
         * type to search for in the arguments. */
        template <typename T, typename... Arguments>
        static T selectArgument(Arguments&&... arguments) {
            if constexpr (sizeof...(Arguments) == 0) {
                return T{};
            } else {
                T* result = nullptr;
                (
                    [&]<typename T0>(T0&& argument) {
                        if constexpr (std::is_same_v<std::remove_cvref_t<T0>, T>) {
                            if (result == nullptr) {
                                result = &argument;
                            }
                        }
                    }(arguments),
                    ...);

                return result == nullptr ? T{} : *result;
            }
        }

        /** @brief Build tuple by applying selectArgument for each type in Arguments. */
        template <size_t... Indices, typename... Arguments>
        static std::tuple<TChildren...> buildInOrder(
            std::index_sequence<Indices...>,
            Arguments&&... arguments
        ) {
            return std::tuple<TChildren...>(
                selectArgument<std::tuple_element_t<Indices, std::tuple<TChildren...>>>(
                    std::forward<Arguments>(arguments)...
                )...
            );
        }
    };
}  // namespace cpp_warships::serialization::helpers