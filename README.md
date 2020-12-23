# file

`file` is a a simple implementation of a file handle in C. On my machine, it outperforms the standard `FILE` given by `stdio.h` when given a buffering mode. Otherwise, it is painfully slow.

### Should I use this?

There is absolutely no reason why you should ever use this. ;) 

If you for some reason want to use this, there are two files you need (found in [this directory](/file/)): `file.h` and `file.c`. You `#include "file.h"` in programs that require the use of this implementation. Simply compile `file.c` alongside your program.

For information on use, see [the wiki](https://github.com/euppal/file/wiki).

### Licensing thingies

This is licensed under the GPL v3 or *any later version*. You can upgrade the licensing if I have not done so already to a further GPL, but you cannot embed this program in proprietary libraries or software. Consider using proprietary libraries for that use case.

### Speed test results for 1MB of File I/O (see [`main.c`](/file/main.c))

Trial | `file` | `stdio.h`
----- | ------ | ---------
1     | 146ms  | 630ms
2     | 145ms  | 774ms
3     | 156ms  | 788ms
4     | 134ms  | 654ms
5     | 171ms  | 654ms
6     | 117ms  | 634ms
7     | 155ms  | 573ms
8     | 149ms  | 604ms

Complete testing can be found [here](/file/speed.md).
