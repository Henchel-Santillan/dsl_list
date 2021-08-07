#ifndef DS_LIST_SLINKED_LIST_H
#define DS_LIST_SLINKED_LIST_H


#include <algorithm>
#include <memory>
#include <memory_resource>
#include <stdexcept>
#include <utility>

#include "internal/list_base.h"
#include "internal/slink_iterator.h"


namespace linear::link
{
    namespace details
    {
        template <Comparable Tp> struct slink_node;     // forward declare slink_node struct

        template <Comparable Tp>
        struct snode_base
        {
            snode_base() : m_next(nullptr) {}

            // Explicitly disable copy constructible and copy assignable behaviour
            snode_base(const snode_base&) = delete;
            snode_base& operator=(const snode_base&) = delete;

            slink_node<Tp> *m_next;

        };  // struct snode_base

        template <Comparable Tp>
        struct slink_node : snode_base<Tp>
        {
            union { Tp m_value; };   // anonymous union to correctly align raw bytes

        };  // struct slink_node

    }   // namespace details


    template <Comparable Tp>
    class slinked_list : public internal::list_base<Tp>
    {
        //*** Private Member Type Aliases ***//
        using snode_base = typename details::snode_base<Tp>;
        using slink_node = typename details::slink_node<Tp>;


    public:
        //*** Member Types ***//
        using allocator_type = typename internal::list_base<Tp>::allocator_type;
        using size_type = typename internal::list_base<Tp>::size_type;
        using reference = typename internal::list_base<Tp>::reference;
        using const_reference = typename internal::list_base<Tp>::const_reference;

        using iterator = internal::iterators::slink_iterator<Tp, false>;
        using const_iterator = internal::iterators::slink_iterator<Tp, true>;


        //*** Member Functions ***//
        // Explicit ctor
        explicit slinked_list(const size_type capacity = default_capacity,
                              allocator_type allocator = {})
                : internal::list_base<Tp>(capacity), m_head(),
                  m_tail_ptr(nullptr), m_allocator(allocator) {}

        // Extended copy-ctor
        slinked_list(const slinked_list &rhs, allocator_type allocator = {})
                : slinked_list(allocator)
        { operator=(rhs); }

        // Move ctor
        slinked_list(slinked_list &&rhs)
                : slinked_list(rhs.m_allocator)
        { operator=(std::move(rhs)); }

        // Extended move ctor
        slinked_list(slinked_list &&rhs, allocator_type allocator)
                : slinked_list(allocator)
        { operator=(std::move(rhs)); }

        virtual ~slinked_list() = default;

        slinked_list& operator=(const slinked_list&);
        slinked_list& operator=(slinked_list&&);
        constexpr void swap(slinked_list&) noexcept;


        //*** Iterators and Access  ***//
        constexpr iterator begin() noexcept override                          { return iterator(&m_head); }
        constexpr const_iterator begin() const noexcept override              { return const_iterator(&m_head); }
        constexpr const_iterator cbegin() const noexcept override             { return begin(); }
        constexpr iterator end() noexcept override                            { return iterator(m_tail_ptr); }
        constexpr const_iterator end() const noexcept override                { return const_iterator(m_tail_ptr); }
        constexpr const_iterator cend() const noexcept override               { return end(); }

        constexpr reference front() override                        { return m_head.m_next->m_value; }
        constexpr const_reference front() const override            { return m_head.m_next->m_value; }
        constexpr reference back() override                         { return m_tail_ptr->m_value; }
        constexpr const_reference back() const override             { return m_tail_ptr->m_value; }

        template <class... Args>
        iterator emplace(const_iterator, Args&&...) override;
        iterator erase(const_iterator, const_iterator) override;


    protected:
        snode_base      m_head;               // Sentinel node to head of the list
        snode_base      *m_tail_ptr;          // Pointer to tail node

    };  // class slinked_list



    //********* Member Function Implementations *********//

    // Copy assign
    template <Comparable Tp>
    slinked_list<Tp>& slinked_list<Tp>::operator=(const slinked_list<Tp> &rhs)
    {
        if (this != &rhs)
            this->try_assignment(rhs);
        return *this;
    }

    // Move assign
    template <Comparable Tp>
    slinked_list<Tp>& slinked_list<Tp>::operator=(slinked_list<Tp> &&rhs)
    {
        if (this != &rhs)
            this->try_move(rhs);
        return *this;
    }

    // Swap
    template <Comparable Tp>
    constexpr void slinked_list<Tp>::swap(slinked_list<Tp> &rhs) noexcept
    {
        if (internal::list_base<Tp>::swap(rhs))
        {
            using std::swap;
            auto *lhs_tail = rhs.empty() ? &m_head : rhs.m_tail_ptr;
            auto *rhs_tail = empty() ? &rhs.m_head : m_tail_ptr;
            swap(m_head.m_next, rhs.m_head.m_next);
            m_tail_ptr = lhs_tail;
            rhs.m_tail_ptr = rhs_tail;
        }
    }

    template <Comparable Tp>
    template <class... Args>
    typename slinked_list<Tp>::iterator slinked_list<Tp>::emplace(typename slinked_list<Tp>::const_iterator pos, Args &&...args)
    {
        auto *node = static_cast<slink_node*>(
                m_allocator.resource()->allocate(sizeof(slink_node), alignof(slink_node)));
        try
        {
            m_allocator.construct(std::addressof(node->m_value),
                                  std::forward<Args>(args)...);
        }

        catch(...)
        {
            m_allocator.resource()->deallocate(node, sizeof(slink_node), alignof(slink_node));
            throw;
        }

        node->m_next = pos.m_before->m_next;
        pos.m_before->m_next = node;
        if (pos.m_before == m_tail_ptr)
            m_tail_ptr = node;

        this->m_size++;
        return pos;
    }

    template <Comparable Tp>
    typename slinked_list<Tp>::iterator
    slinked_list<Tp>::erase(typename slinked_list<Tp>::const_iterator start,
                            typename slinked_list<Tp>::const_iterator finish)
    {
        auto *er_fwd = start.m_before->m_next, *er_rev = finish.m_before->m_next;

        if (er_rev == nullptr)
            m_tail = start.m_before;
        start.m_before->m_next = er_rev;
        while (er_fwd != er_rev)
        {
            auto *curr = er_fwd;
            er_fwd = er_fwd->m_next;
            this->m_size--;
            m_allocator.destroy(std::addressof(curr->m_value));
            m_allocator.resource()->deallocate(curr, sizeof(slink_node), alignof(slink_node));
        }
        return start;
    }


    //********* Non-Member Function Implementations *********//

    template <Comparable Tp>
    constexpr void swap(slinked_list<Tp> &lhs, slinked_list<Tp> &rhs) noexcept
    { lhs.swap(rhs); }

    template <Comparable Tp>
    constexpr bool operator==(const slinked_list<Tp> &lhs, const slinked_list<Tp> &rhs) noexcept
    {
        if (lhs.size() != rhs.size()) return false;
        return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <Comparable Tp>
    [[nodiscard]] constexpr bool operator!=(const slinked_list<Tp> &lhs, const slinked_list<Tp> &rhs) noexcept
    { return !operator==(lhs, rhs);  }

}   // namespace linear::link


#endif //DS_LIST_SLINKED_LIST_H
