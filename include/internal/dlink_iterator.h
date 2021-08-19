#ifndef DS_LIST_DLINK_ITERATOR_H
#define DS_LIST_DLINK_ITERATOR_H


#include <iterator>

#include "base_iterator.h"
#include "../dlinked_list.h"


namespace linear::internal::iterators
{
    template <Comparable Tp, bool isConst>
    class dlink_iterator : public base_iterator<Tp, isConst>
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;

        using difference_type = base_iterator<Tp, isConst>::difference_type;
        using value_type = base_iterator<Tp, isConst>::value_type;
        using pointer = base_iterator<Tp, isConst>::pointer;
        using reference = base_iterator<Tp, isConst>::reference;


        // Required operator overloads to satisfy named requirements of LegacyBidirectionalIterator
        [[nodiscard]] constexpr reference operator*() const noexcept override
        { return m_current->m_value; }

        [[nodiscard]] constexpr pointer operator->() const noexcept override
        { return std::addressof(m_current->m_value); }

        constexpr dlink_iterator& operator++() noexcept override
        {
            m_current = m_current->m_next;
            return *this;
        }

        constexpr dlink_iterator operator++(int) noexcept override
        {
            dlink_iterator temp(*this);
            ++(*this);
            return temp;
        }

        constexpr dlink_iterator& operator--() noexcept
        {
            m_current = m_current->m_prev;
            return *this;
        }

        constexpr dlink_iterator operator--(int) noexcept
        {
            dlink_iterator temp(*this);
            --(*this);
            return temp;
        }

        [[nodiscard]] constexpr auto operator<=>(const dlink_iterator&) const noexcept = default;


    private:
        using dlink_node = typename link::details::dlink_node<Tp>;

        constexpr explicit dlink_iterator(const dlink_node *current)
                : m_current(const_cast<dlink_node*>(current)) {}

        dlink_node *m_current;
        friend class dlinked_list<Tp>;

    };  // class dlink_iterator

}   // namespace linear::internal::iterators


#endif //DS_LIST_DLINK_ITERATOR_H
