#pragma once

#include "constants.h"

template< typename T >
class CPointer
{
	public:
	//avoid breaking of strict aliasing rules in DCAS, use unions
    union
    {
        struct
        {
	        T *ptr;
	        uint_sys counter;
        };

        MSQ_DCAS_TYPE dcasval;
    };

	//initial value of counters do not matter, but initialize them for valgrind
	CPointer() INLINE_ATTR : ptr(0), counter(0) {}
	CPointer(T *inptr, uint_sys incounter) INLINE_ATTR : ptr(inptr), counter(incounter) {}
	CPointer(const volatile CPointer &other) INLINE_ATTR : ptr(other.ptr), counter(other.counter) {}

	//bool operator==(const volatile CPointer &other)
	//{
		//return (ptr == other.ptr) && (counter == other.counter)
	//	return dcasval == other.dcasval;
	//}
	bool operator!=(const volatile CPointer &other) INLINE_ATTR
	{
		//return (ptr != other.ptr) || (counter != other.counter)
		return dcasval != other.dcasval;
	}

	T* getPtr() INLINE_ATTR
	{
		return ptr;
	}

	void setPtr(T *other) INLINE_ATTR volatile
	{
		ptr = other;
	}

	uint_sys getCounter() INLINE_ATTR
	{
		return counter;
	}

	void setCounter(uint_sys c) INLINE_ATTR
	{
		counter = c;
	}

	static bool CAS(volatile CPointer<T> *dest, CPointer<T> &oldval, CPointer<T> &newval) INLINE_ATTR
	{
		return __sync_bool_compare_and_swap(&(dest->dcasval), oldval.dcasval, newval.dcasval);
	}

} __attribute__ ((aligned (MSQ_ALIGNMENT)));
