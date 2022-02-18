#ifndef METATYPE_I_H_969872685611
#define METATYPE_I_H_969872685611

#include <type_traits>

namespace metapp {

class MetaType;

namespace internal_ {

struct NoneUpType {};

enum class MetaMethodAction
{
	constructDefault,
	constructWith,
	getAddress,
	canCast,
	cast,
	canInvoke,
	invoke,
	streamIn,
	streamOut,
};

struct ParamConstruct
{
	MetaTypeData * data;
};

struct ParamConstructWith
{
	MetaTypeData * data;
	const void * value;
};
 
struct ParamGetAddress
{
	const MetaTypeData * data;
	void * value;
};
 
struct ParamCanCast
{
	const MetaType * toMetaType;
	bool result;
};
 
struct ParamCast
{
	Variant * result;
	const Variant * value;
	const MetaType * toMetaType;
};
 
struct ParamCanInvoke
{
	const Variant * arguments;
	size_t argumentCount;
	bool result;
};

struct ParamInvoke
{
	Variant * result;
	void * instance;
	const Variant * func;
	const Variant * arguments;
	size_t argumentCount;
};

struct ParamStreamIn
{
	std::istream * stream;
	Variant * value;
};
 
struct ParamStreamOut
{
	std::ostream * stream;
	const Variant * value;
};
 
struct MetaMethodParam
{
	MetaMethodAction action;
	union {
		ParamConstruct paramConstruct;
		ParamConstructWith paramConstructWith;
		ParamGetAddress paramGetAddress;
		ParamCanCast paramCanCast;
		ParamCast paramCast;
		ParamCanInvoke paramCanInvoke;
		ParamInvoke paramInvoke;
		ParamStreamIn paramStreamIn;
		ParamStreamOut paramStreamOut;
	};
};

using FuncMetaMethod = void (*)(MetaMethodParam & param);

template <typename M>
void commonMetaMethod(MetaMethodParam & param)
{
	switch(param.action) {
	case MetaMethodAction::constructDefault:
		M::constructDefault(*(param.paramConstruct.data));
		break;

	case MetaMethodAction::constructWith:
		M::constructWith(*(param.paramConstructWith.data), param.paramConstructWith.value);
		break;

	case MetaMethodAction::getAddress:
		param.paramGetAddress.value = M::getAddress(*(param.paramGetAddress.data));
		break;

	case MetaMethodAction::canCast:
		param.paramCanCast.result = M::canCast(param.paramCanCast.toMetaType);
		break;

	case MetaMethodAction::cast:
		*param.paramCast.result = M::cast(*(param.paramCast.value), param.paramCast.toMetaType);
		break;

	case MetaMethodAction::canInvoke:
		param.paramCanInvoke.result = M::canInvoke(param.paramCanInvoke.arguments, param.paramCanInvoke.argumentCount);
		break;

	case MetaMethodAction::invoke:
		*param.paramInvoke.result = M::invoke(
			param.paramInvoke.instance,
			*param.paramInvoke.func,
			param.paramInvoke.arguments,
			param.paramInvoke.argumentCount
		);
		break;

	case MetaMethodAction::streamIn:
		M::streamIn(*(param.paramStreamIn.stream), *(param.paramStreamIn.value));
		break;

	case MetaMethodAction::streamOut:
		M::streamOut(*(param.paramStreamOut.stream), *(param.paramStreamOut.value));
		break;
	}
}

struct UpTypeData
{
	const MetaType ** upTypeList;
	uint16_t count;
};


} // namespace internal_


} // namespace metapp

#endif
