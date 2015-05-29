#ifndef _UTILITY_NULL_REFERENCE_
#define _UTILITY_NULL_REFERENCE_
/*
	nullref Global defines

	You can use this two defines: nullref(Type) and NULLREF to 
	assign a null reference in your codes.

	Usage:

		YourClass& getNullref()
		{ return nullref(YourClass); // remember to use in this format }

	or

		YourClass& obj = nullref(YourClass);

	Also, if you've already called this method and assigned a null reference
	to an object, you can call this to judge if it's a null reference:

		YourClass& obj = getNullref();
		if ( obj == NULLREF )

	or

		if ( obj != NULLREF )

	However, it is not recommanded to use this nullref for it disobeyed the 
	initial design of reference in C++. Remember to check whenever you have a
	potential nullref assignment.

	I recommand try-catch blocks rather than this.
*/

// nullref class, similar as nullptr.
class _CLS_NULLREF final{
public:
	_CLS_NULLREF(){}
	_CLS_NULLREF(const _CLS_NULLREF&) = delete;
	_CLS_NULLREF& operator=(const _CLS_NULLREF&) = delete;
	~_CLS_NULLREF(){}
};

// Use the two defines
#define nullref(Type) *((Type*)(nullptr))
#define NULLREF _NULLREF_

// Overload the two operators
template <typename T>
bool operator==(const typename T& ref, const _CLS_NULLREF&)
{
	return ( &ref == nullptr );
}

template <typename T>
bool operator!=(const typename T& ref, const _CLS_NULLREF&)
{
	return !(ref == NULLREF);
}

// Declare an object... do not use it!
static _CLS_NULLREF _NULLREF_;

#endif


