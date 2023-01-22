# Named arguments
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
the following ASM will be (generated)[https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAMzwBtMA7AQwFtMQByARg9KtQYEAysib0QXACx8BBAKoBnTAAUAHpwAMvAFYTStJg1DIApACYAQuYukl9ZATwDKjdAGFUtAK4sGe1wAyeAyYAHI%2BAEaYxCAAzAAc0gAOqAqETgwe3r56KWmOAkEh4SxRMQnSdpgOGUIETMQEWT5%2BXLaY9gUMdQ0ERWGR0XGJtvWNzTltCmN9wQOlQxUAlLaoXsTI7BwmGgCCAPT7ANQAtGfnF5dX1ze3d/d3OwfHALIAkgAqR0GbDEpPhwBxw8SQAnsQ8MAEAQjhBkEsjmYNGYzCckWZYkcGHgABosQyw4CEBBeCJHdCoGHY1T4hhHJgwpIscmUo5sJZAo4fBB4BRHGj0I68o5JXpHVBULnBUG7YjABTQYmkgB0aBY%2B2xeMMGtYmHQJwa8oRSWIqG01QIyqBnOU0RYvLSAiFfIQ0UwEVBR2AxEMBD1pH5xEwmHFkuQCENmADRHpDE9SWiCidqAi9WCwWA9KOaDBnIlRwIPL5SaoBAA7g0Q4Z0PSFEnkHgGXqWcgfIx6l1%2BXRMHyIIWQ%2BYzEIJeXK4OVgXUOTMGJOcEC66jsPSxWg0cy8S1jCg9MITUBAHgshvPgjOvN14YbQ8PaOxkFyGIVCCApOTGvEoAznQQGWBg8FQP5stEwBRiKpLXgoCABvgu54BEl5gQoEF4L8n6xug%2BwkEcdi0JyOZ4D2oYPkuI6rmB1aTiKdqENRxBJn8VFlggqDMv2pErpWzqclQ6zYlBzYxhSOGoAGyEROaDhUex/C0LQqAbmeaAMPgXQKCA1p7IcXKLkwESoAAbiG35PtCWKUqhVYqQuwoJsQ9p1veDAWZsOERnJRxREKDDHl4WA1vOYh4VpxwEUR2HidMvqNrQIokGpxHscuo5BlaIWch8AAS2BLgA8gAYh8ADquwAEo5W8QhHMopW5QAam8AAi2CNUcg67FVlWDgGRWfJluVyF8JWlaVuyhB8ACaRwFUcY1TQA0m8oSNQG2A4jV2BCFVuWlZybwvMoARvC1AZLW4ARyI1S0AOJHBYg1HKEuVfEd7wfC1XK5Vy2VHMNo3jcd235UcLzYKVbiZWNHy7BYbxHZNpCcvlnyhJtVX5Tts3VWVHxvG4cgBGV1VyKVyi5UIOVja1T2hEt%2BWlTd2Cg%2BNypHEtj1fdgdXYONS6QwEATfdgnK7IN/WldtpVHG4uXKBNDPXZlXz9QEzUS3dOVHTDAQ5RjUtzdLhP7QGjW7C8uzXZt01S892W7SF3xvDDcOfBNPXZVlYNs6Es0%2B7sbi47lPszTL42jQHAYfDtXyY7bYO9RTAZlZVN1HPTuUvIjDsDTHwOY%2BzoeowHbxB79fVC3lhXDTlmOe0ccgU9b02e1LzW7EdoTXZ1Pue5yQgFcVZXYGlzynA848T5PU%2BXE8s8YgBKmYJKAD6y%2BhGbLXL2VXer%2BvoONVvpU75lyjKMvZ0Xc1jVz7EWA0CERy7xvB/b0IT/74fx%2Bn%2BfoTnZdLWzz2OYWIR4TwDliG4AgoIEzLwID6Qgr5YjCyAfPHyYC2oQIIF4JI9ATBIMAbsZgbAFCilckQvUho%2BQmAAOxWD2OQkhTBXJYDTLQKhtCCH7AAFQ8N4Xw/hAjBFCOESI0RYjxECP2G%2BVAnhXxAL2H6FgOCmx4LcPpTw015rRmgYwXUXI8HIN2LuLwUkgwAEcvB4CDOgZeQYCDLxYrQLAxA2ocL2EcDxOE7zIGXkwOs0QCAQFylotqKJnJHHKgARTkG8cqrVDRtkEGAMA448F0N2J4o4H4MwFh0Rg1qHw0mz2odfWI6SniKOUX6VR6jYrBO0Qmch%2Bj8F7GMaYpeH49S2MwPYxxzjXHpMyVFRwPi/FKEaEEkJg5%2BxrmFEwCJ2B8oNw%2BgktgSSUkog5GUp4mTslnigQmfJzTyklKKfI3YlSDDVMwToppHxI5HAMgYp4bSYTBHscpaYAydmeOGahbMAhpiYFUCaLkjyxBeHAa1J52ygGnNhbsCpmAlFXMwKog5ui2BHGVDij4OjEGGNebkhMChl6QQIGcxFCjkVVLRTcxpejCktKMXAkxbysCCEIKCWBeSaGDM8XszMGKjlMpOaU8p1KUUqPpZikMOLlSynlM81prKpK8lgdg3B7jPEgGmOgEAIAqBiCUDyhMxSrDwolRcmlqL0W3N1PK2acoCUvNVW80lWCcF0rcHqg1nrcEQMVQoeVzzmWZN1QQfVIBWWYFNWiuFFrxUEMudKyB9qsWKo0A02Vjqg3KpZcQNlzoNVetUZmpOzqQ1hp1b6w1xrY0YvNTQpN5yU3XLTQyrFpUenZqafKplhK3VHF0REegy8AIlq1RkmtkaDUjrHQBVR6r/Xeo%2BFW7ABqDIQqhXg1qMaAzdopcyvlzbKVIqle2jFTTD29r0f2/NRLYJ6QXVQSd8bp0eIjVG%2BdsbF0QOXZq1d67N3bvybu/k9aD09PzSey1nCJEIcQ0h5DSGpEOzyOkAQyabWpqvYyw8gg2a3qxfKvNzKiUYa6OOlFZ7JW0rtZ2kM9zvIwjeA%2BodlGMjUZwao5jbHq2ftre8zA3oxDL0%2BfUQQS7BABhOFwGDHC4Otpw5e9NTGCOseIyGctxLZXYpxWRwdhapKcYENxvCEC%2BMVuAFmp18p10/ME7OkAwnRO0HE4CyTR63COcyR495vnMmjCjeqhQupePWY0AYkD3h31%2Bfixg4GbHtUJZ1XFfIXGbw8cs6dawbQ7PBpxdFkAW7Yu%2BYU4m2j1qL3erw1i5jdW5UGedex4zjJUiYYYFVtttW1Nci0/phVLXyMcY610XzIBTMMHMwxvTtasCiFBLx4rGKAy2aDQ5hNp6EXnvozKu5A3SPDaM0WqbEWvFRpXWWytRX8ECaOJNsbmWaP7b0fN6oTAluWZWzotb1nCtWmPYpltVKDgofBxDyHCG0OjxXcO%2BgayKXKZq6o2taQABeDaiO6b7c1pVI22sFkA8vDoyL2zdZU96tHeBMewOx41rktmGdHfxydqSK6ScI/JxAt4kd1s3cBzlE9KWslpH2by2Ie6%2Bu1uUqpDIs4IGBcyW8MDkujhRZF0FvAHLHBQLjbxqL92gsc9J4j1Rbw5P/c2x%2BjBG7o06L9Tos9SnQc9dR85jHWPec47vXjl1KrCcm651JnnYlnNXcDQL0Nhjw1B7JyHtw3uNu3cMbBkHAIoeZ6z1nmH2kWBeFoI4E4yk0x/GwyjiBtTNEDY%2BEzvrLP/cFqLfnwveAPOCCYMEUlWWLNWrd69%2BrdfGODbxSSw7fvWvN4L44dvpfu8vbcEathYEMWkvJQbyO%2BL11W5TxNtk0%2B28l8738GbivNeZLO5Z/n9mU8xchUcZJ4G5NK48aQVfZLeTedHz2a3qXSDJ5HkyR%2BzNS2xd121tQHyayG1Z1dUJxbxnyPy71P0X0g10zX0/1UQMR30Fz31rSXxNUbVAPTzowgI7T037XxXH2gMbyJXgMP1kGP3n2y0gULRX3xQ/2mF4y3xT2wPzXDQuz9VYP1yIIpwrzIIOx9xIwnwJyn1b1n0YNEL23EMZSH3INxUoMkKgMM1gNkIQIYK71m3QM4Kv03xJW3wK1/zSzoPkKQJ70wPPwvyeywxMIsNvxK1A0fzV2fwcKODf3YPXxcO/wBz4IS3/yjxF2APfTTyq24WzziPiOh05FAkCQRH4BcSu2RyUKr1QAAGtJ8pJkjfE5RzM31FDSD8iYRCjDQSiI8WDIUDFvkRd%2B8fMfDNCjh5oX9fCGcPhANOjgsDVPc6dksbcQiPE/lkA2j5pHdDlki4RPN2jQkAA2DTdvYwn1D3GnBtXLJBSOQDJY8wRYldBEYXG3TJOxPiAQkAZI83AxCAPAkgVcdAXjXopBPsQDJYJYZUGFflDxU9Joynew04zJBnDonwvw4fRqJePo7o3osE6nWnVjcre7cYyY6YkMWYiTBYg4k2JefYswZYljVY7zeErYtmLArkPYg4g4o4xooEjxQVI4bABHI5BnOPM3UPNo97RbZ40tJBNEyIylVLBkr4cDBnJk5FfkhFIUsXTMUQaYWBKcUU6XZzWXTrBXFouk%2BLITUlb8XxRyYAZgUdVdYrUre/GhHzahNwS49VXUsZSEQ0gNAOE07dTooLTEkU/E10zxUgD0xYqknwgUqUhLc44gOkcY8TPxbzOU%2BxIgW42ta4nnOM5zNIx4nkgNbAN4r1D4r4rZH41xa%2BOFYg6rJQior0HpIo4AGol4lA5fBok44EgEs/TUyY6EvrSEqgVs4fHo0tOEjYhEskiI5E7xVE%2B3GYnpOYv4GEeaJYnEyUKk/Eu%2BFYiTd3KNQYggbY7AXYr1PExY7hFdLhfYY4txTUkMsM5Mh4hoJ4yAo4dsyUjM%2B44gVMiBds24u%2BT4gyXM8rU5QsmIhIv8/8wRXPY4WY1I7CLdCEJgfAUwTI0g7IvImQgo8s6onvZecCxsKCso1NUsqo4olCtCyC1CdFVgus48hssQ10kEzsvTF8sEhnTNKi3HagpEmPX5YchnWtH9cdDsiBZJbU5eMLNgVREJeivk9w2LAbKY0culO3Qgm3DE%2BY6cg4rwBgTpdAWzdsnc5S1S/LTNHc7hQ0DQA8/7HcwbShI8vMs4npC42YrSpQNSsPKNFMy81RF8142yvULgCcB8p8twQzCAMy%2BVT8iI78m3ZoiivrUEzU8E6iqE2ivreiuK4fBvZi3zFE9i5zTiv9dYkLUlAS71YSuUQ3O3U0tg4fSSxtUS2SzJeSycrE/E7hdytSoym83E%2Bc3crhRqrgZq3Stqgy4ytqwbfS51A88yzo08y4py4gK83ywq24gy98wUzxP4xFMAkgrChCyopC3ClFVChodCwiiBfA6S2ksiks1oyixKmKjsy6xivNQclisYtitspeNEsswJTExS%2BqrhHI5qjStqxcgrHc%2BVIa%2BUEak64Mqy0Miai8qalyqE14t8r4xa34kK7bK1WIgCzGv8mHYpVqbSRiFhTvNheDLG0muIoC/kbCDI13RsjUvzC6m3aKu5WExmhnOQAOFk5UqNBbT7NM6S/koBZlBnQmugdSEATK19WokEtEgMdmgdAW3YGqr5T6xYnIrcwUNqmk%2BszxCTYFUFd5IUdAVQI5EWthA1S/NwEJOW50srIMx6hkVCPU8ZQJbXY2zw1qS3UJMwQsBkekOURJGEJQZFPkGMLyEITYOsBoUER/TZZGo4ca02sWnCyslC2o12h/FJLw%2BTSqynN6iATotW6cVhMW95IklcgYzY2BQ8I255YHByg1Sa6a7s9MzM%2BgD453Asmm8izXBm43Z6661mvrZuuLb0tmjmpU4fLk3myzF4mSp3QWwxYWnpImsWiW0o688q37euJ0yq%2BexW8cj6pYwuv6z0hcpedWgcU%2Bw4948GjxXWkFFxA29O8DRO82pwrrCBK2ne4ql0u2%2BOyGukV%2Bq4ralOnatOo2jOp/bOmS3O2Ygu%2Buw1GG6a1yjMt8mCZe0Wg1Uu5ciBEkquw21QWuyrVo707y5yme3kjMo4oKlazujPMmhhqHCmtI8FCCjCmC3DCKqg7QhRPrIBp9I0ri9e5QrFTehMXg3ekA/e96hSo%2B/qq%2B0y51UakXe%2B/WwjZ%2BtXIBi2gqm/QXO/OlPM8MsZAJCAdO9204fLaZCMGEVZdsHCYMFgEOqcMO6oHsMLYgaOjZMwGhyyrBKGoB5Oko/CjC/9CB8x5/HOmrPO%2BB9otPBBxu67VnfypRwKju8vJQkEgbds7h47CpPhjBs2kAARl9YR6WqSiRueqRpWqcuRlqucq%2BgGoNEy%2BVMy2%2BgFScvWx%2B9RiBl%2BgpsW7Rip/RuOhOvpg1QJvCvagi0wUJt2zOj26Bv1WB8cmJsh2G58%2BG1BpeLy88x88hmapJgKnFGh5a%2Bhxhs55DHGlBW%2BJeOYR6Z%2BV%2BZeZQIecabKIQSqZeF4BuD4ZeCwbAR5wmNwTeMqAaZaZeT2ZecqD4EmUIMFiaZQDMwKx%2BZeOqXYT%2BN%2BZeAU85YBO%2BW5veFqB5pZX%2BZeHmbWDMlFhmXYK6NwAMRFkwAAVjps8XSu/QdN/W4qtPpcZfiwNXIXQEoQNSAesMQLLwgR5d1D5edQFdGakqMO81XhRbRdXjrIZa9Nf3JadipZtrNMl1FNYLahVZIfi1IDxdNiPjfiefKhec2nec%2BaEG%2Bd%2Bf%2Bf9iBdqjkFBfBchehdhfhYIWxZuYfhNYJddbcGXiuiEFJYgHVcpbxhpaOf1a5c5IytZa4tUTjc6LFbYAlflCleLoNSFf0JFbcHTYoUlaKelffwCODeRdRYeYxaQVTcNd8Mjc1dEpKtV1aiOvrebNCIDbNceeec9jebfltftb%2BcOidZfhdbdeyghewChdKhhcmm9axYxBxf9fud7cJeDaiRiXKgjeTijepcG2OINdCr6yLczbFqAfMUsWsW6V6U8GcRTc5bTZAF5f5dLZzZADzY7wMNFdffFffaAfLYwIgXlerd7draFxPdSyCybbxi1Z3TVxjU7Zg%2B7fXZ3gtZ5gHZta%2BZ%2BdHYBedZBYPnddnc9cXdTyudXZDB7Z3k3ZnaWQpj3YpapZjeVGPfjYZ3PcA%2BlaDF4jsrvYcQfeiCfeg4Sy45LcFYPxsILfE6zY/ZXrRNldUTA8Vcg5Q9Q8bf3ebZ/tizbYg2X3U40%2B9Jo/Nf7deZw7tbw8dcBYnaI7BenY9fna9Yo7B3Obc8SOXeufvmo/Q7fm3ggHIWzWXnY9SqHSaWAWAUfhhD5UM78wZOFQntjTSVi%2BBLyW%2BJS6ZdprZuVfjcyXC4xEi7p1QDsgZBIHAwgDkE0oRExOfYDNoX/r8bpD5VWemrkFuK8A5EtXzNq5GJKXaa%2BXIWiJ2y0nc9G7EUudoaOHxr5DfZa089cAAim%2BOHfmdZ3jXmfkVZPjPgvn/joYDyLW1o8RM8/moEC/65hHDAaCOC4R8c8WO/86oE9w02Ob68xMoUwtUySukMMQNt4h8ggCaYGpaYRGcmBU2CSGi9Is8SYEvCnCoGYGdSOVrXxByIbUAzuJ2Z8r8qYE%2BKObjph5jC4Qh5cXA1m7k9mMoWVHh91ADGp6Ufx9h6OHoDpCOTJ6TvHMp8e82Np4R6NGGYAZFDgXpYsGZ/pfT1Rr2ANvxGCFMcI0NGQC/AjBcR4UNA/LafGr%2B%2BQGSfs2p6xXA1V%2BF6izpZWj9p189yOXl5e87o4BWFoE4Dpd4D8A4C0FIFQE4B80sGsBEnWFcmAR4Df00Bt5WByJADpazTt44EkEd8D9d84F4HUizQIED5WDgFgCQDVCSG7DIAoDmKUSz6uK4BRFICwAMksjqkIjLFykaU4H94FD9HokoAiBj4iGCCjpr94Bb4R9BFygkgtHb9IDVER1ygYFoFBBj6wFpGAHEGd94HwCDBqCMnUhn9IGBWqEQn7%2BEwj5d%2BvAiB9A8Y8CwBj7gRvHb5WCoAMHlHL8wEr%2Br%2B4F4H4EEBEDEHYCkBkEEEUBUHUGX90DaAMCMBQGsDWB9A8EdSJABWDFc1InAE4LlDMCnA9Uu6UwJ70sBcB/eVQfcH4AgCuAJgrQFfipH6AlAyguQd%2BtgKIEZZCgcwAgUMCmCk50BPQcYJ4BaB6A0BXQOgbMGKCDAYgUwGYCQK4G9B8BHAiQCsCTA%2B8X%2Btve3tH2X5u8OARwVQPEEWInBFikgL0MgAmKF9lQsAiALgEIDYQ/eSwXgEnxn4fFSAIfMPvoE4BR9SATvF3lIPj4gBE%2ByfMQRwDMASDrBcfAPoYJWBGR6IGQEAJICAA]:
```asm
main:                                   # @main
        mov     rax, qword ptr [rsi]
        movsxd  rcx, edi
        movsx   eax, byte ptr [rax + rcx]
        ret
```
As you can see, there is nothing superfluous here!
