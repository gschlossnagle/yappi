/*

 yappi.py
 Yet Another Python Profiler

 Sumer Cip 2009 

*/

#include "Python.h"
#include "frameobject.h"
#include "_ycallstack.h"
#include "_yhashtab.h"
#include "_ydebug.h"
#include "_ytiming.h"
#include "_yfreelist.h"
#include "_ystatic.h"
#include "_ymem.h"

// profiler related
#define CURRENTCTX _thread2ctx(frame->f_tstate)

typedef struct {
	PyObject *co; // CodeObject or MethodDef descriptive string.
	long callcount;
	long long tsubtotal;
	long long ttotal;
}_pit;

typedef struct {
	_cstack *cs;
	long id;
	_pit * last_pit;
}_ctx;

typedef struct {
	int builtins;
}_flag;


// stat related definitions
typedef struct {
	char fname[MAX_FUNC_NAME_LEN];
	long callcount;
	double ttot;
	double tsub;
	double tavg;
	char result[MAX_LINE_LEN];
}_statitem;


struct _stat_node_t{
	_statitem *it;
	struct _stat_node_t *next;
};
typedef struct _stat_node_t _statnode;

// profiler global vars
static PyObject *YappiProfileError;
static _statnode *statshead;
static _htab *contexts;
static _htab *pits;
static long long yappoverhead; 	// total profiler overhead
static long long appttotal; 	// total application overhead
static _flag flags;
static _freelist *flpit;
static _freelist *flctx;
static int yappinitialized;
static int yapphavestats;	// start() called at least once or stats cleared?
static int yapprunning;
static time_t yappstarttime;

static _pit *
_create_pit(void)
{
	_pit *pit;
	pit = flget(flpit);
	if (!pit)
		return NULL;
	pit->callcount = 0;
	pit->ttotal = 0;
	pit->tsubtotal = 0;
	pit->co = NULL;	
	return pit;
}

static _ctx *
_create_ctx(void) 
{
	_ctx *ctx = flget(flctx);
	if (!ctx)
		return NULL;
	ctx->cs = screate(100);
	if (!ctx->cs)
		return NULL;
	ctx->last_pit = NULL;
	return ctx;
}


static _ctx *
_thread2ctx(PyThreadState *ts)
{
	_hitem *it = hfind(contexts, (uintptr_t)ts);
	if (!it)
		return NULL;
	return (_ctx *)it->val;
}

// the pit will be cleared by the relevant freelist. we do not free it here.
// we only DECREF the CodeObject or the MethodDescriptive string.
static void
_del_pit(_pit *pit)
{
	// if it is a regular C string all DECREF will do is to decrement the first
	// character's value.
	Py_DECREF(pit->co); 
}

static _pit *
_ccode2pit(void *cco)
{
	PyCFunctionObject *cfn = cco;
	_hitem *it = hfind(pits, (uintptr_t)cco);
	if (!it) {
		_pit *pit = _create_pit();
		if (!pit)
			return NULL;
		if (!hadd(pits, (uintptr_t)cco, (uintptr_t)pit))	
			return NULL;
		// built-in FUNCTION?
		if (cfn->m_self == NULL) {
			PyObject *mod = cfn->m_module;
			char *modname;
			if (mod && PyString_Check(mod)) {
				modname = PyString_AS_STRING(mod);
			}
			else if (mod && PyModule_Check(mod)) {
				modname = PyModule_GetName(mod);
				if (modname == NULL) {
					PyErr_Clear();
					modname = "__builtin__";
				}
			}
			else {
				modname = "__builtin__";
			}
			if (strcmp(modname, "__builtin__") != 0)
				pit->co = PyString_FromFormat("<%s.%s>",
							   modname,
							   cfn->m_ml->ml_name);
			else
				pit->co = PyString_FromFormat("<%s>",
							   cfn->m_ml->ml_name);

		} else { // built-in METHOD?
			PyObject *self = cfn->m_self;
			PyObject *name = PyString_FromString(cfn->m_ml->ml_name);
			if (name != NULL) {
				PyObject *mo = _PyType_Lookup((PyTypeObject *)PyObject_Type(self), name);
				Py_XINCREF(mo);
				Py_DECREF(name);
				if (mo != NULL) {
					PyObject *res = PyObject_Repr(mo);
					Py_DECREF(mo);
					if (res != NULL)
						pit->co = res;
				}
			}
			PyErr_Clear();
			pit->co = PyString_FromFormat("<built-in method %s>",
						   cfn->m_ml->ml_name);
		}
 		return pit;
	}
	return ((_pit *)it->val);
}

// maps the PyCodeObject to our internal pit item via hash table.
static _pit *
_code2pit(void *co)
{
	_hitem *it = hfind(pits, (uintptr_t)co);
	if (!it) {
		_pit *pit = _create_pit();
		if (!pit)
			return NULL;
		if (!hadd(pits, (uintptr_t)co, (uintptr_t)pit))	
			return NULL;
		Py_INCREF((PyObject *)co);
		pit->co = co; //dummy
 		return pit;
	}
	return ((_pit *)it->val);
}

static void
_call_enter(PyObject *self, PyFrameObject *frame, PyObject *arg)
{
	_ctx *context;
	_pit *cp;
	PyObject *last_type, *last_value, *last_tb;
	PyErr_Fetch(&last_type, &last_value, &last_tb);
	
	context = CURRENTCTX;
	if (PyCFunction_Check(arg)) {
     	cp = _ccode2pit((PyCFunctionObject *)arg); 
    } else {
     	cp = _code2pit(frame->f_code);
    } 
	
	// something went wrong. No mem, or another error. we cannot find
	// a corresponding pit. just run away:)
	if (!cp) {
		PyErr_Restore(last_type, last_value, last_tb);
		return;
	}	
	
	spush(context->cs, cp);
	cp->callcount++;
	context->last_pit = cp;	
	
	PyErr_Restore(last_type, last_value, last_tb);
}


static void
_call_leave(PyObject *self, PyFrameObject *frame, PyObject *arg)
{
	_pit *cp, *pp;
	_cstackitem *ci,*pi;
	_ctx *context = CURRENTCTX;
	long long elapsed;

	ci = spop(context->cs);
	if (!ci) {
		dprintf("leaving a frame while callstack is empty.\n");
		return;
	}
	cp = ci->ckey;
	if (!pi) {
		cp->ttotal += (tickcount() - ci->t0);
		return;
	}
	pp = pi->ckey; 
	elapsed = (tickcount() - ci->t0);
	if (scount(context->cs, ci->ckey) > 0) {
		cp->tsubtotal -= elapsed;
	} else {
		cp->ttotal += elapsed;
	}
	pp->tsubtotal += elapsed;
	return;
}

// context will be cleared by the free list. we do not free it here.
// we only free the context call stack.
static void 
_del_ctx(_ctx * ctx) 
{
	sdestroy(ctx->cs);
}

static int 
_yapp_callback(PyObject *self, PyFrameObject *frame, int what,
		  PyObject *arg)
{

	long long t0 = tickcount();
	switch (what) {		
		case PyTrace_CALL:		
			_call_enter(self, frame, arg);
			break;				
		case PyTrace_RETURN: // either normally or with an exception
			_call_leave(self, frame, arg);
			break;

#ifdef PyTrace_C_CALL	/* not defined in Python <= 2.3 */

		case PyTrace_C_CALL:
			if (PyCFunction_Check(arg) && (flags.builtins))
			    _call_enter(self, frame, arg);
			break;

		case PyTrace_C_RETURN:
		case PyTrace_C_EXCEPTION:	
			if (PyCFunction_Check(arg) && (flags.builtins))
			    _call_leave(self, frame, arg);
			break;

#endif
		default:
			break;
	}
	yappoverhead += tickcount() - t0;
	return 0;
}

static void
_profile_thread(PyThreadState *ts)
{	
	ts->use_tracing = 1;	
	ts->c_profilefunc = _yapp_callback;

	_ctx *ctx = _create_ctx();
	if (!ctx)
		return;
		
	// If a ThreadState object is destroyed, currently(v0.2) yappi does not 
	// deletes the associated resources. Instead, we rely on the fact that
	// the ThreadState objects are actually recycled. We are using pointer
	// to map to the internal contexts table, and Python VM will try to use
	// the destructed thread's pointer when a new thread is created. They are
	// pooled inside the VM. This is very hecky solution, but there is no 
	// efficient and easy way to somehow know that a Python Thread is about
	// to be destructed.
	if (!hadd(contexts, (uintptr_t)ts, (uintptr_t)ctx)) {
		_del_ctx(ctx);
		if (!flput(flctx, ctx))
			yerr("Context cannot be recycled. Possible memory leak.[%d bytes]", sizeof(_ctx));
		dprintf("Context add failed. Already added?(%p, %ld)", ts, 
				PyThreadState_GET()->thread_id);
	}
	ctx->id = ts->thread_id;
}

static void
_unprofile_thread(PyThreadState *ts)
{
	ts->use_tracing = 0;	
	ts->c_profilefunc = NULL;
}

static void
_ensure_thread_profiled(PyThreadState *ts)
{
	PyThreadState *p = NULL;
	
	for (p=ts->interp->tstate_head ; p != NULL; p = p->next) {
		if (ts->c_profilefunc != _yapp_callback)
			_profile_thread(ts);
	}	
}

static void
_enum_threads(void (*f) (PyThreadState *))
{
	PyThreadState *p = NULL;
	
	for (p=PyThreadState_GET()->interp->tstate_head ; p != NULL; p = p->next) {
		f(p);
	}	
}

static int
_init_profiler(void)
{
	// already initialized? only after clear_stats() and first time, this flag
	// will be unset.
	if (!yappinitialized) {
		contexts = htcreate(HT_CTX_SIZE, HT_CTX_GROW_FACTOR);
		if (!contexts)
			return 0;
		pits = htcreate(HT_PIT_SIZE, HT_PIT_GROW_FACTOR);
		if (!pits)
			return 0;
		yappoverhead = 0;
		appttotal = 1; // do not make this zero at any time. may lead a division error.
		flpit = flcreate(sizeof(_pit), FL_PIT_SIZE);
		if (!flpit)
			return 0;
		flctx = flcreate(sizeof(_ctx), FL_CTX_SIZE);
		if (!flctx)
			return 0;
		yappinitialized = 1;
		statshead = NULL; //
	}
	return 1;
}

static PyObject*
profile_event(PyObject *self, PyObject *args)
{	
	char *ev;
	PyObject *arg;
	PyStringObject *event;
	PyFrameObject * frame;
	
	if (!PyArg_ParseTuple(args, "OOO", &frame, &event, &arg)) {
        	return NULL;
    }
	
	_ensure_thread_profiled(PyThreadState_GET());	
	
	ev = PyString_AS_STRING(event);

	if (strcmp("call", ev)==0)
		_yapp_callback(self, frame, PyTrace_CALL, arg);
	else if (strcmp("return", ev)==0)
		_yapp_callback(self, frame, PyTrace_RETURN, arg);
	else if (strcmp("c_call", ev)==0)
		_yapp_callback(self, frame, PyTrace_C_CALL, arg);
	else if (strcmp("c_return", ev)==0)
		_yapp_callback(self, frame, PyTrace_C_RETURN, arg);
	else if (strcmp("c_exception", ev)==0)
		_yapp_callback(self, frame, PyTrace_C_EXCEPTION, arg);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject*
start(PyObject *self, PyObject *args)
{	
	if (yapprunning) {
		PyErr_SetString(YappiProfileError, "profiler is already started. yappi is a per-interpreter resource.");
		return NULL;		
	}	

	if (!PyArg_ParseTuple(args, "i", &flags.builtins))
		return NULL;
	
	if (!_init_profiler()) {
		PyErr_SetString(YappiProfileError, "profiler cannot be initialized.");
		return NULL;
	}
		
	_enum_threads(&_profile_thread);
	
	yapprunning = 1;		
	yapphavestats = 1;
	time (&yappstarttime);

	Py_INCREF(Py_None);
	return Py_None;
}

// extracts the function name from a given pit. Note that pit->co may be 
// either a PyCodeObject or a descriptive string.
static char *
_item2fname(_pit *pt, int stripslashes)
{
	int i, sp;
	char *buf;
	PyObject *fname;
	
	if (!pt)
		return NULL;
	
	//printf("pt:%p\n", pt);
	//printf("pt->co:%p \n", pt->co);
	
	if (PyCode_Check(pt->co)) {		
		fname =  PyString_FromFormat( "%s.%s:%d",
				PyString_AS_STRING(((PyCodeObject *)pt->co)->co_filename),
				PyString_AS_STRING(((PyCodeObject *)pt->co)->co_name),
				((PyCodeObject *)pt->co)->co_firstlineno );			
	} else {	
		fname = pt->co;
	}
	
	// get the basename
	sp = 0;
	buf = PyString_AS_STRING(fname); // TODO:memleak on buf?
	for (i=strlen(buf);i>-1;i--) {
		if ( (buf[i] == 92) || (buf[i] == 47) ) {		
			sp = i+1;
			break;
		}
	}
	
	//DECREF should not be done on builtin funcs.
	//TODO: maybe this is problematic, too?
	//have not seen any problem, yet, in live.
	if (PyCode_Check(pt->co)) {
		Py_DECREF(fname);
	}
	return &buf[sp];
}

static int
_pitenumstat(_hitem *item, void * arg)
{	
	long long cumdiff;
	PyObject *efn;
	char *fname;
	_pit *pt;
	
	pt = (_pit *)item->val;	
	if (!pt->ttotal)
		return 0;
	
	cumdiff = pt->ttotal - pt->tsubtotal;
	efn = (PyObject *)arg;
		
	fname = _item2fname(pt, 0);	
	if (!fname)
		fname = "N/A";

	// TODO: we may have MT issues here!!! declaring a preenum func in yappi.py
	// does not help as we need a per-profiler sync. object for this. This means
	// additional complexity and additional overhead. Any idea on this?
	// Do we really have an mt issue here? The parameters that are sent to the 
	// function does not directly use the same ones, they will copied over.
	PyObject_CallFunction(efn, "((sIff))", fname, 
				pt->callcount, pt->ttotal * tickfactor(),
				cumdiff * tickfactor());

	appttotal += pt->tsubtotal;	
	return 0;
}

// copy src to dest. If dlen is greater and padding is true, spaces are padded till dlen.
// otherwise string is trimmed starting from dlen-ZIP_RIGHT_MARGIN_LEN-ZIP_DOT_COUNT.
void
_zipstr(char *src, char *dest, int slen, int dlen, int padding)
{
	int i;
	int rmargin;
	
	rmargin = ZIP_RIGHT_MARGIN_LEN+ZIP_DOT_COUNT;
	if (rmargin > dlen) {
		yerr("right margin is greater than destination string length.");
		return;
	}	
	if (rmargin  < 0) {
		yerr("right margin should be positive.");
		return;
	}	
	if ((slen <= 0) || (dlen <= 0)){
		yerr("nothing to zip to destination string.");
		return;
	}

	if (dlen <= rmargin) {
		if (dlen > slen) {
			yerr("destination length is greater than the source length.");
			return;		
		}
		for(i=0;i<dlen;i++)
			dest[i] = src[i];
	}
	
	// needs trim?
	if (slen > dlen-rmargin) {
		for(i=0;i<dlen-rmargin;i++)
			dest[i] = src[i];
		for(;i<dlen-ZIP_RIGHT_MARGIN_LEN;i++)
			dest[i] = '.';
	} else {
		if (slen > dlen) {
			yerr("source length is greater than the destination length.");
			return;	
		}
		for(i=0;i<slen;i++)
			dest[i] = src[i];
		if (padding) {
			for( ; i<dlen; i++)
				dest[i] = ' ';
		}
	}
}

_statitem *
_create_statitem(char *fname, long callcount, double ttot, double tsub, double tavg)
{
	int i;
	_statitem *si;
	char temp[MAX_LINE_LEN];

	si = (_statitem *)ymalloc(sizeof(_statitem));
	if (!si)
		return NULL;
	// init the result string.
	for(i=0;i<MAX_LINE_LEN-1;i++)
		si->result[i] = ' ';
	si->result[MAX_LINE_LEN-1] = (char)0;
	
	_zipstr(fname, &si->result[0], strlen(fname), MAX_FUNC_NAME_LEN, 1);
	_zipstr(fname, &si->fname[0], strlen(fname), MAX_FUNC_NAME_LEN, 1);
	sprintf(temp, "%lu", callcount);
	_zipstr(temp, &si->result[STAT_CALLCOUNT_COLUMN_IDX], 
			strlen(temp), MAX_CALLCOUNT_COLUMN_LEN, 1);
	si->callcount = callcount;
	sprintf(temp, "%0.6f", ttot);
	_zipstr(temp, &si->result[STAT_TTOT_COLUMN_IDX], 
			strlen(temp), MAX_TIME_COLUMN_LEN, 1);
	si->ttot = ttot;
	sprintf(temp, "%0.6f", tsub);
	_zipstr(temp, &si->result[STAT_TSUB_COLUMN_IDX], 
			strlen(temp), MAX_TIME_COLUMN_LEN, 1);
	si->tsub = tsub;
	sprintf(temp, "%0.6f", tavg);
	_zipstr(temp, &si->result[STAT_TAVG_COLUMN_IDX], 
			strlen(temp), MAX_TIME_COLUMN_LEN, 1);
	si->tavg = tavg;	
	return si;
}

// inserts items to statshead pointed linked list for later usage according to the
// sorttype param. Note that sorting is descending by default. Read reverse from list
// to have a ascending order.
void
_insert_stats_internal(_statnode *sn, int sorttype)
{
	_statnode *p, *prev;

	prev = NULL;
	p = statshead;
	while(p) {
		//dprintf("sn:%p, sn->it:%p : p:%p, p->it:%p.\n", sn, sn->it, p, p->it);	
		if (sorttype == STAT_SORT_TIME_TOTAL) {
			if (sn->it->ttot > p->it->ttot)
				break;
		} else if (sorttype == STAT_SORT_CALL_COUNT) {
			if (sn->it->callcount > p->it->callcount)
				break;
		} else if (sorttype == STAT_SORT_TIME_SUB) {
			if (sn->it->tsub > p->it->tsub)
				break;
		} else if (sorttype == STAT_SORT_TIME_AVG) {
			if (sn->it->tavg > p->it->tavg)
				break;
		} else if (sorttype == STAT_SORT_FUNC_NAME) {
			if (strcmp(sn->it->fname, p->it->fname) > 0)
				break;
		}
		prev = p;
		p = p->next;
	}
	
	// insert at head
	if (!prev) {
		sn->next = statshead;
		statshead = sn;
	} else {
		sn->next = prev->next;
		prev->next = sn;
	}
}

// reverses the statshead list according to a given order.
void
_order_stats_internal(int order)
{
	_statnode *p,*tmp,*pr;

	if (order == STAT_SORT_DESCENDING) {
		; // nothing to do as internal order is by default descending
	} else if (order == STAT_SORT_ASCENDING) {
		   // reverse stat linked list
		   pr = tmp = NULL;
		   p = statshead;
		   while (p != NULL) {
			  tmp  = p->next;
		      p->next = pr;
		      pr = p;
		      p = tmp;
		   }
		   statshead = pr;
	}
}


void
_clear_stats_internal(void)
{
	_statnode *p,*next;

	p = statshead;
	while(p){
		next = p->next;
		yfree(p->it);
		yfree(p);
		p = next;
	}
	statshead = NULL;
}

static int
_pitenumstat2(_hitem *item, void * arg)
{	
	_pit *pt;
	char *fname;
	_statitem *si;
	long long cumdiff;
	
	pt = (_pit *)item->val;	
	if (!pt->ttotal)
		return 0;
		
	cumdiff = pt->ttotal - pt->tsubtotal;
	fname = _item2fname(pt, 1);
	if (!fname)
		fname = "N/A";
	
	si = _create_statitem(fname, pt->callcount, pt->ttotal * tickfactor(),
			cumdiff * tickfactor(), pt->ttotal * tickfactor() / pt->callcount);
	if (!si)
		return 1; // abort enumeration
	_statnode *sni = (_statnode *)ymalloc(sizeof(_statnode));
	if (!sni)
		return 1; // abort enumeration
	sni->it = si;
	
	_insert_stats_internal(sni, (int)arg);
		
	appttotal += cumdiff;
	return 0;
}

static int 
_pitenumdel(_hitem *item, void *arg)
{ 
	_del_pit((_pit *)item->val);
	return 0;
}

static int 
_ctxenumdel(_hitem *item, void *arg)
{
	_del_ctx(((_ctx *)item->val) );
	return 0;
}

static int
_ctxenumstat(_hitem *item, void *arg)
{
	char *fname;
	_ctx * ctx;	
	char temp[MAX_LINE_LEN];
	PyObject *buf;
	
	ctx = (_ctx *)item->val;
	
	fname = _item2fname(ctx->last_pit, 1);
	
	if (!fname)
		fname = "N/A";
	
	sprintf(temp, "Thread %ld:%s", ctx->id, fname);
	buf = PyString_FromString(temp);
	if (!buf)
		return 0; // nothing to do. just continue.
	if (PyList_Append((PyObject *)arg, buf) < 0)
		return 0; // nothing to do. just continue.
	
	return 0;
}

static PyObject*
stop(PyObject *self, PyObject *args)
{	
	if (!yapprunning) {
		PyErr_SetString(YappiProfileError, "profiler is not started yet.");
		return NULL;
	}	

	_enum_threads(&_unprofile_thread);
	
	yapprunning = 0;

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject*
clear_stats(PyObject *self, PyObject *args)
{
	if (yapprunning) {
		PyErr_SetString(YappiProfileError, 
			"profiler is running. Stop profiler before clearing stats.");
		return NULL;		
	}
	
	henum(pits, _pitenumdel, NULL);
	htdestroy(pits);
	henum(contexts, _ctxenumdel, NULL);
	htdestroy(contexts);

	fldestroy(flpit);
	fldestroy(flctx);
	yappinitialized = 0;
	yapphavestats = 0;
		
	YMEMCHECK();

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject*
get_stats(PyObject *self, PyObject *args)
{
	char *rstr;
	_statnode *p;
	PyObject *buf,*li;
	int type, order, limit, fcnt;
	char temp[STAT_MAX_FOOTSTR_LEN];
	
	li = buf = NULL;

	if (!yapphavestats) {
		PyErr_SetString(YappiProfileError, "profiler do not have any statistics. not started?");
		goto err;
	}

	if (!PyArg_ParseTuple(args, "iii", &type, &order, &limit)) {
		PyErr_SetString(YappiProfileError, "invalid param to get_stats");
		goto err;
	}
	// sorttype/order/limit is in valid bounds?
	if ((type < 0) || (type > STAT_SORT_TYPE_MAX)) {
		PyErr_SetString(YappiProfileError, "sorttype param for get_stats is out of bounds");
		goto err;
	}
	if ((order < 0) || (order > STAT_SORT_ORDER_MAX)) {
		PyErr_SetString(YappiProfileError, "sortorder param for get_stats is out of bounds");
		goto err;
	}
	if (limit < STAT_SHOW_ALL) {
		PyErr_SetString(YappiProfileError, "limit param for get_stats is out of bounds");
		goto err;
	}

	// enum and present stats in a linked list.(statshead)
	henum(pits, _pitenumstat2, (void *)type);	
	_order_stats_internal(order);
	
	li = PyList_New(0);
	if (!li)
		goto err;	
	if (PyList_Append(li, PyString_FromString(STAT_HEADER_STR)) < 0)
		goto err;
		
	fcnt = 0;
	p = statshead;
	while(p){ 
		// limit reached?
		if (limit != STAT_SHOW_ALL) {
			if (fcnt == limit)
				break;
		}
		rstr = p->it->result;
		buf = PyString_FromString(p->it->result);
		if (!buf)
			goto err;
		if (PyList_Append(li, buf) < 0)
			goto err;
		
		Py_DECREF(buf);
		fcnt++;
		p = p->next;
	}	
	
	if (PyList_Append(li, PyString_FromString(STAT_FOOTER_STR)) < 0)
		goto err;

	henum(contexts, _ctxenumstat, (void *)li);
	
	if (PyList_Append(li, PyString_FromString(STAT_FOOTER_STR2)) < 0)
			goto err;

	if (snprintf(temp, STAT_MAX_FOOTSTR_LEN, "%d functions profiled in %d threads since %s\n\n	yappi overhead: %0.6f/%0.6f(%%%0.6f)\n\n",
			hcount(pits), hcount(contexts), ctime(&yappstarttime),
			yappoverhead * tickfactor(), appttotal * tickfactor(),
			((yappoverhead * tickfactor() )) / (appttotal * tickfactor()) * 100) == -1) {
		PyErr_SetString(YappiProfileError, "output string cannot be formatted correctly. Stack corrupted?");
		goto err;
	}
	if (PyList_Append(li, PyString_FromString(temp)) < 0)
		goto err;

	// clear the internal pit stat items that are generated temporarily.
	_clear_stats_internal();
	
	return li;
err:
	_clear_stats_internal();
	Py_XDECREF(li);
	Py_XDECREF(buf);
	return NULL;
}

static PyObject*
enum_stats(PyObject *self, PyObject *args)
{
	PyObject *enumfn;
	
	if (!yapphavestats) {
		PyErr_SetString(YappiProfileError, "profiler do not have any statistics. not started?");
		return NULL;
	}		

	if (!PyArg_ParseTuple(args, "O", &enumfn)) {
		PyErr_SetString(YappiProfileError, "invalid param to enum_stats");
		return NULL;
	}	

	if (!PyCallable_Check(enumfn)) {
	   	PyErr_SetString(YappiProfileError, "enum function must be callable");
	   	return NULL;
    }
		
	henum(pits, _pitenumstat, enumfn);
	//_print_footer();
		
	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef yappi_methods[] = {
	{"start", start, METH_VARARGS, NULL},
	{"stop", stop, METH_VARARGS, NULL},
	{"get_stats", get_stats, METH_VARARGS, NULL},
	{"enum_stats", enum_stats, METH_VARARGS, NULL},
	{"clear_stats", clear_stats, METH_VARARGS, NULL},
	{"profile_event", profile_event, METH_VARARGS, NULL}, // for internal usage. do not call this.
	{NULL, NULL}      /* sentinel */
};


PyMODINIT_FUNC
init_yappi(void)
{
	PyObject *m, *d;

	m = Py_InitModule("_yappi",  yappi_methods);
	if (m == NULL)
		return;
	d = PyModule_GetDict(m);
	YappiProfileError = PyErr_NewException("_yappi.error", NULL, NULL);
	PyDict_SetItemString(d, "error", YappiProfileError);

	// add int constants
	PyModule_AddIntConstant(m, "SORTTYPE_NAME", STAT_SORT_FUNC_NAME);
	PyModule_AddIntConstant(m, "SORTTYPE_NCALL", STAT_SORT_CALL_COUNT);
	PyModule_AddIntConstant(m, "SORTTYPE_TTOTAL", STAT_SORT_TIME_TOTAL);
	PyModule_AddIntConstant(m, "SORTTYPE_TSUB", STAT_SORT_TIME_SUB);
	PyModule_AddIntConstant(m, "SORTTYPE_TAVG", STAT_SORT_TIME_AVG);
	PyModule_AddIntConstant(m, "SORTORDER_ASCENDING", STAT_SORT_ASCENDING);
	PyModule_AddIntConstant(m, "SORTORDER_DESCENDING", STAT_SORT_DESCENDING);
	PyModule_AddIntConstant(m, "SHOW_ALL", STAT_SHOW_ALL);
	
	// init the profiler memory and internal constants
	yappinitialized = 0;
	yapphavestats = 0;
	yapprunning = 0;

	if (!_init_profiler()) {
		PyErr_SetString(YappiProfileError, "profiler cannot be initialized.");
		return;
	}
}