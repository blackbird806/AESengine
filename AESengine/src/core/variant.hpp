#ifndef AES_VARIANT_HPP
#define AES_VARIANT_HPP

#include <type_traits>

namespace aes
{
	// see https://stackoverflow.com/questions/26442859/getting-the-biggest-type-from-a-variadic-type-list
	template<typename A, typename B>
	using LargerType = typename std::conditional<sizeof(A) >= sizeof(B), A, B>::type;

	template<typename A, typename B, typename ...Other>
	struct LargerTypeHelper
	{
		using Result = typename LargerTypeHelper<A, B, Other...>::Result;
	};

	template<typename A, typename B>
	struct LargerTypeHelper<A, B>
	{
		using Result = typename LargerType<A, B>::Result;
	};

	template<typename ...Args>
	using LargestType = typename LargerTypeHelper<Args...>::Result;

	template<typename ...Args>
	struct Variant_TODO
	{

	};
	
	template<typename T>
	struct Payload_W
	{
		T value;
	};

	template<typename ...Args>
	struct Variant_Workaroud : Payload_W<Args>...
	{
		template<typename T>
		T& get()
		{
			return Payload_W<T>::value;
		}
		
	};

	template<typename ...Args>
	using Variant = Variant_Workaroud<Args...>;
}

#endif