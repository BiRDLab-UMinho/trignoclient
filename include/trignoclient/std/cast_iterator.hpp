//------------------------------------------------------------------------------
/// @file       cast_iterator.hpp
/// @author     João André
///
/// @brief      Header file providing declaration & definition of std::cast_iterator<> class template.
///
/// std::cast_iterator<> is a random access iterator class template derived from std::iterator<> that allows a different public value type
/// than its underlying/source container, provided that they are implicitly convertible (effectively 'masks' underlying type).
/// Intended to work with std::indexer<>, but was designed generically with a broader scope of application in mind, being compatible with
/// STL sequence containers (std::vector, std::array, std::deque, std::list and potentially std::string)
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_CASTITERATOR_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_CASTITERATOR_HPP_

#include <cassert>
#include <iterator>
#include <type_traits>

namespace std {

//------------------------------------------------------------------------------
/// @brief      Generic class describing a *random access* bidirectional iterator, with different container and dereference (target) type.
///             Allows masking an interator to a different type when types are implicitly convertible.
///
/// @tparam     Container  Generic container of descriptor/tagged type instances.
///                        Must fit STL's Container & SequenceContainer (partial) requirements.
/// @tparam     T          Output/'public' type
///
/// @note       T and Container::value_type must be implicitly convertible.
///
/// @note       Useful when handling types that are interchangeable (e.g. base/derived on polymorphic implementations).
///
/// @note       Meets STL *RandomAccessIterator* <a href="https://cplusplus.com/reference/iterator/RandomAccessIterator/">requirements</a>,
///             and is compatible with STL *SequenceContainer* types.
///
template < typename Container, typename T = typename Container::value_type >
class cast_iterator : std::iterator< std::random_access_iterator_tag, T > {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Base public types.
    ///
    /// @note       Required for use of STL iterator utilities e.g. std::next(), std::distance().
    ///
    using value_type        = typename std::iterator< std::random_access_iterator_tag, T >::value_type;
    using iterator_category = typename std::iterator< std::random_access_iterator_tag, T >::iterator_category;
    using pointer           = typename std::iterator< std::random_access_iterator_tag, T >::pointer;
    using reference         = typename std::iterator< std::random_access_iterator_tag, T >::reference;
    using difference_type   = typename std::iterator< std::random_access_iterator_tag, T >::difference_type;

    //--------------------------------------------------------------------------
    /// @brief      Static assertion on data types/template arguments.
    ///             Required in order to avoid ill-formed expressions within class definition.
    ///
    static_assert(is_convertible< typename Container::value_type, T >() && is_convertible< T, typename Container::value_type >(),
                  "UNDERLYING CONTAINER ELEMENT TYPE [Container::value_type] MUST BE IMPLICITLY CONVERTIBLE!");

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param      container  Container
    /// @param[in]  pos        Initial iterator position w.r.t. container.
    ///
    explicit cast_iterator(Container* container, unsigned int pos = 0);

    // cast_iterator(const cast_iterator<Container,T>& other) { printf("copy!"); };

    //--------------------------------------------------------------------------
    /// @brief      Increment operator (self).
    ///
    cast_iterator& operator++();

    //--------------------------------------------------------------------------
    /// @brief      Increment operator (copy).
    ///
    cast_iterator operator++(int);

    //--------------------------------------------------------------------------
    /// @brief      Decrement operator (self).
    ///
    cast_iterator& operator--();

    //--------------------------------------------------------------------------
    /// @brief      Decrement operator (copy).
    ///
    cast_iterator operator--(int);

    //--------------------------------------------------------------------------
    /// @brief      Conpound addition operator. Increments the iterator *n* times.
    ///
    /// @param[in]  n     Number of increments.
    ///
    cast_iterator< Container, T >& operator+=(const difference_type& n);

    //--------------------------------------------------------------------------
    /// @brief      Conpound subtraction operator. Decrements the iterator *n* times.
    ///
    /// @param[in]  n     Number of decrements.
    ///
    cast_iterator< Container, T >& operator-=(const difference_type& n);

    //--------------------------------------------------------------------------
    /// @brief      Dereference operator.
    ///
    /// @return     Reference to container value @ current iterator position.
    ///
    T& operator*();

    //--------------------------------------------------------------------------
    /// @brief      *const* dereference operator.
    ///
    /// @return     Reference to container value @ current iterator position.
    ///
    const T& operator*() const;

    //--------------------------------------------------------------------------
    /// @brief      Conversion operator to iterator of the underlying container type.
    ///
    operator typename Container::iterator();

    //--------------------------------------------------------------------------
    /// @brief      Conversion operator to *const* iterator of the underlying container type.
    ///
    operator typename Container::const_iterator() const;

    //--------------------------------------------------------------------------
    /// @brief      Equality operator.
    ///
    /// @param[in]  lhs  Left operand (iterator to compare).
    /// @param[in]  rhs  Right operand (iterator to compare).
    ///
    /// @return     True if iterators @ same position w.r.t container, false othewise.
    ///
    template < typename oContainer, typename oT >
    friend bool operator==(const cast_iterator< oContainer, oT >& lhs, const cast_iterator< oContainer, oT >& rhs);

    //------------------------------------------------------------------------------
    /// @brief      'Less than' comparison operator.
    ///
    /// @param[in]  lhs  Left operand (iterator to compare).
    /// @param[in]  rhs  Right operand (iterator to compare).
    ///
    /// @return     True if *lhs* @ previous position w.r.t container than *rhs*, false othewise.
    ///
    template < typename oContainer, typename oT >
    friend bool operator<(const cast_iterator< oContainer, oT >& lhs, const cast_iterator< oContainer, oT >& rhs);

    //--------------------------------------------------------------------------
    /// @brief      Subtraction operator.
    ///
    /// @param[in]  lhs  Left operand (iterator to subtract to).
    /// @param[in]  rhs  Right operand (iterator to subtract).
    ///
    /// @return     Difference between iterator positions.
    ///
    /// @note       Required as part of STL *RandomAccessIterator* requirements, allows use of STL iterator utilities with cast_iterator.
    ///
    template < typename oContainer, typename oT >
    friend typename cast_iterator< oContainer, oT >::difference_type operator-(const cast_iterator< oContainer, oT >& lhs, const cast_iterator< oContainer, oT >& rhs);

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Iterator position.
    ///
    unsigned int _pos;

    //--------------------------------------------------------------------------
    /// @brief      Associated container instance.
    ///
    Container* _container;
};



template < typename Container, typename T >
cast_iterator< Container, T >::cast_iterator(Container* container, unsigned int pos) :
    _container(container),
    _pos(pos) {
        /* ... */
        assert(pos <= _container->size());
}



template < typename Container, typename T >
cast_iterator< Container, T >& cast_iterator< Container, T >::operator++() {
    if (_pos < _container->size()) { _pos++; } return *this;
}



template < typename Container, typename T >
cast_iterator< Container, T > cast_iterator< Container, T >::operator++(int) {
    printf("postfix increment!\n");
    cast_iterator< Container, T > retval = *this;
    ++(*this);
    return retval;
}



template < typename Container, typename T >
cast_iterator< Container, T >& cast_iterator< Container, T >::operator--() {
    if (_pos > 0) {
        _pos--;
    }
    return *this;
}



template < typename Container, typename T >
cast_iterator< Container, T > cast_iterator< Container, T >::operator--(int) {
    cast_iterator< Container, T > retval = *this;
    --(*this);
    return retval;
}



template < typename Container, typename T >
cast_iterator< Container, T >& cast_iterator< Container, T >::operator+=(const typename cast_iterator< Container, T >::difference_type& n) {
    if (static_cast< int >(_pos + n) > static_cast<int>(_container->size()) ) {
        _pos = _container->size();
    } else if (static_cast< int >(_pos + n) < 0) {
        _pos = 0;
    } else {
        _pos += n;
    }
    return *this;
}



template < typename Container, typename T >
cast_iterator< Container, T >& cast_iterator< Container, T >::operator-=(const typename cast_iterator< Container, T >::difference_type& n) {
    if (static_cast< int >(_pos - n) > static_cast<int>(_container->size()) ) {
        _pos = _container->size();
    } else if (static_cast< int >(_pos - n) < 0) {
        _pos = 0;
    } else {
        _pos -= n;
    }
    return *this;
}



template < typename Container, typename T >
T& cast_iterator< Container, T >::operator*() {
    return (*_container)[_pos];
}



template < typename Container, typename T >
const T& cast_iterator< Container, T >::operator*() const {
    return (*_container)[_pos];
}



template < typename Container, typename T >
cast_iterator< Container, T >::operator typename Container::iterator() {
    return (_container->begin() + _pos);
}



template < typename Container, typename T >
cast_iterator< Container, T >::operator typename Container::const_iterator() const {
    return (_container->cbegin() + _pos);
}



//------------------------------------------------------------------------------
/// @brief      Equality operator.
///
/// @param[in]  lhs  Left operand (iterator to compare).
/// @param[in]  rhs  Right operand (iterator to compare).
///
/// @return     True if iterators @ same position w.r.t container, false othewise.
///
template < typename oContainer, typename oT >
bool operator==(const cast_iterator< oContainer, oT >& lhs, const cast_iterator< oContainer, oT >& rhs) {
    return  ((lhs._container == rhs._container) && (lhs._pos == rhs._pos));
}



//------------------------------------------------------------------------------
/// @brief      Inequality operator.
///
/// @param[in]  lhs  Left operand (iterator to compare).
/// @param[in]  rhs  Right operand (iterator to compare).
///
/// @return     True if iterators *not* @ same position w.r.t container, false othewise.
///
/// @note       Wraps around equality operator, thus does not need to be friend to cast_iterator<>.
///
template < typename Container, typename T >
bool operator!=(const cast_iterator< Container, T >& lhs, const cast_iterator< Container, T >& rhs) {
    return !(lhs == rhs);
}



//------------------------------------------------------------------------------
/// @brief      'Less than' comparison operator.
///
/// @param[in]  lhs  Left operand (iterator to compare).
/// @param[in]  rhs  Right operand (iterator to compare).
///
/// @return     True if *lhs* @ previous position w.r.t container than *rhs*, false othewise.
///
template < typename oContainer, typename oT >
bool operator< (const cast_iterator< oContainer, oT >& lhs, const cast_iterator< oContainer, oT >& rhs) {
    return ((lhs._container == rhs._container) && (lhs._pos < rhs._pos));
}



//------------------------------------------------------------------------------
/// @brief      'Greater than' comparison operator.
///
/// @param[in]  lhs  Left operand (iterator to compare).
/// @param[in]  rhs  Right operand (iterator to compare).
///
/// @return     True if *lhs* @ later position w.r.t container than *rhs*, false othewise.
///
/// @note       Wraps around '<' operator, thus does not need to be friend to cast_iterator<>.
///
template < typename Container, typename T >
bool operator> (const cast_iterator< Container, T >& lhs, const cast_iterator< Container, T >& rhs) {
    return operator< (rhs, lhs);
}



//------------------------------------------------------------------------------
/// @brief      'Greater or equal than' comparison operator.
///
/// @param[in]  lhs  Left operand (iterator to compare).
/// @param[in]  rhs  Right operand (iterator to compare).
///
/// @return     True if *lhs* @ previous or equal position w.r.t container than *rhs*, false othewise.
///
/// @note       Wraps around '>' operator, thus does not need to be friend to cast_iterator<>.
///
template < typename Container, typename T >
inline bool operator<=(const cast_iterator< Container, T >& lhs, const cast_iterator< Container, T >& rhs) {
    return !operator> (lhs, rhs);
}



//------------------------------------------------------------------------------
/// @brief      'Greater or equal than' comparison operator.
///
/// @param[in]  lhs  Left operand (iterator to compare).
/// @param[in]  rhs  Right operand (iterator to compare).
///
/// @return     True if *lhs* @ later or equal position w.r.t container than *rhs*, false othewise.
///
/// @note       Wraps around '<' operator, thus does not need to be friend to cast_iterator<>.
///
template < typename Container, typename T >
inline bool operator>=(const cast_iterator< Container, T >& lhs, const cast_iterator< Container, T >& rhs) {
    return !operator< (lhs, rhs);
}



template < typename oContainer, typename oT >
typename cast_iterator< oContainer, oT >::difference_type operator-(const cast_iterator< oContainer, oT >& lhs, const cast_iterator< oContainer, oT >& rhs) {
    return (lhs._pos - rhs._pos);
}



template < typename Container, typename T >
cast_iterator< Container, T > operator+(cast_iterator< Container, T > lhs, const typename cast_iterator< Container, T >::difference_type& n) {
    lhs += n;
    return lhs;
}



template < typename Container, typename T >
cast_iterator< Container, T > operator-(cast_iterator< Container, T > lhs, const typename cast_iterator< Container, T >::difference_type& n) {
    lhs += n;
    return lhs;
}

}  // namespace std

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_CASTITERATOR_HPP_
