# Results of running `main.c` with different buffering modes. 

`FILE_NO_BUFFERING` is very slow, and `stdio.h` beats `file.h` by such an impressive margin. However, with a buffering mode the results tilt more to the `file.h` side.

My iMac
-------
Processor = 3.4 GHz Quad-Core Intel Core i5
Memory = 16 GB 2400 MHz DDR4

512-byte buffer:

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

Optimally-sized buffer:

Trial | `file` | `stdio.h`
----- | ------ | ---------
1     | 57ms   | 678ms
2     | 49ms   | 687ms
3     | 61ms   | 644ms
4     | 65ms   | 656ms
5     | 56ms   | 710ms
6     | 55ms   | 668ms
7     | 50ms   | 624ms
8     | 59ms   | 645ms

[fennecdjay](https://github.com/fennecdjay)'s computer
---------------------
Processor = Intel i5
Memory = 16GB RAM

512-byte buffer:

Trial | `file` | `stdio.h`
----- | ------ | ---------
1     | 302ms   | 373ms
2     | 129ms   | 154ms
3     | 82ms    | 137ms
4     | 80ms    | 138ms
5     | 82ms    | 138ms

Optimally-sized buffer:

Trial | `file` | `stdio.h`
----- | ------ | ---------
1     | 283ms  | 399ms
2     | 100ms  | 165ms
3     | 139ms  | 167ms
4     | 69ms   | 143ms
5     | 72ms   | 148ms
