#ifndef DSL_DOUBLY_LINKED_LIST_H
#define DSL_DOUBLY_LINKED_LIST_H


#include "list_base.h"

#include <initializer_list>
#include <iterator>
#include <memory>
#include <memory_resource>


namespace dsl {

    template <typename Tp> class doubly_linked_list;

    namespace details {

        template <typename Tp> struct doubly_node;

        /**
         * @brief Base representation for a node in a doubly linked list.
         * Contains a non-owning raw pointer to the next node in the list
         * 
         * @tparam Tp 
         */
        template <typename Tp>
        struct doubly_node_base {
            doubly_node_base()
                : m_prev(nullptr)
                , m_next(nullptr) {}

            // Explicitly disallow copy behaviour
            doubly_node_base(const doubly_node_base&) = delete;
            doubly_node_base& operator=(const doubly_node_base&) = delete;

            doubly_node<Tp> *m_prev;
            doubly_node<Tp> *m_next;
        };   

        /**
         * @brief Node in a doubly-linked list. Derives the base representation and wraps
         * the value type in an anonymous union to help align raw bytes.
         * 
         * @tparam Tp 
         */
        template <typename Tp>
        struct doubly_node : doubly_node_base<Tp> {
            union {
                Tp m_value;
            };
        };


        /**
         * @brief Iterator with const pointer and reference member types.
         * Adheres to the named requirements of LegacyBidirectionalIterator.
         * 
         * @tparam Tp 
         */
        template <typename Tp>
        class doubly_const_iterator : public iterator_base<Tp> {
        public:

            //*** Member Types ***//

            using value_type = typename iterator_base<Tp>::value_type;
            using difference_type = typename iterator_base<Tp>::difference_type;

            using iterator_category = std::bidirectional_iterator_tag;
            using pointer = const value_type*;
            using reference = const value_type*&;


            //*** Member Functions ***//

            [[nodiscard]] constexpr pointer operator->() const noexcept {
                return std::addressof(m_curr->m_value);
            }

            [[nodiscard]] constexpr reference operator*() const noexcept {
                return m_curr->m_value;
            }

            constexpr doubly_const_iterator& operator++() noexcept {
                m_curr = m_curr->m_next;
                return *this;
            }

            constexpr doubly_const_iterator operator++(int) noexcept {
                doubly_const_iterator it(*this);
                ++(*this);
                return it;
            }

            constexpr doubly_const_iterator& operator--() noexcept {
                m_curr = m_curr->m_prev;
                return *this;
            }

            constexpr doubly_const_iterator operator--(int) noexcept {
                doubly_const_iterator it(*this);
                ++(*this);
                return it;
            }

            [[nodiscard]] constexpr auto operator<=>(const doubly_const_iterator&) const noexcept = default;


        protected:
            friend class doubly_linked_list<Tp>;

            doubly_node_base<Tp> *m_curr;

            // Non-public explicit constructor to enable iterator construction for derived classes and friend classes
            constexpr explicit doubly_const_iterator(const doubly_node_base<Tp> *curr) 
                : m_curr(const_cast<double_node_base<Tp>*>(curr)) {}
        };


        template <typename Tp>
        class doubly_iterator : public doubly_const_iterator<Tp> {
        public:

            //*** Member Types ***//

            using pointer = Tp*;
            using reference = Tp&;


            //*** Member Functions ***//

            [[nodiscard]] constexpr pointer operator->() const noexcept {
                return std::addressof(this->m_curr->m_value);
            }

            [[nodiscard]] constexpr reference operator*() const noexcept {
                return this->m_curr->m_value;
            }

            constexpr doubly_const_iterator& operator++() noexcept {
                this->m_curr = this->m_curr->m_next;
                return *this;
            }

            constexpr doubly_const_iterator operator++(int) noexcept {
                doubly_iterator it(*this);
                ++(*this);
                return it;
            }

            constexpr doubly_const_iterator& operator--() noexcept {
                this->m_curr = this->m_curr->m_prev;
                return *this;
            }

            constexpr doubly_const_iterator operator--(int) noexcept {
                doubly_iterator it(*this);
                ++(*this);
                return it;
            }


            private:
                friend class doubly_linked_list<Tp>;

                explicit doubly_iterator(const doubly_node_base<Tp> *curr) 
                    : doubly_const_iterator(prev) {}
        };

    }   // namespace details


    template <typename Tp>
    class doubly_linked_list : public details::list_base<Tp> {
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

        using iterator = typename details::doubly_iterator<Tp>;
        using const_iterator = typename details::doubly_const_iterator<Tp>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;


        //*** Member Functions ***//

        //* Constructors *//

        explicit doubly_linked_list(allocator_type allocator = {})
            : list_base()
            , m_allocator(allocator)
            , m_head(nullptr)
            , m_tail(nullptr)
        {}

        doubly_linked_list(const size_type count, 
                           const Tp &value, 
                           allocator_type allocator = {})
            : doubly_linked_list(allocator)
        { assign(count, value); }

        explicit doubly_linked_list(const size_type count, 
                                    allocator_type allocator = {})
            : doubly_linked_list(count, Tp(), allocator) 
        {}

        template <class InputIt>
        doubly_linked_list(InputIt first, InputIt last,
                           allocator_type allocator = {})
            : doubly_linked_list(allocator)
        { insert(begin(), first, last); }

        doubly_linked_list(std::initializer_list<Tp> init, 
                           allocator_type allocator = {})
            : doubly_linked_list(init.begin(), init.end(), allocator) 
        {}


        //* Copy Constructors *//

        doubly_linked_list(const doubly_linked_list &other, 
                           allocator_type allocator = {})
            : doubly_linked_list(other.size(), allocator)
        { operator=(other); } 

        doubly_linked_list(const doubly_linked_list &other)
            : doubly_linked_list(other, std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.get_allocator())) 
        {}


        //* Move Constructors *//
        
        doubly_linked_list(doubly_linked_list &&other, 
                           allocator_type allocator = {})
            : doubly_linked_list(other.size(), allocator) 
        { operator=(std::move(other)); }

        doubly_linked_list(doubly_linked_list &&other)
            : doubly_linked_list(other, other.get_allocator())
        {}


        //* Destructor *//
        ~doubly_linked_list() {
            clear();
        }


        //* Assignment operator overloads *//

        doubly_linked_list& operator=(const doubly_linked_list&);
        doubly_linked_list& operator=(doubly_linked_list&&);


        //* Assign and allocator access *//
        
        constexpr void assign(const size_type, const Tp&);

        template <class InputIt>
        constexpr void assign(InputIt, InputIt);

        constexpr void assign(std::initializer_list<Tp>);

        constexpr allocator_type get_allocator() const noexcept;


        //* Element Access *//

        constexpr reference front() {
            return m_head->m_value;
        }

        constexpr const_reference front() const {
            return m_head->m_value;
        }

        constexpr reference back() {
            return m_tail->m_value;
        }

        constexpr const_reference back() const {
            return m_tail->m_value;
        }


        //* Iterators *//

        constexpr iterator begin() noexcept {
            return iterator(m_head);
        }

        constexpr const_iterator begin() const noexcept {
            return const_iterator(m_head);
        }

        constexpr const_iterator cbegin() const noexcept {
            return const_iterator(m_head);
        }

        constexpr iterator end() noexcept {
            return iterator(m_tail);
        }

        constexpr const_iterator end() const noexcept {
            return const_iterator(m_tail);
        }

        constexpr const_iterator cend() const noexcept {
            return const_iterator(m_tail);
        }

        constexpr reverse_iterator rbegin() noexcept {
            return reverse_iterator(m_tail);
        }

        constexpr const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(m_tail);
        }

        constexpr const_reverse_iterator crbegin() const noexcept {
            return const_reverse_iterator(m_tail);
        }

        constexpr reverse_iterator rend() noexcept {
            return reverse_iterator(m_head);
        }

        constexpr const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(m_head);
        }

        constexpr const_reverse_iterator crend() const noexcept {
            return const_reverse_iterator(m_head);
        }


        //* Modifiers *//

        constexpr void clear() noexcept;

        constexpr iterator insert(const_iterator, const Tp&);
        constexpr iterator insert(const_iterator, Tp&&);
        constexpr iterator insert(const_iterator, size_type, const Tp&);

        template <class InputIt>
        constexpr iterator insert(const_iterator, InputIt, InputIt);

        constexpr iterator insert(const_iterator, std::initializer_list<Tp>);

        template <class... Args>
        constexpr iterator emplace(const_iterator, Args&&...);

        constexpr iterator erase(const_iterator);
        constexpr iterator erase(const_iterator, const_iterator);

        constexpr void push_back(const Tp&);
        constexpr void push_back(Tp&&);

        template <class... Args>
        constexpr reference emplace_back(Args&&...);

        constexpr void pop_back();

        constexpr void push_front(const Tp&);
        constexpr void push_front(Tp&&);

        template <class... Args>
        constexpr reference emplace_front(Args&&...);

        constexpr void pop_front();

        constexpr void resize(const size_type);
        constexpr void resize(const size_type, const Tp&);

        constexpr void swap(doubly_linked_list&) noexcept(std::allocator_traits<allocator_type>::is_always_equal::value);


        //* Operations *//

        void merge(doubly_linked_list&);
        void merge(doubly_linked_list&&);

        template <class Compare>
        void merge(doubly_linked_list&, Compare);

        template <class Compare>
        void merge(doubly_linked_list&&, Compare);

        void splice(const_iterator, doubly_linked_list&);
        void splice(const_iterator, doubly_linked_list&&);
        void splice(const_iterator, doubly_linked_list&, const_iterator);
        void splice(const_iterator, doubly_linked_list&&, const_iterator);
        void splice(const_iterator, doubly_linked_list&, const_iterator, const_iterator);
        void splice(const_iterator, doubly_linked_list&&, const_iterator, const_iterator);

        size_type remove(const Tp&);

        template <class UnaryPredicate>
        size_type remove_if(UnaryPredicate);

        void reverse() noexcept;

        size_type unique();

        template <class BinaryPredicate>
        size_type unique(BinaryPredicate);

        void sort();

        template <class Compare>
        void sort(Compare);


    private:
        using node_base_t = typename details::doubly_node_base<Tp>;
        using node_t = typename details::doubly_node<Tp>;

        allocator_type m_allocator;
        node_base_t *m_head;
        node_base_t *m_tail;

        void try_assignment(const doubly_linked_list&);
        void try_move(doubly_linked_list&&);
        void resize_erase(const size_type);
        void resize_emplace(const size_type, const Tp&);
    };



    //****** Member Function Implementation ******//

    //*** Private ***//

    template <typename Tp>
    void doubly_linked_list<Tp>::try_assignment(const doubly_linked_list<Tp> &other) {
        clear();
        for (const Tp &value : other) 
            push_back(value);
    }

    template <typename Tp>
    void doubly_linked_list<Tp>::try_move(doubly_linked_list<Tp> &&other) {
        clear();
        swap(other);
    }

    template <typename Tp>
    void doubly_linked_list<Tp>::resize_erase(const size_type count) {
        auto first = std::advance(begin(), count);
        erase(first, end());
    }

    template <typename Tp>
    void doubly_linked_list<Tp>::resize_emplace(const size_type count, const Tp &value) {
        for (auto i = 0; i < count; ++i) 
            push_back(value);
    }


    //*** Public ***//

    //* Assignment Operator Overloads *//

    template <typename Tp>
    doubly_linked_list<Tp>& doubly_linked_list<Tp>::operator=(const doubly_linked_list<Tp> &other) {
        if (this != &other) 
            try_assignment(other);
        return *this;
    }

    template <typename Tp>
    doubly_linked_list<Tp>& doubly_linked_list<Tp>::operator=(const doubly_linked_list<Tp> &other) {
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
    doubly_linked_list<Tp>& doubly_linked_list<Tp>::assign(const size_type count, const Tp &value) {
        resize(count);
        for (auto it = begin(); it != end(); ++it) 
            *it = value;
    }

    template <typename Tp>
    template <class InputIt>
    constexpr void doubly_linked_list<Tp>::assign(InputIt first, InputIt last) {
        resize(std::distance(first, last));
        auto it = begin();
        for (; first != last && it != end(); ++first)
            *it = *first;
    }

    template <typename Tp>
    constexpr void doubly_linked_list<Tp>::assign(std::initializer_list<Tp> ilist) {
        resize(ilist.size());
        auto it = begin();
        for (auto ilistIt = ilist.begin(); ilistIt != ilist.end() && it != end(); ++ilistIt)
            *it = *ilistIt; 
    }

    template <typename Tp>
    constexpr doubly_linked_list<Tp>::allocator_type doubly_linked_list<Tp>::get_allocator() const noexcept {
        return m_allocator;
    }


    //* Modifiers *//
    
    template <typename Tp>
    constexpr void doubly_linked_list<Tp>::clear() noexcept {
        erase(begin(), end());
    }

    template <typename Tp>
    constexpr doubly_linked_list<Tp>::iterator doubly_linked_list<Tp>::insert(const_iterator pos, const Tp &value) {
        return emplace(pos, value);
    }

    template <typename Tp>
    constexpr doubly_linked_list<Tp>::iterator doubly_linked_list<Tp>::insert(const_iterator pos, Tp &&value) {
        return emplace(pos, std::move(value));
    }

    template <typename Tp>
    constexpr doubly_linked_list<Tp>::iterator doubly_linked_list<Tp>::insert(const_iterator pos, const size_type count, const Tp &value) {
        auto it = dynamic_cast<iterator>(pos);
        for (auto i = 0; i < count; ++i) 
            it = emplace(it, value);
        return it;
    }

    template <typename Tp>
    template <class InputIt>
    constexpr doubly_linked_list<Tp>::iterator doubly_linked_list<Tp>::insert(const_iterator pos, InputIt first, InputIt last) {
        auto it = dynamic_cast<iterator>(pos);
        for (; first != last; ++first) 
            it = emplace(it, *first);
        return it;
    }

    template <typename Tp>
    constexpr doubly_linked_list<Tp>::iterator doubly_linked_list<Tp>::insert(const_iterator pos, std::initializer_list<Tp> ilist) {
        auto it = dynamic_cast<iterator>(pos);
        for (auto ilistIt = ilist.begin(); ilistIt != ilist.end(); ++ilistIt)
            it = emplace(it, *ilistIt);
        return it;
    }

    template <typename Tp>
    template <class... Args>
    constexpr doubly_linked_list<Tp>::iterator doubly_linked_list<Tp>::emplace(const_iterator pos, Args &&...args) {
        auto pNode = static_cast<node_t*>(m_allocator.resource()->allocate(sizeof(node_t), alignof(node_t)));

        try {
            m_allocator.construct(std::addressof(pNode->m_value), std::forward<Args>(args)...);
        } catch (...) {
            m_allocator.resource()->deallocate(pNode, sizeof(node_t), alignof(node_t));
            throw;
        }

        pNode->m_next = pos.m_curr->m_next;
        pNode->m_prev = pos.m_curr;
        pos.m_curr->m_next->m_prev = pNode;
        pos.m_curr->m_next = pNode;

        if (pos.m_curr == m_tail) 
            m_tail = pNode;
        else if (pos.m_curr == m_head) 
            m_head = pNode;
        
        ++this->m_size;
        return dynamic_cast<iterator>(pos);
    }

    template <typename Tp>
    constexpr doubly_linked_list<Tp>::iterator doubly_linked_list<Tp>::erase(const_iterator pos) {
        return erase(pos, std::next(pos));
    }

    template <typename Tp>
    constexpr doubly_linked_list<Tp>::iterator doubly_linked_list<Tp>::erase(const_iterator first, const_iterator last) {
        auto next = first.m_curr;
        auto past = last.m_curr;

        if (past == nullptr) 
            m_tail = first.m_curr->m_prev;
        
        first.m_curr = past;
        
        while (next != last) {
            auto old = next;
            next = next->m_next;
            --this->m_size;
            std::allocator_traits<allocator_type>::destroy(m_allocator, std::addressof(old));
            m_allocator.resource()->deallocate(old, sizeof(node_t), alignof(node_t));
        }

        return dynamic_cast<iterator>(first);
    }

    template <typename Tp>
    constexpr void doubly_linked_list<Tp>::push_back(const Tp &value) {
        emplace_back(value);
    }

    template <typename Tp>
    constexpr void doubly_linked_list<Tp>::push_back(Tp &&value) {
        emplace_back(std::move(value));
    }

    template <typename Tp>
    template <class... Args>
    constexpr doubly_linked_list<Tp>::reference doubly_linked_list<Tp>::emplace_back(Args &&...args) {
        auto it = emplace(end(), std::forward<Args>(args)...);
        return *it;
    }

    template <typename Tp>
    constexpr void doubly_linked_list<Tp>::pop_back() {
        erase(end());
    }

    template <typename Tp>
    constexpr void doubly_linked_list<Tp>::push_front(const Tp &value) {
        emplace_front(value);
    }

    template <typename Tp>
    constexpr void doubly_linked_list<Tp>::push_front(Tp &&value) {
        emplace_front(std::move(value));
    }

    template <typename Tp>
    template <class... Args>
    constexpr doubly_linked_list<Tp>::reference doubly_linked_list<Tp>::emplace_front(Args &&...args) {
        auto it = emplace(begin(), std::forward<Args>(args)...);
        return *it;
    }

    template <typename Tp>
    constexpr void doubly_linked_list<Tp>::resize(const size_type count) {
        if (count < this->m_size) 
            resize_erase(count);
        else if (count > this->m_size) 
            resize_emplace(count, Tp());
        this->m_size = count;
    }

    template <typename Tp>
    constexpr void doubly_linked_list<Tp>::resize(const size_type count, const Tp &value) {
        if (count < this->m_size) 
            resize_erase(count);
        else if (count > this->m_size) 
            resize_emplace(count, value);
        this->m_size = count;
    }

    template <typename Tp>
    constexpr void doubly_linked_list<Tp>::swap(doubly_linked_list<Tp> &other) noexcept {
        if (m_allocator == other.m_allocator) {
            auto head = other.empty() ? m_tail : other.m_head;
            auto other_head = empty() ? other.m_tail : m_head;

            auto tail = other.empty() ? m_head : other.m_tail;
            auto other_tail = empty() ? other.m_head : m_tail;

            swap(this->m_size, other.m_size);
            m_head = head;
            other.m_head = other_head;
            m_tail = tail;
            other.m_tail = other_tail;
        }
    }

}   // namespace dsl


#endif // DSL_DOUBLY_LINKED_LIST_H