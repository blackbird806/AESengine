#ifndef AES_DEBUG_MATH_HPP
#define AES_DEBUG_MATH_HPP

#include <fmt/format.h>
#include <glm/glm.hpp>

// TODO proper maths formatter

template<int D, typename T>
struct fmt::formatter<glm::vec<D, T>>
{
	static_assert(D < 5);
	static_assert(D > 1);

	using vecT = glm::vec<D, T>;

	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		fmt::formatter<T> f;

		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(vecT const& vec, FormatContext& ctx)
	{
		if constexpr (D == 2)
		{
			return fmt::format_to(ctx.out(), "x:{} y:{}", vec.x, vec.y);
		}
		else if constexpr (D == 3)
		{
			return fmt::format_to(ctx.out(), "x:{} y:{} z:{}", vec.x, vec.y, vec.z);
		}
		else if constexpr (D == 4)
		{
			return fmt::format_to(ctx.out(), "x:{} y:{} z:{} w:{}", vec.x, vec.y, vec.z, vec.w);
		}
	}
};

#endif