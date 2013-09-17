#ifndef __CHAIN_H__
#define __CHAIN_H__
#include "cpointer.h"


class Chain {
public:
	Chain() : next(0) {} 
	Chain(Chain* next) : next(next) { }
	union
	{
		Chain* volatile next;
		CPointer<Chain> cnext;
	};
	void* data;
} __attribute__ ((aligned (64)));
#endif
