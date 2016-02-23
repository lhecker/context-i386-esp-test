# Stack Alignment Tests for Boost.Context

## Usage

This PoC currently only works on Linux i386 and x86_64 (`i386/sysv/elf` and `x86_64/sysv/elf` ABI) and requires GCC 5+.

    $ make

Your current working directoy will now contain the following files:

* `echosse`<br/>A reduced form of the echosse example in the Boost.Context repository.

* `esp`<br/>Reads and prints the `ESP` (i386) or `RSP` (x86_64) register for the main and the context function.

* `stack`<br/>The same as `esp`, but more stable by determining the alignment from the address of a local variable instead.
