# file

`file` is a a simple implementation of a file handle in C. On my machine, it outperforms the standard `FILE` given by `stdio.h` when given a buffering mode. Otherwise, it is painfully slow.

### Should I use this?

There is absolutely no reason why you should ever use this. ;)

### Speed test results for 1MB of File I/O (see `main.c`)

Trial | `file` | `stdio.h`
----- | ------ | ---------
1     | 146ms    | 630ms
2     | 145ms    | 774ms
3     | 156ms    | 788ms
4     | 134ms    | 654ms
5     | 171ms    | 654ms
6     | 117ms    | 634ms
7     | 155ms    | 573ms
8     | 149ms    | 604ms
