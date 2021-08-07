#ifndef DS_LIST_SLINK_ITERATOR_H
#define DS_LIST_SLINK_ITERATOR_H


#include <iterator>
#include <utility>

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

        using difference_type = base_iterator<Tp, isConst>::difference_type;
        using value_type = base_iterator<Tp, isConst>::value_type;
        using pointer = base_iterator<Tp, isConst>::pointer;
        using reference = base_iterator<Tp, isConst>::reference;


        //*** Member Functions ***//

        // Required operator overloads to satisfy named requirements of LegacyForwardIterator
        [[nodiscard]] virtual constexpr reference operator*() const noexcept
        { return m_before->m_next->m_value; }

        [[nodiscard]] virtual constexpr pointer operator->() const noexcept
        { return std::addressof(m_before->m_next->m_value); }

        virtual constexpr slink_iterator& operator++() noexcept
        {
            m_before = m_before->m_next;
            return *this;
        }

        virtual constexpr slink_iterator operator++(int) noexcept
        {
            slink_iterator temp(*this);
            ++(*this);
            return temp;
        }

        [[nodiscard]] constexpr auto operator<=>(const slink_iterator&) const noexcept = default;


    private:
        // Private explicit ctor to enable iterator construction for only friend classes
        explicit slink_iterator(const slink_node<Tp> *before)
                : m_before(const_cast<slink_node<Tp>*>(before)) {}

        slink_node<Tp> *m_before;       // pointer to node before current node (sentinel)
        friend class slinked_list<Tp>;

    };  // class slink_iterator

}   // namespace linear::internal::iterators


#endif //DS_LIST_SLINK_ITERATOR_H
