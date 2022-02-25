#ifndef STD_LIST_H_969872685611
#define STD_LIST_H_969872685611

#include "metapp/metatype.h"

#include <list>

namespace metapp {

template <typename T, typename Allocator>
struct DeclareMetaType <std::list<T, Allocator> >
	: public DeclareMetaTypeBase <std::list<T, Allocator> >
{
	using UpType = T;
	static constexpr TypeKind typeKind = tkStdList;

};


} // namespace metapp


#endif
