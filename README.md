# ds_list
Shared library for common linear, non-primitive data structures to be used in [dstruct-vis](https://github.com/Henchel-Santillan/dstruct-vis). 

## Toolchain
* Environment: `cygwin64 v3.2.0`
* CMake: `v3.19.2`
* Compiler: `gcc 10`
* Debugger: `gdb v9.2`
* CXX_Standard: `C++20`

## Features
* All classes are designed to be stl-compatible. This means C++ standard library algorithms can be applied to them.
* Typical operations can now be user-defined and can take advantage of class-specific iterators and stl algorithms, making wrapper classes for the visualizer easier to write. 


## Supported containers
### List-Types
* Array-based, random-access: `list` (fixed-size), 
* Link-based, sequential access: `slinked_list`, `dlinked_list`

Note that a majority of the `deque` types are simple adapter classes and can be developed by deriving and hiding a fragment of the interfaces defined by the `list` types. What this means is that they simply “wrap” one of the three public containers in the shared library. In particular, `linked_queue` and `linked_stack` implement a common `deque` interface and define `push`, `pop`, and `peek` by means of the methods contained in `dlinked_list`. In a similar vein, `array_queue` and `array_stack` take after `list`. 

For implementations of these adapters, see [dstruct_vis](https://github.com/Henchel-Santillan/dstruct-vis).

The adapters `queue` and `stack` are similar in functionality to `array_queue` and `array_stack`, except `queue` and `stack` are not inherently resizable (static capacity).

### Deque Types
* `linked_queue`, `queue`, `array_queue`
* `linked_stack`, `stack`, `array_stack`
* `array_deque`

## For A Future Release
* The adapter `array_deque` has numerous variations and is not (yet) covered.
* A `vector`-type (to be named `array_list`) is still under construction.
* Build instructions

## Testing (googletest)
`Status` Unfinished
