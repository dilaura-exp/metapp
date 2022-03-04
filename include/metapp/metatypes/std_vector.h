#ifndef METAPP_STD_VECTOR_H_969872685611
#define METAPP_STD_VECTOR_H_969872685611

#include "metapp/metatype.h"
#include "metapp/metatypes/utils/metaindexablebase.h"
#include "metapp/metatypes/utils/metaiterablebase.h"

#include <vector>

namespace metapp {

template <typename T, typename Allocator>
struct DeclareMetaTypeBase <std::vector<T, Allocator> >
	: public DeclareMetaTypeObject <std::vector<T, Allocator> >,
		public MetaIndexableBase<std::vector<T, Allocator> >,
		public MetaIterableBase<std::vector<T, Allocator> >
{
	using UpType = T;
	static constexpr TypeKind typeKind = tkStdVector;

	using VectorType = std::vector<T, Allocator>;

};


} // namespace metapp


#endif

