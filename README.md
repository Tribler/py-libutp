
# ptyhon wrapper for libutp


The only existing one is terrible and has no documentation. 

Documentation: http://www.tribler.org/py-libutp/

Dependencies:

* libutp (install with make install)
* build essentials (gcc, ld, gdb, etc) 
* doxygen
* graphviz
* valgrind
* massif-visualizer
* git


# building:

run 
```bash
make install
```
to install libutp on your system first (uses git)

and then run 
```bash
make lib
```
To compile the c library. After this is done you can import the python library as normal. 
