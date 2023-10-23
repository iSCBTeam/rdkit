#include <RDGeneral/export.h>
#ifndef _RD_ZSTRING_VIEW_H
#define _RD_ZSTRING_VIEW_H

#include <string>
#include <string_view>

namespace RDKit {

template<
	class CharT,
	class Traits = std::char_traits<CharT>
> class basic_zstring_view: public std::basic_string_view<CharT, Traits>
{
	public:

	using sv = std::basic_string_view<CharT, Traits>;

	constexpr basic_zstring_view() noexcept:
		sv() {}

	explicit constexpr basic_zstring_view(const basic_zstring_view &other) noexcept = default;

	explicit constexpr basic_zstring_view(const CharT *s):
		sv(s) {}

	constexpr basic_zstring_view(std::nullptr_t) = delete;

	constexpr const CharT *c_str() const noexcept
	{
		return this->data();
	}

	constexpr void remove_suffix(typename sv::size_type n) = delete;

	constexpr basic_zstring_view substr(typename sv::size_type pos = 0) const
	{
		return basic_zstring_view(*this);
	}

	constexpr sv substr(typename sv::size_type pos = 0, typename sv::size_type count = sv::npos) const
	{
		const sv &view = *this;
		return view.substr(pos, count);
	}
};

using zstring_view = basic_zstring_view<char>;
using wzstring_view = basic_zstring_view<wchar_t>;
//using u8zstring_view = basic_zstring_view<char8_t>;
using u16zstring_view = basic_zstring_view<char16_t>;
using u32zstring_view = basic_zstring_view<char32_t>;

}

#endif /* _RD_ZSTRING_VIEW_H */
