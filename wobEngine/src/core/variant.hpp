#ifndef WOB_VARIANT_HPP
#define WOB_VARIANT_HPP

namespace wob
{
	template<typename T, typename... Ts>
	struct VariadicUnion {
		union {
			T t;
			VariadicUnion<Ts...> ts;
		};
	};

	template<typename T>
	struct VariadicUnion<T> {
		T t;
	};

	template<typename ...Args>
	struct Variant
	{
		VariadicUnion<Args...> values;
	};
}

#endif // !WOB_VARIANT_HPP
