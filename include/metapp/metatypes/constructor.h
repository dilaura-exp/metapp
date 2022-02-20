#ifndef CONSTRUCTOR_H_969872685611
#define CONSTRUCTOR_H_969872685611

#include "metapp/metatype.h"
#include "metapp/variant.h"
#include "metapp/utility.h"

namespace metapp {

template <typename T>
struct Constructor
{
};

template <typename RT, typename ...Args>
struct DeclareMetaType <Constructor<RT (Args...)> >
	: public DeclareMetaTypeBase <Constructor<RT (Args...)> >
{
private:
	using FunctionType = Constructor<RT (Args...)>;
	using Class = RT;

public:
	using UpType = TypeList<RT, Args...>;
	static constexpr TypeKind typeKind = tkConstructor;

	static bool canInvoke(const Variant * arguments, const size_t argumentCount)
	{
		return MetaFunctionInvokeChecker<Args...>::canInvoke(arguments, argumentCount);
	}

	static Variant invoke(void * /*instance*/, const Variant & /*func*/, const Variant * arguments, const size_t argumentCount)
	{
		return MetaConstructorInvoker<Class, Args...>::construct(arguments, argumentCount);
	}

};

} // namespace metapp


#endif

