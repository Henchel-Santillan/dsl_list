#ifndef DSL_LIST_H
#define DSL_LIST_H


#include "list_base.h"

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <stdexcept>


namespace dsl {

    namespace details {

        static const std::size_t growth_multiplier = 1.5;

        /**
         * @brief Iterator with const pointer and reference types.
         * Adheres to the named requirements of LegacyRandomAccessIterator.
         * 
         * @tparam Tp 
         */
        template <typename Tp>
        class list_const_iterator : public iterator_base<Tp> {
        public:

            //*** Member Types ***//

            using value_type = typename iterator_base<Tp>::value_type;
            using difference_type = typename iterator_base<Tp>::difference_type;

            using iterator_category = std::random_access_iterator_tag;

            using pointer = const value_type*;
            using reference = const value_type&;


            //*** Member Functions ***//

            list_const_iterator() noexcept 
                : m_ptr(nullptr) {}

            list_const_iterator(const pointer ptr) noexcept 
                : m_ptr(const_cast<pointer>(ptr)) {}

            [[nodiscard]] reference operator*() const noexcept { 
                return *m_ptr;
            }

            [[nodiscard]] pointer operator->() const noexcept {
                return m_ptr;
            }

            list_const_iterator& operator++() noexcept {
                m_ptr++;
                return *this;
            }

            list_const_iterator operator++(int) noexcept {
                list_const_iterator it(*this);
                ++(*this);
                return it;
            }

            list_const_iterator& operator--() noexcept {
                m_ptr--;
                return *this;
            }

            list_const_iterator operator--(int) noexcept {
                list_const_iterator it(*this);
                --(*this);
                return it;
            }

            list_const_iterator& operator+=(const difference_type offset) noexcept {
                m_ptr += offset;
                return *this;
            }

            [[nodiscard]] list_const_iterator operator+(const difference_type offset) const noexcept {
                list_const_iterator it(*this);
                it += offset;
                return it;
            }

            list_const_iterator& operator-=(const difference_type offset) noexcept {
                return *this += -offset;
            }

            [[nodiscard]] list_const_iterator operator-(const difference_type offset) const noexcept {
                list_const_iterator it(*this);
                it -= offset;
                return it;
            }

            [[nodiscard]] difference_type operator-(const list_const_iterator &other) const noexcept {
                return m_ptr - other.m_ptr;
            }

            [[nodiscard]] reference operator[](const difference_type offset) const noexcept {
                return *(*this + offset);
            }

            bool operator==(const list_const_iterator &other) const noexcept {
                return m_ptr == other.m_ptr;
            }

            bool operator!=(const list_const_iterator &other) const noexcept {
                return !operator==(other);
            }

        protected:
            pointer m_ptr;
        };


        /**
         * @brief Iterator with non-const pointer and reference member types. 
         * Adheres to the named requirements of LegacyRandomAccessIterator.
         * 
         * @tparam Tp 
         */
        template <typename Tp>
        class list_iterator : public list_const_iterator<Tp> {
        public:


            //*** Member Types ***//

            using base_t = list_const_iterator<Tp>;
            using value_type = typename base_t::value_type;
            using difference_type = typename base_t::difference_type;

            using pointer = value_type*;
            using reference = value_type&;


            //*** Member Functions ***//

            list_iterator() noexcept 
                : list_const_iterator<Tp>() {}

            list_iterator(const pointer ptr) noexcept 
                : list_const_iterator<Tp>(ptr) {}

            [[nodiscard]] reference operator*() const noexcept { 
                return *this->m_ptr;
            }

            [[nodiscard]] pointer operator->() const noexcept {
                return this->m_ptr;
            }

            list_iterator& operator++() noexcept {
                base_t::operator++();
                return *this;
            }

            list_iterator operator++(int) noexcept {
                list_iterator it(*this);
                ++(*this);
                return it;
            }

            list_iterator& operator--() noexcept {
                base_t::operator--();
                return *this;
            }

            list_iterator operator--(int) noexcept {
                list_iterator it(*this);
                --(*this);
                return it;
            }

            list_iterator& operator+=(const difference_type offset) noexcept {
                base_t::operator+=(offset);
                return *this;
            }

            [[nodiscard]] list_iterator operator+(const difference_type offset) const noexcept {
                list_iterator it(*this);
                it += offset;
                return it;
            }

            list_iterator& operator-=(const difference_type offset) noexcept {
                base_t::operator-=(offset);
                return *this;
            }

            [[nodiscard]] list_iterator operator-(const difference_type offset) const noexcept {
                list_iterator it(*this);
                it -= offset;
                return it;
            }

            using base_t::operator-;

            [[nodiscard]] reference operator[](const difference_type offset) const noexcept {
                return const_cast<reference>(base_t::operator[](offset));
            }
        };

    }   // namespace details


    /**
     * @brief Array-based list-type representing a vector-like container.
     * 
     * @tparam Tp 
     */
    template <typename Tp>
    class list : public details::list_base<Tp> {
    public:

        //*** Member Types ***//

        using value_type = typename details::list_base<Tp>::value_type;
        using size_type = typename details::list_base<Tp>::size_type;
        using difference_type = typename details::list_base<Tp>::difference_type;

        using reference = typename details::list_base<Tp>::reference;
        using const_reference = typename details::list_base<Tp>::const_reference;

        using allocator_type = std::pmr::polymorphic_allocator<std::byte>;
        using pointer = std::allocator_traits<allocator_type>::pointer;
        using const_pointer = std::allocator_traits<allocator_type>::const_pointer;

        using iterator = typename details::list_iterator<Tp>;
        using const_iterator = typename details::list_const_iterator<Tp>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;


        //*** Member Functions ***/

        //* Constructors *//

        explicit list(allocator_type allocator = {})
            : details::list_base<Tp>()
            , m_allocator(allocator)
            , m_data(nullptr) 
            , m_capacity(0)
        {}

        list(const size_type count, 
             const Tp &value, 
             allocator_type allocator = {})
            : list(allocator)
        { assign(count, value); }

        explicit list(const size_type count, 
                      allocator_type allocator = {})
            : list(count, Tp(), allocator)
        {}

        template <class InputIt>
        list(InputIt first, InputIt last,
             allocator_type allocator)
            : list(allocator)
        { insert(begin(), first, last); }

        list(std::initializer_list<Tp> init, 
             allocator_type allocator = {})
            : list(init.first(), init.last(), allocator)
        {}


        //* Copy Constructors *//

        list(const list &other, 
             allocator_type allocator = {})
            : list(other.size(), allocator) 
        { operator=(other); }

        list(const list &other) 
            : list(other, std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.get_allocator()))
        {}


        //* Move Construtors *//

        list(list &&other, 
             allocator_type allocator = {})
            : list(other.size(), allocator) 
        { operator=(std::move(other)); }

        list(list &&other) 
            : list(other, other.get_allocator())
        {}


        //* Destructor *//
        ~list() {
            erase(begin(), end());
            m_data = nullptr;
        }


        //* Assignment operator overloads *//

        list& operator=(const list&);
        list& operator=(list&&);


        //* Assign and allocator access *//

        void assign(const size_type, const Tp&);
        
        template <class InputIt>
        void assign(InputIt, InputIt);

        void assign(std::initializer_list<Tp>);

        allocator_type get_allocator() const noexcept;


        //* Element Access *//

        reference at(const size_type);
        const_reference at(const size_type) const;

        reference operator[](const size_type pos) {
            return m_data[pos];
        } 

        const_reference operator[](const size_type pos) const {
            return m_data[pos];
        }

        reference front() {
            return m_data[0];
        }

        const_reference front() const {
            return m_data[0];
        }

        reference back() {
            return m_data[this->m_size];
        }

        const_reference back() const {
            return m_data[this->m_size];
        }

        Tp* data() noexcept {
            return m_data;
        }

        const Tp* data() const noexcept {
            return m_data;
        }


        //* Iterators *//

        iterator begin() noexcept { 
            return iterator(&m_data[0]); 
        }

        const_iterator begin() const noexcept { 
            return const_iterator(&m_data[0]); 
        }

        const_iterator cbegin() const noexcept { 
            return const_iterator(&m_data[0]); 
        }

        iterator end() noexcept { 
            return iterator(&m_data[this->m_size]); 
        }

        const_iterator end() const noexcept { 
            return const_iterator(&m_data[this->m_size]); 
        }

        const_iterator cend() const noexcept { 
            return const_iterator(&m_data[this->m_size]);
        }

        reverse_iterator rbegin() noexcept { 
            return reverse_iterator(end()); 
        }

        const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        const_reverse_iterator crbegin() const noexcept { 
            return const_reverse_iterator(end()); 
        }

        reverse_iterator rend() noexcept { 
            return reverse_iterator(begin()); 
        }

        const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(begin());
        }

        const_reverse_iterator crend() const noexcept { 
            return const_reverse_iterator(begin()); 
        }


        //* Capacity *//

        void reserve(const size_type);
        size_type capacity() const noexcept;
        void shrink_to_fit();

        
        //* Modifiers *//

        void clear() noexcept;

        iterator insert(const_iterator, const Tp&);
        iterator insert(const_iterator, Tp&&);
        iterator insert(const_iterator, size_type, const Tp&);
        
        template <class InputIt>
        iterator insert(const_iterator, InputIt, InputIt);

        iterator insert(const_iterator, std::initializer_list<Tp>);

        template <class... Args>
        iterator emplace(const_iterator, Args&&...);

        iterator erase(const_iterator);
        iterator erase(const_iterator, const_iterator);

        void push_back(const Tp&);
        void push_back(Tp&&);

        template <class... Args>
        reference emplace_back(Args&&...);

        void pop_back();

        void resize(const size_type);
        void resize(const size_type, const Tp&);

        void swap(list&) noexcept(std::allocator_traits<allocator_type>::propagate_on_container_swap::value || 
                                            std::allocator_traits<allocator_type>::is_always_equal::value);


    private:

        //*** Members ***//

        allocator_type m_allocator;
        Tp *m_data;
        size_type m_capacity;


        //*** Functions ***//

        void check_bounds(const size_type) const;

        void try_copy(const list&);
        void try_move(list&&);
        void resize_erase(const size_type);
        void resize_emplace(const size_type, const Tp&);    

        size_type compute_growth(const size_type) const noexcept;
        void reallocate_exactly(const size_type);    
    };



    //****** Member Function Implementations ******//

    //*** Private ***//

    template <typename Tp>
    void list<Tp>::check_bounds(const size_type pos) const {
        if (pos >= this->m_size) 
            throw std::out_of_range("Index out of bounds.");
    }

    template <typename Tp>
    void list<Tp>::try_copy(const list &other) {
        erase(begin(), end());
        m_capacity = other.m_capacity;
        m_data = static_cast<Tp*>(m_allocator.resource()->allocate(sizeof(Tp) * m_capacity, alignof(Tp)));
        std::copy(other.begin(), other.end(), m_data);
    }

    template <typename Tp>
    void list<Tp>::try_move(list &&other) {
        erase(begin(), end());
        m_capacity = other.m_capacity;
        m_data = static_cast<Tp*>(m_allocator.resource()->allocate(sizeof(Tp) * m_capacity, alignof(Tp)));
        std::move(other.begin(), other.end(), m_data);
        other.clear();
    }

    template <typename Tp>
    void list<Tp>::resize_erase(const size_type count) {
        erase(begin() + count, end());
    }

    template <typename Tp>
    void list<Tp>::resize_emplace(const size_type count, const Tp &value) {
        for (auto i = 0; i < count; ++i) 
            push_back(value);
    }

    template <typename Tp>
    typename list<Tp>::size_type list<Tp>::compute_growth(const size_type new_size) const noexcept {
        return (this->max_size() - (m_capacity / 2) || m_capacity * details::growth_multiplier < new_size) ? new_size : m_capacity * details::growth_multiplier;
    }

    template <typename Tp>
    void list<Tp>::reallocate_exactly(const size_type min_size) {
        size_type new_cap = compute_growth(min_size);
        auto data = static_cast<Tp*>(m_allocator.resource()->allocate(sizeof(Tp) * new_cap, alignof(Tp)));
        
        for (auto i = 0; i < this->m_size; ++i) {
            m_allocator.construct(std::addressof(data[i]), std::move(m_data[i]));
            std::allocator_traits<allocator_type>::destroy(m_allocator, std::addressof(m_data[i]));
        }

        m_allocator.resource()->deallocate(m_data, sizeof(Tp) * m_capacity, alignof(Tp));
        m_data = data;
        m_capacity = new_cap;
    }

    //*** Public ***//

    //* Assignment operator overloads *//

    template <typename Tp>
    list<Tp>& list<Tp>::operator=(const list<Tp> &other) {
        if (this != other) 
            try_copy(other);
        return *this;
    }

    template <typename Tp>
    list<Tp>& list<Tp>::operator=(list<Tp> &&other) {
        if (this != other) {
            if (m_allocator == other.m_allocator) 
                try_move(std::move(other));
            else 
                operator=(other);   // copy assignment
        }
        return *this;
    }


    //* Assign and allocator access *//

    template <typename Tp>
    void list<Tp>::assign(const size_type count, const Tp &value) {
        resize(count, value);
        for (auto it = begin(); it != end(); ++it) 
            *it = value;
    }

    template <typename Tp>
    template <class InputIt>
    void list<Tp>::assign(InputIt first, InputIt last) {
        resize(std::distance(first, last));
        auto it = begin();
        for (; first != last && it != end(); ++first) 
            *it = *first;
    }

    template <typename Tp>
    void list<Tp>::assign(std::initializer_list<Tp> ilist) {
        assign(ilist.begin(), ilist.end());
    }

    template <typename Tp>
    typename list<Tp>::allocator_type list<Tp>::get_allocator() const noexcept {
        return m_allocator;
    }


    //* Element Access *//

    template <typename Tp>
    typename list<Tp>::reference list<Tp>::at(const size_type pos) {
        check_bounds(pos);
        return m_data[pos];
    }

    template <typename Tp>
    typename list<Tp>::const_reference list<Tp>::at(const size_type pos) const {
        check_bounds(pos);
        return m_data[pos];
    }


    template <typename Tp>
    void list<Tp>::reserve(const size_type new_cap) {
        if (new_cap > this->max_size()) 
            throw std::length_error("New capacity cannot be larger than the maximum supported list size.");
        
        if (new_cap > m_capacity) 
            reallocate_exactly(new_cap);
    }

    template <typename Tp>
    typename list<Tp>::size_type list<Tp>::capacity() const noexcept {
        return m_capacity;
    }

    template <typename Tp>
    void list<Tp>::shrink_to_fit() {
        for (auto i = this->m_size; i < m_capacity; ++i) {
            std::allocator_traits<allocator_type>::destroy(m_allocator, std::addressof(m_data + i));
            m_allocator.resource()->deallocate(m_data + i, sizeof(Tp), alignof(Tp));
        }
        m_capacity = this->m_size;
    }


    //* Modifiers *//

    template <typename Tp>
    void list<Tp>::clear() noexcept {
        for (auto it = begin(); it != end(); ++it) {
            --this->m_size;
            std::allocator_traits<allocator_type>::destroy(m_allocator, std::addressof(*it));
        }
    }

    template <typename Tp>
    typename list<Tp>::iterator list<Tp>::insert(const_iterator pos, const Tp &value) {
        return emplace(pos, value);
    }

    template <typename Tp>
    typename list<Tp>::iterator list<Tp>::insert(const_iterator pos, Tp &&value) {
        return emplace(pos, std::move(value));
    }

    template <typename Tp>
    typename list<Tp>::iterator list<Tp>::insert(const_iterator pos, const size_type count, const Tp &value) {
        if (this->m_size + count > m_capacity) 
            reallocate_exactly(m_capacity + count);

        auto it = pos - begin();
        std::move(it, end(), it + 1);
        *it = std::move(value);

        this->m_size += count;
        return dynamic_cast<iterator>(it);
    }

    template <typename Tp>
    template <class InputIt>
    typename list<Tp>::iterator list<Tp>::insert(const_iterator pos, InputIt first, InputIt last) {
        auto count = std::distance(first, last);
        if (this->m_size + count > m_capacity) 
            reallocate_exactly(m_capacity + count);
        
        auto it = pos - begin();
        std::move(it, end(), it + count);
        std::copy(first, last, it);

        this->m_size += count;
        return dynamic_cast<iterator>(it);
    }

    template <typename Tp>
    typename list<Tp>::iterator list<Tp>::insert(const_iterator pos, std::initializer_list<Tp> ilist) {
        return insert(pos, ilist.begin(), ilist.end());
    }

    template <typename Tp>
    template <class... Args>
    typename list<Tp>::iterator list<Tp>::emplace(const_iterator pos, Args &&...args) {
        auto index = pos - begin();
        &m_data[index] = static_cast<Tp*>(m_allocator.resource()->allocate(sizeof(Tp), alignof(Tp)));
        
        try {
            m_allocator.construct(&m_data[index], std::forward<Args>(args)...);
        } catch (...) {
            m_allocator.resource()->deallocate(&m_data[index], sizeof(Tp), alignof(Tp));
            throw;
        }

        ++this->m_size;
        return dynamic_cast<iterator>(pos);
    }

    template <typename Tp>
    typename list<Tp>::iterator list<Tp>::erase(const_iterator pos) {
        erase(pos, std::next(pos));
    }

    template <typename Tp>
    typename list<Tp>::iterator list<Tp>::erase(const_iterator first, const_iterator last) {
        while (first != last) {
            auto elem = *first;
            --this->m_size;
            std::allocator_traits<allocator_type>::destroy(m_allocator, std::addressof(elem));
            m_allocator.resource()->deallocate(std::addressof(elem), sizeof(Tp), alignof(Tp));
            ++first;
        }
        return dynamic_cast<iterator>(first);
    }

    template <typename Tp>
    void list<Tp>::push_back(const Tp &value) {
        emplace_back(value);
    }

    template <typename Tp>
    void list<Tp>::push_back(Tp &&value) {
        emplace_back(std::move(value));
    }

    template <typename Tp>
    template <class... Args>    
    typename list<Tp>::reference list<Tp>::emplace_back(Args &&...args) {
        auto it = emplace(std::prev(end()), std::forward<Args>(args)...);
        return *it;
    }

    template <typename Tp>
    void list<Tp>::pop_back() {
        erase(std::prev(end()));
    }

    template <typename Tp>
    void list<Tp>::resize(const size_type count) {
        resize(count, Tp());
    }

    template <typename Tp>
    void list<Tp>::resize(const size_type count, const Tp &value) {
        if (this->m_size == count)
            return;

        bool size_equals_capacity = this->m_size == m_capacity;
        bool reallocated = false;
        
        if (size_equals_capacity && this->m_size < count) {
            resize_emplace(count, Tp());
            reallocated = true;
        } else if (!size_equals_capacity && this->m_size < count && count < m_capacity) {
                for (auto i = this->m_size; i < this->m_size + count; ++i) 
                    m_data[i] = value;
        } else resize_erase(count);

        if (reallocated) 
            m_capacity = count;
        else 
            this->m_size = count;
    }

    template <typename Tp>
    void list<Tp>::swap(list &other) noexcept(std::allocator_traits<allocator_type>::propagate_on_container_swap::value || std::allocator_traits<allocator_type>::is_always_equal::value) {
        if (m_allocator == other.m_allocator) {
            using std::swap;
            swap(this->m_size, other.m_size);
            swap(m_capacity, other.m_capacity);
            swap(m_data, other.m_data);
        }
    }


    //*** Non-Member Function Implementations ***//

    template <typename Tp>
    bool operator==(const list<Tp> &lhs, const list<Tp> &rhs) {
        return (lhs.size() != rhs.size()) ? false : std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename Tp>
    bool operator!=(const list<Tp> &lhs, const list<Tp> &rhs) {
        return !operator==(lhs, rhs);
    }

}   // namespace dsl 


#endif // DSL_LIST_H