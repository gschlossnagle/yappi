# class YThreadStats (v0.92) #

<font face='Consolas'>

<b><i>class</i></b> yappi.<b>YThreadStats</b>

<b>get()</b>
<blockquote>This method retrieves the current thread stats.      yappi.get_thread_stats() is actually just a wrapper for this function. YThreadStats holds the stat items as a list of <i>YThreadStat</i> object.</blockquote>

<blockquote>A <i>YThreadStat</i> object holds the following information:<br>
<table><thead><th> <b>Key</b> </th><th> <b>Description</b> </th></thead><tbody>
<tr><td> id         </td><td> thread id given by the OS </td></tr>
<tr><td> name       </td><td> class name of the current thread object which is derived from threading.Thread class </td></tr>
<tr><td> ttot       </td><td> total time spent in the last executed function. See <a href='https://code.google.com/p/yappi/wiki/ClockTypes_v082'>Clock Types</a> to interpret this value correctly. </td></tr>
<tr><td> sched_count </td><td> number of times this thread is scheduled. </td></tr></blockquote></tbody></table>

<b>print_all(out=sys.stdout, columns={0:("name",13), 1:("tid", 15), 2:("ttot", 8), 3:("scnt", 10)})</b>
<blockquote>This method prints the current profile stats to <i>"out"</i> which is  <i>"stdout"</i> by default.<br>
<i>columns</i> dict specifies the formatting of the printable columns. Every column is formatted<br>
according to the parameters of this dict's item.</blockquote>

<blockquote>An example item:<br>
0:("name",13) means that the first column in position '0' has title name and its<br>
size is 13. Yappi will allocate 13 characters for this column when printing.</blockquote>

<b>sort(sort_type, sort_order="desc")</b>
<blockquote>This method sorts the current profile stats according to the  <i>"sort_type"</i> param.</blockquote>

<blockquote>The following are the valid <i>"sort_type"</i> params:<br>
<table><thead><th> <b>Sort Types</b> </th></thead><tbody>
<tr><td> name              </td></tr>
<tr><td> id                </td></tr>
<tr><td> totaltime/ttot    </td></tr>
<tr><td> schedcount/scnt   </td></tr></blockquote></tbody></table>

<blockquote>The following are the valid <i>"sort_order"</i> params:<br>
<table><thead><th> <b>Sort Orders</b> </th></thead><tbody>
<tr><td> descending/desc    </td></tr>
<tr><td> ascending/asc      </td></tr></blockquote></tbody></table>

<b>clear()</b>
<blockquote>Clears the retrieved stats. Note that this only clears the current object's stat list. You need to explicitly call <i>yappi.clear_stats()</i> to clear the current profile stats.</blockquote>

<b>empty()</b>
<blockquote>Returns a boolean indicating whether we have any stats available or not.</blockquote>

</font>