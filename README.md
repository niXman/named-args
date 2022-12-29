# Tiny-args
One more tiny implementation of the concept of named function arguments for C++11 with zero runtime overhead

# Example
```cpp
#include "tiny-args.hpp"

#include <string>

/*************************************************************************************************/

// declaration of args-group with it's members
struct {
    TINY_ARGS_ARGUMENT(fname, std::string);
    TINY_ARGS_ARGUMENT(fsize, int);
    TINY_ARGS_ARGUMENT(fmode, char);
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
    // if the 'fmode' was not passed to this function - it will leads to compile-time error!
    auto fname = tinyargs::get(args.fname, tuple);
    auto fsize = tinyargs::get(args.fsize, tuple);

    // get as optional.
    // it the option was not passed to the function, then 'r' will be used for 'fmode'.
    auto fmode = tinyargs::get(args.fmode, args.fmode = 'r', tuple);

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
    // if the 'fmode' was not passed to this function - it will leads to compile-time error!
    auto fname = tinyargs::get(args.fname, std::forward<Args>(a)...);
    auto fsize = tinyargs::get(args.fsize, std::forward<Args>(a)...);

    // get as optional.
    // it the option was not passed to the function, then 'r' will be used for 'fmode'.
    auto fmode = tinyargs::get(args.fmode, args.fmode = 'r', std::forward<Args>(a)...);

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
    TINYARGS_ARGUMENT(fname, const char *);
    TINYARGS_ARGUMENT(fsize, int);
} const args;

template<typename ...Args>
int func(Args && ...a) noexcept {
    auto *ptr = tinyargs::get(args.fname, std::forward<Args>(a)...);
    auto len  = tinyargs::get(args.fsize, std::forward<Args>(a)...);
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
