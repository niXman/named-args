# Tiny-args
C++11 tiny implementation of the concept of named function arguments with zero runtime overhead

# Example
```cpp
#include "named-args.hpp"

#include <string>

/*************************************************************************************************/

// declaration of args-group with it's members
struct {
    NAMEDARGS_ARG(fname, std::string);
    NAMEDARGS_ARG(fsize, int);
    NAMEDARGS_ARG(fmode, char);
} static const args;

/*************************************************************************************************/

// fname - required
// fsize - required
// fmode - optional

// variadic packed as tuple
template<typename ...Args>
int process_file_0(Args && ...a) {
    // pack variadic-list into a tuple.
    auto tuple = std::make_tuple(std::forward<Args>(a)...);

    // get as required.
    auto fname = namedargs::get(args.fname, tuple);
    auto fsize = namedargs::get(args.fsize, tuple);

    // get as optional.
    // it the 'fmode' was not passed to the function, then 'r' will be used for 'fmode'.
    auto fmode = namedargs::get(args.fmode, args.fmode = 'r', tuple);

    assert(fmode == k_fmode);
    assert(fname == k_fname);
    assert(fsize == k_fsize);

    return fmode;
}

// fname - required
// fsize - required
// fmode - optional

// variadic NOT packed as tuple
template<typename ...Args>
int process_file_1(Args && ...a) {
    // get as required.
    auto fname = namedargs::get(args.fname, std::forward<Args>(a)...);
    auto fsize = namedargs::get(args.fsize, std::forward<Args>(a)...);

    // get as optional.
    // it the 'fmode' was not passed to the function, then 'r' will be used for 'fmode'.
    auto fmode = namedargs::get(args.fmode, args.fmode = 'r', std::forward<Args>(a)...);

    assert(fmode == k_fmode);
    assert(fname == k_fname);
    assert(fsize == k_fsize);

    return fmode;
}

/*************************************************************************************************/

// usage
int main(int, char **argv) {
    // the order of the specified arguments doesn't matter!
    int r = process_file_0(
         args.fname = argv[1]
        ,args.fsize = atoi(argv[2])
        ,args.fmode = argv[3][0]
    );
    assert(r == 'r');

    r = process_file_1(
         args.fname = argv[1]
        ,args.fsize = atoi(argv[2])
        ,args.fmode = argv[3][0]
    );
    assert(r == 'r');

    return r;
}
/*************************************************************************************************/
```

# Overhead
For this code sample:
```cpp
struct {
    NAMEDARGS_ARG(fname, const char *);
    NAMEDARGS_ARG(fsize, int);
} const args;

template<typename ...Args>
int func(Args && ...a) noexcept {
    auto *ptr = namedargs::get(args.fname, std::forward<Args>(a)...);
    auto len  = namedargs::get(args.fsize, std::forward<Args>(a)...);
    return ptr[len];
}

int main(int argc, char **argv) {
    return func(args.fname = argv[0], args.fsize = argc);
}

```
the following ASM will be generated:
```asm
main:                                   # @main
        mov     rax, qword ptr [rsi]
        movsxd  rcx, edi
        movsx   eax, byte ptr [rax + rcx]
        ret
```
As you can see, there is nothing superfluous here!
