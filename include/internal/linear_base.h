#ifndef LINEAR_BASE_H
#define LINEAR_BASE_H


#include <cstddef>
#include <initializer_list>


namespace dsl::details {

    /**
     * @brief Base class for all iterators. Defines shared member types.
     * 
     * @tparam Tp 
     */
    template <typename Tp>
    class iterator_base {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Tp;
    };


    /**
     * @brief Base class (pure virtual interface) for all linear containers. Defines shared member types 
     * as well as the public interface.
     * 
     * @tparam Tp 
     */
    template <typename Tp>
    class linear_base {
    public:

        //*** Member Types***//
        using value_type = Tp;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
    
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using iterator = iterator_base<Tp>;
        using const_iterator = iterator_base<Tp>;


        //*** Member Functions ***//
        linear_base() = default;
        linear_base(const linear_base&) = default;
        linear_base(linear_base&&) noexcept = default;
        
        linear_base& operator=(const linear_base&) = default;
        linear_base& operator=(linear_base&&) noexcept = default;

        virtual ~linear_base() = default;


        //* Element Access *//
        virtual constexpr iterator begin() noexcept = 0;
        virtual constexpr const_iterator begin() const noexcept = 0;
        virtual constexpr const_iterator cbegin() const noexcept = 0;

        virtual constexpr iterator end() noexcept = 0;
        virtual constexpr const_iterator end() const noexcept = 0;
        virtual constexpr const_iterator cend() const noexcept = 0;

        virtual constexpr reference front() = 0;
        virtual constexpr const_reference front() const = 0;

        virtual constexpr reference back() = 0;
        virtual constexpr const_reference back() const = 0;


        //* Modifiers *//
        virtual constexpr void clear() noexcept = 0;
        
        virtual constexpr iterator insert(const_iterator, const Tp&) = 0;
        virtual constexpr iterator insert(const_iterator, Tp&&) = 0;
        virtual constexpr iterator insert(const_iterator, size_type, const Tp&) = 0;
        virtual constexpr iterator insert(const_iterator, std::initializer_list<Tp>) = 0;

        virtual constexpr iterator erase(const_iterator) = 0;
        virtual constexpr iterator erase(const_iterator, const_iterator) = 0;

        virtual constexpr void push_back(const Tp&) = 0;
        virtual constexpr void push_back(Tp&&) = 0;

        virtual constexpr void pop_back() = 0;

        virtual constexpr void resize(size_type) = 0;
        virtual constexpr void resize(size_type, const Tp&) = 0;
    };

}   // namespace dsl::details


#endif // LINEAR_BASE_H