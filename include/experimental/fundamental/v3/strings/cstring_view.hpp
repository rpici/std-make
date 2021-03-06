// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// (C) Copyright 2019 Vicente J. Botet Escriba

#ifndef JASEL_EXPERIMENTAL_STRINGS_CSTRING_VIEW_HPP
#define JASEL_EXPERIMENTAL_STRINGS_CSTRING_VIEW_HPP

#include <experimental/contract.hpp>
#include <experimental/fundamental/v2/config.hpp>
#include <experimental/fundamental/v3/config/requires.hpp>
#include <experimental/fundamental/v3/strings/null_terminated.hpp>
#include <experimental/string_view.hpp>
#include <cstring>

namespace std
{
namespace experimental
{
inline namespace fundamental_v3
{

template <class CharT, class Traits = char_traits<CharT>>
class basic_cstring_view;

template <class CharT, class Traits>
class basic_cstring_view {
	constexpr bool null_terminated()
	{
		return ntxs::valid(sv_.data(), sv_.length());
	}

public:
	// types
	using traits_type            = Traits;
	using value_type             = CharT;
	using pointer                = value_type *;
	using const_pointer          = const value_type *;
	using reference              = value_type &;
	using const_reference        = const value_type &;
	using const_iterator         = value_type const *; // see 20.4.2.2
	using iterator               = const_iterator;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator       = std::reverse_iterator<const_iterator>;
	using size_type              = size_t;
	using difference_type        = ptrdiff_t;
	using string_view_type       = basic_string_view<CharT, Traits>;
	using string_type            = basic_string<CharT, Traits>;

	static constexpr size_type npos = size_type(-1);

	// [string.view.cstring], construction and assignment
	constexpr basic_cstring_view() noexcept : sv_(&null_terminated_traits<CharT>::zero, 0) {}
	constexpr basic_cstring_view(const basic_cstring_view &) noexcept = default;
	JASEL_CXX14_CONSTEXPR basic_cstring_view &operator=(const basic_cstring_view &) noexcept = default;

	// explicit conversion from a c-string
	constexpr basic_cstring_view(null_terminated_t, const CharT *str)
	        : sv_(str, Traits::length(str))
	{
		// There is no way to check the following, as we don't have a limit for the string
		// JASEL_EXPECTS(null_terminated());
	}

	// explicit conversion from a a c-string and his length
	JASEL_CXX14_CONSTEXPR basic_cstring_view(null_terminated_t, const CharT *str, size_type len)
	        : sv_(str, len)
	{
		JASEL_EXPECTS(null_terminated());
	}
	// explicit conversion from a string view which must be null terminated
	constexpr basic_cstring_view(null_terminated_t, const string_view_type &sv) noexcept
	        : basic_cstring_view(sv.data(), sv.length())
	{
	}

	// implicit conversion from a string
	constexpr basic_cstring_view(const string_type &str) noexcept
	        : sv_(str.c_str(), str.length())
	{
	}

	// [string.view.cstring], iterator support
	constexpr const_iterator         begin() const noexcept { return sv_.begin(); }
	constexpr const_iterator         end() const noexcept { return sv_.end(); }
	constexpr const_iterator         cbegin() const noexcept { return sv_.cbegin(); }
	constexpr const_iterator         cend() const noexcept { return sv_.cend(); }
	constexpr const_reverse_iterator rbegin() const noexcept { return sv_.rbegin(); }
	constexpr const_reverse_iterator rend() const noexcept { return sv_.rend(); }
	constexpr const_reverse_iterator crbegin() const noexcept { return sv_.crbegin(); }
	constexpr const_reverse_iterator crend() const noexcept { return sv_.crend(); }
	friend constexpr const_iterator  begin(basic_cstring_view sv) noexcept
	{
		return sv.begin();
	}
	friend constexpr const_iterator end(basic_cstring_view sv) noexcept
	{
		return sv.end();
	}
	// [string.view.cstring], capacity
	constexpr size_type          size() const noexcept { return sv_.size(); }
	constexpr size_type          length() const noexcept { return sv_.length(); }
	constexpr size_type          max_size() const noexcept { return sv_.max_size(); }
	[[nodiscard]] constexpr bool empty() const noexcept { return sv_.empty(); }
	// [string.view.cstring], element access
	constexpr const_reference operator[](size_type pos) const { return sv_[pos]; }
	constexpr const_reference at(size_type pos) const { return sv_.at(pos); }
	constexpr const_reference front() const { return sv_.front(); }
	constexpr const_reference back() const { return sv_.back(); }
	constexpr const_pointer   data() const noexcept { return sv_.data(); }
	constexpr const CharT *   c_str() const noexcept { return data(); }
	operator basic_string_view<CharT, Traits>() const noexcept { return sv_; }
	// [string.view.cstring], modifiers
	JASEL_CXX14_CONSTEXPR void remove_prefix(size_type n) { sv_.remove_prefix(n); }
	//constexpr void remove_suffix(size_type n) { return sv_.data(); }
	JASEL_CXX14_CONSTEXPR void swap(basic_cstring_view &s) noexcept { sv_.swap(s.sv_); }
	// [string.view.cstring], string operations
	constexpr size_type copy(CharT *s, size_type n, size_type pos = 0) const
	{
		return sv_.copy(s, n, pos);
	}
	constexpr basic_cstring_view substr(size_type pos) const
	{
		return basic_cstring_view(sv_.substr(pos));
	}

	constexpr string_view_type substr(size_type pos, size_type n) const
	{
		return sv_.substr(pos, n);
	}
	constexpr int compare(string_view_type s) const noexcept
	{
		return sv_.compare(s);
	}
	constexpr int compare(size_type pos1, size_type n1, string_view_type s) const
	{
		return sv_.compare(pos1, n1, s);
	}
	constexpr int compare(size_type pos1, size_type n1, string_view_type s,
	                      size_type pos2, size_type n2) const
	{
		return sv_.compare(pos1, n1, s, pos2, n2);
	}

	constexpr int compare(const CharT *s) const
	{
		return sv_.compare(s);
	}
	constexpr int compare(size_type pos1, size_type n1, const CharT *s) const
	{
		return sv_.compare(pos1, n1, s);
	}

	constexpr int compare(size_type pos1, size_type n1, const CharT *s, size_type n2) const
	{
		return sv_.compare(pos1, n1, s, n2);
	}
	constexpr bool starts_with(string_view_type x) const noexcept
	{
		return sv_.starts_with(x);
	}
	constexpr bool starts_with(CharT x) const noexcept
	{
		return sv_.starts_with(x);
	}
	constexpr bool starts_with(const CharT *x) const
	{
		return sv_.starts_with(x);
	}
	constexpr bool ends_with(string_view_type x) const noexcept
	{
		return sv_.ends_with(x);
	}
	constexpr bool ends_with(CharT x) const noexcept
	{
		return sv_.ends_with(x);
	}
	constexpr bool ends_with(const CharT *x) const
	{
		return sv_.ends_with(x);
	}
	// [string.view.cstring], searching
	constexpr size_type find(string_view_type s, size_type pos = 0) const noexcept
	{
		return sv_.find(s, pos);
	}
	constexpr size_type find(CharT c, size_type pos = 0) const noexcept
	{
		return sv_.find(c, pos);
	}
	constexpr size_type find(const CharT *s, size_type pos, size_type n) const
	{
		return sv_.find(s, pos, n);
	}
	constexpr size_type find(const CharT *s, size_type pos = 0) const
	{
		return sv_.find(s, pos);
	}
	constexpr size_type rfind(string_view_type s, size_type pos = npos) const noexcept
	{
		return sv_.rfind(s, pos);
	}
	constexpr size_type rfind(CharT c, size_type pos = npos) const noexcept
	{
		return sv_.rfind(c, pos);
	}
	constexpr size_type rfind(const CharT *s, size_type pos, size_type n) const
	{
		return sv_.rfind(s, pos, n);
	}
	constexpr size_type rfind(const CharT *s, size_type pos = npos) const
	{
		return sv_.rfind(s, pos);
	}
	constexpr size_type find_first_of(string_view_type s, size_type pos = 0) const noexcept
	{
		return sv_.find_first_of(s, pos);
	}
	constexpr size_type find_first_of(CharT c, size_type pos = 0) const noexcept
	{
		return sv_.find_first_of(c, pos);
	}
	constexpr size_type find_first_of(const CharT *s, size_type pos, size_type n) const
	{
		return sv_.find_first_of(s, pos, n);
	}
	constexpr size_type find_first_of(const CharT *s, size_type pos = 0) const
	{
		return sv_.find_first_of(s, pos);
	}
	constexpr size_type find_last_of(string_view_type s, size_type pos = npos) const noexcept
	{
		return sv_.find_last_of(s, pos);
	}
	constexpr size_type find_last_of(CharT c, size_type pos = npos) const noexcept
	{
		return sv_.find_last_of(c, pos);
	}
	constexpr size_type find_last_of(const CharT *s, size_type pos, size_type n) const
	{
		return sv_.find_last_of(s, pos, n);
	}
	constexpr size_type find_last_of(const CharT *s, size_type pos = npos) const
	{
		return sv_.find_last_of(s, pos);
	}
	constexpr size_type find_first_not_of(string_view_type s, size_type pos = 0) const noexcept
	{
		return sv_.find_first_not_of(s, pos);
	}
	constexpr size_type find_first_not_of(CharT c, size_type pos = 0) const noexcept
	{
		return sv_.find_first_not_of(c, pos);
	}
	constexpr size_type find_first_not_of(const CharT *s, size_type pos,
	                                      size_type n) const
	{
		return sv_.find_first_not_of(s, pos, n);
	}
	constexpr size_type find_first_not_of(const CharT *s, size_type pos = 0) const
	{
		return sv_.find_first_not_of(s, pos);
	}
	constexpr size_type find_last_not_of(string_view_type s,
	                                     size_type        pos = npos) const noexcept
	{
		return sv_.find_last_not_of(s, pos);
	}
	constexpr size_type find_last_not_of(CharT c, size_type pos = npos) const noexcept
	{
		return sv_.find_last_not_of(c, pos);
	}
	constexpr size_type find_last_not_of(const CharT *s, size_type pos,
	                                     size_type n) const
	{
		return sv_.find_last_not_of(s, pos, n);
	}
	constexpr size_type find_last_not_of(const CharT *s, size_type pos = npos) const
	{
		return sv_.find_last_not_of(s, pos);
	}

private:
	string_view_type sv_;
};

using cstring_view = basic_cstring_view<char>;
//using u8cstring_view = basic_cstring_view<char8_t>;
using u16cstring_view = basic_cstring_view<char16_t>;
using u32cstring_view = basic_cstring_view<char32_t>;
using wcstring_view   = basic_cstring_view<wchar_t>;

// hash
// todo: add hash specialization

// Stream Inserter
template <class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &      os,
           const basic_cstring_view<CharT, Traits> &str)
{
	os << str.c_str();
	return os;
}

} // namespace fundamental_v3
} // namespace experimental
} // namespace std
#endif // header
