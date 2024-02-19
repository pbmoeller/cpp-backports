#ifndef CPPBP_STRING_VIEW_HPP
#define CPPBP_STRING_VIEW_HPP

#include <cstddef>      // std::size_t, std::ptrdiff_t
#include <iterator>     // std::reverse_iterator
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

    // Constructors         [string.view.cons]
public:
    constexpr basic_string_view() noexcept
        : m_str{nullptr}
        , m_size{0u}
    { }

    constexpr basic_string_view(const basic_string_view &other) noexcept = default;

    constexpr basic_string_view(const_pointer str, size_type count)
        : m_str{str}
        , m_size{count}
    { }

    constexpr basic_string_view(const_pointer str)
        : m_str{str}
        , m_size{traits_type::length(str)}
    { }
    
    constexpr basic_string_view(std::nullptr_t) = delete;

    // Assignment
public:
    constexpr basic_string_view& operator=(const basic_string_view &view) noexcept = default;

    // Iterators            [string.view.iterators]
public:
    constexpr const_iterator begin() const noexcept
    {
        return m_str;
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return m_str;
    }

    constexpr const_iterator end() const noexcept
    {
        return m_str + m_size;
    }

    constexpr const_iterator cend() const noexcept
    {
        return m_str + m_size;
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator{end()};
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator{end()};
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator{begin()};
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator{begin()};
    }

    // Element Access       [string.view.access]
public:
    constexpr const_reference operator[](size_type pos) const noexcept
    {
        // TODO: assert if out of range?
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
        // TODO: assert if empty?
        return *m_str;
    }

    constexpr const_reference back() const noexcept
    {
        // TODO: assert if empty?
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

    // Capacity             [string.view.capacity]
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

    // Modifiers            [string.view.modifiers]
public:
    constexpr void remove_prefix(size_type n)
    {
        m_str += n;
        m_size -= n;
    }

    constexpr void remove_suffix(size_type n)
    {
        m_size -= n;
    }

    constexpr void swap(basic_string_view& v) noexcept
    {
        // std::swap is not constexpr before C++20
        const basic_string_view tmp{*this};
        *this = v;
        v = tmp;
    }

    // String operations    [string.view.ops]
public:
    constexpr size_type copy(pointer dst, size_type count, size_type pos = 0) const
    {
        if(pos >= m_size) {
            throw std::out_of_range("basic_string_view::copy: position out of range");
        }

        const size_type rcount = std::min(m_size - pos, count);

        traits_type::copy(dst, m_str + pos, rcount);

        return rcount;
    }

    constexpr basic_string_view substr(size_type pos = 0, size_type count = npos) const
    {
        if(pos >= m_size) {
            throw std::out_of_range("basic_string_view::copy: position out of range");
        }
        const size_type rcount = std::min(m_size - pos, count);
        return basic_string_view{m_str + pos, rcount};
    }

    constexpr int compare(basic_string_view v) const noexcept
    {
        const size_type rcount = std::min(m_size, v.m_size);
        // Check: compare is not constexpr in C++11 !!!
        const int ret = traits_type::compare(m_str, v.m_str, rlen);
        if(ret != 0) {
            return ret;
        }
        if(m_size < v.m_size) {
            return -1;
        }
        if(m_size > v.m_size) {
            return 1;
        }
        return 0;
    }

    constexpr int compare(size_type pos1, size_type count1, basic_string_view v) const
    {
        return substr(pos1, count1).compare(v);
    }

    constexpr int compare(size_type pos1, size_type count1,
                          basic_string_view v, size_type pos2, size_type count2) const
    {
        return substr(pos1, count1).compare(v.substr(pos2, count2));
    }

    constexpr int compare(const_pointer s) const
    {
        return compare(basic_string_view(s));
    }

    constexpr int compare(size_type pos1, size_type count1, const_pointer s) const
    {
        return substr(pos1, count1).compare(basic_string_view(s);
    }

    constexpr int compare(size_type pos1, size_type count1, const_pointer s, size_type count2) const
    {
        return substr(pos1, count1).compare(basic_string_view(s, count2));
    }

    constexpr bool starts_with(basic_string_view sv) const noexcept;
    constexpr bool starts_with(value_type ch) const noexcept;
    constexpr bool starts_with(const_pointer s) const;

    constexpr bool ends_with(basic_string_view sv) const noexcept;
    constexpr bool ends_with(value_type ch) const noexcept;
    constexpr bool ends_with(const_pointer s) const;

    constexpr bool contains(basic_string_view sv) const noexcept;
    constexpr bool contains(value_type c) const noexcept;
    constexpr bool contains(const_pointer s) const;

    constexpr size_type find(basic_string_view v, size_type pos = 0) const noexcept;
    constexpr size_type find(value_type ch, size_type pos = 0) const noexcept;
    constexpr size_type find(const_pointer s, size_type pos, size_type count) const;
    constexpr size_type find(const_pointer s, size_type pos = 0) const;

    constexpr size_type rfind(basic_string_view v, size_type pos = npos) const noexcept;
    constexpr size_type rfind(value_type ch, size_type pos = npos) const noexcept;
    constexpr size_type rfind(const_pointer s, size_type pos, size_type count) const;
    constexpr size_type rfind(const_pointer s, size_type pos = npos) const;

    constexpr size_type find_first_of(basic_string_view v, size_type pos = 0) const noexcept;
    constexpr size_type find_first_of(value_type ch, size_type pos = 0) const noexcept;
    constexpr size_type find_first_of(const_pointer s, size_type pos, size_type count) const;
    constexpr size_type find_first_of(const_pointer s, size_type pos = 0) const;

    constexpr size_type find_last_of(basic_string_view v, size_type pos = npos) const noexcept;
    constexpr size_type find_last_of(value_type ch, size_type pos = npos) const noexcept;
    constexpr size_type find_last_of(const_pointer s, size_type pos, size_type count) const;
    constexpr size_type find_last_of(const_pointer s, size_type pos = npos) const;

    constexpr size_type find_first_not_of(basic_string_view v, size_type pos = 0) const noexcept;
    constexpr size_type find_first_not_of(value_type ch, size_type pos = 0) const noexcept;
    constexpr size_type find_first_not_of(const_pointer s, size_type pos, size_type count) const;
    constexpr size_type find_first_not_of(const_pointer s, size_type pos = 0) const;

    constexpr size_type find_last_not_of(basic_string_view v, size_type pos = npos) const noexcept;
    constexpr size_type find_last_not_of(value_type ch, size_type pos = npos) const noexcept;
    constexpr size_type find_last_not_of(const_pointer s, size_type pos, size_type count) const;
    constexpr size_type find_last_not_of(const_pointer s, size_type pos = npos) const;

    // Private Member
private:
    const_pointer   m_str;
    size_type       m_size;
};

// Static member initialization
template<typename CharT, typename Traits>
const typename basic_string_view<CharT, Traits>::size_type basic_string_view<CharT, Traits>::npos;

// Type aliases
using string_view       = basic_string_view<char>;
using u8string_view     = basic_string_view<char8_t>;
using u16string_view    = basic_string_view<char16_t>;
using u32string_view    = basic_string_view<char32_t>;
using wstring_view      = basic_string_view<wchar_t>;

} // namespace cppbp

#endif // CPPBP_STRING_VIEW_HPP
