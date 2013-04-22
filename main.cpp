#include <iostream>
#include <stdlib.h>
#include <proc/sysinfo.h>
#include <string.h>
#include <deque>
#include <queue>
#include <unistd.h>

/**
*	Author: Robert Metzger (metzgerr@web.de)
*
* The purpose of this tool is to constantly allocate n MB of main memory
* on a linux machine. 
* I've used this tool to do I/O hard disk benchmarks and we wanted to have
* a fixed amount of page cache pages around.
*
*/

using namespace std;

struct mem {
	int mb_cached;
	int mb_buffers;
	int mb_free;
};

mem m_info;

// a chunk of allocated memory
struct chunk{
	void* ptr;
	int size;
};

void update_m_info() {
	meminfo();
	m_info.mb_cached = kb_main_cached/1024;
	m_info.mb_buffers = kb_main_buffers/1024;
	m_info.mb_free = kb_main_free/1024;
	//cout << "Cached: " << m_info.mb_cached << " Buffers: " << m_info.mb_buffers
	//	<< " Free: " << m_info.mb_free << "\r";
}


int main(int argc, char* argv[])
{
	// info from /proc/meminfo
	m_info.mb_cached = 0;
	m_info.mb_buffers = 0;
	m_info.mb_free = 0;
	// the amount of mem the user wants for the cache.
	int desiredCacheSize = -1;

	deque<chunk>*  allocated_memory = new deque<chunk>;

	if(argc <= 1) {
		cout << "Usage: ballooner <desiredCacheSizeMegaBytes>" << endl;
	} else {
		desiredCacheSize = strtol(argv[1], NULL, 10);
		if(desiredCacheSize <= 0) {
			cout << "Values equal or lower to zero are not supported";
			return 1;
		}

		update_m_info();

		while(true) {
			if(m_info.mb_free+m_info.mb_cached > desiredCacheSize) {
				// we need to allocate memory
				int alloc_mb = ( (m_info.mb_cached+m_info.mb_free)-desiredCacheSize) /2; // edit this param for finer alloc granularity
				if(alloc_mb <= 2) {
					usleep(5000); // sleep for 0.005 seconds
					goto endofLoop;
				}
				long amount = (long)alloc_mb*1024*1024;
				cout << "Allocating " << alloc_mb << " MB " << endl;
				void * buffer =  malloc ( amount );
				memset(buffer, 1, amount);
				chunk newAll;
				newAll.ptr = buffer;
				newAll.size = alloc_mb;
				allocated_memory->push_front(newAll);
			} else {
				// free up again.
				if(allocated_memory->size() == 0) {
					cout << "No memory allocated!" << endl;
					goto endofLoop;
				} 
				chunk somemem = allocated_memory->front();
				allocated_memory->pop_front();
				if(somemem.ptr != NULL) {
					cout << "Freeing " << somemem.size << "MB" << endl;
					free(somemem.ptr);
				} else {
					cout << "Can not free more memory! Exiting...";
					return 0;
				}
			}
			
			endofLoop:
			update_m_info();
		}
	}
}