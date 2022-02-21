#ifndef MEMBER_DATA_H_969872685611
#define MEMBER_DATA_H_969872685611

#include "metapp/metatype.h"

namespace metapp {

template <typename Class, typename T>
struct DeclareMetaType <T Class::*, typename std::enable_if<! std::is_function<T>::value>::type>
	: public DeclareMetaTypeBase <T Class::*>
{
	using UpType = TypeList <Class, T>;
	static constexpr TypeKind typeKind = tkMemberPointer;

	static Variant accessibleGet(const Variant & accessible, const void * instance) {
		return ((const Class *)instance)->*(accessible.get<T Class::*>());
	}

	static void accessibleSet(const Variant & accessible, void * instance, const Variant & value) {
		doAccessibleSet<Class, T>(accessible, instance, value);
	}

private:
	template <typename C, typename U>
	static void doAccessibleSet(
		const Variant & /*accessible*/,
		void * /*instance*/,
		const Variant & /*value*/,
		typename std::enable_if<! AccessibleIsAssignable<T C::*>::value>::type * = nullptr
	) {
	}

	template <typename C, typename U>
	static void doAccessibleSet(
		const Variant & accessible,
		void * instance,
		const Variant & value,
		typename std::enable_if<AccessibleIsAssignable<T C::*>::value>::type * = nullptr
	) {
		((Class *)instance)->*(accessible.get<T Class::*>()) = value.cast<U>().template get<const U &>();
	}

};


} // namespace metapp


#endif

