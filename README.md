dynamic-ballooner
=================

The purpose of this tool is to constantly allocate n MB of main memory on a
linux machine.  I've used this tool to do I/O hard disk benchmarks and we
wanted to have a fixed amount of page cache pages around.

Dependencies
-------------------------
 * procps
 * Linux Operating system
 * make, gcc and alike

Usage
-------------------------
        ./allocator <Desired Amount Of Free Memory>


Building
-------------------------
	git clone https://github.com/rmetzger/dynamic-ballooner.git
	cd dynamic-ballooner
	make


License
-------------------------

The code is licenced with LGPL. http://www.gnu.org/copyleft/lesser.html

You can always ask me if you want to have it with a difference license.
