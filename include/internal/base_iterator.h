#ifndef DS_LIST_BASE_ITERATOR_H
#define DS_LIST_BASE_ITERATOR_H


#include <utility>
#include "traits.h"


namespace linear::internal::iterators
{
    template <Comparable Tp, bool isConst>
    struct base_iterator
    {
        using difference_type = std::ptrdiff_t;
        using value_type = Tp;
        using pointer = std::conditional_t<isConst, const Tp*, Tp*>;
        using reference = std::conditional_t<isConst, const Tp&, Tp&>;

    };  // struct base_iterator

}   // namespace linear::internal::iterators

#endif //DS_LIST_BASE_ITERATOR_H
