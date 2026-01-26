#include "core/aes.hpp"
#include "core/string.hpp"
#include "core/hashmap.hpp"

using namespace aes;

int main()
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
		AES_ASSERT(strB.capacity() == 0);
	}
	{
		AES_LOG("[TEST] RHI");
		HashMap<String, int> map(16);
		map.add(String("hello"), 2);
		map.add(String("world"), 15);
		map.add(String("pistav"), 12);
		map["hello"] = 16;

		AES_ASSERT(map.size() == 3);
		map.remove(String("hello"));
		map.rehash(2);
		map.rehash(32);
		AES_ASSERT(map.size() == 2);

		int out = 0;
		bool b = map.tryFind(String("Hello"), out);
		AES_ASSERT(b == false);

		for (auto const& [k, v] : map)
		{
			//AES_LOG("elem {} : {}", k, v);
			printf("%s %d\n", k.c_str(), v);
		}

	}
	return 0;
}