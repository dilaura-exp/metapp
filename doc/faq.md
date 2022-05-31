[//]: # (Auto generated file, don't modify this file.)

# Infrequently Asked Questions
<!--begintoc-->
* [metapp doesn't recognize the built-in types such as int, it give type kind `tkObject` instead of `tkInt` for int.](#a3_1)
* [When getting meta interface from a MetaType, I need to check if it's reference and skip the reference, that's cumbersome.](#a3_2)
* [How can I convert a Variant of pointer to non-pointer?](#a3_3)
* [What's the difference between metapp and the compile time reflection feature in coming C++ standard?](#a3_4)
* [When a Variant holds a container such as std::vector or std::map, isn't convenient to use array operator [] to access the elements?](#a3_5)
* [To register meta data in MetaRepo and MetaClass, there are different functions such as registerAccessible, registerCallable, registerConstructor, etc, can't we make a single registering function that detects the meta data type automatically?](#a3_6)
<!--endtoc-->

<a id="a3_1"></a>
### metapp doesn't recognize the built-in types such as int, it give type kind `tkObject` instead of `tkInt` for int.

Your forget to `#include "metapp/allmetatypes.h"`. The header needs to be included where ever `metapp` is used.

<a id="a3_2"></a>
### When getting meta interface from a MetaType, I need to check if it's reference and skip the reference, that's cumbersome.

The utility function `getNonReferenceMetaType` is our friend. The function will get the referred type if the meta type
is a reference, otherwise it returns the meta type directly. Example code,

```c++
//Include the header for `getNonReferenceMetaType`
#include "metapp/utilities/utility.h"
```

```c++
const metapp::MetaType * metaType = metapp::getMetaType<Foo>();

// Good
const metapp::MetaClass * metaClass = metapp::getNonReferenceMetaType(metaType)->getMetaClass();

// Bad, it's verbose and the logic is duplicated everywhere
const metapp::MetaClass * metaClass;
if(metaType->isReference()) {
  metaClass = metaType->getUpType()->getMetaClass();
}
else {
  metaClass = metaType->getMetaClass();
}

// Worse, don't do it. There is no MetaClass on a reference.
const metapp::MetaClass * metaClass = metaType->getMetaClass();
```

<a id="a3_3"></a>
### How can I convert a Variant of pointer to non-pointer?

The first argument in some meta interface functions, such as `MetaAccessible::get/set`, `MetaCallable::invoke`, the first argument
is a Variant that requires to be either value or reference, but not pointer.
If we have a pointer and want to convert it to non-pointer so that we can pass it to those functions, we can
call function `depointer` in `utility.h` to covert it to non-pointer.

<a id="a3_4"></a>
### What's the difference between metapp and the compile time reflection feature in coming C++ standard?

metapp is runtime reflection. A program runs almost all time at "running time". With metapp, we can
get type information from dynamical libraries, or bind meta data to script engine, or get property data
from external file to use in a GUI property editor. Those are not easy to do at compile time.  

<a id="a3_5"></a>
### When a Variant holds a container such as std::vector or std::map, isn't convenient to use array operator [] to access the elements?

For example, `someVariant[1] = 5`.  
The problem is, metapp supports `MetaIndexable` and `MetaMappable`, both of which allow to array style access.  
To support the `[]` operator, `Variant` will first try to get `MetaMappable`, then if not it tries to get `MetaIndexable`.
Such kinds of "trials", or to say, "guess", is not only having performance cost, but also is error prone and the user may
abuse using the `[]` operator.  
Also, most likely metapp will support operators in meta type in the future, then that will conflict if `Variant` supports `[]`
specially for now.

<a id="a3_6"></a>
### To register meta data in MetaRepo and MetaClass, there are different functions such as registerAccessible, registerCallable, registerConstructor, etc, can't we make a single registering function that detects the meta data type automatically?

Yes it's possible, we can make a function, such as `versatileRegister`, then we can write,  

```c++
mc.versatileRegister(&MyClass::value); // register as accessible
mc.versatileRegister(&MyClass::getName); // register as callable
```

The problem is, it's quite confusing and error prone. How to register a Variant that implements both MetaCallable and MetaAccessible?
`versatileRegister` may register it in wrong type without noticeable error.  
The API should be clear, less confusing, less implicitly operations.