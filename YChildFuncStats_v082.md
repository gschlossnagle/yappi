# class YChildFuncStats (v0.82) #

<font face='Courier New'>

<b><i>class</i></b> yappi.<b>YChildFuncStats</b>

<blockquote><i>YChildFuncStat.children</i> holds a list of child functions called from the main executed function as a <i>YChildFuncStats</i> object. <i>YChildFuncStats</i> class holds a list of <i>YChildFuncStat</i> objects.</blockquote>

<blockquote>So, as an example if a calls b, then a.children will hold b as a <i>YChildFuncStat</i> object.</blockquote>

<blockquote>A <i>YChildFuncStat</i> object holds the following information:<br>
<table><thead><th> <b>Key</b> </th><th> <b>Description</b> </th></thead><tbody>
<tr><td> index      </td><td> A unique number for the stat </td></tr>
<tr><td> module     </td><td> Module name of the child function </td></tr>
<tr><td> lineno     </td><td> Line number of the child function </td></tr>
<tr><td> name       </td><td> Name of the child function </td></tr>
<tr><td> full_name  </td><td> module:lineno name - unique full name of the child function </td></tr>
<tr><td> ncall      </td><td> number of times the child function is called. </td></tr>
<tr><td> nactualcall </td><td> number of times the child function is called, excluding the recursive calls. </td></tr>
<tr><td> ttot       </td><td> total time spent in the child function. Or in other words, total time parent spent in calling this child. See <a href='https://code.google.com/p/yappi/wiki/ClockTypes_v082'>Clock Types</a> to interpret this value correctly. </td></tr>
<tr><td> tsub       </td><td> total time spent in the child function. Or in other words, total time child spent in itself(subcalls excluded) when called by a specific parent. See <a href='https://code.google.com/p/yappi/wiki/ClockTypes_v082'>Clock Types</a> to interpret this value correctly. </td></tr>
<tr><td> builtin    </td><td> boolean flag showing if the executed function is a builtin </td></tr>
<tr><td> tavg       </td><td> per-call average total time spent in the child function. See <a href='https://code.google.com/p/yappi/wiki/ClockTypes_v082'>Clock Types</a> to interpret this value correctly. </td></tr></blockquote></tbody></table>

<b>print_all(out=sys.stdout)</b>
<blockquote>This method prints the current profile stats to <i>"out"</i> which is  <i>"stdout"</i> by default.</blockquote>

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

<b>debug_print()</b>
<blockquote>This method <i>debug</i> prints the current profile stats to <i>"stdout". Debug print prints out callee functions and more detailed info than the <i>print_all()</i> function call.</i></blockquote>


</font>