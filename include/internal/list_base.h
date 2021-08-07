#ifndef DS_LIST_LIST_BASE_H
#define DS_LIST_LIST_BASE_H


#include <limits>
#include <memory_resource>

#include "base_iterator.h"
#include "linear_base.h"


namespace linear::internal
{
    template <Comparable Tp>
    class list_base : public linear_base<Tp>
    {
    public:
        //*** Member Types ***//
        using reference = typename linear_base<Tp>::reference;
        using const_reference = typename linear_base<Tp>::const_reference;
        using size_type = typename linear_base<Tp>::size_type;
        using difference_type = std::ptrdiff_t;

        using iterator = linear_base<Tp>::iterator;
        using const_iterator = linear_base<Tp>::const_iterator;
        using allocator_type = std::pmr::polymorphic_allocator<std::byte>;


        //*** Member Functions ***//
        // Explicit ctor
        explicit list_base(const size_type capacity = default_capacity,
                           allocator_type allocator = {})
            : m_capacity(capacity),
              m_size(0),
              m_allocator(allocator) {}

        // Extended copy ctor
        list_base(const list_base &rhs, allocator_type allocator = {})
            : m_capacity(rhs.m_capacity),
              m_size(rhs.m_size),
              m_allocator(allocator) {}

        // Move ctor
        list_base(list_base &&rhs)
            : list_base(rhs.m_allocator)
        { operator=(std::move(rhs)); }

        // Extended move ctor
        list_base(list_base &&rhs, allocator_type allocator)
            : list_base(allocator)
        { operator=(std::move(rhs)); }

        virtual ~list_base() { erase(begin(), end()); }

        list_base& operator=(const list_base&) = default;
        list_base& operator=(list_base&&) noexcept = default;

        virtual constexpr void swap(list_base&) noexcept;


        //*** Access ***//
        [[nodiscard]] constexpr size_type capacity() const noexcept     { return m_capacity; }
        [[nodiscard]] constexpr size_type size() const noexcept         { return m_size; }
        [[nodiscard]] size_type max_size() const noexcept               { return std::numeric_limits<difference_type>::max(); }
        [[nodiscard]] constexpr bool empty() const noexcept             { return m_size == 0; }
        [[nodiscard]] allocator_type get_allocator() const noexcept     { return m_allocator; }


        //*** Modifiers ***//
        constexpr void push_back(const Tp &value)           { emplace(end(), value); }
        constexpr void push_back(Tp &&value)                { emplace(end(), std::move(value)); }
        constexpr void push_front(const Tp &value)          { emplace(begin(), value); }
        constexpr void push_front(Tp &&value)               { emplace(begin(), std::move(value)); }
        constexpr void pop_front()                          { erase(end()); }
        constexpr void pop_back()                           { erase(begin()); }

        template <class... Args> reference emplace_back(Args &&...args)   { emplace(end(), std::forward(args)...); }
        template <class... Args> reference emplace_front(Args &&...args)  { emplace(begin(), std::forward(args)...); }

        iterator insert(const_iterator pos, const Tp &value)    { return emplace(pos, value); }
        iterator insert(const_iterator pos, Tp &&value)         { return emplace(pos, std::move(value)); }
        iterator erase(const_iterator pos)                      { const_iterator ci = pos; return erase(pos, ++ci); }


    protected:
        size_type m_capacity, m_size;
        allocator_type m_allocator;


        void try_assignment(const list_base&);
        void try_move(list_base&&);

    };  // class list_base



    //****** Member Function Implementations ******//

    // Helper function for copy assignment
    template <Comparable Tp>
    void list_base<Tp>::try_assignment(const list_base<Tp> &rhs)
    {
        erase(begin(), end());
        for (const auto &e : rhs)
            push_back(e);
    }


    // Helper function for move assignment
    template <Comparable Tp>
    void list_base<Tp>::try_move(list_base &&rhs)
    {
        if (rhs.m_allocator == m_allocator)
        {
            erase(begin(), end());
            swap(rhs);
        }
        else
            try_assignment(rhs);
    }


    template <Comparable Tp>
    constexpr void list_base<Tp>::swap(list_base<Tp> &rhs) noexcept
    {
        if (rhs.m_allocator == m_allocator)
        {
            using std::swap;
            swap(rhs.m_capacity, m_capacity);
            swap(rhs.m_size, m_size);
        }
    }

}   // namespace linear::internal::container


#endif //DS_LIST_LIST_BASE_H
