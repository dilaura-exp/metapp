#include "../test.h"

#include "metapp/variant.h"
#include "metapp/metatypes/metatypes.h"

#include <iostream>

namespace {

int sum(const metapp::Variant * arguments, const size_t argumentCount)
{
	int total = 0;
	for(size_t i = 0; i < argumentCount; ++i) {
		total += arguments[i].cast<int>().get<int>();
	}
	return total;
}

TEST_CASE("metatypes, VariadicFunction, sum")
{
	metapp::Variant v = (metapp::VariadicFunction<decltype(&sum)>(&sum));
	REQUIRE(v.getMetaType()->getTypeKind() == metapp::tkVariadicFunction);
	REQUIRE(v.getMetaType()->getUpType()->getTypeKind() == metapp::tkFunction);

	SECTION("0 arguments") {
		REQUIRE(v.getMetaType()->getMetaCallable()->canInvoke(nullptr, 0));
		REQUIRE(v.getMetaType()->getMetaCallable()->invoke(v, nullptr, nullptr, 0).get<int>() == 0);
	}
	SECTION("2 arguments") {
		metapp::Variant arguments[] = { 6, 3 };
		REQUIRE(v.getMetaType()->getMetaCallable()->canInvoke(arguments, 2));
		REQUIRE(v.getMetaType()->getMetaCallable()->invoke(v, nullptr, arguments, 2).get<int>() == 9);
	}
	SECTION("5 arguments") {
		metapp::Variant arguments[] = { 6, 3, 1, 5, 8 };
		REQUIRE(v.getMetaType()->getMetaCallable()->canInvoke(arguments, 5));
		REQUIRE(v.getMetaType()->getMetaCallable()->invoke(v, nullptr, arguments, 5).get<int>() == 23);
	}
}

struct MyClass
{
	std::string join(const metapp::Variant * arguments, const size_t argumentCount)
	{
		std::string result(value);
		for(size_t i = 0; i < argumentCount; ++i) {
			result += arguments[i].cast<std::string>().get<std::string>();
		}
		return result;
	}

	std::string value;
};

TEST_CASE("metatypes, VariadicFunction, MyClass::join")
{
	metapp::Variant v(metapp::createVariadicFunction(&MyClass::join));
	SECTION("0 arguments") {
		MyClass obj { "first" };
		REQUIRE(v.getMetaType()->getMetaCallable()->canInvoke(nullptr, 0));
		REQUIRE(v.getMetaType()->getMetaCallable()->invoke(v, &obj, nullptr, 0).get<const std::string&>()
			== 
			"first"
		);
	}
	SECTION("2 arguments") {
		MyClass obj { "first" };
		metapp::Variant arguments[] = { "abc", "hello" };
		REQUIRE(v.getMetaType()->getMetaCallable()->canInvoke(arguments, 2));
		REQUIRE(v.getMetaType()->getMetaCallable()->invoke(v, &obj, arguments, 2).get<const std::string&>()
			== 
			"firstabchello"
		);
	}
}


} // namespace