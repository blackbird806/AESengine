#ifndef WOB_RANGES_HPP
#define WOB_RANGES_HPP

#include "utility.hpp"

namespace wob::ranges
{
	// TODO concepts

	template<typename Range, typename T>
	auto find(Range&& range, T const& value)
	{
		for (auto it = begin(range); it != end(range); ++it)
		{
			if (*it == value)
			{
				return it;
			}
		}
		return end(range);
	}

	template<typename Range, typename Pred>
	auto findIf(Range&& range, Pred&& pred)
	{
		for (auto it = begin(range); it != end(range); ++it)
		{
			if (pred(*it))
			{
				return it;
			}
		}
		return end(range);
	}
}

#endif