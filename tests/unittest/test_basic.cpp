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
#include "metapp/allmetatypes.h"
#include "metapp/interfaces/metaclass.h"
#include "metapp/utilities/utility.h"

#include <string>
#include <iostream>
#include <climits>

class MyClass
{
public:
	MyClass() {}
	MyClass(const std::string & a, const int b) {}
};

template <>
struct metapp::DeclareMetaType<MyClass> : metapp::DeclareMetaTypeBase<MyClass>
{
	static const metapp::MetaClass * getMetaClass() {
		static const metapp::MetaClass metaClass(
			metapp::getMetaType<MyClass>(),
			[](metapp::MetaClass & mc) {
				mc.registerConstructor(metapp::Constructor<MyClass()>());
				mc.registerConstructor(metapp::Constructor<MyClass(const std::string &, const int)>());
			}
		);
		return &metaClass;
	}
};

TEST_CASE("play camp")
{
	metapp::Variant ctor = metapp::Constructor<MyClass (const std::string &, const int)>();
	metapp::Variant instance(metapp::callableInvoke(ctor, nullptr, "hello", 5));
	delete instance.get<MyClass *>();

	//metapp::dumpMetaType(std::cout, metapp::getMetaType<char *(*(* * [][8])())[]>());
}

