# An album of some C codes.

A number of experimental C codes in a variety of fields. Some math problems are taken from [here](https://mathworld.wolfram.com/UnsolvedProblems.html).

## Compiler call in Linux:
***

### gcc 
```
gcc -Wall -Wextra -pedantic -lcunit -lm -std=c11 file_name.c
```
### clang
```
clang -Wall -Wextra -pedantic -lcunit -lm -std=c11 file_name.c
```
### Executing output file in same directory:
```
./a.out
```
***

## Windows compiling:
### IDE
There are many integrated development environments (IDE) available for variety of operating systems. one bundled with a mingw compiler is called [Code::Blocks](https://www.codeblocks.org/downloads/binaries/).

***
## Notes on some C codes here:
***
### 196-algorithm.c
Some hints and stats for 196 problem. Also called "Lychrel numbers".

Output text explanation:

fd_ld: first/last digit equality

cl_cr: center_left/center_right digit eq

av: average of digits

odds: oddly occurred digits

***
### quick_sort_pem.c
Contains two seemingly rare ideas in quick sort.

***
### sudoku_search.c
A Sudoku searcher based on "constraint satisfaction" ideas.

***
### solitary_number.c
The quest to find out whether ten is alone or it has one friend at least.

***
And more C codes to investigate.

***
## External library:
This repository uses [CUnit](http://cunit.sourceforge.net/) testing library.

***
## Donate
Paypal donation e-mail is <ompp00@protonmail.com>


