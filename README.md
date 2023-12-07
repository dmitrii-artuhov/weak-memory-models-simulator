# Weak memory models simulator

## Description

This project implements a couple interpreters for a simple programming language that are capable of running under different memory models.

## Programming language

It has the following instructions set:

- `r = 1` put constant into register.
- `r1 = r2 # r3` binary operation on two registers.
- `if r goto L` conditional jump on label `L`.
- `load m #x r` load value from memory by address stored in location `x` into register `r`.
- `store m #x r` store value from register `r` into memory by location `x`.
- `r1 = cas m #x r2 r3` compare-and-swap value in memory by location named `x`, expected value is stored in `r2`, desired value is stored in `r3`, shoud return the actually read value in register `r1`.
- `r1 = fai m #x r2` fetch-and-increment value in memory by location `x`, the value to increment by is stored in `r2`, should return the read value prior increment in register `r1`.
- `fence m` memory fence instruction.
- `thread_goto L` create a new thread with start instruction at label `L`.

Notes on instructions:

- all values are assumed to be (fixed-width) integers.
- binary operation `#` can be one of the following: `+`, `-`, `*`, `/`.
- each instruction can be prefixed with label, e.g. `L: r = 1`.
- `m` denotes access mode: `SEQ_CST`, `REL`, `ACQ`, `REL_ACQ`, `RLX`.

## Weak memory models

Program supports 4 memory models:
- **SC** (sequencially consistent, [more info](https://www.cs.utexas.edu/~bornholt/post/memory-models.html))
- **TSO** (total store ordering, [more info](https://www.cs.utexas.edu/~bornholt/post/memory-models.html#:~:text=A%20popular%20memory%20model%20that,latency%2C%20making%20execution%20significantly%20faster.))
- **PSO** (partial store ordering, [more info](https://link.springer.com/article/10.1007/s11036-022-01989-5))
- **Strong RA** (strong release acquire, [more info](https://plv.mpi-sws.org/sra/paper.pdf)): *does not yet support fences*.

## Interpreters

Program supports 4 modes of executions:
- **Non-deterministic**: chooses one random execution on each run.
- **Tracing**: same as non-determinstic, but it logs the intermediate results and thread interleavings.
- **Model-checking**: enumerates all possible executions of a given program.
- **Interactive**: allows user to make a decision on what execution to explore at each choice point.


## Building

**Requirements**:
- `C++17` or higher
- `CMake v3.15` or higher

**Local setup**:

- `git clone https://github.com/dmitrii-artuhov/weak-memory-models-simulator.git && cd ./weak-memory-models-simulator`
- `mkdir build && cd build`
- `cmake .. && make`

## Running
After building the project there will be 2 targets generated: `tests` (*for now only has tests on lexer, I will cover the rest later*) and `simulator`.

To run the `simulator` you have to pass some arguments:
- `--file`: path to file containing program code.
- `--mm`: memory model, one of `sc` (default), `tso`, `pso`, `sra`.
- `--it`: execution mode, one of `nd` (non-deterministic, default), `tr` (tracing), `mc` (model-checking), `i` (interactive).


## Examples

1. **Store buffering**:
    ```js
    thread_goto T1 // create new thread with start instruction at location `T1`
    thread_goto T2 // same for `T2`
    goto END

    T1: r = 1
    store RLX #x r // write 1 to `x`
    load RLX #y a // read from `y`
    goto END

    T2: r = 1
    store RLX #y r // write 1 to `y`
    load RLX #x b // read from `x`
    goto END

    END: // just an empty label
    ```

    Running program with TSO memory model in model-checking mode:
    ```txt
    > ./simulator --file ../tests/test-data/store-buffering.txt --mm tso --it mc

    =========== Memory state ===========
    Memory: // final state of memory
    x: 1
    y: 1
    Store buffers: 
    [empty]
    ====================================

    =========== Thread states ==========
    Thread 0 // this is a main thread (that created the rest)
    [empty]

    Thread 1
    a: 0 // value read in register `a`
    r: 1

    Thread 2
    b: 0 // value read in register `b`
    r: 1

    ====================================

    =========== Memory state ===========
    Memory:
    y: 1
    x: 1
    Store buffers: 
    [empty]
    ====================================

    =========== Thread states ==========
    Thread 0
    [empty]

    Thread 1
    a: 0
    r: 1

    Thread 2
    b: 1
    r: 1

    ====================================

    =========== Memory state ===========
    Memory:
    x: 1
    y: 1
    Store buffers: 
    [empty]
    ====================================

    =========== Thread states ==========
    Thread 0
    [empty]

    Thread 1
    a: 1
    r: 1

    Thread 2
    b: 0
    r: 1

    ====================================

    =========== Memory state ===========
    Memory:
    y: 1
    x: 1
    Store buffers: 
    [empty]
    ====================================

    =========== Thread states ==========
    Thread 0
    [empty]

    Thread 1
    a: 1
    r: 1

    Thread 2
    b: 1
    r: 1

    ====================================
    Total states generated: 4297 // number of program states explored
    Final states count (unique in terms of thread subsystems states): 4
    ```

    Above we can see 4 different outcomes of the store-buffering example under TSO. Model checking mode explores all *interesting* executions of a program in order to cover all possible outcomes (by outcomes I mean all final unique thread states).

2. **Message passing**:
    ```js
    thread_goto T1
    thread_goto T2
    goto END

    T1: r = 1
    store RLX #x r
    load RLX #y a
    goto END

    T2: r = 1
    store RLX #y r
    load RLX #x b
    goto END

    END:
    ```

    Running program with Strong RA memory model in model-checking mode:
    ```txt
    > ./simulator --file ../tests/test-data/message-passing.txt --mm sra --it mc

    =========== Memory state ===========
    Memory:
    <RLX; x: 1 @ 1>
    <RLX; y: 1 @ 1>
    Global timestamps:
    x @ 1
    y @ 1
    Thread views:
    Thread 0: 
    Thread 1: 
    x @ 1
    y @ 1
    Thread 2: 

    ====================================

    =========== Thread states ==========
    Thread 0
    [empty]

    Thread 1
    r: 1

    Thread 2
    a: 0
    b: 0

    ====================================

    =========== Memory state ===========
    Memory:
    <RLX; x: 1 @ 1>
    <RLX; y: 1 @ 1>
    Global timestamps:
    x @ 1
    y @ 1
    Thread views:
    Thread 0: 
    Thread 1: 
    x @ 1
    y @ 1
    Thread 2: 
    x @ 1

    ====================================

    =========== Thread states ==========
    Thread 0
    [empty]

    Thread 1
    r: 1

    Thread 2
    a: 0
    b: 1

    ====================================

    =========== Memory state ===========
    Memory:
    <RLX; x: 1 @ 1>
    <RLX; y: 1 @ 1>
    Global timestamps:
    x @ 1
    y @ 1
    Thread views:
    Thread 0: 
    Thread 1: 
    x @ 1
    y @ 1
    Thread 2: 
    y @ 1

    ====================================

    =========== Thread states ==========
    Thread 0
    [empty]

    Thread 1
    r: 1

    Thread 2
    a: 1
    b: 0

    ====================================

    =========== Memory state ===========
    Memory:
    <RLX; x: 1 @ 1>
    <RLX; y: 1 @ 1>
    Global timestamps:
    x @ 1
    y @ 1
    Thread views:
    Thread 0: 
    Thread 1: 
    x @ 1
    y @ 1
    Thread 2: 
    x @ 1
    y @ 1

    ====================================

    =========== Thread states ==========
    Thread 0
    [empty]

    Thread 1
    r: 1

    Thread 2
    a: 1
    b: 1

    ====================================
    Total states generated: 2716
    Final states count (unique in terms of thread subsystems states): 4    
    ```

    Let's modify the code by adding release memory order for all writes and acquire for all reads:
    ```js
    thread_goto T1
    thread_goto T2
    goto END

    T1: r = 1
    store REL #x r
    store REL #y r
    goto END

    T2: load ACQ #y a
    load ACQ #x b
    goto END

    END:
    ```

    And run again with the same arguments:
    ```txt
    ...
    Total states generated: 2642
    Final states count (unique in terms of thread subsystems states): 3
    ```

    I did not put the whole output, but we see that one outcome became impossible. To be certain `{ a: 1, b: 0 }` became impossible (and it is expected result).

