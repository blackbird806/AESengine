#ifndef AES_OPTIONAL_HPP
#define AES_OPTIONAL_HPP

namespace aes
{
	// see : http://www.club.cc.cmu.edu/~ajo/disseminate/2017-02-15-Optional-From-Scratch.pdf
	template<typename T>
	class Optional
	{
	public:

	private:
		union
		{
			char dummy;
			T value;
		};
		bool hasValue;
	};
}

#endif