# file

`file` is a a simple implementation of a file handle in C. On my machine, it outperforms the standard `FILE` given by `stdio.h` when given a buffering mode. Otherwise, it is painfully slow.

### Should I use this?

There is absolutely no reason why you should ever use this. ;)

### Speed test results (see `main.c`)

Trial | `file` | `stdio.h`
----- | ------ | ---------
1     | 5ms    | 6ms
2     | 3ms    | 5ms
3     | 4ms    | 7ms
4     | 3ms    | 6ms
5     | 3ms    | 5ms
6     | 6ms    | 5ms
7     | 3ms    | 5ms
8     | 3ms    | 5ms
