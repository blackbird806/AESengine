#ifndef AES_SCOPED_HPP
#define AES_SCOPED_HPP

namespace aes
{
	template<typename T>
	concept Destructible = requires(T a)
	{
		{ a.destroy() };
	};

	template<Destructible T>
	struct Scoped : public T
	{
		Scoped& operator=(T&& rhs) noexcept
		{
			T::operator=(std::forward<T>(rhs));
			return *this;
		}

		~Scoped()
		{
			T::destroy();
		}
	};
}

#endif