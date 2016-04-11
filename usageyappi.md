# Introduction #

**Using yappi and Interpreting the statistics**
```
 /// todo:
```

Note that there are several columns in the profiler stats. The first one is **function stats**:

  * **name**: name of the function being profiled
  * **#n**: is the total callcount of the function.
  * **tsub**:  total spent time in the function minus the total time spent in the other functions called from this function.
  * **ttot**: total time spent in the function.
  * **tavg**: is same as ttot/ccnt. Average total time.

the **thread stats** field gives information about the threads in the profiled application.

  * **name**: class name of the threading.thread object.
  * **tid**: thread identifier.
  * **fname**: name of the last executed function in this thread.
  * **ttot**: total time spent in this thread.
  * **scnt**: number of times the thread is scheduled.
