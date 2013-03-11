#include <iostream>
#include <stdlib.h>
#include <proc/sysinfo.h>
#include <string.h>
#include <queue>
#include <unistd.h>


using namespace std;

struct mem {
	int mb_cached;
	int mb_buffers;
	int mb_free;
};

mem m_info;

void update_m_info() {
	meminfo();
	m_info.mb_cached = kb_main_cached/1024;
	m_info.mb_buffers = kb_main_buffers/1024;
	m_info.mb_free = kb_main_free/1024;
	cout << "Cached: " << m_info.mb_cached << " Buffers: " << m_info.mb_buffers
		<< " Free: " << m_info.mb_free << endl;
}

template <typename T>
void debug_queue(const queue<T>* q) {
	queue<T>* n = new queue<T>(*q);

	int len = q->size();
	cout << "Len=" << len << endl;
	for(int i = 0; i < len; ++i) {
		T f = n->front();
		cout << "Element " << i << " value " << f << endl;
		n->pop();
	}
}



// some learning
void devel() {
	queue<int>* q = new queue<int>;

	q->push(1);
	q->push(2);
	q->push(3);
	q->push(4);
	q->push(5);

	debug_queue(q);
	debug_queue(q);
}

int main(int argc, char* argv[])
{
	// devel();
	// return 0;

	// info from /proc/meminfo
	m_info.mb_cached = 0;
	m_info.mb_buffers = 0;
	m_info.mb_free = 0;
	// the amount of mem the user wants for the cache.
	int desiredCacheSize = -1;

	queue<void*>*  allocated_memory = new queue<void*>;


	if(argc <= 1) {
		cout << "Usage: ballooner <desiredCacheSizeMegaBytes>" << endl;
	} else {
		desiredCacheSize = strtol(argv[1], NULL, 10);
		if(desiredCacheSize <= 0) {
			cout << "Values equal or lower to zero are not supported";
			return 1;
		}
		cout << "desiredCacheSize="<<desiredCacheSize<< endl;

		update_m_info();
		int set = 0;
		while(true) {
			cout<< "Currently are " << m_info.mb_cached << "MB in the page cache" << endl;

			if(m_info.mb_cached > desiredCacheSize + m_info.mb_buffers) {
				// we need to allocate memory
				int alloc_mb = (m_info.mb_cached+m_info.mb_free)/2;
				if(alloc_mb > desiredCacheSize) {
					alloc_mb = alloc_mb-desiredCacheSize;
				}
				long amount = (long)alloc_mb*1024*1024;
				cout << "Allocating " << alloc_mb << " MB " << endl;
				void * buffer =  malloc ( amount );
				cout << "buffer?? " << buffer << endl;
				memset(buffer, set++, amount);
				allocated_memory->push(buffer);
				cout << "Debugging queue" << endl;
				debug_queue(allocated_memory);
			} else {
				// free up again.
				void* somemem = allocated_memory->back();
				if(somemem != NULL) {
					free(somemem);
				} else {
					cout << "Can not free more memory! Exiting...";
					return 0;
				}
			}
			sleep(4); // don't stress the kernel too much
			update_m_info();
		}
	}
}


