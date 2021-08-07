#ifndef DS_LIST_ARRAY_LIST_H
#define DS_LIST_ARRAY_LIST_H


#include "list.h"


namespace linear::randomaccess
{
    template <Comparable Tp>
    class array_list : public list<Tp>
    {
    public:
        //*** Member Types ***//
        using allocator_type = list<Tp>::allocator_type;
        using size_type = list<Tp>::size_type;
        using reference = list<Tp>::reference;
        using const_reference = list<Tp>::const_reference;

        using iterator = list<Tp>::iterator;
        using const_iterator = list<Tp>::const_iterator;
        using reverse_iterator = list<Tp>::reverse_iterator;
        using const_reverse_iterator = list<Tp>::const_reverse_iterator;


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
        iterator erase(const_iterator, cosnt_iterator) override;
        iterator erase(const_iterator) override;

    };  // class array_list



    //********* Member Function Implementations *********//

    template <Comparable Tp>
    template <class ...Args>
    typename array_list<Tp>::iterator array_list<Tp>::emplace(const_iterator pos,
                                                              Args &&...args)
    {
        if (this->m_size == this->m_capacity)
            resize();

    }


    // Resizes the list-type
    template <Comparable Tp>
    constexpr void array_list<Tp>::resize()
    {

    }

}   // namespace linear::randomaccess

#endif //DS_LIST_ARRAY_LIST_H
