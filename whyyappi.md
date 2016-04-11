<font face='Consolas'>
<h1>Motivation:</h1>
<i>CPython</i> standart distribution is coming with three profilers. <i>cProfile</i>, <i>Profile</i> and <i>hotshot</i>. <i>cProfile</i> module is implemented as a C module based on <i>lsprof</i>, <i>Profile</i> is in pure Python and the hotshot can be seen as a small subset of a cProfile. The motivation to implement a new profiler is that all of these profilers lacks the support of multi-threaded programs and CPU time profiling per thread basis. If you want to profile a multi-threaded application, you must give an entry point to these profilers and then maybe merge the outputs. None of these profilers is designed to work on long-running multi-threaded application. While implementing a game server, it turns out that is is impossible to profile an application retrieve the statistics then stop and then start later on on the fly(without affecting the profiled application). With the experience of implementing a game server in Python, we have identified most of the problems, tricky parts regarding profiler usage and so, we have come up with simple but powerful requirements.<br>
<br>
<h1>Requirements:</h1>
<ul><li>Profiler should be started/stopped at any time from any thread in the application.<br>
</li><li>Profile statistics should be obtained from any thread at any time.<br>
</li><li>Profile statistics will be calculated from <b>per-thread CPU time</b>.(new in v0.62)<br>
</li><li>"Profiler pollution"(effect on the application run-time) should be very minimal.</li></ul>

<h1>Limitations:</h1>
<ul><li>Latest version of Yappi supports Python 2.6.x <= x <= Python.3.4</li></ul>

</font>