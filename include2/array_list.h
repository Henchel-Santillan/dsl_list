#ifndef DS_LIST_ARRAY_LIST_H
#define DS_LIST_ARRAY_LIST_H


#include "list.h"


namespace linear::randomaccess
{
    namespace details
    {
        static constexpr int default_growth_factor = 2;
    }   //namespace details

    template <Comparable Tp>
    class array_list : public list<Tp>
    {
    public:
        //*** Member Types ***//
        using allocator_type = typename list<Tp>::allocator_type;
        using size_type = typename list<Tp>::size_type;
        using reference = typename list<Tp>::reference;
        using const_reference = typename list<Tp>::const_reference;

        using iterator = typename list<Tp>::iterator;
        using const_iterator = typename list<Tp>::const_iterator;
        using reverse_iterator = typename list<Tp>::reverse_iterator;
        using const_reverse_iterator = typename list<Tp>::const_reverse_iterator;


        //*** Member Functions ***//
        // Explicit ctor
        explicit array_list(const size_type capacity = default_capacity,
                            allocator_type allocator = {})
            : list<Tp>(capacity, allocator) {}

        // Extended copy ctor
        array_list(const array_list &rhs, allocator_type allocator = {}) noexcept
            : list<Tp>(rhs, allocator) {}

        // Move ctor
        array_list(array_list &&rhs) noexcept
            : list<Tp>(rhs) {}

        // Extended move ctor
        array_list(array_list &&rhs, allocator_type allocator) noexcept
            : list<Tp>(rhs, allocator) {}

        // Copy assignment
        array_list& operator=(const array_list &rhs) noexcept
        { list<Tp>::operator=(rhs); return *this; }

        // Move assignment
        array_list& operator=(array_list &&rhs) noexcept
        { list<Tp>::operator=(rhs); return *this; }

        virtual ~array_list() = default;

        //*** Modifiers ***//
        template <class ...Args> iterator emplace(const_iterator, Args&&...) override;


    protected:
        void resize(size_type, size_type);

    };  // class array_list



    //********* Member Function Implementations *********//

    // Resize helper function
    // Probably some illegal things going on here
    template <Comparable Tp>
    void array_list<Tp>::resize(const typename array_list<Tp>::size_type old_capacity,
                                const typename array_list<Tp>::size_type new_capacity)
    {
        constexpr auto offset =
                std::max(old_capacity, new_capacity) - std::min(old_capacity, new_capacity);

        if (old_capacity < new_capacity)
        {
            auto *alloc_ptr = static_cast<Tp*>(
                    this->m_allocator.resource()->allocate(sizeof(Tp) * offset, alignof(Tp) * offset));
            try
            {
                this->m_allocator.construct(std::addressof(*alloc_ptr),
                                            std::forward<Args>(args)...);
            }

            catch(...)
            {
                this->m_allocator.resource()->deallocate(alloc_ptr, sizeof(Tp), alignof(Tp));
                throw;
            }
        }

        else
        {
            const_iterator it = begin() + offset;
            list<Tp>::erase(it, end());
        }
    }


    template <Comparable Tp>
    template <class ...Args>
    typename array_list<Tp>::iterator array_list<Tp>::emplace(typename array_list<Tp>::const_iterator pos,
                                                              Args &&...args)
    {
        if (this->m_size == this->m_capacity)
        {
            auto temp = this->m_capacity;
            this->m_capacity *= details::default_growth_factor;
            resize(temp, this->m_capacity);
        }
        return list<Tp>::emplace(pos, args);
    }
    
    
    
    //********* Non-Member Function Implementations *********//

    // Non-member swap; specialization of std::swap - calls lhs.swap(rhs)
    template <Comparable Tp>
    constexpr void swap(array_list<Tp> &lhs, array_list<Tp> &rhs) noexcept
    { lhs.swap(rhs); }

    // Equality comparison operator overload
    template <Comparable Tp>
    [[nodiscard]] constexpr bool operator==(const array_list<Tp> &lhs, const array_list<Tp> &rhs) noexcept
    { return list<Tp>::operator==(lhs, rhs); }

    template <Comparable Tp>
    [[nodiscard]] constexpr bool operator!=(const array_list<Tp> &lhs, const array_list<Tp> &rhs) noexcept
    { return !list<Tp>::operator==(lhs, rhs); }

}   // namespace linear::randomaccess

#endif //DS_LIST_ARRAY_LIST_H
