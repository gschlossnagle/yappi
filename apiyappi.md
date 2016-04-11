# Reference Manual #


---


**Latest version: 0.62**

<font face='Courier New'>
yappi.<b>start</b>(builtins=False)<br>
</font>

_Remarks:_

Starts profiling all threads in the current interpreter instance. This function can be called from any thread at any time.  Note that yappi is per-interpreter resource, you cannot run more than one instance per-interpreter. A relevant exception will be raised if you do so.
Resumes previous profiling data if stop() is called previously.(Note: you can clear the profiling stats via clear\_stats() between a successfull start()/stop() sequence.)

_Params:_

Function will take an optional parameter named **builtins**. As the name suggests, this means we also want to profile builtin functions used by standart Python modules. It is _False_ by _default_.


---


<font face='Courier New'>
yappi.<b>stop</b>()<br>
</font>

_Remarks:_

Stops the currently running yappi instance. Same profiling session might be resumed later by calling start().(unless clear\_stats() is called in that period.)


---


<font face='Courier New'>
yappi.<b>enum_func_stats</b>(fenum)<br>
</font>

_Remarks:_

Enumerates the profile stats one by one. The parameter _fenum_ should be a callable Python function with one parameter, say _stat\_entry_ which will be invoked for every profiled function. The parameter is a Python tuple instance consisting all of the information related to function profiling: ('function\_name', 'callcount', 'ttot', 'tsub')

_Example:_

```
>def foo(): pass    
>import yappi
>yappi.start()
>foo()
>def estat(stat_entry):
>    print stat_entry
>yappi.enum_stats(estat)
>('tt.py.foo:4', 1, 0.40625, 0.28125)
```


---


<font face='Courier New'>
yappi.<b>enum_thread_stats</b>()<br>
</font>

_Remarks:_

Enumerates the profile stats one by one. The parameter _fenum_ should be a callable Python function with one parameter, say _stat\_entry_ which will be invoked for every profiled function. The parameter is a Python tuple instance consisting all of the information related to thread profiling: ('thread\_name', 'thread\_id', 'last\_func', 'ttot', 'sched\_count')

_Example:_

```
>def foo(): pass    
>import yappi
>yappi.start()
>foo()
>def estat(stat_entry):
>    print stat_entry
>yappi.thread_stats(estat)
>('_MainThread', 6232, 'tt.py.foo:4', 0.421875, 1)
```

---


<font face='Courier New'>
yappi.<b>get_func_stats</b>(sorttype, sortorder, limit)<br>
</font>

_Remarks:_

Returns a YStats object. YStats object will have two list attributes(func\_stats, thread\_stats). These
attributes are a list of YStatDict instances. A YStatDict object for functions contains:
('name', 'ncall', 'ttot', 'tsub', 'tavg')
and for thread profiling information, it contains:
('name', 'id', 'last\_func', 'ttot', 'sched\_count')
See the example below for the usage.

_Params:_

  * <font face='Courier New'><b>sorttype</b></font>: There are several columns in the statistics results. This parameter controls on which column you want the sort to be based on. The valid values for this parameter are:
    * <font face='Courier New'>yappi.SORTTYPE_NAME</font>  : Sorts the results according to function name.
    * <font face='Courier New'>yappi.SORTTYPE_NCALL</font> : Sorts the results according to their call count._(default)_
    * <font face='Courier New'>yappi.SORTTYPE_TTOTAL</font>: Sorts the results according to their total time.
    * <font face='Courier New'>yappi.SORTTYPE_TSUB</font>  : Sorts the results according to their total subtime.      Subtime means the total spent time in the function minus the total time spent in the other functions called from this function.
    * <font face='Courier New'>yappi.SORTTYPE_TAVG</font>  : Sorts the results according to their total average time.
  * <font face='Courier New'><b>sortorder</b></font>: The order type of the column. Valid values for this parameter are:
    * <font face='Courier New'>yappi.SORTORDER_ASC</font> : Sorts the values in ascending order.
    * <font face='Courier New'>yappi.SORTORDER_DESC</font> : Sorts the values in descending order._(default)_
  * <font face='Courier New'><b>limit</b></font>: There may be too many functions profiled and user is not interested in many of them. So, limit gives the user the ability to limit the result set returned by the function. With this and above param, a user easily retrieves the top running functions that he/she is interested in.
  * <font face='Courier New'>yappi.SHOW_ALL</font> : Returns all of the statistic results._(default)_
  * <font face='Courier New'><b>thread_stats_on</b></font>: If set to true, returns thread profiling information. True by _default_.

_Example:_

```
>import yappi
>def foo(): pass
>yappi.start()
>foo()
>stats = yappi.get_stats()
>print "%s called %s times." % (stats.func_stats[0].name, stats.func_stats[0].ncall)
>print "thread %s(id=%d) scheduled %d times." % (stats.thread_stats[0].name, stats.thread_stats[0].id, 
>    stats.thread_stats[0].sched_count)
>yappi.py.__init__:50 called 4 times.
>thread _MainThread(id=7428) scheduled 1 times.
```


---


<font face='Courier New'>
yappi.<b>print_stats</b>(out=sys.stdout, sorttype, sortorder, limit)<br>
</font>

_Params:_

  * <font face='Courier New'><b>out</b></font>: The fd which the output is redirected to. Default is sys.stdout.

_Remarks:_
Remaining parameters are same as the get\_stats() function.


---


<font face='Courier New'>
yappi.<b>clear_stats</b>()<br>
</font>

_Remarks:_

Clears the profiler results. Note that the only way to really delete the statistics is this function. The results stays with the application unless application(all threads including the main thread) exists or clear\_stats() is called. yappi does not mess with the result set upon stop() or start(). It is user's responsibility to clear the statistics whenever appropiate. This also means without calling clear\_stats(), you can resume previously stopped profile informations via start().


---


<font face='Courier New'>
yappi.<b>is_running</b>()<br>
</font>

_Remarks:_

Returns a boolean indicating whether yappi is running or not.


---


<font face='Courier New'>
yappi.<b>clock_type</b>()<br>
</font>

_Remarks:_

Returns the underlying clock type and its resolution yappi uses to retrieve per-thread CPU time.

```
'''
On windows:
'''
>import yappi
>print yappi.clock_type()
>{'clock_type': 'getthreadtimes', 'resolution': '100ns'}
```