#include "core/wob.hpp"
#include "core/string.hpp"
#include "core/hashmap.hpp"

using namespace wob;

int main()
{
	{
		String str;
		WOB_ASSERT(str.size() == 0);
		WOB_ASSERT(str.capacity() == 0);
		str.append("a");
		WOB_ASSERT(str.size() == 1);
		WOB_ASSERT(str == "a");
	}
	{
		String str("hello");
		WOB_ASSERT(str.size() == 5);
		WOB_ASSERT(str == "hello");
		WOB_ASSERT(str != "hollo");
		WOB_ASSERT(str != "Hello");
		str.append("world");
		WOB_ASSERT(str == String("helloworld"));
		WOB_ASSERT(str != String("test"));

		String strB(str);
		WOB_ASSERT(str == strB);

		String strC(wob::move(str));
		WOB_ASSERT(strC == strB);
	}
	{
		String str("123");
		String strB;
		strB = wob::move(str);
		WOB_ASSERT(strB == "123");
		strB.clear();
		WOB_ASSERT(strB.size() == 0);
		WOB_ASSERT(strB.empty());
		WOB_ASSERT(strB.capacity() > 0);
		strB.shrink();
		WOB_ASSERT(strB.capacity() == 0);
	}
	{
		WOB_LOG("[TEST] RHI");
		HashMap<String, int> map(16);
		map.add(String("hello"), 2);
		map.add(String("world"), 15);
		map.add(String("pistav"), 12);
		map["hello"] = 16;

		WOB_ASSERT(map.size() == 3);
		map.remove(String("hello"));
		map.rehash(2);
		map.rehash(32);
		WOB_ASSERT(map.size() == 2);

		int out = 0;
		bool b = map.tryFind(String("Hello"), out);
		WOB_ASSERT(b == false);

		for (auto const& [k, v] : map)
		{
			//WOB_LOG("elem {} : {}", k, v);
			printf("%s %d\n", k.c_str(), v);
		}

	}
	return 0;
}