#ifndef DS_LIST_SLINK_ITERATOR_H
#define DS_LIST_SLINK_ITERATOR_H


#include <iterator>

#include "base_iterator.h"
#include "../slinked_list.h"


namespace linear::internal::iterators
{
    template <Comparable Tp, bool isConst>
    class slink_iterator : public base_iterator<Tp, isConst>
    {
    public:
        //*** Member Types ***//
        using iterator_category = std::forward_iterator_tag;

        using difference_type = typename base_iterator<Tp, isConst>::difference_type;
        using value_type = typename base_iterator<Tp, isConst>::value_type;
        using pointer = typename base_iterator<Tp, isConst>::pointer;
        using reference = typename base_iterator<Tp, isConst>::reference;


        //*** Member Functions ***//

        // Required operator overloads to satisfy named requirements of LegacyForwardIterator
        [[nodiscard]] constexpr reference operator*() const noexcept
        { return m_before->m_next->m_value; }

        [[nodiscard]] constexpr pointer operator->() const noexcept
        { return std::addressof(m_before->m_next->m_value); }

        constexpr slink_iterator& operator++() noexcept
        {
            m_before = m_before->m_next;
            return *this;
        }

        constexpr slink_iterator operator++(int) noexcept
        {
            slink_iterator temp(*this);
            ++(*this);
            return temp;
        }

        [[nodiscard]] constexpr auto operator<=>(const slink_iterator&) const noexcept = default;


    private:
        using slink_node = typename link::details::slink_node<Tp>;

        // Private explicit ctor to enable iterator construction for only friend classes
        constexpr explicit slink_iterator(const slink_node *before)
                : m_before(const_cast<slink_node*>(before)) {}

        slink_node *m_before;       // pointer to node before current node (sentinel)
        friend class slinked_list<Tp>;

    };  // class slink_iterator

}   // namespace linear::internal::iterators


#endif //DS_LIST_SLINK_ITERATOR_H
