#pragma once

#include <tuple>
#include <type_traits>

namespace cpp_warships::game_saves::helpers {
    // Forward declarations to avoid circular dependencies
    template <typename T, typename... Ts>
    struct is_one_of;

    template <typename... TChildren>
    struct TupleBuilder;

    /**
     * @brief Helper type trait to check if a type T is in a parameter pack Ts...
     * @tparam T The type to check.
     * @tparam Ts The parameter pack of types to check against.
     */
    template <typename T, typename... Ts>
    struct is_one_of : std::disjunction<std::is_same<T, Ts>...> {};

    /**
     * @brief Helper to create a tuple with filtered or default-constructed elements
     * @tparam TChildren The types to include in the final output tuple.
     */
    template <typename... TChildren>
    struct TupleBuilder {
        /**
         * @brief Unified entrypoint to building tuple. Builds a tuple of types TChildren from the provided arguments.
         * @tparam Args The pack of types of the arguments to process.
         * @param args The pack of arguments to process.
         * @return A tuple containing the selected or default-constructed values of type TChildren.
         *
         * If an argument of type TChildren is found, it is included in the tuple;
         * If no argument of type TChildren is found, a default-constructed value of that type is included;
         * The order of the tuple matches the order of TChildren;
         */
        template <typename... Args>
        static std::tuple<TChildren...> build(Args&&... args) {
            return build_impl(std::index_sequence_for<TChildren...>{}, std::forward<Args>(args)...);
        }

    private:
        /**
         * @brief Helper to find the first argument of type T
         * @tparam T The type to search for in the arguments.
         * @tparam Args The pack of types of the arguments to process.
         *
         * If none is found, it returns a default-constructed value of type T.
         * Default construction is required to ensure the correct sequence and size of the tuple.
         */
        template <typename T, typename... Args>
        static T select_arg(Args&&... args) {
            if constexpr (sizeof...(Args) == 0) {
                return T{};
            } else {
                T* result{};
                bool found = false;
                ([&]<typename T0>(T0&& arg) {
                    if (!found && std::is_same_v<T0, T> && // Exact type match
                        std::is_constructible_v<T, T0&&>) {
                        result = dynamic_cast<T*>(&arg);
                        found = true;
                    }
                }(args), ...);
                return *result;
            }
        }

        /**
         * @brief Build tuple by applying select_arg for each type in Args.
         */
        template <size_t... Is, typename... Args>
        static std::tuple<TChildren...> build_impl(std::index_sequence<Is...>, Args&&... args) {
            return std::tuple<TChildren...>(select_arg<std::tuple_element_t<Is, std::tuple<TChildren...>>>(std::forward<Args>(args)...)...);
        }
    };
} // namespace cpp_warships::game_saves::helpers