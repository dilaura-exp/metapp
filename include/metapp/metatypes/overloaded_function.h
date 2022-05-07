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

#ifndef METAPP_OVERLOADED_FUNCTION_H_969872685611
#define METAPP_OVERLOADED_FUNCTION_H_969872685611

#include "metapp/metatype.h"
#include "metapp/variant.h"
#include "metapp/interfaces/metacallable.h"
#include "metapp/utilities/utility.h"

#include <deque>

namespace metapp {

class OverloadedFunction
{
public:
	OverloadedFunction() : callableList() {}

	const std::deque<Variant> & getCallableList() const {
		return callableList;
	}

	void addCallable(const Variant & callable) {
		callableList.push_back(callable);
	}

private:
	std::deque<Variant> callableList;
};

template <>
struct DeclareMetaType <OverloadedFunction >
{
public:
	static constexpr TypeKind typeKind = tkOverloadedFunction;

	static const MetaCallable * getMetaCallable() {
		static const MetaCallable metaCallable(
			&metaCallableGetParameterCount,
			&metaCallableGetReturnType,
			&metaCallableGetParameterType,
			&metaCallableRankInvoke,
			&metaCallableCanInvoke,
			&metaCallableInvoke
		);
		return &metaCallable;
	}

	static size_t metaCallableGetParameterCount(const Variant & /*func*/)
	{
		return 0;
	}

	static const MetaType * metaCallableGetReturnType(const Variant & /*func*/)
	{
		return getMetaType<void>();
	}

	static const MetaType * metaCallableGetParameterType(const Variant & /*func*/, const size_t /*index*/)
	{
		return getMetaType<void>();
	}

	static unsigned int metaCallableRankInvoke(const Variant & func, const Variant * arguments, const size_t argumentCount)
	{
		unsigned int maxRank;
		const auto & callableList = func.get<const OverloadedFunction &>().getCallableList();
		findCallable(callableList.begin(), callableList.end(), arguments, argumentCount, &maxRank);
		return maxRank;
	}

	static bool metaCallableCanInvoke(const Variant & func, const Variant * arguments, const size_t argumentCount)
	{
		const auto & callableList = func.get<const OverloadedFunction &>().getCallableList();
		for(const auto & callable : callableList) {
			if(callable.getMetaType()->getMetaCallable()->canInvoke(callable, arguments, argumentCount)) {
				return true;
			}
		}
		return false;
	}

	static Variant metaCallableInvoke(const Variant & func, void * instance, const Variant * arguments, const size_t argumentCount)
	{
		const auto & callableList = func.get<const OverloadedFunction &>().getCallableList();
		auto it = findCallable(callableList.begin(), callableList.end(), arguments, argumentCount, nullptr);
		if(it != callableList.end()) {
			return (*it).getMetaType()->getMetaCallable()->invoke(*it, instance, arguments, argumentCount);
		}
		errorIllegalArgument();
		return Variant();
	}

};


} // namespace metapp


#endif
