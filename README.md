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

// our function which we want to relax =)
template<typename ...Args>
int process_file(Args && ...a) {
    // pack variadic-list into a tuple.
    auto tuple = std::make_tuple(std::forward<Args>(a)...);

    // get as required.
    // if the 'fmode' was not passed to this function - it will leads to compile-time error!
    auto fname = tinyargs::get_arg(tuple, args.fname);
    auto fsize = tinyargs::get_arg(tuple, args.fsize);

    // get as optional.
    // it the option was not passed to the function, then 'r' will be used for 'fmode'.
    auto fmode = tinyargs::get_arg(tuple, args.fmode, args.fmode = 'r');

    return fmode;
}

/*************************************************************************************************/

// usage
int main(int, char **argv) {
    // the order of the specified arguments doesn't matter!
    int r = process_file(
         args.fname = argv[1]
        ,args.fsize = atoi(argv[2])
        ,args.fmode = argv[3][0]
    );

    return r;
}
/*************************************************************************************************/
```
