#ifndef METAPP_METATYPE_H_969872685611
#define METAPP_METATYPE_H_969872685611

#include "metapp/compiler.h"
#include "metapp/typekind.h"
#include "metapp/metatypedata.h"
#include "metapp/exception.h"
#include "metapp/utilities//typelist.h"

// The ugly macro trick is to bypass cyclic include variant.h and metatype.h
#define METAPP_VARIANT_IMPL_H_969872685611
#include "metapp/variant.h"
#undef METAPP_VARIANT_IMPL_H_969872685611

#include "metapp/implement/internal/metatype_i.h"

#include <type_traits>
#include <initializer_list>
#include <istream>
#include <ostream>

namespace metapp {

class MetaType;
class MetaClass;
class MetaCallable;
class MetaAccessible;
class MetaArray;
class MetaEnum;
class UnifiedType;

template <typename T, typename Enabled = void>
struct DeclareMetaType;

template <typename T>
const MetaType * getMetaType();

template <typename T>
const UnifiedType * getUnifiedType();

bool areMetaTypesMatched(const MetaType * fromMetaType, const MetaType * toMetaType, const bool strictMode);

class UnifiedType
{
public:
	~UnifiedType() = default;

	TypeKind getTypeKind() const noexcept;

	const MetaClass * getMetaClass() const;
	const MetaCallable * getMetaCallable() const;
	const MetaAccessible * getMetaAccessible() const;
	const MetaArray * getMetaArray() const;
	const MetaEnum * getMetaEnum() const;

	void * constructData(MetaTypeData * data, const void * copyFrom) const;
	
	void destroy(void * instance) const;

	void * getAddress(const MetaTypeData & data) const;
	
	bool canCast(const Variant & value, const MetaType * toMetaType) const;
	Variant cast(const Variant & value, const MetaType * toMetaType) const;

	void streamIn(std::istream & stream, Variant & value) const;
	void streamOut(std::ostream & stream, const Variant & value) const;

private:
	constexpr UnifiedType(
		const TypeKind typeKind,
		const internal_::MetaMethodTable & metaMethodTable
	) noexcept;
	UnifiedType() = delete;
	UnifiedType(const UnifiedType &) = delete;
	UnifiedType(UnifiedType &&) = delete;

	template <typename T>
	friend const UnifiedType * getUnifiedType();

	const void * doGetMetaInterface(const internal_::MetaInterfaceKind kind) const;

private:
	TypeKind typeKind;
	internal_::MetaMethodTable metaMethodTable;
};

namespace internal_ {

template <typename T>
auto doGetMetaType()
	-> typename std::enable_if<std::is_same<T, NoneUpType>::value, const MetaType *>::type;
template <typename T>
auto doGetMetaType()
	-> typename std::enable_if<! std::is_same<T, NoneUpType>::value, const MetaType *>::type;
} // namespace internal_

class MetaType
{
public:
	~MetaType() = default;

	const UnifiedType * getUnifiedType() const noexcept;

	const MetaType * getUpType() const noexcept;
	const MetaType * getUpType(const size_t i) const;
	size_t getUpTypeCount() const noexcept;

	TypeKind getTypeKind() const noexcept;

	constexpr bool isConst() const noexcept;
	constexpr bool isVolatile() const noexcept;
	constexpr bool isClassMember() const noexcept;

	const MetaClass * getMetaClass() const;
	const MetaCallable * getMetaCallable() const;
	const MetaAccessible * getMetaAccessible() const;
	const MetaArray * getMetaArray() const;
	const MetaEnum * getMetaEnum() const;

	void * construct() const;
	void * copyConstruct(const void * copyFrom) const;

	// re-implementable meta methods
	void * constructData(MetaTypeData * data, const void * copyFrom) const;

	void destroy(void * instance) const;

	void * getAddress(const MetaTypeData & data) const;
	
	bool canCast(const Variant & value, const MetaType * toMetaType) const;
	Variant cast(const Variant & value, const MetaType * toMetaType) const;

	void streamIn(std::istream & stream, Variant & value) const;
	void streamOut(std::ostream & stream, const Variant & value) const;

private:
	constexpr MetaType(
		const UnifiedType * unifiedType,
		const internal_::UpTypeData & upTypeData,
		const TypeFlags typeFlags
	) noexcept;
	MetaType() = delete;
	MetaType(const MetaType &) = delete;
	MetaType(MetaType &&) = delete;

	template <typename T>
	friend auto internal_::doGetMetaType()
		-> typename std::enable_if<! std::is_same<T, internal_::NoneUpType>::value, const MetaType *>::type;

private:
	const UnifiedType * unifiedType;
	internal_::UpTypeData upTypeData;
	TypeFlags typeFlags;
};

template <typename T>
struct DeclareMetaTypeObject
{
private:
	using Decayed = typename std::decay<typename std::remove_reference<T>::type>::type;

public:
	using UpType = internal_::NoneUpType;

	static constexpr TypeKind typeKind = tkObject;
	static constexpr TypeFlags typeFlags = 0;

	static void * constructData(MetaTypeData * data, const void * copyFrom) {
		if(data != nullptr) {
			data->construct<Decayed>(copyFrom);
			return nullptr;
		}
		else {
			if(copyFrom == nullptr) {
				return new Decayed();
			}
			else {
				return new Decayed(*(Decayed *)copyFrom);
			}
		}
	}

	static void destroy(void * instance)
	{
		doDestroy(static_cast<Decayed *>(instance));
	}

	static void * getAddress(const MetaTypeData & data)
	{
		return data.getAddress();
	}

	static bool canCast(const Variant & value, const MetaType * toMetaType)
	{
		if(std::is_void<Decayed>::value) {
			return false;
		}
		const MetaType * fromMetaType = getMetaType<T>();
		if(areMetaTypesMatched(fromMetaType, toMetaType, true)) {
			return true;
		}
		if(fromMetaType->getTypeKind() != tkReference
			&& toMetaType->getTypeKind() == tkReference
			&& fromMetaType->canCast(value, toMetaType->getUpType())
			) {
			return true;
		}
		return false;
	}

	static Variant cast(const Variant & value, const MetaType * toMetaType)
	{
		return doCast<Decayed>(value, toMetaType);
	}

	static void streamIn(std::istream & stream, Variant & value) {
		variantStreamIn<T>(stream, value);
	}

	static void streamOut(std::ostream & stream, const Variant & value) {
		variantStreamOut<T>(stream, value);
	}

private:
	template <typename U>
	static void doDestroy(U * /*instance*/, typename std::enable_if<std::is_void<U>::value>::type * = nullptr) {
	}

	template <typename U>
	static void doDestroy(U * instance, typename std::enable_if<! std::is_void<U>::value>::type * = nullptr) {
		#if defined(METAPP_COMPILER_GCC) || defined(METAPP_COMPILER_CLANG)
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
		#endif
				delete instance;
		#if defined(METAPP_COMPILER_GCC) || defined(METAPP_COMPILER_CLANG)
		#pragma GCC diagnostic pop
		#endif
	}

	template <typename U>
	static Variant doCast(const Variant & /*value*/, const MetaType * /*toMetaType*/, typename std::enable_if<std::is_void<U>::value>::type * = nullptr) {
		errorBadCast();
		return Variant();
	}

	template <typename U>
	static Variant doCast(const Variant & value, const MetaType * toMetaType, typename std::enable_if<! std::is_void<U>::value>::type * = nullptr) {
		const MetaType * fromMetaType = getMetaType<T>();
		if(areMetaTypesMatched(fromMetaType, toMetaType, true)) {
			return value;
		}
		if(fromMetaType->getTypeKind() != tkReference
			&& toMetaType->getTypeKind() == tkReference
			&& fromMetaType->canCast(value, toMetaType->getUpType())
			) {
			return fromMetaType->cast(value, toMetaType->getUpType());
		}
		errorBadCast();
		return Variant();
	}
};

template <typename T, typename Enabled = void>
struct DeclareMetaTypeBase : public DeclareMetaTypeObject<T>
{
};

template <typename T, typename Enabled>
struct DeclareMetaType : public DeclareMetaTypeBase<T>
{
};

template <typename T>
const UnifiedType * getUnifiedType()
{
	using M = DeclareMetaType<T>;

	static const UnifiedType unifiedType (
		M::typeKind,
		internal_::MetaMethodTable {
			&M::constructData,

			&M::destroy,

			&M::getAddress,

			&M::canCast,
			&M::cast,

			&M::streamIn,
			&M::streamOut,

			internal_::MakeMetaInterfaceData<M>::getMetaInterfaceData(),
		}
	);
	return &unifiedType;
}

template <typename T>
const MetaType * getMetaType();

template <>
struct DeclareMetaTypeBase <void> : public DeclareMetaTypeObject<void>
{
	static constexpr TypeKind typeKind = tkVoid;

	static void * constructData(MetaTypeData * /*data*/, const void * /*value*/) {
		return nullptr;
	}

};

} // namespace metapp

#include "metapp/variant.h"

#include "metapp/implement/metatype_impl.h"

#endif
