#ifndef DS_LIST_TRAITS_H
#define DS_LIST_TRAITS_H


#include <concepts>
#include <cstddef>


namespace linear
{
    static constexpr const std::size_t default_capacity {16};

    template <typename Tp>
    concept Comparable = requires(Tp a, Tp b)
    {
        { a == b } -> std::same_as<bool>;
        { a != b } -> std::same_as<bool>;
        { a < b  } -> std::same_as<bool>;
        { a <= b } -> std::same_as<bool>;
        { a > b  } -> std::same_as<bool>;
        { a >= b } -> std::same_as<bool>;

    };  // concept Comparable

}   // namespace linear


#endif //DS_LIST_TRAITS_H
