#ifndef CPPBP_TYPE_TRAITS_HPP
#define CPPBP_TYPE_TRAITS_HPP

namespace cppbp {

template<typename T>
struct type_identity {
    using type = T;
};
template<typename T>
using type_identity_t = typename type_identity<T>::type;

} // namespace cppbp

#endif // CPPBP_TYPE_TRAITS_HPP
