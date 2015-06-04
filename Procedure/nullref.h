#pragma once
/*
	nullref Global defines
 
	You can use this two defines: NULLREF(Type) and nullref to
	assign a null reference in your codes.
 
	Usage:
 
		 YourClass& getNullref()
		{ return NULLREF(YourClass); // remember to use in this format }
 
	or
 
		YourClass& obj = NULLREF(YourClass);
 
	Also, if you've already called this method and assigned a null reference
	to an object, you can call this to judge if it's a null reference:
 
		YourClass& obj = getNullref();
		if ( obj == nullref )
 
	or
 
		if ( obj != nullref )

	The usage above can be applied for Xcode and VS. Notice: if you ever tried
	to return a NULLREF(YourClass) while your actual return type is NOT a 
	reference, Xcode will throw a EXC_BAD_ACCESS but vs will do nothing.
 
	However, it is not recommanded to use this nullref for it disobeyed the
	initial design of reference in C++. Remember to check whenever you have a
	potential nullref assignment.
 
	I recommand try-catch blocks rather than this.
 */

// Disable warning C4005: "nullref" macro redefinition in vs
#ifdef _MSC_VER
#pragma warning (disable : 4005)
#endif

// Use the two defines
#define NULLREF(Type) *((Type*)(nullptr))
#define nullref _NULLREF_

// nullref class, similar as nullptr.
class _CLS_NULLREF final{
public:
    _CLS_NULLREF(){}
    _CLS_NULLREF(const _CLS_NULLREF&) = delete;
    _CLS_NULLREF& operator=(const _CLS_NULLREF&) = delete;
    ~_CLS_NULLREF(){}
};

// Declare an object... do not use it!
static const _CLS_NULLREF _NULLREF_;

// Overload the two operators
template <typename T>
bool operator==(const T& ref, const _CLS_NULLREF&)
{
    return ( &ref == nullptr );
}

template <typename T>
bool operator!=(const T& ref, const _CLS_NULLREF&)
{
    return !(ref == nullref);
}

#ifdef _MSC_VER
#pragma warning (default : 4005)
#endif