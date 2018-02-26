memtrack
========

C library to detect memory leaks. After including it's header file
it overrides standard memory allocation functions with it's own
variants. Call memtrack_start() to start collecting allocation info.
At any given point you can dump information on allocated but unfreed
memory chunks.

This can be useful to check that between some code checkpoints all
data was freed.
