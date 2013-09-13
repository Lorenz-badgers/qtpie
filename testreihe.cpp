
#include <iostream>

#include "Chain.h"
#include "tests.h"
#include "queues.h"


//#define WOSCH
//#define HASWELL


#define PRINT_ALL_ROUND(Type) \
	cout << "test round mit "<< #Type <<" beginnt. \tThreads: " << threads << "\tOperationen: " << ops << "\tElemente zu Beginn: " << elements << endl
#define PRINT_ALL_MPSC(Type) \
	cout << "test mpsc mit\t"<< #Type <<" beginnt.\tProducers: " << threads << "\tOperationen: " << ops << endl

#define PRINT_NOTHING(Type) 

using namespace std;

#define testround(Type, elements, threads, ops, output) { \
	long time;	\
	Type *queue = new Type();	\
	Test_round t(queue, elements, threads, ops);	\
	output(Type);	\
	time = t.starttest();	\
	cout << time << "ms" << endl;	\
	t.cleanup();	\
	delete queue;	\
}

#define testmpsc(Type, threads, ops, output) {	\
	long time;	\
	Type *queue = new Type();	\
	Test_mpmc t(queue, threads, ops);	\
	output(Type);	\
	time = t.starttest();	\
	cout << time << "ms" << endl;	\
	t.cleanup();	\
	delete queue;	\
}



#define testqtv(QType, TType, stream, variable, stop, vopt, vthreads, vops) {	\
	stream << #QType << ",";	\
	{	\
		long opt = vopt;	\
		long threads = vthreads;	\
		long ops = vops;	\
		for(; variable <= stop; variable++){	\
			testqt(QType, TType, stream, opt, threads, ops);	\
		}	\
		stream << endl;	\
	}	\
}

#define testqt(QType, TType, stream, opt, threads, ops) {	\
	long time;	\
	QType *queue = new QType();	\
	TType t(queue, opt, threads, ops);	\
	time = t.starttest();	\
	stream << "\t" << time << ",";	\
	t.cleanup();	\
	delete queue;	\
}

void compareall(long elements, int threads, long opsround, long opsmpsc){

	long ops = opsround;

	testround(SpinQueue, elements, threads, ops, PRINT_ALL_ROUND);
	testround(MutexQueue, elements, threads, ops, PRINT_ALL_ROUND);
	testround(TLMutexQueue, elements, threads, ops, PRINT_ALL_ROUND);
	testround(TLSpinQueue , elements, threads, ops, PRINT_ALL_ROUND);
	testround(MSQueue , elements, threads, ops, PRINT_ALL_ROUND);
#ifdef WOSCH
	testround(WnbsQueue , elements, threads, ops, PRINT_ALL_ROUND);
#endif
#ifdef HASWELL
	testround(TransQueue , elements, threads, ops, PRINT_ALL_ROUND);
	testround(TLTransQueue , elements, threads, ops, PRINT_ALL_ROUND);
#endif


	ops = opsmpsc;

	testmpsc(SpinQueue, threads, ops, PRINT_ALL_MPSC);
	testmpsc(MutexQueue, threads, ops, PRINT_ALL_MPSC);
	testmpsc(TLMutexQueue, threads, ops, PRINT_ALL_MPSC);
	testmpsc(TLSpinQueue , threads, ops, PRINT_ALL_MPSC);
	testmpsc(MSQueue , threads, ops, PRINT_ALL_MPSC);
	testmpsc(MPSCQueue, threads, ops, PRINT_ALL_MPSC);
#ifdef WOSCH
	testmpsc(WnbsQueue,  threads, ops, PRINT_ALL_MPSC);
#endif
#ifdef HASWELL
	testmpsc(TransQueue, threads, ops, PRINT_ALL_MPSC);
	testmpsc(TLTransQueue , threads, ops, PRINT_ALL_MPSC);
#endif


}


#define morethreads(Type, end, opsround, opsmpsc, quotient) {	\
	cout << "test round mit " << #Type << " beginnt. \tOperationen: " << opsround << "\tElemente: #Threads/" << quotient << ".\tSteigende Threadzahl von 1 bis " << end << endl;	\
	for(int threads = 1; threads<=end; threads++){	\
		testround(Type, threads/quotient, threads, opsround, PRINT_NOTHING);	\
	}	\
	cout << "test mpsc mit " << #Type << " beginnt. \tOperationen: " << opsmpsc <<  ".\tSteigende Threadzahl von 1 bis " << end << endl;	\
	for(int threads = 1; threads<=end; threads++){	\
		testmpsc(Type, threads, opsmpsc, PRINT_NOTHING);	\
	}	\
}


int main(){
	
	testqtv(MSQueue, Test_mpmc, cout, threads, 10, 1, 1, 1<<20);
//	testqt(MSQueue, Test_mpmc, cout, 1, 1, 1<<20);
//	compareall(4,6,1<<20,1<<20);
//	morethreads(MSQueue, 20, 1<<25, 1<<20, 1);
//	morethreads(TLSpinQueue, 20, 1<<25, 1<<20, 1);
}
