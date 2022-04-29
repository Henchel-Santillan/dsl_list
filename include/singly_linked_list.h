#ifndef DSL_SINGLY_LINKED_LIST_H
#define DSL_SINGLY_LINKED_LIST_H


#include "list_base.h"

#include <initializer_list>
#include <iterator>
#include <memory>
#include <memory_resource>


namespace dsl {

    template <typename Tp> class singly_linked_list;

    namespace details {

        template <typename Tp> struct singly_node;

        /**
         * @brief Base representation for a node in a singly linked list. 
         * Contains a non-owning raw pointer to the next node in the list.
         * 
         * @tparam Tp 
         */
        template <typename Tp>
        struct singly_node_base {
            singly_node_base() : m_next(nullptr) {}

            // Explicitly disallow copy behaviour
            singly_node_base(const singly_node_base&) = delete;
            singly_node_base& operator=(const singly_node_base&) = delete;

            singly_node<Tp> *m_next;
        };

        /**
         * @brief Node in a singly-linked list. Derives the base representation and wraps 
         * the value type in an anonymous union to help align raw bytes.
         * 
         * @tparam Tp 
         */
        template <typename Tp>
        struct singly_node : singly_node_base<Tp> {
            union {
                Tp m_value;
            };
        };


        /**
         * @brief Iterator with const pointer and reference member types.
         * Adheres to the named requirements of LegacyForwardIterator.
         * 
         * @tparam Tp 
         */
        template <typename Tp>
        class singly_const_iterator : public iterator_base<Tp> {
        public:

            //*** Member Types ***//

            using value_type = typename iterator_base<Tp>::value_type;
            using difference_type = typename iterator_base<Tp>::difference_type;

            using iterator_category = std::forward_iterator_tag;
            using pointer = const value_type*;
            using reference = const value_type&;


            //*** Member Functions ***//

            [[nodiscard]] pointer operator->() const noexcept {
                return std::addressof(m_prev->m_next->m_value);
            }

            [[nodiscard]] reference operator*() const noexcept {
                return m_prev->m_next->m_value;
            }

            singly_const_iterator& operator++() noexcept {
                m_prev = m_prev->m_next;
                return *this;
            }

            singly_const_iterator operator++(int) noexcept {
                singly_const_iterator it(*this);
                ++(*this);
                return it;
            }

            bool operator==(const singly_const_iterator &other) const noexcept {
                return m_prev == other.m_prev;
            }

            bool operator!=(const singly_const_iterator &other) const noexcept {
                return !operator==(other);
            }

        protected:
            friend class singly_linked_list<Tp>;

            singly_node_base<Tp> *m_prev;

            // Non-public explicit constructor to enable iterator construction for derived classes and friend classes
            explicit singly_const_iterator(const singly_node_base<Tp> *prev) 
                : m_prev(const_cast<singly_node_base<Tp>*>(prev)) {}
        };


        /**
         * @brief Iterator with non-const pointer and reference member types.
         * Adheres to the named requirements of LegacyForwardIterator.
         * 
         * @tparam Tp 
         */
        template <typename Tp>
        class singly_iterator : public singly_const_iterator<Tp> {
        public:

            //*** Member Types ***//

            using base_t = singly_const_iterator<Tp>;
            using value_type = typename base_t::value_type;

            using pointer = value_type*;
            using reference = value_type&;


            //*** Member Functions ***//

            [[nodiscard]] pointer operator->() const noexcept {
                return std::addressof(this->m_prev->m_next->m_value);
            }

            [[nodiscard]] reference operator*() const noexcept {
                return this->m_prev->m_next->m_value;
            }

            singly_iterator& operator++() noexcept {
                base_t::operator++();
                return *this;
            }

            singly_iterator operator++(int) noexcept {
                singly_iterator it(*this);
                ++(*this);
                return it;
            }


        private:
            friend class singly_linked_list<Tp>;

            explicit singly_iterator(const singly_node_base<Tp> *prev)
                : singly_const_iterator<Tp>(prev) {}
        };

    }   // namespace details


    /**
     * @brief Singly-linked list container. 
     * 
     * @tparam Tp 
     */
    template <typename Tp>
    class singly_linked_list : public details::list_base<Tp> {
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

        using iterator = typename details::singly_iterator<Tp>;
        using const_iterator = typename details::singly_const_iterator<Tp>;


        //*** Member Functions ***//

        //* Constructors *//

        explicit singly_linked_list(allocator_type allocator = {})
            : details::list_base<Tp>()
            , m_allocator(allocator)
            , m_head()
            , m_tail(nullptr)
        {}

        singly_linked_list(const size_type count,
                           const Tp& value,
                           allocator_type allocator = {})
            : singly_linked_list(allocator)
        { assign(count, value); }

        explicit singly_linked_list(const size_type count, 
                                    allocator_type allocator = {})
            : singly_linked_list(count, Tp(), allocator)
        {}


        template <class InputIt>
        singly_linked_list(InputIt first, InputIt last,
                           allocator_type allocator = {})
            : singly_linked_list(allocator)
        { insert_after(before_begin(), first, last); }


        singly_linked_list(std::initializer_list<Tp> init, 
                           allocator_type allocator = {})
            : singly_linked_list(init.before_begin(), init.end(), allocator)
        {}


        //* Copy Constructors *//

        singly_linked_list(const singly_linked_list &other,
                           allocator_type allocator = {})
            : singly_linked_list(other.size(), allocator)
        { operator=(other); }
            
        singly_linked_list(const singly_linked_list &other)
            : singly_linked_list(other, std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.get_allocator()))
        {}


        //* Move Constructors *//

        singly_linked_list(singly_linked_list &&other, 
                           allocator_type allocator)
            : singly_linked_list(other.size(), allocator)
        { operator=(std::move(other)); }

        singly_linked_list(singly_linked_list &&other) 
            : singly_linked_list(other, other.get_allocator())
        {}


        //* Destructor *//
        ~singly_linked_list() {
            clear();
        }


        //* Assignment operator overloads *//

        singly_linked_list& operator=(const singly_linked_list&);
        singly_linked_list& operator=(singly_linked_list&&);


        //* Assign and allocator access *//
        
        void assign(const size_type, const Tp&);
        
        template <class InputIt>
        void assign(InputIt, InputIt);

        void assign(std::initializer_list<Tp>);

        allocator_type get_allocator() const noexcept;


        //* Element Access *//

        reference front() {
            return m_head.m_next->m_value;
        }

        const_reference front() const {
            return m_head.m_next->m_value;
        }


        //* Iterators *//

        iterator before_begin() noexcept {
            return iterator(&m_head);
        }

        const_iterator before_begin() const noexcept {
            return const_iterator(&m_head);
        }

        const_iterator cbefore_begin() const noexcept {
            return const_iterator(&m_head);
        }

        iterator begin() noexcept {
            return iterator(m_head.m_next);
        }

        const_iterator begin() const noexcept {
            return const_iterator(m_head.m_next);
        }

        const_iterator cbegin() const noexcept {
            return const_iterator(m_head.m_next);
        }

        iterator end() noexcept {
            return iterator(m_tail);
        }

        const_iterator end() const noexcept {
            return const_iterator(m_tail);
        }

        const_iterator cend() const noexcept {
            return const_iterator(m_tail);
        }


        //* Modifiers *//

        void clear() noexcept;

        iterator insert_after(const_iterator, const Tp&);
        iterator insert_after(const_iterator, Tp&&);
        iterator insert_after(const_iterator, const size_type, const Tp&);
        
        template <class InputIt>
        iterator insert_after(const_iterator, InputIt, InputIt);

        iterator insert_after(const_iterator, std::initializer_list<Tp>);

        template <class... Args>
        iterator emplace_after(const_iterator, Args&&...);

        iterator erase_after(const_iterator);
        iterator erase_after(const_iterator, const_iterator);

        void push_front(const Tp&);
        void push_front(Tp&&);

        template <class... Args>
        void emplace_front(Args&&...);

        template <class... Args>
        reference emplace_front(Args&&...);

        void pop_front();

        void resize(const size_type);
        void resize(const size_type, const Tp&);

        void swap(singly_linked_list&) noexcept(std::allocator_traits<allocator_type>::is_always_equal::value);


    private:

        //* Using Directives *//

        using node_base_t = typename details::singly_node_base<Tp>;
        using node_t = typename details::singly_node<Tp>;


        //* Members *//

        allocator_type m_allocator;
        node_base_t  m_head;
        node_base_t *m_tail;


        //* Functions *//

        void try_copy(const singly_linked_list&);
        void try_move(singly_linked_list&&);
        void resize_erase(const size_type);
        void resize_emplace(const size_type, const Tp&);
    };



    //****** Member Function Implementations ******//

    //*** Private ***//

    template <typename Tp>
    void singly_linked_list<Tp>::try_copy(const singly_linked_list<Tp> &other) {
        clear();
        for (const Tp& value : other) 
            insert_after(end(), value);
    }


    template <typename Tp>
    void singly_linked_list<Tp>::try_move(singly_linked_list &&other) {
        clear();
        swap(other);
    }

    template <typename Tp>
    void singly_linked_list<Tp>::resize_erase(const size_type count) {
        auto first = std::advance(begin(), count);
        erase_after(first, end());
    }

    template <typename Tp>
    void singly_linked_list<Tp>::resize_emplace(const size_type count, const Tp &value) {
        auto it = end();
        while (count > 0) {
            it = emplace_after(it, value);
            --count;
        }    
    }


    //*** Public ***//

    //* Assignment Operator Overloads *//

    template <typename Tp>
    singly_linked_list<Tp>& singly_linked_list<Tp>::operator=(const singly_linked_list<Tp> &other) {
        if (this != &other) 
            try_copy(other);
        return *this;
    }

    template <typename Tp>
    singly_linked_list<Tp>& singly_linked_list<Tp>::operator=(singly_linked_list<Tp> &&other) {
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
    void singly_linked_list<Tp>::assign(const size_type count, const Tp& value) {
        resize(count);
        for (auto it = begin(); it != end(); ++it) 
            *it = value;
    }

    template <typename Tp>
    template <class InputIt>
    void singly_linked_list<Tp>::assign(InputIt first, InputIt last) {
        resize(std::distance(first, last));
        auto it = begin();
        for (; first != last && it != end(); ++first)
            *it = *first;
    }

    template <typename Tp>
    void singly_linked_list<Tp>::assign(std::initializer_list<Tp> ilist) {
        assign(ilist.begin(), ilist.end());
    }

    template <typename Tp>
    typename singly_linked_list<Tp>::allocator_type singly_linked_list<Tp>::get_allocator() const noexcept {
        return m_allocator;
    }


    //* Modifiers *//

    template <typename Tp>
    void singly_linked_list<Tp>::clear() noexcept {
        erase_after(before_begin(), end());
    }

    template <typename Tp>
    typename singly_linked_list<Tp>::iterator singly_linked_list<Tp>::insert_after(const_iterator pos, const Tp &value) {
        return emplace_after(pos, value);
    }

    template <typename Tp>
    typename singly_linked_list<Tp>::iterator singly_linked_list<Tp>::insert_after(const_iterator pos, Tp &&value) {
        return emplace_after(pos, std::move(value));
    }

    template <typename Tp>
    typename singly_linked_list<Tp>::iterator singly_linked_list<Tp>::insert_after(const_iterator pos, const size_type count, const Tp &value) {
        auto it = dynamic_cast<iterator>(pos);
        for (auto i = 0; i < count; ++i) 
            it = emplace_after(pos, value);
        return it;
    }

    template <typename Tp>
    template <class InputIt>
    typename singly_linked_list<Tp>::iterator singly_linked_list<Tp>::insert_after(const_iterator pos, InputIt first, InputIt last) {
        auto it = dynamic_cast<iterator>(pos);
        for (; first != last; ++first) 
            it = emplace_after(it, *first);
        return it;
    }

    template <typename Tp>
    typename singly_linked_list<Tp>::iterator singly_linked_list<Tp>::insert_after(const_iterator pos, std::initializer_list<Tp> ilist) {
        return insert_after(pos, ilist.begin(), ilist.end());
    }

    template <typename Tp>
    template <class... Args>
    typename singly_linked_list<Tp>::iterator singly_linked_list<Tp>::emplace_after(const_iterator pos, Args &&...args) {
        auto node = static_cast<node_t*>(m_allocator.resource()->allocate(sizeof(node_t), alignof(node_t)));
        
        try {
            m_allocator.construct(std::addressof(node->m_value), std::forward<Args>(args)...);
        } catch (...) {
            m_allocator.resource()->deallocate(node, sizeof(node_t), alignof(node_t));
            throw;
        }

        // Note: pos.m_prev->m_next gets the node at the current iterator position
        // pos.m_prev->m_next->m_next as an rvalue means the actual next node of the node at the current iterator position
        // pos.m_prev->m_next->m_next as an lvalue means setting the next node of the node at the current iterator position to the newly created node
        pNode->m_next = pos.m_prev->m_next->m_next;
        pos.m_prev->m_next->m_next = node;

        if (pos.m_prev == m_tail) 
            m_tail = node;

        ++this->m_size;
        return dynamic_cast<iterator>(pos);
    }

    template <typename Tp>
    typename singly_linked_list<Tp>::iterator singly_linked_list<Tp>::erase_after(const_iterator pos) {
        return (pos, std::next(pos));
    }

    template <typename Tp>
    typename singly_linked_list<Tp>::iterator singly_linked_list<Tp>::erase_after(const_iterator first, const_iterator last) {
        auto next = first.m_prev->m_next;
        auto past = last.m_prev->m_next;

        if (past == nullptr) 
            m_tail = first.m_prev;

        first.m_prev->m_next = past;

        while (next != past) {
            auto old = next;
            next = next->m_next;
            --this->m_size;
            std::allocator_traits<allocator_type>::destroy(m_allocator, std::addressof(old->m_value));
            m_allocator.resource()->deallocate(old, sizeof(node_t), alignof(node_t));
        }

        return dynamic_cast<iterator>(first);
    }

    template <typename Tp>
    void singly_linked_list<Tp>::push_front(const Tp &value) {
        emplace_front(value);
    }

    template <typename Tp>
    void singly_linked_list<Tp>::push_front(Tp &&value) {
        emplace_front(std::move(value));
    }

    template <typename Tp>
    template <class... Args>
    void singly_linked_list<Tp>::emplace_front(Args &&...args) {
        emplace_after(before_begin(), std::forward<Args>(args)...);
    }

    template <typename Tp>
    template <class... Args>
    typename singly_linked_list<Tp>::reference singly_linked_list<Tp>::emplace_front(Args &&...args) {
        auto it = emplace_after(before_begin(), std::forward<Args>(args)...);
        return *it;
    }

    template <typename Tp>
    void singly_linked_list<Tp>::pop_front() {
        erase_after(before_begin());
    }

    template <typename Tp>
    void singly_linked_list<Tp>::resize(const size_type count) {
        if (count < this->m_size)         // need to erase elements
            resize_erase(count);
        else if (count > this->m_size)    // need to add elements
            resize_emplace(count, Tp());
        this->m_size = count;
    }

    template <typename Tp>
    void singly_linked_list<Tp>::resize(const size_type count, const Tp &value) {
        if (count < this->m_size) 
            resize_erase(count);
        else if (count > this->m_size) 
            resize_emplace(count, value);
        this->m_size = count;
    }

    template <typename Tp>
    void singly_linked_list<Tp>::swap(singly_linked_list<Tp> &other) noexcept(std::allocator_traits<allocator_type>::is_always_equal::value) {
        if (m_allocator == other.m_allocator) {
            auto tail = other.empty() ? &m_head : other.m_tail;
            auto other_tail = this->empty() ? &other.m_head : m_tail;

            using std::swap;
            swap(m_head.m_next, other.m_head.m_next);
            swap(this->m_size, other.m_size);
            m_tail = tail;
            other.m_tail = other_tail;
        }
    }


    //*** Non-Member Function Implementations ***//

    template <typename Tp>
    bool operator==(const singly_linked_list<Tp> &lhs, const singly_linked_list<Tp> &rhs) noexcept {
        return (lhs.size() != rhs.size()) ? false : std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename Tp>
    bool operator!=(const singly_linked_list<Tp> &lhs, const singly_linked_list<Tp> &rhs) noexcept {
        return !operator==(lhs, rhs);
    }


}   // namespace dsl


#endif // DSL_SINGLY_LINKED_LIST_H