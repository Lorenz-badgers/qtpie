
#include <iostream>
#include <fstream>
#include <climits>

#include "Chain.h"
#include "tests.h"
#include "queues.h"



//#define HASWELL
#define CORES 8
//#define USEAVG
	//use avareage result instead of minimum result

#ifdef HASWELL
#define HASWELLQUEUES		\
	code(TransQueue);	\
	code(TLTransQueue);	
#else
#define HASWELLQUEUES
#endif				

#ifdef USEAVG
#define avg(retavg, n, fun, args...) {	\
	int i=n;			\
	long retavg2;			\
	retavg =0;			\
	while(i-->0){			\
		fun(retavg2, args);	\
		retavg += retavg2;	\
	}				\
	retavg /=n;			\
}
#else
#define avg(retavg, n, fun, args...) {	\
	int i=n;			\
	long retavg2;			\
	retavg = LONG_MAX;		\
	while(i-->0){			\
		fun(retavg2, args);	\
		if(retavg2 < retavg)	\
		retavg = retavg2;	\
	}				\
}
#endif

#define singletest(retsingle, QType, TType, args...) {	\
	QType *queue = new QType;			\
	TType t(queue, args);				\
	retsingle = t.starttest();			\
	t.cleanup();					\
	delete queue;					\
}

#define iterqueues(code) {	\
	code(SpinQueue);	\
	code(TLSpinQueue);	\
	code(MutexQueue);	\
	code(TLMutexQueue);	\
	code(MSQueue);		\
	HASWELLQUEUES		\
}

#define iterqueuesMPSC(code) {	\
	iterqueues(code)	\
	code(MPSCQueue);	\
}

#define testandprintround(TQueue) {					\
		long ret=0;					\
		avg(ret, iterations, singletest, TQueue, Test_round, (threads*percent)/100, threads, ops);\
		stream << ret << "\t";				\
}

#define testandprintmpsc(TQueue) {					\
		long ret=0;					\
		avg(ret, iterations, singletest, TQueue, Test_mpmc, threads, 1, ops);\
		stream << ret << "\t";				\
}

#define testandprintmpmc(TQueue) {					\
		long ret=0;					\
		avg(ret, iterations, singletest, TQueue, Test_mpmc, threads, threads, ops);\
		stream << ret << "\t";				\
}

#define printname(Object) stream << #Object << "\t" 


using namespace std;


void roundall(long percent, long ops, int iterations, ofstream &stream){
	stream << "#Testround,\tInhalt = Threads * " << percent << "%\t Operationen: " << ops << endl;
	stream << "#t\t";
	iterqueues(printname);
	stream << endl;
	for (int threads = 1; threads < 2*CORES; threads++){	
		stream << threads << "\t";			
		iterqueues(testandprintround);
		stream << endl;				
	}
}

void mpscall(long ops, int iterations, ofstream &stream){
	stream << "#Testmpsc,\t Operationen: " << ops << endl;
	stream << "#t\t";
	iterqueuesMPSC(printname);
	stream << endl;
	for (int threads = 1; threads < 2*CORES; threads++){	
		stream << threads << "\t";			
		iterqueuesMPSC(testandprintmpsc);
		stream << endl;				
	}
}

void mpmcall(long ops, int iterations, ofstream &stream){
	stream << "#Testmpmc,\t Operationen: " << ops << endl;
	stream << "#t\t";
	iterqueues(printname);
	stream << endl;
	for (int threads = 1; threads < 2*CORES; threads++){	
		stream << threads << "\t";			
		iterqueues(testandprintmpmc);
		stream << endl;				
	}
}

int main(){
	/*usecase 1:
	avg(retval, n, fun, args...) 
	n = iterations
	fun = singletest(retval, QType, TType, args...) 	
	TType(args) = Test_round(inhalt, threads, ops)
	TType(args) = Test_mpmc(producers, consumers, ops)
	*/

/* example:
	long ret=0;
	avg(ret, ITERATIONS, singletest, SpinQueue, Test_round, 20, 8, 1<<20);
	cout << ret << "\t";
	cout endl;
*/

	/*usecase 2:
	roundall(percent, ops, iterations, ostream);	
	mpscall(ops, iterations, ostream);	
	mpmcall(ops, iterations, ostream);	
	*/
	
	ofstream roundstream;
	ofstream mpscstream;
	ofstream mpmcstream;

	roundstream.open("./data/roundall");
	roundall(100, 1<<10, 10, roundstream);
	roundstream.close();
	mpscstream.open("./data/mpscstream");
	mpscall(1<<10, 10, mpscstream);
	mpscstream.close();
	mpmcstream.open("./data/mpmcstream");
	mpmcall(1<<10, 10, mpmcstream);
	mpmcstream.close();
}
