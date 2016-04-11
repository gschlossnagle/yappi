_**Please note that current version of yappi (v0.82) is not compatible with the older versions. Lots of existing API have been changed.**_

# Reference Manual (v0.82) #

<font face='Courier New'>
yappi.<b>start</b>(builtins=False, profile_threads=True)<br>
<br>
<blockquote>Starts profiling all threads in the current interpreter instance. This function can be called from any thread at any time. Resumes profiling if stop() is called previously.</blockquote>

<blockquote>Current parameters:<br>
<table><thead><th> <b>Param</b> </th><th> <b>Description</b> </th></thead><tbody>
<tr><td> builtins     </td><td> Profile builtin functions used by standart Python modules. It is <i>False</i> by <i>default</i>. </td></tr>
<tr><td> profile_threads </td><td>  Profile all of the threads if 'true', else, profile only the calling thread. </td></tr></blockquote></tbody></table>

<font face='Courier New'>
yappi.<b>stop</b>()<br>
</font>

<blockquote>Stops the currently running yappi instance. Same profiling session might be resumed later by calling start().</blockquote>


<font face='Courier New'>
yappi.<b>clear_stats</b>()<br>
</font>

<blockquote>Clears the profiler results. The results stays in memory unless application(all threads including the main thread) exists or clear_stats() is called.</blockquote>


<font face='Courier New'>
yappi.<b>get_func_stats</b>()<br>
</font>

<blockquote>Returns the function stats as <a href='https://code.google.com/p/yappi/wiki/YFuncStats_v082'>YFuncStats</a>  object.</blockquote>


<font face='Courier New'>
yappi.<b>get_thread_stats</b>()<br>
</font>

<blockquote>Returns the thread stats as <a href='https://code.google.com/p/yappi/wiki/YThreadStats_v082'>YThreadStats</a>  object.</blockquote>


<font face='Courier New'>
yappi.<b>is_running</b>()<br>
</font>

<blockquote>Returns a boolean indicating whether profiler is running or not.</blockquote>


<font face='Courier New'>
yappi.<b>get_clock_type</b>()<br>
</font>

<blockquote>Returns information about the underlying clock type Yappi uses to measure timing.</blockquote>

<pre><code>'''<br>
Example on windows:<br>
'''<br>
&gt;&gt;&gt; import yappi<br>
&gt;&gt;&gt; yappi.get_clock_type()<br>
{'resolution': '100ns', 'api': 'getthreadtimes', 'type': 'cpu'}<br>
&gt;&gt;&gt;<br>
</code></pre>


<font face='Courier New'>
yappi.<b>set_clock_type</b>(type)<br>
</font>

<blockquote>Sets the underlying clock type. <i>type</i> can be following:</blockquote>

<table><thead><th> <b>Clock Type</b> </th><th> <b>Description</b> </th></thead><tbody>
<tr><td> Wall              </td><td> <a href='http://en.wikipedia.org/wiki/Wall_time'>Details</a> </td></tr>
<tr><td> CPU               </td><td> <a href='http://en.wikipedia.org/wiki/CPU_time'>Details</a> </td></tr></tbody></table>


<font face='Courier New'>
yappi.<b>get_mem_usage</b>()<br>
</font>

<blockquote>Returns the internal memory usage of the profiler itself.</blockquote>

<font face='Courier New'>
yappi.<b>convert2pstats</b>(stats)<br>
</font>

<blockquote>Converts the internal stat type of yappi(which is returned by a call to YFuncStats.get()) as <a href='http://docs.python.org/3.4/library/profile.html#module-pstats'>pstats</a> object.</blockquote>

</font>