# Tutorial for using MetaRepo and runtime meta data

`MetaRepo` is used to register and retrieve meta types at running time.
The registered meta types can be any C++ types, no matter if they are declared with DeclareMetaType or not.   

## Include headers
Header for MetaRepo

```c++
#include "metapp/metarepo.h"
```

To use the built-in meta types, we must include "metapp/allmetatypes.h"

```c++
#include "metapp/allmetatypes.h"
```

## Define some C++ variables and types to reflect for

Global variable

```c++
std::vector<std::string> textList { "hello", "world" };
```

Global getter/setter

```c++
int value_ = 0;
void setValue(const int newValue)
{
  value_ = newValue;
}

int getValue()
{
  return value_;
}
```

Global function

```c++
std::string concat(const int a, const bool b, const std::string & c)
{
  return std::to_string(a)
    + (b ? "true" : "false")
    + c
  ;
}
```

Global class

```c++
class MyClass
{
};
```

## Register meta data

To get meta data from MetaRepo, we must register meta data first.  
Usually the global meta data should be registered to the global MetaRepo getting from `metapp::getMetaRepo()`.

```c++
  metapp::MetaRepo * metaRepo = metapp::getMetaRepo();
```

Register an accessible. An accessible is any meta type that implements meta interface `MetaAccessible`.  
An accessible can be a pointer to data, pointer to member data, or Accessor.  
The second argument in `registerAccessible` is a `metapp::Variant`. Any data can be casted to `metapp::Variant`
implicitly, so we don't need to specify `metapp::Variant` explicitly.

```c++
  metaRepo->registerAccessible("textList", &textList);
  metaRepo->registerAccessible("value", metapp::createAccessor<int>(&getValue, &setValue));
```

Register an callable. A callable is any meta type that implements meta interface `MetaCallable`.  
A callable can be a pointer to function, pointer to member function, or std::function, and so on.  

```c++
  metaRepo->registerCallable("concat", &concat);
```

Register a C++ type, here is class MyClass.  
This is only to demonstrate how to use registered meta type. There is a separate completed tutorial for how to use MetaClass.  

```c++
  metaRepo->registerType("MyClass", metapp::getMetaType<MyClass>());
```

## Use field meta data

Get the `MetaRepo` to which we registered the meta data

```c++
  metapp::MetaRepo * metaRepo = metapp::getMetaRepo();
```

Get the meta data of field "value"

```c++
metapp::RegisteredAccessible fieldValue = metaRepo->getAccessible("value");
```

Call metapp::accessibleGet to get the value of the field. The first parameter is the Variant.  
Call getTarget() to get the underlying Variant.

```c++
ASSERT(metapp::accessibleGet(fieldValue.getTarget(), nullptr).get<int>() == 0);
```

getTarget() can also be omitted, the RegisteredAccessible can convert to Variant automatically

```c++
ASSERT(metapp::accessibleGet(fieldValue, nullptr).get<int>() == 0);
```

Now let's set a new value

```c++
metapp::accessibleSet(fieldValue, nullptr, 5);
ASSERT(getValue() == 5);
ASSERT(metapp::accessibleGet(fieldValue, nullptr).get<int>() == 5);
```

Now append some new texts to textList

```c++
metapp::RegisteredAccessible fieldtextList = metaRepo->getAccessible("textList");
metapp::accessibleGet(fieldtextList, nullptr).get<std::vector<std::string> &>().push_back("good");
ASSERT(metapp::accessibleGet(fieldtextList, nullptr).get<const std::vector<std::string> &>()[0] == "hello");
ASSERT(metapp::accessibleGet(fieldtextList, nullptr).get<const std::vector<std::string> &>()[1] == "world");
ASSERT(metapp::accessibleGet(fieldtextList, nullptr).get<const std::vector<std::string> &>()[2] == "good");
```

## Use function meta data

```c++
  metapp::MetaRepo * metaRepo = metapp::getMetaRepo();
```

Get the meta data of method "concat".

```c++
metapp::RegisteredCallable methodConcat = metaRepo->getCallable("concat");
```

Call metapp::callableInvoke to invoke the method, and pass the arguments.  
The return value is a metapp::Variant.

```c++
metapp::Variant result = metapp::callableInvoke(methodConcat, nullptr, 38, true, "Great");
ASSERT(result.get<const std::string &>() == "38trueGreat");
```

## Use registered types

```c++
  metapp::MetaRepo * metaRepo = metapp::getMetaRepo();

  metapp::RegisteredType myClassType = metaRepo->getType("MyClass");
  ASSERT(myClassType.getTarget() == metapp::getMetaType<MyClass>());
```