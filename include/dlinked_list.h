#ifndef DS_LIST_DLINKED_LIST_H
#define DS_LIST_DLINKED_LIST_H


#include <memory>

#include "internal/dlink_iterator.h"
#include "internal/list_base.h"


namespace linear::link
{
    namespace details
    {
        // Forward declaration of value-holding node for a doubly linked list
        template <Comparable Tp> struct dlink_node;

        // Struct representing pointer-holding base for dlink_node
        // Copy behaviour is explicitly disallowed
        template <Comparable Tp>
        struct dnode_base
        {
            dnode_base() : m_prev(nullptr), m_next(nullptr) {}

            dnode_base(const dnode_base&) = delete;
            dnode_base& operator=(const dnode_base&) = delete;

            dlink_node<Tp> *m_prev, *m_next;

        };  // struct dnode_base


        template <Comparable Tp>
        struct dlink_node : dnode_base<Tp>
        { union{ Tp m_value; }; };  // struct dlink_node

    } // namespace details


    template <Comparable Tp>
    class dlinked_list : public internal::list_base<Tp>
    {
        //*** Private Member Type Aliases ***//
        using dnode_base = typename details::dnode_base<Tp>;
        using dlink_node = typename details::dlink_node<Tp>;


    public:
        //*** Member Types ***//
        using allocator_type = typename internal::list_base<Tp>::allocator_type;
        using size_type = typename internal::list_base<Tp>::size_type;
        using reference = typename internal::list_base<Tp>::reference;
        using const_reference = typename internal::list_base<Tp>::const_reference;

        using iterator = typename internal::iterators::dlink_iterator<Tp, false>;
        using const_iterator = typename internal::iterators::dlink_iterator<Tp, true>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;


        //*** Member Functions ***//
        // Explicit ctor
        explicit dlinked_list(const size_type capacity = default_capacity,
                              allocator_type allocator = {})
            : internal::list_base<Tp>(capacity, allocator),
              m_head(nullptr), 
              m_tail(nullptr){}

        // Extended copy-ctor
        dlinked_list(const dlinked_list &rhs, allocator_type allocator = {})
            : dlinked_list(rhs.m_capacity, allocator)
        { operator=(rhs); }

        // Move ctor
        dlinked_list(dlinked_list &&rhs)
            : dlinked_list(rhs.m_capacity, rhs.m_allocator)
        { operator=(std::move(rhs)); }

        // Extended move ctor
        dlinked_list(dlinked_list &&rhs, allocator_type allocator)
            : dlinked_list(rhs.m_capacity, allocator)
        { operator=(std::move(rhs)); }

        virtual ~dlinked_list() = default;

        dlinked_list& operator=(const dlinked_list&);
        dlinked_list& operator=(dlinked_list&&);
        constexpr void swap(dlinked_list&) noexcept;


        //*** Iterators and Access ***//
        constexpr iterator begin() noexcept override                { return iterator(m_head); }
        constexpr const_iterator begin() const noexcept override    { return const_iterator(m_head); }
        constexpr const_iterator cbegin() const noexcept override   { return begin(); }

        constexpr iterator end() noexcept override                  { return iterator(m_tail); }
        constexpr const_iterator end() const noexcept override      { return const_iterator(m_tail); }
        constexpr const_iterator cend() const noexcept override     { return end(); }

        constexpr reverse_iterator rbegin() noexcept                { return reverse_iterator(end()); }
        constexpr const_reverse_iterator crbegin() const noexcept   { return const_reverse_iterator(end()); }
        constexpr reverse_iterator rend() noexcept                  { return reverse_iterator(begin()); }
        constexpr const_reverse_iterator crend() const noexcept     { return const_reverse_iterator(begin()); }

        constexpr reference front() override                        { return m_head->m_value; }
        constexpr const_reference front() const override            { return m_head->m_value; }
        constexpr reference back() override                         { return m_tail->m_value; }
        constexpr const_reference back() const override             { return m_tail->m_value; }


        //*** Modifiers ***//
        template <class... Args> iterator emplace(const_iterator, Args&&...) override;
        iterator erase(const_iterator, const_iterator) override;

    protected:
        dnode_base *m_head, *m_tail;

    };  // class dlinked_list



    //********* Member Function Implementations *********//

    // Copy assign
    template <Comparable Tp>
    dlinked_list<Tp>& dlinked_list<Tp>::operator=(const dlinked_list<Tp> &rhs)
    {
        if (this != &rhs)
            this->try_assignment(rhs);
        return *this;
    }

    // Move assign
    template <Comparable Tp>
    dlinked_list<Tp> & dlinked_list<Tp>::operator=(dlinked_list<Tp> &&rhs)
    {
        if (this != &rhs)
            this->try_move(rhs);
        return *this;
    }

    // Swap: primitives, pointers, and POD types are std::swap(ed) for a strong exception guarantee
    template <Comparable Tp>
    constexpr void dlinked_list<Tp>::swap(dlinked_list<Tp> &rhs) noexcept
    {
        if (internal::list_base<Tp>::swap(rhs))
        {
            using std::swap;
            swap(rhs.m_head, m_head);
            swap(rhs.m_tail, m_tail);
        }
    }

    template <Comparable Tp>
    template <class ...Args>
    typename dlinked_list<Tp>::iterator dlinked_list<Tp>::emplace(typename dlinked_list<Tp>::const_iterator pos,
                                                                  Args &&...args)
    {
        auto *node = static_cast<dlink_node*>(
                this->m_allocator.resource()->allocate(sizeof(dlink_node), alignof(dlink_node)));
        try
        {
            this->m_allocator.construct(std::addressof(node->m_value),
                                  std::forward<Args>(args)...);
        }

        catch(...)
        {
            this->m_allocator.resource()->deallocate(node, sizeof(dlink_node), alignof(dlink_node)));
            throw;
        }

        auto *temp = pos->m_current->m_prev;
        temp->m_next = node;
        node->m_prev = temp;
        node->m_next = pos->m_current;
        pos->m_current->m_prev = node;

        this->m_size++;
        return pos;
    }

    template <Comparable Tp>
    typename dlinked_list<Tp>::iterator
    dlinked_list<Tp>::erase(typename dlinked_list<Tp>::const_iterator start,
                            typename dlinked_list<Tp>::const_iterator finish)
    {
        auto *source = start->m_current, *target = finish->m_current;

        if (target == nullptr)
            m_tail = start->m_current->m_prev;
        source->m_next = target;
        target->m_prev = source;

        while (source != target)
        {
            auto *curr = source;
            source = source->m_next;
            this->m_size--;
            this->m_allocator.destroy(std::addressof(curr->m_value));
            this->m_allocator.resource()->deallocate(curr, sizeof(dlink_node), alignof(dlink_node));
        }
        return start;
    }

}   // namespace linear::link


#endif //DS_LIST_DLINKED_LIST_H
