#ifndef DS_LIST_LIST_ITERATOR_H
#define DS_LIST_LIST_ITERATOR_H


#include <iterator>
#include <utility>

#include "base_iterator.h"


namespace linear::internal::iterators
{
    template<Comparable Tp, bool isConst>
    class list_iterator : public base_iterator<Tp, isConst>
    {
    public:
        //*** Member Types ***//
        using iterator_concept = std::contiguous_iterator_tag;
        using iterator_category = std::random_access_iterator_tag;

        using difference_type = base_iterator<Tp, isConst>::difference_type;
        using value_type = base_iterator<Tp, isConst>::value_type;
        using pointer = base_iterator<Tp, isConst>::pointer;
        using reference = base_iterator<Tp, isConst>::reference;


        //*** Member Functions ***//
        constexpr list_iterator() noexcept = default;
        constexpr explicit list_iterator(pointer ptr) noexcept
            : m_ptr(ptr) {}


        // Required operator overloads to fulfill LegacyRandomAccessIterator requirements
        [[nodiscard]] constexpr reference operator*() const noexcept    { return *m_ptr; }
        [[nodiscard]] constexpr pointer operator->() const noexcept     { return m_ptr; }

        constexpr list_iterator& operator++() noexcept
        {
            m_ptr++;
            return *this;
        }

        constexpr list_iterator operator++(int) noexcept
        {
            list_iterator temp(*this);
            ++(*this);
            return temp;
        }

        constexpr list_iterator& operator--() noexcept
        {
            m_ptr--;
            return *this;
        }

        constexpr list_iterator operator--(int) noexcept
        {
            list_iterator temp(*this);
            --(*this);
            return temp;
        }

        constexpr list_iterator& operator+=(const difference_type offset) noexcept
        {
            m_ptr += offset;
            return *this;
        }

        constexpr list_iterator operator+(const difference_type offset) noexcept
        {
            list_iterator temp(*this);
            temp += offset;
            return temp;
        }

        [[nodiscard]] constexpr list_iterator& operator-=(const difference_type offset) noexcept
        { return *this += -offset; }

        constexpr list_iterator operator-(const difference_type offset) noexcept
        {
            list_iterator temp(*this);
            temp -= offset;
            return temp;
        }

        [[nodiscard]] constexpr difference_type operator-(const list_iterator &rhs) const noexcept
        { return m_ptr - rhs.m_ptr; }

        [[nodiscard]] constexpr reference operator[](const difference_type offset) const noexcept
        { return *(*this + offset); }

        // Since only member is a pointer type, <=> should generate all of ==, !=, <, <=, >, >=
        [[nodiscard]] constexpr auto operator<=>(const list_iterator &) const noexcept = default;


    private:
        pointer m_ptr = nullptr;

    };  // class list_iterator

}   // namespace linear::internal::iterators


#endif //DS_LIST_LIST_ITERATOR_H
