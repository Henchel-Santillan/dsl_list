#ifndef DS_LIST_LIST_H
#define DS_LIST_LIST_H


#include <algorithm>
#include <memory>
#include <memory_resource>
#include <stdexcept>
#include <utility>

#include "internal/list_base.h"
#include "internal/list_iterator.h"


namespace linear::randomaccess
{
    template <Comparable Tp>
    class list : public internal::list_base<Tp>
    {
    public:
        //*** Member Types ***//
        using allocator_type = std::pmr::polymorphic_allocator<std::byte>;
        using size_type = typename internal::list_base<Tp>::size_type;
        using reference = typename internal::list_base<Tp>::reference;
        using const_reference = typename internal::list_base<Tp>::const_reference;

        using iterator = internal::iterators::list_iterator<Tp, false>;
        using const_iterator = internal::iterators::list_iterator<Tp, true>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;


        //*** Member Functions ***//
        // Explicit ctor
        explicit list(const size_type capacity = default_capacity, allocator_type allocator = {})
                : internal::list_base<Tp>(capacity),
                  m_data(m_capacity <= 0 ? nullptr : std::make_unique<Tp[]>(m_capacity)),
                  m_allocator(allocator) {}

        // Extended copy ctor
        list(const list &rhs, allocator_type allocator = {})
                : list(rhs.m_capacity, allocator)
        { operator=(rhs); }

        // Move ctor
        list(list &&rhs) noexcept
                : list(rhs.m_capacity, rhs.m_allocator)
        { operator=(std::move(rhs)); }

        // Extended move ctor
        list(list &&rhs, allocator_type allocator) noexcept
                : list(rhs.m_capacity, allocator)
        { operator=(std::move(rhs)); }

        virtual ~list() = default;

        list& operator=(const list&);
        list& operator=(list&&) noexcept;
        constexpr void swap(list&) noexcept;


        //*** Iterators and Access ***//
        constexpr iterator begin() noexcept                         { return iterator(&m_data[0]); }
        constexpr const_iterator begin() const noexcept             { return const_iterator(&m_data[0]); }
        constexpr const_iterator cbegin() const noexcept            { return begin(); }
        constexpr iterator end() noexcept                           { return iterator(&m_data[this->m_size]); }
        constexpr const_iterator end() const noexcept               { return const_iterator(&m_data[this->m_size]); }
        constexpr const_iterator cend() const noexcept              { return end(); }
        constexpr reverse_iterator rbegin() noexcept                { return reverse_iterator(end()); }
        constexpr const_reverse_iterator crbegin() const noexcept   { return const_reverse_iterator(end()); }
        constexpr reverse_iterator rend() noexcept                  { return reverse_iterator(begin()); }
        constexpr const_reverse_iterator crend() const noexcept     { return const_reverse_iterator(begin()); }

        constexpr reference front() override                        { return m_data[0]; }
        constexpr const_reference front() const override            { return m_data[0]; }
        constexpr reference back() override                         { return m_data[this->m_size]; }
        constexpr const_reference back() const override             { return m_data[this->m_size]; }

        constexpr reference operator[](const size_type pos)               { return m_data[pos]; }
        constexpr const_reference operator[](const size_type pos) const   { return m_data[pos]; }

        constexpr reference at(size_type);
        constexpr const_reference at(const size_type pos) const           { return at(pos); }


        //*** Modifiers ***//
        template <class... Args> virtual iterator emplace(const_iterator, Args&&...);

        virtual iterator erase(const_iterator, const_iterator);
        virtual iterator erase(const_iterator);

        // Changes size but not capacity
        constexpr void clear() noexcept;


    protected:
        std::unique_ptr<Tp[]> m_data;
        allocator_type m_allocator;

    };  // class list



    //********* Member Function Implementations *********//

    // Copy assign
    template <Comparable Tp>
    list<Tp>& list<Tp>::operator=(const list<Tp> &rhs)
    {
        if (this == &rhs) return *this;
        erase(begin(), end());
        for (const auto &e : rhs)
            push_back(e);
        return *this;
    }


    // Move assign
    template <Comparable Tp>
    list<Tp>& list<Tp>::operator=(list<Tp> &&rhs) noexcept
    {
        if (this == &rhs) return *this;
        if (m_allocator == rhs.m_allocator)
        {
            erase(begin(), end());
            swap(rhs);
        }
        else
            operator=(rhs);     // copy assign
        return *this;
    }


    // Swap: does nothing if left-hand allocator != right-hand allocator
    template <Comparable Tp>
    constexpr void list<Tp>::swap(list<Tp> &rhs) noexcept
    {
        if (m_allocator == rhs.m_allocator)
        {
            using std::swap;                            // enable Koenig lookup
            swap(rhs.m_capacity, this->m_capacity);
            swap(rhs.m_size, this->m_size);
            rhs.m_data.swap(m_data);
        }
    }


    template <Comparable Tp>
    constexpr typename list<Tp>::reference list<Tp>::at(const size_type pos)
    {
        if (pos < 0 || pos >= this->m_size)
            throw std::out_of_range("Index out of bounds.");
        return m_data[pos];
    }


    template <Comparable Tp>
    template <class... Args>
    typename list<Tp>::iterator list<Tp>::emplace(const_iterator pos, Args &&...args)
    {
        auto *val = static_cast<Tp*>(m_allocator.resource()->allocate(sizeof(Tp), alignof(Tp)));
        try
        {
            m_allocator.construct(std::addressof(*val),
                                  std::forward<Args>(args)...);
        }

        catch (...)
        {
            m_allocator.resource()->deallocate(val, sizeof(Tp), alignof(Tp));
            throw;
        }

        m_data[pos - begin()] = *val;
        this->m_size++;
        return static_cast<iterator>(pos);
    }


    template <Comparable Tp>
    typename list<Tp>::iterator list<Tp>::erase(const_iterator b, const_iterator e)
    {
        auto it = b;
        while (it != e)
        {
            auto elem = at(it);
            this->m_size--;
            std::allocator_traits<allocator_type>::destroy(std::addressof(elem));
            m_allocator.resource()->deallocate(&elem, sizeof(Tp), alignof(Tp));
            ++it;
        }
        return static_cast<iterator>(b);
    }


    template <Comparable Tp>
    typename list<Tp>::iterator list<Tp>::erase(const_iterator pos)
    {
        const_iterator ci = pos;
        return erase(pos, ++ci);
    }


    template <Comparable Tp>
    constexpr void list<Tp>::clear() noexcept
    {
        auto it = begin();
        while (it != end())
        {
            auto elem = at(it);
            std::allocator_traits<allocator_type>::destroy(std::addressof(elem));
            this->m_size--;
            ++it;
        }
    }



    //********* Non-Member Function Implementations **********//

    // Non-member swap; specialization of std::swap - calls lhs.swap(rhs)
    template <Comparable Tp>
    constexpr void swap(list<Tp> &lhs, list<Tp> &rhs) noexcept
    { lhs.swap(rhs); }


    // Equality comparison operator overload
    template <Comparable Tp>
    constexpr bool operator==(const list<Tp> &lhs, const list<Tp> &rhs) noexcept
    {
        if (lhs.size() != rhs.size()) return false;
        return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <Comparable Tp>
    [[nodiscard]] constexpr bool operator!=(const list<Tp> &lhs, const list<Tp> &rhs) noexcept
    { return !operator==(lhs, rhs); }

}   // namespace linear::randomaccess

#endif //DS_LIST_LIST_H
