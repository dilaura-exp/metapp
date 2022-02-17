#ifndef ARITHMETIC_H_969872685611
#define ARITHMETIC_H_969872685611

#include "metapp/metatype.h"
#include "metapp/typelist.h"

namespace metapp {

namespace internal_ {

using ArithmeticTypeList = TypeList<
	bool,
	char, wchar_t,
	signed char, unsigned char,
	short, unsigned short,
	int, unsigned int,
	long, unsigned long,
	long long, unsigned long long,
	float, double, long double
>;

inline const char * const * getArithmeticTypeNameList()
{
	static const std::array<const char *, TypeListCount<internal_::ArithmeticTypeList>::value> nameList {
		"bool",
		"char", "wchar_t",
		"signed char", "unsigned char",
		"short", "unsigned short",
		"int", "unsigned int",
		"long", "unsigned long",
		"long long", "unsigned long long",
		"float", "double", "long double"
	};
	return nameList.data();
}

} // namespace internal_

template <typename T>
struct DeclareMetaTypeBase <T,
	typename std::enable_if<TypeListIn<internal_::ArithmeticTypeList, T>::value>::type> : public DeclarePodMetaType<T>
{
	static constexpr TypeKind typeKind = TypeKind(tkFundamentalBegin + TypeListIndexOf<internal_::ArithmeticTypeList, T>::value);

	static const char * getName() {
		return internal_::getArithmeticTypeNameList()[TypeListIndexOf<internal_::ArithmeticTypeList, T>::value];
	}

	static bool canCast(const MetaType * toMetaType) {
		return toMetaType->getTypeKind() >= tkArithmeticBegin
			&& toMetaType->getTypeKind() <= tkArithmeticEnd;
	}

	using CastFunc = Variant (*)(const MetaTypeData & data);
	static const CastFunc * getCastFunctions() {
		static const std::array<CastFunc, TypeListCount<internal_::ArithmeticTypeList>::value> castFunctions {
			&podCast<T, bool>,
			&podCast<T, char>, &podCast<T, wchar_t>,
			&podCast<T, signed char>, &podCast<T, unsigned char>,
			&podCast<T, short>, &podCast<T, unsigned short>,
			&podCast<T, int>, &podCast<T, unsigned int>,
			&podCast<T, long>, &podCast<T, unsigned long>,
			&podCast<T, long long>, &podCast<T, unsigned long long>,
			&podCast<T, float>, &podCast<T, double>, &podCast<T, long double>
		};
		return castFunctions.data();
	}

	static Variant cast(const MetaTypeData & data, const MetaType * toMetaType) {
		return getCastFunctions()[toMetaType->getTypeKind() - tkArithmeticBegin](data);
	}

};


} // namespace metapp


#endif

