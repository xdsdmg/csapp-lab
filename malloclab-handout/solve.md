# Malloc Lab

## References

- https://zhuanlan.zhihu.com/p/150100073
- https://zero4drift.github.io/posts/csapp-malloclab-jie-ti-si-lu-ji-lu/

## Layout

```
+---------+
|0       1|
+---------+
|2*DSIZE 1|
+---------+
|         |
+---------+
|         |
+---------+ 
|2*DSIZE 1|
+---------+
|5*DSIZE 1|
+---------+
|         |
+---------+
|         |
+---------+
|         |
+---------+
|         |
+---------+ <-- heap_lisp / mem_brk
|5*DSIZE 1|
+---------+
|DSIZE   1|
+---------+ 
|DSIZE   1|
+---------+
|0        |
+---------+ <-- mem_start_brk 
```

## Answer

- [mm.c](./mm.c)
