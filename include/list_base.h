#ifndef DSL_LIST_BASE_H
#define DSL_LIST_BASE_H


#include <cstddef>
#include <limits>


namespace dsl::details {

    template <typename Tp>
    struct iterator_base {
        using value_type = Tp;
        using difference_type = std::ptrdiff_t;
    };

    /**
     * @brief Partially specialized template class serving as the base for all linear containers. 
     * Implements capacity-related functions and defines common member types.
     * 
     * @tparam Tp 
     */
    template <typename Tp>
    class list_base {
    public:

        //*** Member Types ***//

        using value_type = Tp;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using reference = value_type&;
        using const_reference = const value_type&;


        //*** Member Functions ***//

        list_base()
            : m_size(0) {}

        explicit list_base(const size_type count) 
            : m_size(count) {}

        list_base(const list_base&) = default;
        list_base(list_base&&) noexcept = default;
        
        list_base& operator=(const list_base&) = default;
        list_base& operator=(list_base&&) noexcept = default;

        virtual ~list_base() = default;


        //* Capacity *//

        [[nodiscard]] constexpr bool empty() const noexcept {
            return m_size == 0;
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return m_size;
        }

        [[nodiscard]] constexpr size_type max_size() const noexcept {
            return std::numeric_limits<difference_type>::max();
        }

        
    protected:
        size_type m_size;
    };
    
}   // namespace dsl::details


#endif // DSL_LIST_BASE_H