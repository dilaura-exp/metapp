// metapp library
// 
// Copyright (C) 2022 Wang Qi (wqking)
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "test.h"

#include "metapp/variant.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/metatypes/metatypes.h"
#include "metapp/inheritancerepo.h"

#include <string>
#include <iostream>
#include <climits>

namespace {

struct A
{
	int a;
	int value;
};

struct B : A
{
	int b;
	int value;
};

struct B2
{
	int b2;
};

struct C : B, B2
{
	int c;
	int value;
	int notReflected;
};

} // namespace

template <>
struct metapp::DeclareMetaType <A> : public metapp::DeclareMetaTypeBase <A>
{
	static const metapp::MetaClass * getMetaClass() {
		static const metapp::MetaClass metaClass(
			metapp::getMetaType<A>(),
			[](metapp::MetaClass & mc) {
				mc.addField("a", &A::a);
				mc.addField("value", &A::value);
			}
		);
		return &metaClass;
	}

};

template <>
struct metapp::DeclareMetaType <B> : public metapp::DeclareMetaTypeBase <B>
{
	static void setup()
	{
		metapp::getInheritanceRepo()->registerBase<B, A>();
	}

	static const metapp::MetaClass * getMetaClass() {
		static const metapp::MetaClass metaClass(
			metapp::getMetaType<B>(),
			[](metapp::MetaClass & mc) {
				mc.addField("b", &B::b);
				mc.addField("value", &B::value);
			}
		);
		return &metaClass;
	}

};

template <>
struct metapp::DeclareMetaType <B2> : public metapp::DeclareMetaTypeBase <B2>
{
	static const metapp::MetaClass * getMetaClass() {
		static const metapp::MetaClass metaClass(
			metapp::getMetaType<B2>(),
			[](metapp::MetaClass & mc) {
				mc.addField("b2", &B2::b2);
			}
		);
		return &metaClass;
	}

};

template <>
struct metapp::DeclareMetaType <C> : public metapp::DeclareMetaTypeBase <C>
{
	static void setup()
	{
		metapp::getInheritanceRepo()->registerBase<C, B, B2>();
	}

	static const metapp::MetaClass * getMetaClass() {
		static const metapp::MetaClass metaClass(
			metapp::getMetaType<C>(),
			[](metapp::MetaClass & mc) {
				mc.addField("c", &C::c);
				mc.addField("value", &C::value);
			}
		);
		return &metaClass;
	}

};


TEST_CASE("MetaClass, field, struct B")
{
	auto metaTypeB = metapp::getMetaType<B>();
	auto metaClassB = metaTypeB->getMetaClass();
	B b;
	b.a = 3;
	b.value = 5;
	REQUIRE(! metaClassB->getField("notExist").isValid());
	const auto & bFieldValue = metaClassB->getField("value").getField();
	REQUIRE(bFieldValue.getMetaType()->getMetaAccessible()->get(bFieldValue, &b).template get<int>() == 5);
	const auto & bFieldA = metaClassB->getField("a").getField();
	REQUIRE(bFieldA.getMetaType()->getMetaAccessible()->get(bFieldA, &b).template get<int>() == 3);
}

TEST_CASE("MetaClass, field, struct C")
{
	auto metaTypeC = metapp::getMetaType<C>();
	auto metaClassC = metaTypeC->getMetaClass();
	C c;
	c.a = 3;
	c.c = 8;
	c.value = 5;
	REQUIRE(! metaClassC->getField("notExist").isValid());
	const auto & cFieldValue = metaClassC->getField("value").getField();
	REQUIRE(cFieldValue.getMetaType()->getMetaAccessible()->get(cFieldValue, &c).template get<int>() == 5);
	const auto & cFieldA = metaClassC->getField("a").getField();
	REQUIRE(cFieldA.getMetaType()->getMetaAccessible()->get(cFieldA, &c).template get<int>() == 3);

	std::map<std::string, int> fieldNameMap;
	auto fieldList = metaClassC->getFields();
	for(auto it = fieldList.begin(); it != fieldList.end(); ++it) {
		++fieldNameMap[it->getName()];
	}
	REQUIRE(fieldNameMap["value"] == 3);
	REQUIRE(fieldNameMap["c"] == 1);
	REQUIRE(fieldNameMap["b"] == 1);
	REQUIRE(fieldNameMap["a"] == 1);
	REQUIRE(fieldNameMap["b2"] == 1);
	REQUIRE(fieldNameMap.size() == 5);
	REQUIRE(fieldNameMap["notReflected"] == 0);
}

