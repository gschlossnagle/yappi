# class YFuncStats (v0.92) #

<font face='Consolas'>

<b><i>class</i></b> yappi.<b>YFuncStats</b>


<b>get()</b>
<blockquote>This method retrieves the current profiling stats.      yappi.get_func_stats() is actually just a wrapper for this function. YFuncStats holds the stat items as a list of <i>YFuncStat</i> object.</blockquote>

<blockquote>A <i>YFuncStat</i> object holds the following information:<br>
<table><thead><th> <b>Key</b> </th><th> <b>Description</b> </th></thead><tbody>
<tr><td> index      </td><td> A unique number for the stat </td></tr>
<tr><td> module     </td><td> Module name of the executed function </td></tr>
<tr><td> lineno     </td><td> Line number of the executed function </td></tr>
<tr><td> name       </td><td> Name of the executed function </td></tr>
<tr><td> full_name  </td><td> module:lineno name - unique full name of the executed function </td></tr>
<tr><td> ncall      </td><td> number of times the executed function is called. </td></tr>
<tr><td> nactualcall </td><td> number of times the executed function is called, excluding the recursive calls. </td></tr>
<tr><td> builtin    </td><td> boolean flag showing if the executed function is a builtin </td></tr>
<tr><td> ttot       </td><td> total time spent in the executed function. See <a href='https://code.google.com/p/yappi/wiki/ClockTypes_v082'>Clock Types</a> to interpret this value correctly. </td></tr>
<tr><td> tsub       </td><td> total time spent in the executed function, excluding subcalls. See <a href='https://code.google.com/p/yappi/wiki/ClockTypes_v082'>Clock Types</a> to interpret this value correctly. </td></tr>
<tr><td> tavg       </td><td> per-call average total time spent in the executed function. See <a href='https://code.google.com/p/yappi/wiki/ClockTypes_v082'>Clock Types</a> to interpret this value correctly. </td></tr>
<tr><td> children   </td><td> list of functions called from the executed function. See <a href='https://code.google.com/p/yappi/wiki/YChildFuncStats_v092'>YChildFuncStats</a> object </td></tr></blockquote></tbody></table>

<b>add(path, type="ystat")</b>
<blockquote>This method loads the saved profile stats stored in file <i>"path"</i>. <i>"type"</i> indicates the type of the saved profile stats.</blockquote>

<blockquote>The following are the load formats currently available:<br>
<table><thead><th> <b>Format</b> </th></thead><tbody>
<tr><td> ystat         </td></tr></blockquote></tbody></table>

<b>save(path, type="ystat")</b>
<blockquote>This method saves the current profile stats to <i>"path"</i>. <i>"type"</i> indicates the target type that the profile stats will be saved in. Currently only loading from <i>"ystat"</i> format is possible. <i>"ystat"</i> is the current yappi internal format.</blockquote>

<blockquote>The following are the save formats currently available:<br>
<table><thead><th> <b>Format</b> </th></thead><tbody>
<tr><td> ystat         </td></tr>
<tr><td> <a href='http://docs.python.org/3.3/library/profile.html?highlight=pstat#pstats.Stats.print_stats'>pstat</a> </td></tr>
<tr><td> <a href='http://kcachegrind.sourceforge.net/html/CallgrindFormat.html'>callgrind</a> </td></tr></blockquote></tbody></table>


<b>print_all(out=sys.stdout, columns={0:("name",36), 1:("ncall", 5), 2:("tsub", 8), 3:("ttot", 8), 4:("tavg",8)})</b>
<blockquote>This method prints the current profile stats to <i>"out"</i> which is  <i>"stdout"</i> by default.<br>
<i>columns</i> dict specifies the formatting of the printable columns. Every column is formatted<br>
according to the parameters of this dict's item.</blockquote>

<blockquote>An example item:<br>
0:("name",36) means that the first column in position '0' has title name and its<br>
size is 36. Yappi will allocate 36 characters for this column when printing.</blockquote>

<b>sort(sort_type, sort_order="desc")</b>
<blockquote>This method sorts the current profile stats according to the  <i>"sort_type"</i> param.</blockquote>

<blockquote>The following are the valid <i>"sort_type"</i> params:<br>
<table><thead><th> <b>Sort Types</b> </th></thead><tbody>
<tr><td> name              </td></tr>
<tr><td> callcount/ncall   </td></tr>
<tr><td> totaltime/ttot    </td></tr>
<tr><td> subtime/tsub      </td></tr>
<tr><td> avgtime/tavg      </td></tr></blockquote></tbody></table>

<blockquote>The following are the valid <i>"sort_order"</i> params:<br>
<table><thead><th> <b>Sort Orders</b> </th></thead><tbody>
<tr><td> descending/desc    </td></tr>
<tr><td> ascending/asc      </td></tr></blockquote></tbody></table>

<b>clear()</b>
<blockquote>Clears the retrieved stats. Note that this only clears the current object's stat list. You need to explicitly call <i>yappi.clear_stats()</i> to clear the current profile stats.</blockquote>

<b>empty()</b>
<blockquote>Returns a boolean indicating whether we have any stats available or not.</blockquote>

<b>strip_dirs()</b>
<blockquote>Strip the directory information from the results. Affects the child function stats, too.</blockquote>

<b>debug_print()</b>
<blockquote>This method <i>debug</i> prints the current profile stats to <i>"stdout". Debug print prints out callee functions and more detailed info than the <i>print_all()</i> function call.</i></blockquote>

</font>