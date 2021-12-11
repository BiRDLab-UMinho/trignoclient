//------------------------------------------------------------------------------
/// @file       sorted_indexer.hpp
/// @author     João André
///
/// @brief      Header file providing declaration & definition of std::sorted_indexer<> class template, as well as specialization std::series<>.
///
/// std::sorted_indexer<> is a pseudo-container class template i.e. container adapter (as per STL definition), that implements high-level indexing & sorting functionality
/// to STL's sequence containers, while also providing a pulic interface matching STL's *Container* and *SequenceContainer* named requirements. This means
/// by itself it can be treated as a container and will work with any of STL's tools (e.g. <utility>), while being versatile enough
///
/// While STL already provides associative containers in std:map<> and std::set<>, the major advantage of std::indexer<> is that it leverages the performance edge
/// of different sequence containers (std::array, std::vector, std::deque, etc), makes no assumption on the ordering/sorting of the underlying data, can be locked
/// to prevent dynamic resizing operations, maintains efficient random access to elements, is extensible, and thus is versatile enough to fit differerent usage scenarios.
///
//------------------------------------------------------------------------------

#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_SORTEDINDEXER_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_SORTEDINDEXER_HPP_

#include <type_traits>
#include <cassert>
#include <algorithm>
#include "indexer.hpp"         // std::indexer<>
#include "range_iterator.hpp"  // std::range_iterator<>


namespace std {

//------------------------------------------------------------------------------
/// @brief      Indexer class template that derives from std::indexer<> that implements sorting by keys and overrides key lookups,
///             in order to optimize for containers where elements can be sorted by descriptor key (arithmetic types).
///
/// @tparam     Container  Generic container of key/tagged type instances.
///                        Must fit STL's Container & SequenceContainer (partial) requirements.
/// @tparam     T          Public/interface type.
/// @tparam     Locked     Whether runtime size manipulation is allowed after initialization.
///                        Useful when size of the container does not change but elements do.
///
/// @note       In order to avoid compromising performance, there is no forceful sorting when adding elements to the indexer.
///             It is up to the user to either add elements in order or call sort() before attempting random access by key.
///
template < typename Container, typename T = typename Container::value_type, bool Locked = false >
class sorted_indexer : public indexer< Container, T, Locked > {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Value key wrapper type (== container value type). Provided to fit with STL 'Container' requirements.
    ///
    using typename indexer< Container, T, Locked >::value_type;

    //--------------------------------------------------------------------------
    /// @brief      Container type. Provided to fit with STL 'Container' requirements.
    ///
    using typename indexer< Container, T, Locked >::container_type;

    //--------------------------------------------------------------------------
    /// @brief      Container element type. Must be an indexable type implicitly convertible to & assignable from T.
    ///
    using typename indexer< Container, T, Locked >::element_type;

    //--------------------------------------------------------------------------
    /// @brief      Container element type. Must be an indexable type implicitly convertible to & assignable from T.
    ///
    using typename indexer< Container, T, Locked >::element_reference;

    //--------------------------------------------------------------------------
    /// @brief      Container element type. Must be an indexable type implicitly convertible to & assignable from T.
    ///
    using typename indexer< Container, T, Locked >::element_const_reference;

    //--------------------------------------------------------------------------
    /// @brief      Descriptor type.
    ///
    using typename indexer< Container, T, Locked >::key_type;

    //--------------------------------------------------------------------------
    /// @brief      Size/index type. Provided to fit with STL 'Container' requirements.
    ///
    using typename indexer< Container, T, Locked >::size_type;

    //--------------------------------------------------------------------------
    /// @brief      Value reference type. Provided to fit with STL 'Container' requirements.
    ///
    using typename indexer< Container, T, Locked >::reference;

    //--------------------------------------------------------------------------
    /// @brief      Value const reference type. Provided to fit with STL 'Container' requirements.
    ///
    using typename indexer< Container, T, Locked >::const_reference;

    //--------------------------------------------------------------------------
    /// @brief      Iterator type. Provided to fit with STL 'Container' requirements.
    ///
    using typename indexer< Container, T, Locked >::iterator;

    //--------------------------------------------------------------------------
    /// @brief      Const iterator type. Provided to fit with STL 'Container' requirements.
    ///
    using typename indexer< Container, T, Locked >::const_iterator;

    //--------------------------------------------------------------------------
    /// @brief      Range iterator type. Allows access to an iteratable subset of underlying container.
    ///
    template < typename oT = value_type >
    using element_range = range_iterator< Container, oT >;

    //--------------------------------------------------------------------------
    /// @brief      Const range iterator type. Allows access to an iteratable subset of underlying container.
    ///
    template < typename oT = value_type >
    using const_element_range = range_iterator< const Container, const oT >;

    //--------------------------------------------------------------------------
    /// @brief      Explicit declaration of base type methods required in sorted_indexer<> definition.
    ///
    using indexer< Container, T, Locked >::begin;
    using indexer< Container, T, Locked >::end;
    using indexer< Container, T, Locked >::operator();

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance, passing the arguments to the Container constructor (Container::Container(...))
    ///
    /// @param      args  Container constructor arguments.
    ///
    /// @tparam     Args  Variadic parameter pack describing types of constructor arguments.
    ///
    /// @note       Discards the need to explicitly declare default, copy and move constructors, required to meet STL 'Container' requirements.
    ///
    template < typename... Args >
    sorted_indexer(Args&&... args);

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new (copied) instance.
    ///
    /// @param[in]  other  Instance to copy/move.
    ///
    /// @note       Only available if Container is *not constructible* with an std::indexer instance.
    ///
    template < typename = typename enable_if< !is_constructible< Container, const sorted_indexer< Container, T, Locked > >::value >::type >
    sorted_indexer(const sorted_indexer< Container, T, Locked >& other);  // note -> Container type can't be constructible with (const indexer<>&) arguments!

    //--------------------------------------------------------------------------
    /// @brief      Get element position from its identifier.
    ///
    /// @param[in]  id  Element key/identifier.
    ///
    /// @return     Position in container if element is found, -1 otherwise.
    ///
    size_type find(const key_type& key) const noexcept override;

    //--------------------------------------------------------------------------
    /// @brief      Sorts the vector according to descriptor keys.
    ///
    /// @note       Keys must be sortable (comparable with '<=').
    ///
    void sort();

    //--------------------------------------------------------------------------
    /// @brief      Get range iterator @ begin of container, with given *size* and *overlap*.
    ///
    /// @param[in]  size       Size of iterator range.
    /// @param[in]  overlap    Consecutive range overlap (for incrementing range). Defaults to 0.
    ///
    /// @tparam     oT         Output iterator value type. Defaults to value_type. Underlying element type *must* be convertible to this type.
    ///
    /// @return     Range iterator with given parameters.
    ///
    template < typename oT = value_type >
    element_range< oT > begin(size_type width, size_type overlap = 0);

    //--------------------------------------------------------------------------
    /// @brief      Get *const* range iterator @ begin of container, with given *size* and *overlap*.
    ///
    /// @param[in]  size       Size of iterator range.
    /// @param[in]  overlap    Consecutive range overlap (for incrementing range). Defaults to 0.
    ///
    /// @tparam     oT         Output iterator value type. Defaults to value_type. Underlying element type *must* be convertible to this type.
    ///
    /// @return     Const range iterator with given parameters.
    ///
    /// @note       Templated return type allows explictly requesting either value_type or element_type references @ *pos*.
    ///
    template < typename oT = value_type >
    const_element_range< oT > begin(size_type width, size_type overlap = 0) const;

    //--------------------------------------------------------------------------
    /// @brief      Get range iterator @ end of container, with given *size* and *overlap*.
    ///
    /// @param[in]  size       Size of iterator range.
    /// @param[in]  overlap    Consecutive range overlap (for incrementing range). Defaults to 0.
    ///
    /// @tparam     oT         Output iterator value type. Defaults to value_type. Underlying element type *must* be convertible to this type.
    ///
    /// @return     Range iterator with given parameters.
    ///
    /// @note       Templated return type allows explictly requesting either value_type or element_type references @ *pos*.
    ///
    template < typename oT = value_type >
    element_range< oT > end(size_type width, size_type overlap = 0);

    //--------------------------------------------------------------------------
    /// @brief      Get *const* range iterator @ end of container, with given *size* and *overlap*.
    ///
    /// @param[in]  size       Size of iterator range.
    /// @param[in]  overlap    Consecutive range overlap (for incrementing range). Defaults to 0.
    ///
    /// @tparam     oT         Output iterator value type. Defaults to value_type. Underlying element type *must* be convertible to this type.
    ///
    /// @return     Const range iterator with given parameters.
    ///
    /// @note       Templated return type allows explictly requesting either value_type or element_type references @ *pos*.
    ///
    template < typename oT = value_type >
    const_element_range< oT > end(size_type width, size_type overlap = 0) const;

    //--------------------------------------------------------------------------
    /// @brief      Get an iterative & iteratable range of samples starting @ given *position*.
    ///
    /// @param[in]  pos      Range start position.
    /// @param[in]  width    Length of range (number of elements). Defaults to 1 (single element range).
    /// @param[in]  overlap  Overlap between consecutive ranges when incrementing range interator. Defaults to 0 (no overlap)
    ///
    /// @tparam     oT       Output iterator value type. Defaults to value_type. Underlying element type *must* be convertible to this type.
    ///
    /// @return     Range iterator instance.
    ///
    /// @note       Single-element width and no overlap imply resulting range iterator behaves like normal random access iterator over container.
    ///
    /// @note       Templated return type allows explictly requesting either value_type or element_type references @ *pos*.
    ///
    template < typename oT = value_type >
    element_range< oT > range(size_type pos, size_type size = 1, size_type overlap = 0);

    //--------------------------------------------------------------------------
    /// @brief      Get an iterative & iteratable *const* range of samples starting @ given *position*.
    ///
    /// @param[in]  pos      Range start position.
    /// @param[in]  width    Length of range (number of elements). Defaults to 1 (single element range).
    /// @param[in]  overlap  Overlap between consecutive ranges when incrementing range interator. Defaults to 0 (no overlap)
    ///
    /// @tparam     oT       Output iterator value type. Defaults to value_type. Underlying element type *must* be convertible to this type.
    ///
    /// @return     Const range iterator instance.
    ///
    /// @note       Single-element width and no overlap imply resulting range iterator behaves like normal random access iterator over container.
    ///
    /// @note       Templated return type allows explictly requesting either value_type or element_type references @ *pos*.
    ///
    template < typename oT = value_type >
    const_element_range< oT > range(size_type pos, size_type size = 1, size_type overlap = 0) const;

    //--------------------------------------------------------------------------
    /// @brief      Get an iterative & iteratable range of samples starting @ element w/ given *key*.
    ///
    /// @param[in]  key      Element key @ start of range.
    /// @param[in]  width    Length of range (number of elements). Defaults to 1 (single element range).
    /// @param[in]  overlap  Overlap between consecutive ranges when incrementing range interator. Defaults to 0 (no overlap)
    ///
    /// @tparam     oT        Output iterator value type. Defaults to value_type. Underlying element type *must* be convertible to this type.
    /// @tparam     Arg       Dummy template argument to force SFINAE check over key_type.
    /// @tparam     <unnamed> SFINAE check that auto-disables operator overload if key_type and size_type are implicitely convertible (e.g. numerical keys).
    ///
    /// @return     Range iterator instance.
    ///
    /// @note       Single-element width and no overlap imply resulting range iterator behaves like normal random access iterator over container.
    ///
    /// @note       Templated return type allows explictly requesting either value_type or element_type references @ *pos*.
    ///
    template < typename oT = value_type, typename Arg = key_type, typename = typename enable_if< !is_convertible< Arg, size_type >::value >::type >
    element_range< oT > range(const key_type& key, size_type width = 1, size_type overlap = 0);

    //--------------------------------------------------------------------------
    /// @brief      Get an iterative & iteratable *const* range of samples starting @ element w/ given *key*.
    ///
    /// @param[in]  key      Element key @ start of range.
    /// @param[in]  width    Length of range (number of elements). Defaults to 1 (single element range).
    /// @param[in]  overlap  Overlap between consecutive ranges when incrementing range interator. Defaults to 0 (no overlap)
    ///
    /// @tparam     oT        Output iterator value type. Defaults to value_type. Underlying element type *must* be convertible to this type.
    /// @tparam     Arg       Dummy template argument to force SFINAE check over key_type.
    /// @tparam     <unnamed> SFINAE check that auto-disables operator overload if key_type and size_type are implicitely convertible (e.g. numerical keys).
    ///
    /// @return     Range iterator instance.
    ///
    /// @note       Single-element width and no overlap imply resulting range iterator behaves like normal random access iterator over container.
    ///
    /// @note       Templated return type allows explictly requesting either value_type or element_type references @ *pos*.
    ///
    template < typename oT = value_type, typename Arg = key_type, typename = typename enable_if< !is_convertible< Arg, size_type >::value >::type >
    const_element_range< oT > range(const key_type& key, size_type width = 1, size_type overlap = 0) const;

    //--------------------------------------------------------------------------
    /// @brief      Function call operator operator providing access to element range between values w/ *from* and *to* keys, respectively.
    ///
    /// @param[in]  from  Key of element @ start of range.
    /// @param[in]  to    Key of element @ end of range.
    ///
    /// @return     Element range for *value_type* (cf. range_iterator).
    ///
    element_range< value_type > operator()(const key_type& from, const key_type& to, size_type overlap = 0);

    //--------------------------------------------------------------------------
    /// @brief      Function call operator operator providing access to element range between values w/ *from* and *to* keys, respectively.
    ///
    /// @param[in]  from  Key of element @ start of range.
    /// @param[in]  to    Key of element @ end of range.
    ///
    /// @return     Element range for *value_type* (cf. range_iterator).
    ///
    const_element_range< value_type > operator()(const key_type& from, const key_type& to, size_type overlap = 0) const;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Underlying data container.
    ///
    using indexer< Container, T, Locked >::_data;
};




template < typename Container, typename T, bool Locked >
template < typename... Args >
sorted_indexer< Container, T, Locked >::sorted_indexer(Args&&... args) :
    indexer< Container, T, Locked >(std::forward< Args >(args)...) {
        /* ... */
        // force sorting when constructing!
        // may add unnecessary overhead when input data is already sorted!
        sort();
}



// comparer for keys of element types
// @todo declare/define within 'tagged.hpp' as operator overloads?
//       it would elminate the needd to have a custom comparer
template < typename Container, typename T, bool Locked >
struct key_comparer {
    // between different tagged elements
    bool operator()(const typename sorted_indexer< Container, T, Locked >::element_type& element, const typename sorted_indexer< Container, T, Locked >::element_type& other) {
        return (element.key < other.key);
    }
    // between a tagged element and a key
    bool operator()(const typename sorted_indexer< Container, T, Locked >::element_type& element, const typename sorted_indexer< Container, T, Locked >::key_type& key) {
        return (element.key < key);
    }
};



template < typename Container, typename T, bool Locked >
typename sorted_indexer< Container, T, Locked >::size_type sorted_indexer< Container, T, Locked >::find(const sorted_indexer< Container, T, Locked >::key_type& key) const noexcept {
    // perform binary search to find first element that is *less or equal*
    auto it = std::lower_bound(_data.begin(), _data.end(), key, key_comparer< Container, T, Locked >());
    // get position in container by computing the distance between
    return std::distance(_data.begin(), it);
}



template < typename Container, typename T, bool Locked >
void sorted_indexer< Container, T, Locked >::sort() {
    std::sort(_data.begin(), _data.end(), key_comparer< Container, T, Locked >());
}



template < typename Container, typename T, bool Locked >
template < typename oT >
sorted_indexer< Container, T, Locked >::element_range< oT > sorted_indexer< Container, T, Locked >::begin(typename sorted_indexer< Container, T, Locked >::size_type width,
                                                                                                          typename sorted_indexer< Container, T, Locked >::size_type overlap) {
    static_assert(is_convertible< value_type, oT >(),
                  "[value_type] MUST BE CONVERTIBLE TO OUTPUT TYPE [oT]");
    //
    auto n = width > this->size() ? this->size() : width;
    return element_range< oT >(&_data, 0, n, overlap);
}



template < typename Container, typename T, bool Locked >
template < typename oT >
sorted_indexer< Container, T, Locked >::const_element_range< oT > sorted_indexer< Container, T, Locked >::begin(typename sorted_indexer< Container, T, Locked >::size_type width,
                                                                                                                typename sorted_indexer< Container, T, Locked >::size_type overlap) const {
    static_assert(is_convertible< value_type, oT >(),
                  "[value_type] MUST BE CONVERTIBLE TO OUTPUT TYPE [oT]");
    //
    auto n = width > this->size() ? this->size() : width;
    return const_element_range< oT >(&_data, 0, n, overlap);
}



template < typename Container, typename T, bool Locked >
template < typename oT >
sorted_indexer< Container, T, Locked >::element_range< oT > sorted_indexer< Container, T, Locked >::end(typename sorted_indexer< Container, T, Locked >::size_type width,
                                                                                                        typename sorted_indexer< Container, T, Locked >::size_type overlap) {
    static_assert(is_convertible< value_type, oT >(),
                  "[value_type] MUST BE CONVERTIBLE TO OUTPUT TYPE [oT]");
    //
    auto n = width > this->size() ? 0 : (this->size() - width);
    return element_range< oT >(&_data, this->size() - n, n, overlap);
}



template < typename Container, typename T, bool Locked >
template < typename oT >
sorted_indexer< Container, T, Locked >::const_element_range< oT > sorted_indexer< Container, T, Locked >::end(typename sorted_indexer< Container, T, Locked >::size_type width,
                                                                                                              typename sorted_indexer< Container, T, Locked >::size_type overlap) const {
    static_assert(is_convertible< value_type, oT >(),
                  "[value_type] MUST BE CONVERTIBLE TO OUTPUT TYPE [oT]");
    //
    auto n = width > this->size() ? 0 : (this->size() - width);
    return const_element_range< oT >(&_data, this->size() - n, n, overlap);
}



template < typename Container, typename T, bool Locked >
template < typename oT >
sorted_indexer< Container, T, Locked >::element_range< oT > sorted_indexer< Container, T, Locked >::range(typename sorted_indexer< Container, T, Locked >::size_type pos,
                                                                                                          typename sorted_indexer< Container, T, Locked >::size_type width,
                                                                                                          typename sorted_indexer< Container, T, Locked >::size_type overlap) {
    static_assert(is_convertible< value_type, oT >(),
                  "[value_type] MUST BE CONVERTIBLE TO OUTPUT TYPE [oT]");
    //
    // construct range iterator on return
    // value casting is performed implictly by range_iterator conversion operators (cf. range_iterator)
    return element_range< oT >(&_data, pos, width, overlap);
}



template < typename Container, typename T, bool Locked >
template < typename oT >
sorted_indexer< Container, T, Locked >::const_element_range< oT > sorted_indexer< Container, T, Locked >::range(typename sorted_indexer< Container, T, Locked >::size_type pos,
                                                                                                                typename sorted_indexer< Container, T, Locked >::size_type width,
                                                                                                                typename sorted_indexer< Container, T, Locked >::size_type overlap) const {
    static_assert(is_convertible< value_type, oT >(),
                  "[value_type] MUST BE CONVERTIBLE TO OUTPUT TYPE [oT]");
    //
    // construct range iterator on return
    // value casting is performed implictly by range_iterator conversion operators (cf. range_iterator)
    return const_element_range< oT >(&_data, pos, width, overlap);
}



template < typename Container, typename T, bool Locked >
template < typename oT, typename, typename >
sorted_indexer< Container, T, Locked >::element_range< oT > sorted_indexer< Container, T, Locked >::range(const typename sorted_indexer< Container, T, Locked >::key_type& key,
                                                                                                          typename sorted_indexer< Container, T, Locked >::size_type width,
                                                                                                          typename sorted_indexer< Container, T, Locked >::size_type overlap) {
    static_assert(is_convertible< value_type, oT >(),
                  "[value_type] MUST BE CONVERTIBLE TO OUTPUT TYPE [oT]");
    //
    // construct range iterator on return
    // value casting is performed implictly by range_iterator conversion operators (cf. range_iterator)
    return element_range< oT >(&_data, find(key), width, overlap);
}



template < typename Container, typename T, bool Locked >
template < typename oT, typename, typename >
sorted_indexer< Container, T, Locked >::const_element_range< oT > sorted_indexer< Container, T, Locked >::range(const typename sorted_indexer< Container, T, Locked >::key_type& key,
                                                                                                                typename sorted_indexer< Container, T, Locked >::size_type width,
                                                                                                                typename sorted_indexer< Container, T, Locked >::size_type overlap) const {
    static_assert(is_convertible< value_type, oT >(),
                  "[value_type] MUST BE CONVERTIBLE TO OUTPUT TYPE [oT]");
    //
    // construct range iterator on return
    // value casting is performed implictly by range_iterator conversion operators (cf. range_iterator)
    return const_element_range< oT >(&_data, find(key), width, overlap);
}



template < typename Container, typename T, bool Locked >
sorted_indexer< Container, T, Locked >::element_range< typename sorted_indexer< Container, T, Locked >::value_type > sorted_indexer< Container, T, Locked >::operator()(const typename sorted_indexer< Container, T, Locked >::key_type& from,
                                                                                                                                                                        const typename sorted_indexer< Container, T, Locked >::key_type& to,
                                                                                                                                                                        typename sorted_indexer< Container, T, Locked >::size_type overlap) {
    auto pos_begin = find(from);
    auto pos_end = find(to);
    assert(pos_end > pos_begin && ("INVALID ELEMENT KEYS"));
    return element_range< value_type>(&_data, pos_begin, pos_end - pos_begin, overlap);
}



template < typename Container, typename T, bool Locked >
sorted_indexer< Container, T, Locked >::const_element_range< typename sorted_indexer< Container, T, Locked >::value_type > sorted_indexer< Container, T, Locked >::operator()(const typename sorted_indexer< Container, T, Locked >::key_type& from,
                                                                                                                                                                              const typename sorted_indexer< Container, T, Locked >::key_type& to,
                                                                                                                                                                              typename sorted_indexer< Container, T, Locked >::size_type overlap) const {
    auto pos_begin = find(from);
    auto pos_end = find(to);
    assert(pos_end > pos_begin && ("INVALID ELEMENT KEYS"));
    return const_element_range< value_type>(&_data, pos_begin, pos_end - pos_begin, overlap);
}




//------------------------------------------------------------------------------
/// @brief      Public std::indexer specialization to std::deque with numerical floating point keys, and generalizing over given type T through std::tagged<> indexable wrapper.
///
/// @note       Practical/intuitive specialization, exploiting std::indexer interface to provide a pseudo-container with associative properties.
///
/// @tparam     T       Data type (*not* the container data type!)
/// @tparam     Locked  Whether container is resizeable after initialization. Defaults to false (expandable container).
/// @tparam     Id      Type of key/identifier. Defaults to float.
///
template < typename T, typename Key = float, bool Locked = false >
using series = sorted_indexer< std::deque< std::tagged< T, Key > >, T, Locked >;

}  // namespace std

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_STD_SORTEDINDEXER_HPP_
