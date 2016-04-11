<font face='Consolas'>
<h1>Usage Manual (v0.92)</h1>

A typical example on profiling with yappi, includes at least 3 lines of code:<br>
<br>
<pre><code>import yappi<br>
def a(): <br>
    for i in range(10000000): pass<br>
<br>
yappi.start()<br>
a()<br>
yappi.get_func_stats().print_all()<br>
yappi.get_thread_stats().print_all()<br>
</code></pre>

And the output of running above script:<br>
<br>
<pre><code>Clock type: cpu<br>
Ordered by: totaltime, desc<br>
<br>
name                                    #n         tsub      ttot      tavg<br>
deneme.py:35 a                          1          0.296402  0.296402  0.296402<br>
<br>
name           tid              ttot      scnt<br>
_MainThread    6016             0.296402  1<br>
</code></pre>

Let's inspect the results in detail. So, first line:<br>
<pre><code>Clock type: cpu<br>
</code></pre>
This indicates the profiling timing stats shown are retrieved using the CPU clock. That means the actual CPU time spent in the function is shown. Yappi provides two modes of operation: CPU and Wall time profiling. You can change the setting by a call to <i>yappi.set_clock_type()</i> API. See <a href='https://code.google.com/p/yappi/wiki/ClockTypes_v082'>Clock Types</a> to interpret different timing values correctly.<br>
<br>
Second is:<br>
<pre><code>Ordered by: totaltime, desc<br>
</code></pre>
It is obvious. It shows the sort order and sort key of the shown profiling stats. You can see the valid values for this in <i>YFuncStats().sort()</i> API.<br>
<br>
Ok, now we actually see the statistic of the function a():<br>
<pre><code>name                                    #n         tsub      ttot      tavg<br>
deneme.py:35 a                          1          0.296402  0.296402  0.296402<br>
</code></pre>
Let's explain the fields in detail:<br>
<table><thead><th> <b>Title</b> </th><th> <b>Description</b> </th></thead><tbody>
<tr><td> name         </td><td> the full unique name of the called function. </td></tr>
<tr><td> #n           </td><td> how many times this function is called. </td></tr>
<tr><td> tsub         </td><td> how many time this function has spent in total, subcalls excluded. See <a href='https://code.google.com/p/yappi/wiki/ClockTypes_v082'>Clock Types</a> to interpret this value correctly. </td></tr>
<tr><td> ttot         </td><td> how many time this function has spent in total, subcalls included. See <a href='https://code.google.com/p/yappi/wiki/ClockTypes_v082'>Clock Types</a> to interpret this value correctly. </td></tr>
<tr><td> tavg         </td><td> how many time this function has spent in average, subcalls included. See <a href='https://code.google.com/p/yappi/wiki/ClockTypes_v082'>Clock Types</a> to interpret this value correctly. </td></tr></tbody></table>

The next lines shows the thread stats. So, let see:<br>
<pre><code>name           tid              ttot      scnt<br>
_MainThread    6016             0.296402  1<br>
</code></pre>
<table><thead><th> <b>Title</b> </th><th> <b>Description</b> </th></thead><tbody>
<tr><td> name         </td><td> the class name of the Thread.(this is the name of the class inherits the threading.Thread class) </td></tr>
<tr><td> tid          </td><td> the thread id.     </td></tr>
<tr><td> ttot         </td><td> how many time this thread has spent in total. See <a href='https://code.google.com/p/yappi/wiki/ClockTypes_v082'>Clock Types</a> to interpret this value correctly. </td></tr>
<tr><td> scnt         </td><td> how many times this thread is scheduled. </td></tr></tbody></table>

</font>