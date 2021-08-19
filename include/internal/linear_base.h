#ifndef DS_LIST_LINEAR_BASE_H
#define DS_LIST_LINEAR_BASE_H


#include "base_iterator.h"


namespace linear::internal
{
    template <Comparable Tp>
    class linear_base
    {
    public:
        //*** Member Types ***//
        using value_type = Tp;
        using reference = value_type&;
        using const_reference = const value_type&;
        using size_type = std::size_t;

        using iterator = typename internal::iterators::base_iterator<Tp, false>;
        using const_iterator = typename internal::iterators::base_iterator<Tp, true>;


        //*** Member Functions ***//
        linear_base() = default;

        linear_base(const linear_base&) = default;
        linear_base(linear_base&&) noexcept = default;
        linear_base& operator=(const linear_base&) = default;
        linear_base& operator=(linear_base&&) noexcept = default;
        virtual ~linear_base() = default;


        //*** Element Access ***//
        virtual constexpr iterator begin() noexcept = 0;
        virtual constexpr const_iterator begin() const noexcept = 0;
        virtual constexpr const_iterator cbegin() const noexcept = 0;

        virtual constexpr iterator end() noexcept = 0;
        virtual constexpr const_iterator end() const noexcept = 0;
        virtual constexpr const_iterator cend() const noexcept = 0;

        virtual constexpr reference front() = 0;
        virtual constexpr const_reference front() const = 0;
        virtual constexpr reference back() = 0;
        virtual constexpr const_reference back() const = 0;


        //*** Modifiers ***//
        template <class... Args>
        virtual iterator emplace(const_iterator, Args&&...) = 0;
        virtual iterator erase(const_iterator, const_iterator) = 0;

    };  // class linear_base

}   // namespace linear::internal::container


#endif //DS_LIST_LINEAR_BASE_H
