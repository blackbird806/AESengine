#include "tests.hpp"
#include "core/aes.hpp"
#include "core/string.hpp"

using namespace aes;

void aes::test_string()
{
	{
		String str;
		AES_ASSERT(str.size() == 0);
		AES_ASSERT(str.capacity() == 0);
		str.append("a");
		AES_ASSERT(str.size() == 1);
		AES_ASSERT(str == "a");
	}
	{
		String str("hello");
		AES_ASSERT(str.size() == 5);
		AES_ASSERT(str == "hello");
		AES_ASSERT(str != "hollo");
		AES_ASSERT(str != "Hello");
		str.append("world");
		AES_ASSERT(str == String("helloworld"));
		AES_ASSERT(str != String("test"));

		String strB(str);
		AES_ASSERT(str == strB);

		String strC(std::move(str));
		AES_ASSERT(strC == strB);
	}
	{
		String str("123");
		String strB;
		strB = std::move(str);
		AES_ASSERT(strB == "123");
		strB.clear();
		AES_ASSERT(strB.size() == 0);
		AES_ASSERT(strB.empty());
		AES_ASSERT(strB.capacity() > 0);
		strB.shrink();
	}
}