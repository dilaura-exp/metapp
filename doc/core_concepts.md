[//]: # (Auto generated file, don't modify this file.)

# Core concepts and mechanism
<!--begintoc-->
* [Overview](#a2_1)
* [TypeKind - type kind type](#a2_2)
* [Variant - the core data](#a2_3)
* [MetaType - the core meta type](#a2_4)
* [UnifiedType -- CV-unaware meta type](#a2_5)
* [UpType - the most powerful concept in the meta type system](#a2_6)
* [DeclareMetaType](#a2_7)
* [Meta interface](#a2_8)
* [MetaRepo](#a2_9)
* [Accessible](#a2_10)
* [Callable](#a2_11)
<!--endtoc-->

<a id="a2_1"></a>
## Overview

This document gives an overview on the core concepts and core design in metapp. It's to give you a rough idea on how metapp works. There are separate document for each topics.  

<a id="a2_2"></a>
## TypeKind - type kind type

`metapp::TypeKind` is a 16 bit unsigned integer that represents the meta type kind. For example, `metapp::tkInt` is the meta type for `int`.  
Each meta type has one and only one TypeKind, different meta types may have the same TypeKind, that's to say, TypeKind is not unique. For example, `metapp::tkStdSharedPtr` represents all meta types of `std::shared_ptr<T>`, in which T can be any C++ type.  

See [Built-in meta types document](metatypes/list_all.md) for a list of built-in type kinds.

<a id="a2_3"></a>
## Variant - the core data

`metapp::Variant` allows to store data of any type. `Variant` = `MetaType` + data instance.  
`Variant` is used extensively in metapp library.  

<a id="a2_4"></a>
## MetaType - the core meta type

`metapp::MetaType` is the core class to represent the meta type. Unlike some other reflection libraries which are meta class based, everything in metapp is meta type. A class is a meta type, an enum is a meta type, the same for functions, constructors, containers, etc.  
With MetaType, we can construct the underlying object, destroy the object, get object value, cast the type, etc.  
A MetaType can be obtained at compile time using function `metapp::getMetaType()`, or at run time via class `MetaRepo`.  

Prototype of `getMetaType()`  
```c++
template <typename T>
const MetaType * getMetaType();
```

The pointer returned by `getMetaType()` is always the same for the same T. For example,  

```c++
ASSERT(metapp::getMetaType<int>() == metapp::getMetaType<int>());
ASSERT(metapp::getMetaType<std::string>() == metapp::getMetaType<std::string>());
```

**Note**: MetaType is CV-aware (CV means const-volatile). That's to say, for the same T, different CV qualified types will result different MetaType. For example,  

```c++
ASSERT(metapp::getMetaType<int>() != metapp::getMetaType<const int>());
ASSERT(metapp::getMetaType<std::string>() != metapp::getMetaType<volatile std::string>());
```

**Note**: `getMetaType()` can be used on any C++ type, the tye doesn't need to be registered to metapp.  

<a id="a2_5"></a>
## UnifiedType -- CV-unaware meta type

MetaType member function `getUnifiedType()` can retrieve the underlying UnifiedType.  

Prototype of `getUnifiedType()`  
```c++
const void * getUnifiedType() const noexcept;
```

Each MetaType has one and only one UnifiedType. UnifiedType is similar to MetaType, except that UnifiedType is CV-unaware. That's to say, for the same T, no matter it's qualified with const or volatile, the UnifiedType is always the same. So,  

```c++
ASSERT(metapp::getMetaType<int>()->getUnifiedType() == metapp::getMetaType<const int>()->getUnifiedType());
ASSERT(metapp::getMetaType<std::string>()->getUnifiedType() == metapp::getMetaType<volatile std::string>()->getUnifiedType());
```

`UnifiedType` is an opaque type, so it's a `const void *`. Its only function is to identify or compare types.  

Both MetaType and UnifiedType can be used to identify C++ type, they are the C++ `typeid` in metapp.  

<a id="a2_6"></a>
## UpType - the most powerful concept in the meta type system

A MetaType has 0, 1, or multiple UpTypes. An UpType is a MetaType object.  
Below member functions in MetaType can be used to retrieve UpType.  

```c++
const MetaType * getUpType() const noexcept;
const MetaType * getUpType(const size_t i) const;
size_t getUpTypeCount() const noexcept;
```

A MetaType only has one TypeKind, so it can represent only one type information, it can't represent compound information. For example, for type `int *`, the TypeKind of the MetaType is `tkPointer`, the `int` type is lost. For `std::vector<char>`, the TypeKind of the MetaType is `tkStdVector`, the `char` type is lost.  
UpType is used to represent the lost type information.  

Here are some examples,  

MetaType `int *` has one UpType. 

```c++
ASSERT(metapp::getMetaType<int *>()->getTypeKind() == metapp::tkPointer);
ASSERT(metapp::getMetaType<int *>()->getUpTypeCount() == 1);
ASSERT(metapp::getMetaType<int *>()->getUpType()->getTypeKind() == metapp::tkInt);
```

MetaType `int * &` has one UpType, and the UpType has one another UpType.    

```c++
ASSERT(metapp::getMetaType<int * &>()->getTypeKind() == metapp::tkReference);
ASSERT(metapp::getMetaType<int * &>()->getUpTypeCount() == 1);
ASSERT(metapp::getMetaType<int * &>()->getUpType()->getTypeKind() == metapp::tkPointer);
ASSERT(metapp::getMetaType<int * &>()->getUpType()->getUpType()->getTypeKind() == metapp::tkInt);
```

MetaType `std::vector<char>` has one UpType.    

```c++
ASSERT(metapp::getMetaType<std::vector<char> >()->getTypeKind() == metapp::tkStdVector);
ASSERT(metapp::getMetaType<std::vector<char> >()->getUpTypeCount() == 1);
ASSERT(metapp::getMetaType<std::vector<char> >()->getUpType()->getTypeKind() == metapp::tkChar);
```

MetaType `void (int *)` has two UpTypes, the first is the result type `void`, the second is the parameter `int *`. Since the parameter is a pointer, the parameter also has an UpType.    

```c++
ASSERT(metapp::getMetaType<void (int *)>()->getTypeKind() == metapp::tkFunction);
ASSERT(metapp::getMetaType<void (int *)>()->getUpTypeCount() == 2);
ASSERT(metapp::getMetaType<void (int *)>()->getUpType(0)->getTypeKind() == metapp::tkVoid);
ASSERT(metapp::getMetaType<void (int *)>()->getUpType(1)->getTypeKind() == metapp::tkPointer);
ASSERT(metapp::getMetaType<void (int *)>()->getUpType(1)->getUpType()->getTypeKind() == metapp::tkInt);
```

MetaType `int` doesn't have any UpType.    

```c++
ASSERT(metapp::getMetaType<int>()->getTypeKind() == metapp::tkInt);
ASSERT(metapp::getMetaType<int>()->getUpTypeCount() == 0);
ASSERT(metapp::getMetaType<int>()->getUpType() == nullptr);
```

UpType represents complicated C++ type recursively. With UpType, metapp can represent any C++ type.

See [Built-in meta types document](built-in-meta-types.md) for the UpTypes for each TypeKind.

<a id="a2_7"></a>
## DeclareMetaType

Even though metapp works on any C++ type which are not known to metapp, it's useful to provide metapp more information on a certain type. The template `DeclareMetaType` is to provide such information. For example,  

```c++
class MyClass {};

constexpr metapp::TypeKind tkMyClass = metapp::tkUser;

// DeclareMetaType must be in global or in namespace metapp
template <>
struct metapp::DeclareMetaType <MyClass> : public metapp::DeclareMetaTypeBase <MyClass>
{
  static constexpr metapp::TypeKind typeKind = tkMyClass;
};
```

```c++
ASSERT(metapp::getMetaType<MyClass>()->getTypeKind() == tkMyClass);
```

<a id="a2_8"></a>
## Meta interface 

The core class MetaType only has very few public functions, and provides very few functions. metapp obeys the principle "You don't pay (or pay very little) for what you don't use". We usually don't need many function from most meta types, so the core MetaType is quite small. The powerful features are extensions in MetaType, which called meta interface. For example, if a meta type implements the interface MetaClass, then we can get the class information such as methods, fields, constructors, etc, from the interface. If the meta type doesn't implement the interface MetaClass, we can not and don't need to get class information from it.  

Here is a list of MetaType member functions to retrieve meta interfaces,  
```c++
const MetaClass * getMetaClass() const;
const MetaCallable * getMetaCallable() const;
const MetaAccessible * getMetaAccessible() const;
const MetaEnum * getMetaEnum() const;
const MetaIndexable * getMetaIndexable() const;
const MetaIterable * getMetaIterable() const;
const MetaStreaming * getMetaStreaming() const;
const MetaMappable * getMetaMappable() const;
const MetaMember * getMetaMember() const;
const void * getMetaUser() const;
```

<a id="a2_9"></a>
## MetaRepo

`MetaRepo` class is used to register and retrieve meta information at running time.  
We can use function `MetaRepo * getMetaRepo();` to get the global `MetaRepo`, or we can create arbitrary `MetaRepo` instances.  
A `MetaRepo` can contain meta types (classes, enums, etc), accessibles, callables, and other `MetaRepo` (nested repos).  
We can use nested repos to simulate C++ `namespace`, that's to say, a repo is a namespace.  

<a id="a2_10"></a>
## Accessible

"Accessible" can be pointer to global variable, member data, the object created by metapp::createAccessor, or anything that implements meta interface `MetaAccessible`. The term "accessible" is used for "field" or "property" in other reflection system.  

<a id="a2_11"></a>
## Callable

"Callable" can be global free function, member function, std::function, or anything that implements meta interface `MetaCallable`. The term "callable" is used for "method" or "function" in other reflection system.