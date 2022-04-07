#ifndef LIST_BASE_H
#define LIST_BASE_H


#include "linear_base.h"

#include <limits>


namespace dsl::details {

    /**
     * @brief Partially specialized template class. Derives the linear_base interface, and
     * further defines member types and implements capacity-specific methods.
     * 
     * @tparam Tp 
     */
    template <typename Tp>
    class list_base : public linear_base<Tp> {
    public:

        //*** Member Types ***//

        // Re-definitions
        using reference = typename linear_base<Tp>::reference;
        using const_reference = typename linear_base<Tp>::const_reference;
        using size_type = typename linear_base<Tp>::size_type;
        using difference_type = typename linear_base<Tp>::difference_type;
        using iterator = typename linear_base<Tp>::iterator;
        using const_iterator = typename linear_base<Tp>::const_iterator;


        //*** Member Functions ***//

        explicit list_base(const size_type capacity)
            : m_capacity(capacity)
            , m_size(0)
        {}

        list_base(const list_base&) = default;
        list_base(list_base&&) noexcept = default;
        
        list_base& operator=(const list_base&) = default;
        list_base& operator=(list_base&&) noexcept = default;

        virtual ~list_base() = default;


        //* Access *//
        [[nodiscard]] constexpr bool empty() const noexcept         { return m_size == 0; }
        [[nodiscatd]] constexpr size_type capacity() const noexcept { return m_capacity; }
        [[nodiscard]] constexpr size_type size() const noexcept     { return m_size; }
        [[nodiscard]] constexpr size_type max_size() const noexcept { return std::numeric_limits<difference_type>::max(); }

    protected:
        size_type m_capacity;
        size_type m_size;
    };

}   // namespace dsl::details


#endif // LIST_BASE_H