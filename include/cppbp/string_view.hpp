#ifndef CPPBP_STRING_VIEW_HPP
#define CPPBP_STRING_VIEW_HPP

#include <cppbp/type_traits.hpp>    // cppbp::type_identity_t

#include <memory>       // std::addressof
#include <cassert>      // assert
#include <cstddef>      // std::size_t, std::ptrdiff_t
#include <iterator>     // std::reverse_iterator
#include <iostream>     // std::basic_ostream
#include <stdexcept>    // std::out_of_range,
#include <string>       // std::char_traits
#include <utility>      // std::swap

namespace cppbp {

template<typename CharT, typename Traits = std::char_traits<CharT>>
class basic_string_view final
{
    // Types
public:
    using traits_type               = Traits;
    using value_type                = CharT;

    using pointer                   = value_type*;
    using const_pointer             = const value_type*;
    using reference                 = value_type&;
    using const_reference           = const value_type&;

    using const_iterator            = const value_type*;
    using iterator                  = const_iterator;
    using const_reverse_iterator    = std::reverse_iterator<const_iterator>;
    using reverse_iterator          = const_reverse_iterator;

    using size_type                 = std::size_t;
    using difference_type           = std::ptrdiff_t;

    static constexpr size_type npos{static_cast<size_type>(-1)};

    // Construction and Assignment                                                [string.view.cons]
public:
    constexpr basic_string_view() noexcept
        : m_str{nullptr}
        , m_size{0u}
    { }

    constexpr basic_string_view(const basic_string_view &other) noexcept = default;
    constexpr basic_string_view& operator=(const basic_string_view &view) noexcept = default;

    constexpr basic_string_view(const_pointer str)
        : m_str{str}
        , m_size{traits_type::length(str)}
    { }

    constexpr basic_string_view(const_pointer str, size_type count)
        : m_str{str}
        , m_size{count}
    { }
    
    constexpr basic_string_view(std::nullptr_t) = delete;

    // Iterators                                                             [string.view.iterators]
public:
    constexpr const_iterator begin() const noexcept
    {
        return m_str;
    }

    constexpr const_iterator end() const noexcept
    {
        return m_str + m_size;
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return m_str;
    }

    constexpr const_iterator cend() const noexcept
    {
        return m_str + m_size;
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator{end()};
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator{begin()};
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator{end()};
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator{begin()};
    }

    // Capacity                                                               [string.view.capacity]
public:
    constexpr size_type size() const noexcept
    {
        return m_size;
    }

    constexpr size_type length() const noexcept
    {
        return m_size;
    }

    constexpr size_type max_size() const noexcept
    {
        // Implementation based on GCC's version.
        return (npos - sizeof(size_type) - sizeof(void*)) / sizeof(value_type) / 4;
    }

    constexpr bool empty() const noexcept
    {
        return m_size == 0;
    }

    // Element Access                                                           [string.view.access]
public:
    constexpr const_reference operator[](size_type pos) const noexcept
    {
        assert(pos < m_size);
        return m_str[pos];
    }

    constexpr const_reference at(size_type pos) const
    {
        if(pos >= m_size) {
            throw std::out_of_range("basic_string_view::at: position out of range");
        }
        return m_str[pos];
    }

    constexpr const_reference front() const noexcept
    {
        assert(m_size > 0);
        return *m_str;
    }

    constexpr const_reference back() const noexcept
    {
        assert(m_size > 0);
        return *m_str[m_size - 1];
    }

    constexpr const_pointer c_str() const noexcept
    {
        return m_str;
    }

    constexpr const_pointer data() const noexcept
    {
        return m_str;
    }

    // Modifiers                                                             [string.view.modifiers]
public:
    constexpr void remove_prefix(size_type n)
    {
        assert(n <= m_size);
        m_str += n;
        m_size -= n;
    }

    constexpr void remove_suffix(size_type n)
    {
        assert(n <= m_size);
        m_size -= n;
    }

    constexpr void swap(basic_string_view& v) noexcept
    {
        // std::swap is not constexpr before C++20
        const basic_string_view tmp{*this};
        *this = v;
        v = tmp;
    }

    // String operations                                                           [string.view.ops]
public:
    constexpr size_type copy(pointer dst, size_type n, size_type pos = 0) const
    {
        if(pos > m_size) {
            throw std::out_of_range("basic_string_view::copy: position out of range");
        }

        const size_type rlen = std::min(m_size - pos, n);

        traits_type::copy(dst, m_str + pos, rlen);

        return rlen;
    }

    constexpr basic_string_view substr(size_type pos = 0, size_type n = npos) const
    {
        if(pos > m_size) {
            throw std::out_of_range("basic_string_view::copy: position out of range");
        }
        const size_type rlen = std::min(m_size - pos, n);
        return basic_string_view{m_str + pos, rlen};
    }

    constexpr int compare(basic_string_view str) const noexcept
    {
        const size_type rlen = std::min(m_size, str.m_size);
        // Check: compare is not constexpr in C++11 !!!
        const int ret = traits_type::compare(m_str, str.m_str, rlen);
        if(ret != 0) {
            return ret;
        }
        if(m_size < str.m_size) {
            return -1;
        }
        if(m_size > str.m_size) {
            return 1;
        }
        return 0;
    }

    constexpr int compare(size_type pos1, size_type n1, basic_string_view str) const
    {
        return substr(pos1, n1).compare(str);
    }

    constexpr int compare(size_type pos1, size_type n1,
                          basic_string_view str, size_type pos2, size_type n2) const
    {
        return substr(pos1, n1).compare(str.substr(pos2, n2));
    }

    constexpr int compare(const_pointer s) const
    {
        return compare(basic_string_view(s));
    }

    constexpr int compare(size_type pos1, size_type n1, const_pointer s) const
    {
        return substr(pos1, n1).compare(basic_string_view(s));
    }

    constexpr int compare(size_type pos1, size_type n1, const_pointer s, size_type n2) const
    {
        return substr(pos1, n1).compare(basic_string_view(s, n2));
    }

    constexpr bool starts_with(basic_string_view x) const noexcept
    {
        return substr(0, x.size()) == x;
    }

    constexpr bool starts_with(value_type x) const noexcept
    {
        return !empty() && traits_type::eq(front(), x);
    }

    constexpr bool starts_with(const_pointer x) const
    {
        return starts_with(basic_string_view(x));
    }

    constexpr bool ends_with(basic_string_view x) const noexcept
    {
        return (m_size >= x.m_size) && (compare(m_size - x.m_size, npos, x) == 0);
    }

    constexpr bool ends_with(value_type x) const noexcept
    {
        return !empty() && traits_type::eq(back(), x);
    }

    constexpr bool ends_with(const_pointer x) const
    {
        return ends_with(basic_string_view(x));
    }

    constexpr bool contains(basic_string_view x) const noexcept
    {
        return find(x) != npos;
    }

    constexpr bool contains(value_type x) const noexcept
    {
        return find(x) != npos;
    }

    constexpr bool contains(const_pointer x) const
    {
        return find(x) != npos;
    }

    // Searching                                                                  [string.view.find]
public:
    constexpr size_type find(basic_string_view str, size_type pos = 0) const noexcept
    {
        if(pos > m_size) {
            return npos;
        }
        if(pos + str.size() > m_size) {
            return npos;
        }

        const auto offset = pos;
        const auto increments = m_size - str.size() - offset;

        for(auto i = 0u; i <= increments; ++i) {
            const auto j = i + offset;
            if(substr(j, str.m_size) == v) {
                return j;
            }
        }

        return npos;
    }

    constexpr size_type find(value_type ch, size_type pos = 0) const noexcept
    {
        return find(basic_string_view(std::addressof(ch), 1), pos);
    }

    constexpr size_type find(const_pointer s, size_type pos, size_type n) const
    {
        return find(basic_string_view(s, n), pos);
    }

    constexpr size_type find(const_pointer s, size_type pos = 0) const
    {
        return find(basic_string_view(s), pos);
    }

    constexpr size_type rfind(basic_string_view str, size_type pos = npos) const noexcept
    {
        if(empty()) {
            return str.empty() ? 0u : npos;
        }
        if(str.empty()) {
            return std::min(m_size - 1, pos);
        }
        if(str.m_size() > m_size) {
            return npos;
        }

        auto i = std::min(pos, (m_size - str.m_size));
        while(i != npos) {
            if(substr(i, str.m_size) == v) {
                return i;
            }
            --i;
        }

        return npos;
    }

    constexpr size_type rfind(value_type ch, size_type pos = npos) const noexcept
    {
        return rfind(basic_string_view(std::addressof(ch), 1), pos);
    }

    constexpr size_type rfind(const_pointer s, size_type pos, size_type n) const
    {
        return rfind(basic_string_view(s, n), pos);
    }

    constexpr size_type rfind(const_pointer s, size_type pos = npos) const
    {
        return rfind(basic_string_view(s), pos);
    }

    constexpr size_type find_first_of(basic_string_view str, size_type pos = 0) const noexcept
    {
        for(auto i = pos; i < m_size; ++i) {
            if(is_one_of(m_str[i], str)) {
                return i;
            }
        }
        return npos;
    }

    constexpr size_type find_first_of(value_type ch, size_type pos = 0) const noexcept
    {
        return find_first_of(basic_string_view(std::addressof(ch), 1), pos);
    }

    constexpr size_type find_first_of(const_pointer s, size_type pos, size_type n) const
    {
        return find_first_of(basic_string_view(s, n), pos);
    }

    constexpr size_type find_first_of(const_pointer s, size_type pos = 0) const
    {
        return find_first_of(basic_string_view(s), pos);
    }

    constexpr size_type find_last_of(basic_string_view str, size_type pos = npos) const noexcept
    {
        if(empty()) {
            return npos;
        }

        const auto last_index = std::min(m_size - 1, pos);
        for(auto i = 0u; i <= last_index; ++i) {
            const auto j = last_index - 1;
            if(is_one_of(m_str[j], str)) {
                return j;
            }
        }
        return npos;
    }

    constexpr size_type find_last_of(value_type ch, size_type pos = npos) const noexcept
    {
        return find_last_of(basic_string_view(std::addressof(ch), 1), pos);
    }

    constexpr size_type find_last_of(const_pointer s, size_type pos, size_type n) const
    {
        return find_last_of(basic_string_view(s, n), pos);
    }

    constexpr size_type find_last_of(const_pointer s, size_type pos = npos) const
    {
        return find_last_of(basic_string_view(s), pos);
    }

    constexpr size_type find_first_not_of(basic_string_view str, size_type pos = 0) const noexcept
    {
        for(auto i = pos; i < m_size; ++i) {
            if(!is_one_of(m_str[i], str)) {
                return i;
            }
        }
        return npos;
    }

    constexpr size_type find_first_not_of(value_type ch, size_type pos = 0) const noexcept
    {
        return find_first_not_of(basic_string_view(std::addressof(ch), 1), pos);
    }

    constexpr size_type find_first_not_of(const_pointer s, size_type pos, size_type n) const
    {
        return find_first_not_of(basic_string_view(s, n), pos);
    }

    constexpr size_type find_first_not_of(const_pointer s, size_type pos = 0) const
    {
        return find_first_not_of(basic_string_view(s), pos);
    }

    constexpr size_type find_last_not_of(basic_string_view str, size_type pos = npos) const noexcept
    {
        if(empty()) {
            return npos;
        }

        const auto last_index = std::min(m_size - 1, pos);
        for(auto i = 0u; i <= last_index; ++i) {
            const auto j = last_index - 1;
            if(!is_one_of(m_str[j], str)) {
                return j;
            }
        }
        return npos;
    }

    constexpr size_type find_last_not_of(value_type ch, size_type pos = npos) const noexcept
    {
        return find_last_not_of(basic_string_view(std::addressof(ch), 1), pos);
    }

    constexpr size_type find_last_not_of(const_pointer s, size_type pos, size_type n) const
    {
        return find_last_not_of(basic_string_view(s, n), pos);
    }

    constexpr size_type find_last_not_of(const_pointer s, size_type pos = npos) const
    {
        return find_last_not_of(basic_string_view(s), pos);
    }

    // Helper
private:
    static bool is_one_of(value_type c, basic_string_view str)
    {
        for(auto s : str) {
            if(c == s) {
                return true;
            }
        }
        return false;
    }

    // Private Member
private:
    const_pointer   m_str;
    size_type       m_size;
};

// Static member initialization

template<typename CharT, typename Traits>
const typename basic_string_view<CharT, Traits>::size_type basic_string_view<CharT, Traits>::npos;

// Comparison functions                                                     [string.view.comparison]

// operator==

template<typename CharT, typename Traits>
constexpr bool operator==(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept
{
    return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
}

template<typename CharT, typename Traits>
constexpr bool operator==(cppbp::type_identity_t<basic_string_view<CharT, Traits>> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept
{
    return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
}

template<typename CharT, typename Traits>
constexpr bool operator==(basic_string_view<CharT, Traits> lhs,
                          cppbp::type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept
{
    return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
}

// operator!=

template<typename CharT, typename Traits>
constexpr bool operator!=(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept
{
    return !(lhs == rhs);
}

template<typename CharT, typename Traits>
constexpr bool operator!=(cppbp::type_identity_t<basic_string_view<CharT, Traits>> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept
{
    return !(lhs == rhs);
}

template<typename CharT, typename Traits>
constexpr bool operator!=(basic_string_view<CharT, Traits> lhs,
                          cppbp::type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept
{
    return !(lhs == rhs);
}

// operator<

template<typename CharT, typename Traits>
constexpr bool operator<(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept
{
    return x.compare(y) < 0;
}

template<typename CharT, typename Traits>
constexpr bool operator<(cppbp::type_identity_t<basic_string_view<CharT, Traits>> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept
{
    return x.compare(y) < 0;
}

template<typename CharT, typename Traits>
constexpr bool operator<(basic_string_view<CharT, Traits> lhs,
                          cppbp::type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept
{
    return x.compare(y) < 0;
}

// operator>

template<typename CharT, typename Traits>
constexpr bool operator>(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept
{
    return x.compare(y) > 0;
}

template<typename CharT, typename Traits>
constexpr bool operator>(cppbp::type_identity_t<basic_string_view<CharT, Traits>> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept
{
    return x.compare(y) > 0;
}

template<typename CharT, typename Traits>
constexpr bool operator>(basic_string_view<CharT, Traits> lhs,
                          cppbp::type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept
{
    return x.compare(y) > 0;
}

// operator<=

template<typename CharT, typename Traits>
constexpr bool operator<=(basic_string_view<CharT, Traits> lhs,
    basic_string_view<CharT, Traits> rhs) noexcept
{
    return x.compare(y) <= 0;
}

template<typename CharT, typename Traits>
constexpr bool operator<=(cppbp::type_identity_t<basic_string_view<CharT, Traits>> lhs,
    basic_string_view<CharT, Traits> rhs) noexcept
{
    return x.compare(y) <= 0;
}

template<typename CharT, typename Traits>
constexpr bool operator<=(basic_string_view<CharT, Traits> lhs,
    cppbp::type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept
{
    return x.compare(y) <= 0;
}

// operator>=

template<typename CharT, typename Traits>
constexpr bool operator>=(basic_string_view<CharT, Traits> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept
{
    return x.compare(y) >= 0;
}

template<typename CharT, typename Traits>
constexpr bool operator>=(cppbp::type_identity_t<basic_string_view<CharT, Traits>> lhs,
                          basic_string_view<CharT, Traits> rhs) noexcept
{
    return x.compare(y) >= 0;
}

template<typename CharT, typename Traits>
constexpr bool operator>=(basic_string_view<CharT, Traits> lhs,
                          cppbp::type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept
{
    return x.compare(y) >= 0;
}

// Inserters and extractors                                                         [string.view.io]

template<typename charT, typename Traits>
std::basic_ostream<charT, Traits>&
operator<<(std::basic_ostream<charT, Traits> &os, basic_string_view<charT, Traits> str)
{
    return os.write(str.data(), str.size());
}

// Type aliases

using string_view       = basic_string_view<char>;
using u16string_view    = basic_string_view<char16_t>;
using u32string_view    = basic_string_view<char32_t>;
using wstring_view      = basic_string_view<wchar_t>;

// Hash support                                                                   [string.view.hash]

template<typename T>
struct hash;

template<>
struct hash<string_view>
{
public:
    std::size_t operator()(string_view str) const noexcept
    {
        return std::hash<std::string>()(std::string(str.data(), str.size()));
    }
};

template<>
struct hash<wstring_view>
{
public:
    std::size_t operator()(wstring_view str) const noexcept
    {
        return std::hash<std::wstring>()(std::wstring(str.data(), str.size()));
    }
};

template<>
struct hash<u16string_view>
{
public:
    std::size_t operator()(u16string_view str) const noexcept
    {
        return std::hash<std::u16string>()(std::u16string(str.data(), str.size()));
    }
};

template<>
struct hash<u32string_view>
{
public:
    std::size_t operator()(u32string_view str) const noexcept
    {
        return std::hash<std::u32string>()(std::u32string(str.data(), str.size()));
    }
};

// Suffix for basic_string_view literals                                      [string.view.literals]

inline namespace literals {
inline namespace string_view_literals {

inline constexpr basic_string_view<char>
operator ""sv(const char* str, std::size_t len) noexcept
{
    return basic_string_view<char>{str, len};
}

inline constexpr basic_string_view<char16_t>
operator ""sv(const char16_t* str, std::size_t len) noexcept
{
    return basic_string_view<char16_t>{str, len};
}

inline constexpr basic_string_view<char32_t>
operator ""sv(const char32_t* str, std::size_t len) noexcept
{
    return basic_string_view<char32_t>{str, len};
}

inline constexpr basic_string_view<wchar_t>
operator ""sv(const wchar_t* str, std::size_t len) noexcept
{
    return basic_string_view<wchar_t>{str, len};
}

} // inline namespace literals
} // inline namespace string_view_literals

} // namespace cppbp

#endif // CPPBP_STRING_VIEW_HPP
