#ifndef DSL_LIST_H
#define DSL_LIST_H


#include "list_base.h"

#include <initializer_list>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <stdexcept>


namespace dsl {

    namespace details {

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

            using iterator_concept = std::contiguous_iterator_tag;
            using iterator_category = std::random_access_iterator_tag;

            using pointer = const value_type*;
            using reference = const value_type&;


            //*** Member Functions ***//

            list_const_iterator() noexcept 
                : m_ptr(nullptr) {}

            list_const_iterator(pointer ptr) noexcept 
                : m_ptr(ptr) {}

            [[nodiscard]] constexpr reference operator*() const noexcept { 
                return *m_ptr;
            }

            [[nodiscard]] constexpr pointer operator->() const noexcept {
                return m_ptr;
            }

            constexpr list_const_iterator& operator++() noexcept {
                m_ptr++;
                return *this;
            }

            constexpr list_const_iterator operator++(int) noexcept {
                list_const_iterator it(*this);
                ++(*this);
                return it;
            }

            constexpr list_const_iterator& operator--() noexcept {
                m_ptr--;
                return *this;
            }

            constexpr list_const_iterator operator--(int) noexcept {
                list_const_iterator it(*this);
                --(*this);
                return it;
            }

            constexpr list_const_iterator& operator+=(const difference_type offset) noexcept {
                m_ptr += offset;
                return *this;
            }

            [[nodiscard]] constexpr list_const_iterator operator+(const difference_type offset) const noexcept {
                list_const_iterator it(*this);
                it += offset;
                return it;
            }

            constexpr list_const_iterator& operator-=(const difference_type offset) noexcept {
                return *this += -offset;
            }

            [[nodiscard]] constexpr list_const_iterator operator-(const difference_type offset) const noexcept {
                list_const_iterator it(*this);
                it -= offset;
                return it;
            }

            [[nodiscard]] constexpr difference_type operator-(const list_const_iterator &other) const noexcept {
                return m_ptr - other.m_ptr;
            }

            [[nodiscard]] constexpr reference operator[](const difference_type offset) const noexcept {
                return *(*this + offset);
            }

            [[nodiscard]] constexpr auto operator<=>(const list_const_iterator&) const noexcept = default;;


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

            using pointer = Tp*;
            using reference = Tp&;


            //*** Member Functions ***//

            list_iterator() noexcept 
                : m_ptr(nullptr) {}

            list_iterator(pointer ptr) noexcept 
                : m_ptr(ptr) {}

            [[nodiscard]] constexpr reference operator*() const noexcept { 
                return *m_ptr;
            }

            [[nodiscard]] constexpr pointer operator->() const noexcept {
                return m_ptr;
            }

            constexpr list_iterator& operator++() noexcept {
                base_t::operator++();
                return *this;
            }

            constexpr list_iterator operator++(int) noexcept {
                list_iterator it(*this);
                base_t::operator++();
                return it;
            }

            constexpr list_iterator& operator--() noexcept {
                base_t::operator--();
                return *this;
            }

            constexpr list_iterator operator--(int) noexcept {
                list_iterator it(*this);
                base_t::operator--();
                return it;
            }

            constexpr list_iterator& operator+=(const difference_type offset) noexcept {
                base_t::operator+=(offset);
                return *this;
            }

            [[nodiscard]] constexpr list_iterator operator+(const difference_type offset) const noexcept {
                list_iterator it(*this);
                it += offset;
                return it;
            }

            constexpr list_iterator& operator-=(const difference_type offset) noexcept {
                base_t::operator-=(offset);
                return *this;
            }

            [[nodiscard]] constexpr list_iterator operator-(const difference_type offset) const noexcept {
                list_iterator it(*this);
                it -= offset;
                return it;
            }

            using base_t::operator-;

            [[nodiscard]] constexpr reference operator[](const difference_type offset) const noexcept {
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
            : list_base()
            , m_allocator(allocator)
            , m_data(nullptr) 
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
            clear();
            delete m_data;
            m_data = nullptr;
        }


        //* Assignment operator overloads *//

        list& operator=(const list&);
        list& operator=(list&&);


        //* Assign and allocator access *//

        constexpr void assign(const size_type, const Tp&);
        
        template <class InputIt>
        constexpr void assign(InputIt, InputIt);

        constexpr void assign(std::initializer_list<Tp>);

        constexpr allocator_type get_allocator() const noexcept;


        //* Element Access *//

        constexpr reference at(const size_type);
        constexpr const_reference at(const size_type) const;

        constexpr reference operator[](const size_type pos) {
            return m_data[pos];
        } 

        constexpr const_reference operator[](const size_type pos) const {
            return m_data[pos];
        }

        constexpr reference front() {
            return m_data[0];
        }

        constexpr const_reference front() const {
            return m_data[0];
        }

        constexpr reference back() {
            return m_data[this->m_size - 1];
        }

        constexpr const_reference back() const {
            return m_data[this->m_size - 1];
        }

        constexpr Tp* data() noexcept {
            return m_data;
        }

        constexpr const Tp* data() const noexcept {
            return m_data;
        }


        //* Iterators *//

        constexpr iterator begin() noexcept { 
            return iterator(&m_data[0]); 
        }

        constexpr const_iterator begin() const noexcept { 
            return const_iterator(&m_data[0]); 
        }

        constexpr const_iterator cbegin() const noexcept { 
            return const_iterator(&m_data[0]); 
        }

        constexpr iterator end() noexcept { 
            return iterator(&m_data[this->m_size]); 
        }

        constexpr const_iterator end() const noexcept { 
            return const_iterator(&m_data[this->m_size]); 
        }

        constexpr const_iterator cend() const noexcept { 
            return const_iterator(&m_data[this->m_size]);
        }

        constexpr reverse_iterator rbegin() noexcept { 
            return reverse_iterator(end()); 
        }

        constexpr const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        constexpr const_reverse_iterator crbegin() const noexcept { 
            return const_reverse_iterator(end()); 
        }

        constexpr reverse_iterator rend() noexcept { 
            return reverse_iterator(begin()); 
        }

        constexpr const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(begin());
        }

        constexpr const_reverse_iterator crend() const noexcept { 
            return const_reverse_iterator(begin()); 
        }


        //* Capacity *//

        constexpr void reserve(const size_type);
        constexpr size_type capacity() const noexcept;
        constexpr void shrink_to_fit();

        
        //* Modifiers *//

        constexpr void clear() noexcept;

        constexpr iterator insert(const_iterator, const Tp&);
        constexpr iterator insert(const_iterator, Tp&&);
        constexpr iterator insert(const_iterator, size_type, const Tp&);
        
        template <class InputIt>
        constexpr iterator insert(const_iterator, InputIt, InputIt);

        constexpr iterator insert(const_iterator, std::initializer_list<Tp>);

        template <class... Args>
        constepxr iterator emplace(const_iterator, Args&&...);

        constexpr iterator erase(const_iterator);
        constexpr iterator erase(const_iterator, const_iterator);

        constexpr void push_back(const Tp&);
        constexpr void push_back(Tp&&);

        template <class... Args>
        constexpr reference emplace_back(Args&&...);

        constexpr void pop_back();

        constexpr void resize(const size_type);
        constexpr void resize(const size_type, const Tp&);

        constexpr void swap(list&) noexcept(std::allocator_traits<allocator_type>::propagate_on_container_swap::value || 
                                            std::allocator_traits<allocator_type>::is_always_equal::value);


    private:
        allocator_type m_allocator;
        Tp *m_data;

        void try_assignment(const doubly_linked_list&);
        void try_move(doubly_linked_list&&);
        void resize_erase(const size_type);
        void resize_emplace(const size_type, const Tp&);        
    };



    //****** Member Function Implementations ******//

    //*** Private ***//

    //*** Public ***//

}   // namespace dsl 


#endif // DSL_LIST_H