# Tiny-args
One more tiny implementation of the concept of named function arguments for C++11

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
    auto fname = tinyargs::get_arg(args.fname, tuple);
    auto fsize = tinyargs::get_arg(args.fsize, tuple);

    // get as optional.
    // it the option was not passed to the function, then 'r' will be used for 'fmode'.
    auto fmode = tinyargs::get_arg(args.fmode, args.fmode = 'r', tuple);

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
    auto fname = tinyargs::get_arg(args.fname, std::forward<Args>(a)...);
    auto fsize = tinyargs::get_arg(args.fsize, std::forward<Args>(a)...);

    // get as optional.
    // it the option was not passed to the function, then 'r' will be used for 'fmode'.
    auto fmode = tinyargs::get_arg(args.fmode, args.fmode = 'r', std::forward<Args>(a)...);

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
