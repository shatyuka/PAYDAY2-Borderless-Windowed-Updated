#pragma once

// Declare lua_State early, as we want to load the Lua files
// later on for ease of reading
typedef struct lua_State lua_State;

/**
 * Initialize the plugin. This is called when the DLL is first loaded and will only be called
 * once.
 *
 * This may be called before PAYDAY's main method has even been called, so don't expect that
 * anything has already been set up.
 *
 * Note that you should use this instead of DLLMain (Windows) or __attribute(constructor) for
 * any custom initialization logic. This is guaranteed to be called after SuperBLT is set up, and
 * stuff like the logging functions work.
 */
void Plugin_Init();

/**
 * Do state-specific setup. Plugins should no longer register global variables to store their
 * state - rather, they should use `Plugin_PushLua` and return the table from that.
 *
 * The idea here is that, if two different DLLs end up getting included, they shouldn't
 * overwrite each other.
 *
 * This also gives more flexibility to Lua, and allows the global variables to be refractored
 * without modifying the native libraries.
 *
 * Old documentation, kept for archival reasons:
 *
 * Adds this plugin's global variables to a given Lua state. Since PAYDAY 2 loads
 * the Lua state multiple times (such as when changing levels), it's absolutely critical
 * that you keep this in mind. Additionally, more than one of these states can be active
 * at once, so you shouldn't use global variables to store the current state, as mysterious
 * crashes will be a likely result.
 */
void Plugin_Setup_Lua(lua_State *L);

/**
 * Update this plugin. Called once per frame.
 *
 * The main use for this is callbacks - if you need to perform anything asynchronously, ensure
 * that whatever you do you don't call any Lua functions from another thread. Instead, queue up
 * the results and act on them here.
 */
void Plugin_Update();

/**
 * Pushes a Lua value onto the stack, and returns the number of values it wishes to
 * return (see Lua's `lua_pushcfunction` to see how this works).
 *
 * This is the result returned (after `true` to indicate success) from `blt.load_native`.
 * For example:
 *
 * local success, val_1, val_2, val_3 = blt.load_native(...)
 *
 * Then the values `val_1`, `val_2`, and `val_3` would correspond to the 1st, 2nd and
 * 3rd values returned from `Plugin_PushLua`.
 *
 * Please note that this function may be called multiple times, if `blt.load_native` is called
 * multiple times (with the same filename it doesn't load the same DLL again, and it is perfectly
 * valid).
 */
int Plugin_PushLua(lua_State *L);

//////////////////////////
// Logging system
// Including ./logging.h

#if defined(__cplusplus)
// The different logging functions available
enum class LogType {
	LOGGING_FUNC = 0,
	LOGGING_LOG,
	LOGGING_LUA,
	LOGGING_WARN,
	LOGGING_ERROR
};
#define PD2HOOK_LOG_FUNC(msg) PD2HOOK_LOG_LEVEL(msg, LogType::LOGGING_FUNC, __FILE__, 0, FOREGROUND_BLUE, FOREGROUND_GREEN, FOREGROUND_INTENSITY)
#define PD2HOOK_LOG_LOG(msg) PD2HOOK_LOG_LEVEL(msg, LogType::LOGGING_LOG, __FILE__, __LINE__, FOREGROUND_BLUE, FOREGROUND_GREEN, FOREGROUND_INTENSITY)
#define PD2HOOK_LOG_LUA(msg) PD2HOOK_LOG_LEVEL(msg, LogType::LOGGING_LUA, NULL, -1, FOREGROUND_RED, FOREGROUND_BLUE, FOREGROUND_GREEN, FOREGROUND_INTENSITY)
#define PD2HOOK_LOG_WARN(msg) PD2HOOK_LOG_LEVEL(msg, LogType::LOGGING_WARN, __FILE__, __LINE__, FOREGROUND_RED, FOREGROUND_GREEN, FOREGROUND_INTENSITY)
#define PD2HOOK_LOG_ERROR(msg) PD2HOOK_LOG_LEVEL(msg, LogType::LOGGING_ERROR, __FILE__, __LINE__, FOREGROUND_RED, FOREGROUND_INTENSITY)
#define PD2HOOK_LOG_EXCEPTION(e) PD2HOOK_LOG_WARN(e)
#define PD2HOOK_DEBUG_CHECKPOINT PD2HOOK_LOG_LOG("Checkpoint")
#endif
// Done Including ./logging.h

//////////////////////////
// Platform specific stuff

#if defined(WIN32)
#define SBLT_API_EXPORT extern "C" __declspec(dllexport)
#elif defined(__GNUC__)
#define SBLT_API_EXPORT extern "C" __attribute__ ((visibility ("default")))
#endif

/////////////////////////
// Flattened Lua headers
// Including ./lua.h

#ifdef _WIN32

// Define the Lua function pointers
#undef INIT_FUNC
// Including ./sblt_msw32_impl/lua_macros.h

/*
** $Id: lua.h,v 1.218.1.5 2008/08/06 13:30:12 roberto Exp $
** Lua - An Extensible Extension Language
** Lua.org, PUC-Rio, Brazil (http://www.lua.org)
** See Copyright Notice at the end of this file
*/


#ifndef lua_h
#define lua_h

#include <stdarg.h>
#include <stddef.h>


// Including ./sblt_msw32_impl/luaconf.h

/*
** Configuration header.
** Copyright (C) 2005-2017 Mike Pall. See Copyright Notice in luajit.h
*/

#ifndef luaconf_h
#define luaconf_h

#ifndef WINVER
#define WINVER 0x0501
#endif
#include <limits.h>
#include <stddef.h>

/* Default path for loading Lua and C modules with require(). */
#if defined(_WIN32)
/*
** In Windows, any exclamation mark ('!') in the path is replaced by the
** path of the directory of the executable file of the current process.
*/
#define LUA_LDIR	"!\\lua\\"
#define LUA_CDIR	"!\\"
#define LUA_PATH_DEFAULT \
  ".\\?.lua;" LUA_LDIR"?.lua;" LUA_LDIR"?\\init.lua;"
#define LUA_CPATH_DEFAULT \
  ".\\?.dll;" LUA_CDIR"?.dll;" LUA_CDIR"loadall.dll"
#else
/*
** Note to distribution maintainers: do NOT patch the following lines!
** Please read ../doc/install.html#distro and pass PREFIX=/usr instead.
*/
#ifndef LUA_MULTILIB
#define LUA_MULTILIB	"lib"
#endif
#ifndef LUA_LMULTILIB
#define LUA_LMULTILIB	"lib"
#endif
#define LUA_LROOT	"/usr/local"
#define LUA_LUADIR	"/lua/5.1/"
#define LUA_LJDIR	"/luajit-2.0.5/"

#ifdef LUA_ROOT
#define LUA_JROOT	LUA_ROOT
#define LUA_RLDIR	LUA_ROOT "/share" LUA_LUADIR
#define LUA_RCDIR	LUA_ROOT "/" LUA_MULTILIB LUA_LUADIR
#define LUA_RLPATH	";" LUA_RLDIR "?.lua;" LUA_RLDIR "?/init.lua"
#define LUA_RCPATH	";" LUA_RCDIR "?.so"
#else
#define LUA_JROOT	LUA_LROOT
#define LUA_RLPATH
#define LUA_RCPATH
#endif

#define LUA_JPATH	";" LUA_JROOT "/share" LUA_LJDIR "?.lua"
#define LUA_LLDIR	LUA_LROOT "/share" LUA_LUADIR
#define LUA_LCDIR	LUA_LROOT "/" LUA_LMULTILIB LUA_LUADIR
#define LUA_LLPATH	";" LUA_LLDIR "?.lua;" LUA_LLDIR "?/init.lua"
#define LUA_LCPATH1	";" LUA_LCDIR "?.so"
#define LUA_LCPATH2	";" LUA_LCDIR "loadall.so"

#define LUA_PATH_DEFAULT	"./?.lua" LUA_JPATH LUA_LLPATH LUA_RLPATH
#define LUA_CPATH_DEFAULT	"./?.so" LUA_LCPATH1 LUA_RCPATH LUA_LCPATH2
#endif

/* Environment variable names for path overrides and initialization code. */
#define LUA_PATH	"LUA_PATH"
#define LUA_CPATH	"LUA_CPATH"
#define LUA_INIT	"LUA_INIT"

/* Special file system characters. */
#if defined(_WIN32)
#define LUA_DIRSEP	"\\"
#else
#define LUA_DIRSEP	"/"
#endif
#define LUA_PATHSEP	";"
#define LUA_PATH_MARK	"?"
#define LUA_EXECDIR	"!"
#define LUA_IGMARK	"-"
#define LUA_PATH_CONFIG \
  LUA_DIRSEP "\n" LUA_PATHSEP "\n" LUA_PATH_MARK "\n" \
  LUA_EXECDIR "\n" LUA_IGMARK

/* Quoting in error messages. */
#define LUA_QL(x)	"'" x "'"
#define LUA_QS		LUA_QL("%s")

/* Various tunables. */
#define LUAI_MAXSTACK	65500	/* Max. # of stack slots for a thread (<64K). */
#define LUAI_MAXCSTACK	8000	/* Max. # of stack slots for a C func (<10K). */
#define LUAI_GCPAUSE	200	/* Pause GC until memory is at 200%. */
#define LUAI_GCMUL	200	/* Run GC at 200% of allocation speed. */
#define LUA_MAXCAPTURES	32	/* Max. pattern captures. */

/* Compatibility with older library function names. */
#define LUA_COMPAT_MOD		/* OLD: math.mod, NEW: math.fmod */
#define LUA_COMPAT_GFIND	/* OLD: string.gfind, NEW: string.gmatch */

/* Configuration for the frontend (the luajit executable). */
#if defined(luajit_c)
#define LUA_PROGNAME	"luajit"  /* Fallback frontend name. */
#define LUA_PROMPT	"> "	/* Interactive prompt. */
#define LUA_PROMPT2	">> "	/* Continuation prompt. */
#define LUA_MAXINPUT	512	/* Max. input line length. */
#endif

/* Note: changing the following defines breaks the Lua 5.1 ABI. */
#define LUA_INTEGER	ptrdiff_t
#define LUA_IDSIZE	60	/* Size of lua_Debug.short_src. */
/*
** Size of lauxlib and io.* on-stack buffers. Weird workaround to avoid using
** unreasonable amounts of stack space, but still retain ABI compatibility.
** Blame Lua for depending on BUFSIZ in the ABI, blame **** for wrecking it.
*/
#define LUAL_BUFFERSIZE	(BUFSIZ > 16384 ? 8192 : BUFSIZ)

/* The following defines are here only for compatibility with luaconf.h
** from the standard Lua distribution. They must not be changed for LuaJIT.
*/
#define LUA_NUMBER_DOUBLE
#define LUA_NUMBER		double
#define LUAI_UACNUMBER		double
#define LUA_NUMBER_SCAN		"%lf"
#define LUA_NUMBER_FMT		"%.14g"
#define lua_number2str(s, n)	sprintf((s), LUA_NUMBER_FMT, (n))
#define LUAI_MAXNUMBER2STR	32
#define LUA_INTFRMLEN		"l"
#define LUA_INTFRM_T		long

/* Linkage of public API functions. */
#if defined(LUA_BUILD_AS_DLL)
#if defined(LUA_CORE) || defined(LUA_LIB)
#define LUA_API		__declspec(dllexport)
#else
#define LUA_API		__declspec(dllimport)
#endif
#else
#define LUA_API		hello please remove me // extern // Comment out all the API lines
#endif

#define LUALIB_API	LUA_API

/* Support for internal assertions. */
#if defined(LUA_USE_ASSERT) || defined(LUA_USE_APICHECK)
#include <assert.h>
#endif
#ifdef LUA_USE_ASSERT
#define lua_assert(x)		assert(x)
#endif
#ifdef LUA_USE_APICHECK
#define luai_apicheck(L, o)	{ (void)L; assert(o); }
#else
#define luai_apicheck(L, o)	{ (void)L; }
#endif

#endif
// Done Including ./sblt_msw32_impl/luaconf.h


#define LUA_VERSION	"Lua 5.1"
#define LUA_RELEASE	"Lua 5.1.4"
#define LUA_VERSION_NUM	501
#define LUA_COPYRIGHT	"Copyright (C) 1994-2008 Lua.org, PUC-Rio"
#define LUA_AUTHORS	"R. Ierusalimschy, L. H. de Figueiredo & W. Celes"

/* option for multiple returns in `lua_pcall' and `lua_call' */
#define LUA_MULTRET	(-1)


/*
** pseudo-indices
*/
#define LUA_REGISTRYINDEX	(-10000)
#define LUA_ENVIRONINDEX	(-10001)
#define LUA_GLOBALSINDEX	(-10002)
#define lua_upvalueindex(i)	(LUA_GLOBALSINDEX-(i))


/* thread status; 0 is OK */
#define LUA_YIELD	1
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRERR	5


typedef struct lua_State lua_State;

typedef int(*lua_CFunction) (lua_State *L);


/*
** functions that read/write blocks when loading/dumping Lua chunks
*/
typedef const char * (*lua_Reader) (lua_State *L, void *ud, size_t *sz);

typedef int(*lua_Writer) (lua_State *L, const void* p, size_t sz, void* ud);


/*
** prototype for memory-allocation functions
*/
typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);


/*
** basic types
*/
#define LUA_TNONE		(-1)

#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8



/* minimum Lua stack available to a C function */
#define LUA_MINSTACK	20


/*
** generic extra include file
*/
#if defined(LUA_USER_H)
#include LUA_USER_H
#endif


/* type of numbers in Lua */
typedef LUA_NUMBER lua_Number;


/* type for integer functions */
typedef LUA_INTEGER lua_Integer;



/*
** state manipulation
*/
/*
LUA_API lua_State *(lua_newstate)(lua_Alloc f, void *ud);
LUA_API void       (lua_close)(lua_State *L);
LUA_API lua_State *(lua_newthread)(lua_State *L);

LUA_API lua_CFunction(lua_atpanic) (lua_State *L, lua_CFunction panicf);
*/


/*
** basic stack manipulation
*/
/*
LUA_API int   (lua_gettop)(lua_State *L);
LUA_API void  (lua_settop)(lua_State *L, int idx);
LUA_API void  (lua_pushvalue)(lua_State *L, int idx);
LUA_API void  (lua_remove)(lua_State *L, int idx);
LUA_API void  (lua_insert)(lua_State *L, int idx);
LUA_API void  (lua_replace)(lua_State *L, int idx);
LUA_API int   (lua_checkstack)(lua_State *L, int sz);

LUA_API void  (lua_xmove)(lua_State *from, lua_State *to, int n);
*/


/*
** access functions (stack -> C)
*/

/*
LUA_API int             (lua_isnumber)(lua_State *L, int idx);
LUA_API int             (lua_isstring)(lua_State *L, int idx);
LUA_API int             (lua_iscfunction)(lua_State *L, int idx);
LUA_API int             (lua_isuserdata)(lua_State *L, int idx);
LUA_API int             (lua_type)(lua_State *L, int idx);
LUA_API const char     *(lua_typename)(lua_State *L, int tp);

LUA_API int            (lua_equal)(lua_State *L, int idx1, int idx2);
LUA_API int            (lua_rawequal)(lua_State *L, int idx1, int idx2);
LUA_API int            (lua_lessthan)(lua_State *L, int idx1, int idx2);

LUA_API lua_Number(lua_tonumber) (lua_State *L, int idx);
LUA_API lua_Integer(lua_tointeger) (lua_State *L, int idx);
LUA_API int             (lua_toboolean)(lua_State *L, int idx);
LUA_API const char     *(lua_tolstring)(lua_State *L, int idx, size_t *len);
LUA_API size_t(lua_objlen) (lua_State *L, int idx);
LUA_API lua_CFunction(lua_tocfunction) (lua_State *L, int idx);
LUA_API void	       *(lua_touserdata)(lua_State *L, int idx);
LUA_API lua_State      *(lua_tothread)(lua_State *L, int idx);
LUA_API const void     *(lua_topointer)(lua_State *L, int idx);
*/


/*
** push functions (C -> stack)
*/
/*
LUA_API void  (lua_pushnil)(lua_State *L);
LUA_API void  (lua_pushnumber)(lua_State *L, lua_Number n);
LUA_API void  (lua_pushinteger)(lua_State *L, lua_Integer n);
LUA_API void  (lua_pushlstring)(lua_State *L, const char *s, size_t l);
LUA_API void  (lua_pushstring)(lua_State *L, const char *s);
LUA_API const char *(lua_pushvfstring)(lua_State *L, const char *fmt,
	va_list argp);
LUA_API const char *(lua_pushfstring)(lua_State *L, const char *fmt, ...);
LUA_API void  (lua_pushcclosure)(lua_State *L, lua_CFunction fn, int n);
LUA_API void  (lua_pushboolean)(lua_State *L, int b);
LUA_API void  (lua_pushlightuserdata)(lua_State *L, void *p);
LUA_API int   (lua_pushthread)(lua_State *L);
*/


/*
** get functions (Lua -> stack)
*/
/*
LUA_API void  (lua_gettable)(lua_State *L, int idx);
LUA_API void  (lua_getfield)(lua_State *L, int idx, const char *k);
LUA_API void  (lua_rawget)(lua_State *L, int idx);
LUA_API void  (lua_rawgeti)(lua_State *L, int idx, int n);
LUA_API void  (lua_createtable)(lua_State *L, int narr, int nrec);
LUA_API void *(lua_newuserdata)(lua_State *L, size_t sz);
LUA_API int   (lua_getmetatable)(lua_State *L, int objindex);
LUA_API void  (lua_getfenv)(lua_State *L, int idx);
*/


/*
** set functions (stack -> Lua)
*/
/*
LUA_API void  (lua_settable)(lua_State *L, int idx);
LUA_API void  (lua_setfield)(lua_State *L, int idx, const char *k);
LUA_API void  (lua_rawset)(lua_State *L, int idx);
LUA_API void  (lua_rawseti)(lua_State *L, int idx, int n);
LUA_API int   (lua_setmetatable)(lua_State *L, int objindex);
LUA_API int   (lua_setfenv)(lua_State *L, int idx);
*/


/*
** `load' and `call' functions (load and run Lua code)
*/
/*
LUA_API void  (lua_call)(lua_State *L, int nargs, int nresults);
LUA_API int   (lua_pcall)(lua_State *L, int nargs, int nresults, int errfunc);
LUA_API int   (lua_cpcall)(lua_State *L, lua_CFunction func, void *ud);
LUA_API int   (lua_load)(lua_State *L, lua_Reader reader, void *dt,
	const char *chunkname);

LUA_API int (lua_dump)(lua_State *L, lua_Writer writer, void *data);
*/


/*
** coroutine functions
*/
/*
LUA_API int  (lua_yield)(lua_State *L, int nresults);
LUA_API int  (lua_resume)(lua_State *L, int narg);
LUA_API int  (lua_status)(lua_State *L);
*/

/*
** garbage-collection function and options
*/

#define LUA_GCSTOP		0
#define LUA_GCRESTART		1
#define LUA_GCCOLLECT		2
#define LUA_GCCOUNT		3
#define LUA_GCCOUNTB		4
#define LUA_GCSTEP		5
#define LUA_GCSETPAUSE		6
#define LUA_GCSETSTEPMUL	7

//LUA_API int (lua_gc)(lua_State *L, int what, int data);


/*
** miscellaneous functions
*/

/*
LUA_API int   (lua_error)(lua_State *L);

LUA_API int   (lua_next)(lua_State *L, int idx);

LUA_API void  (lua_concat)(lua_State *L, int n);

LUA_API lua_Alloc(lua_getallocf) (lua_State *L, void **ud);
LUA_API void lua_setallocf(lua_State *L, lua_Alloc f, void *ud);
*/



/*
** ===============================================================
** some useful macros
** ===============================================================
*/

#define lua_pop(L,n)		lua_settop(L, -(n)-1)

#define lua_newtable(L)		lua_createtable(L, 0, 0)

#define lua_register(L,n,f) (lua_pushcfunction(L, (f)), lua_setglobal(L, (n)))

#define lua_pushcfunction(L,f)	lua_pushcclosure(L, (f), 0)

#define lua_strlen(L,i)		lua_objlen(L, (i))

#define lua_isfunction(L,n)	(lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L,n)	(lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L,n)	(lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n)		(lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L,n)	(lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isthread(L,n)	(lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L,n)		(lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n)	(lua_type(L, (n)) <= 0)

#define lua_pushliteral(L, s)	\
	lua_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)

#define lua_setglobal(L,s)	lua_setfield(L, LUA_GLOBALSINDEX, (s))
#define lua_getglobal(L,s)	lua_getfield(L, LUA_GLOBALSINDEX, (s))

#define lua_tostring(L,i)	lua_tolstring(L, (i), NULL)



/*
** compatibility macros and functions
*/

#define lua_open()	luaL_newstate()

#define lua_getregistry(L)	lua_pushvalue(L, LUA_REGISTRYINDEX)

#define lua_getgccount(L)	lua_gc(L, LUA_GCCOUNT, 0)

#define lua_Chunkreader		lua_Reader
#define lua_Chunkwriter		lua_Writer


/* hack */
//LUA_API void lua_setlevel(lua_State *from, lua_State *to);


/*
** {======================================================================
** Debug API
** =======================================================================
*/


/*
** Event codes
*/
#define LUA_HOOKCALL	0
#define LUA_HOOKRET	1
#define LUA_HOOKLINE	2
#define LUA_HOOKCOUNT	3
#define LUA_HOOKTAILRET 4


/*
** Event masks
*/
#define LUA_MASKCALL	(1 << LUA_HOOKCALL)
#define LUA_MASKRET	(1 << LUA_HOOKRET)
#define LUA_MASKLINE	(1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT	(1 << LUA_HOOKCOUNT)

typedef struct lua_Debug lua_Debug;  /* activation record */


									 /* Functions to be called by the debuger in specific events */
typedef void(*lua_Hook) (lua_State *L, lua_Debug *ar);


/*
LUA_API int lua_getstack(lua_State *L, int level, lua_Debug *ar);
LUA_API int lua_getinfo(lua_State *L, const char *what, lua_Debug *ar);
LUA_API const char *lua_getlocal(lua_State *L, const lua_Debug *ar, int n);
LUA_API const char *lua_setlocal(lua_State *L, const lua_Debug *ar, int n);
LUA_API const char *lua_getupvalue(lua_State *L, int funcindex, int n);
LUA_API const char *lua_setupvalue(lua_State *L, int funcindex, int n);
LUA_API int lua_sethook(lua_State *L, lua_Hook func, int mask, int count);
LUA_API lua_Hook lua_gethook(lua_State *L);
LUA_API int lua_gethookmask(lua_State *L);
LUA_API int lua_gethookcount(lua_State *L);
*/

/* From Lua 5.2. */
/*
LUA_API void *lua_upvalueid(lua_State *L, int idx, int n);
LUA_API void lua_upvaluejoin(lua_State *L, int idx1, int n1, int idx2, int n2);
LUA_API int lua_loadx(lua_State *L, lua_Reader reader, void *dt,
	const char *chunkname, const char *mode);
*/


struct lua_Debug {
	int event;
	const char *name;	/* (n) */
	const char *namewhat;	/* (n) `global', `local', `field', `method' */
	const char *what;	/* (S) `Lua', `C', `main', `tail' */
	const char *source;	/* (S) */
	int currentline;	/* (l) */
	int nups;		/* (u) number of upvalues */
	int linedefined;	/* (S) */
	int lastlinedefined;	/* (S) */
	char short_src[LUA_IDSIZE]; /* (S) */
								/* private part */
	int i_ci;  /* active function */
};

/* }====================================================================== */


/******************************************************************************
* Copyright (C) 1994-2008 Lua.org, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#endif
// Done Including ./sblt_msw32_impl/lua_macros.h
// Including ./sblt_msw32_impl/misc.h

// Needed for BUFSIZ
#include <stdio.h>

typedef struct luaL_Reg {
	const char *name;
	lua_CFunction func;
} luaL_Reg;

typedef struct luaL_Buffer {
	char *p;			/* current position in buffer */
	int lvl;  /* number of strings in the stack (level) */
	lua_State *L;
	char buffer[LUAL_BUFFERSIZE];
} luaL_Buffer;
// Done Including ./sblt_msw32_impl/misc.h
// Including ./sblt_msw32_impl/fptrs.h
#define CREATE_NORMAL_CALLABLE_SIGNATURE(name, ret, a, b, c, ...) IMPORT_FUNC(name, ret, __VA_ARGS__)

#if defined(__cplusplus)
#define IMPORT_FUNC(name, ret, ...) \
	extern "C" { extern ret (*name)(__VA_ARGS__); }
#else
#define IMPORT_FUNC(name, ret, ...) \
	extern ret (*name)(__VA_ARGS__);
#include <stdbool.h>
#endif

#ifdef INIT_FUNC

#include <vector>
class AutoFuncSetup;
std::vector<AutoFuncSetup*> all_lua_funcs_list;

class AutoFuncSetup {
public:
	AutoFuncSetup(const char *name, void **ptr) : name(name), ptr(ptr) {
		all_lua_funcs_list.push_back(this);
	}
	const char *name;
	void **ptr;
};

#undef IMPORT_FUNC
#define IMPORT_FUNC(name, ret, ...) \
	extern "C" { ret (*name)(__VA_ARGS__) = 0; } \
	AutoFuncSetup name ## _func_setup(#name, (void**) &name);

#endif


IMPORT_FUNC(pd2_log, void, const char* message, int level, const char* file, int line)
IMPORT_FUNC(is_active_state, bool, lua_State *L)
IMPORT_FUNC(luaL_checkstack, void, lua_State *L, int sz, const char *msg)

CREATE_NORMAL_CALLABLE_SIGNATURE(lua_call, void, "\x8B\x44\x24\x08\x8B\x54\x24\x04\xFF\x44\x24\x0C\x8D\x0C\xC5\x00", "xxxxxxxxxxxxxxxx", 0, lua_State*, int, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pcall, int, "\x8B\x54\x24\x04\x8B\x4C\x24\x10\x53\x56\x8B\x72\x08\x8A", "xxxxxxxxxxxxxx", 0, lua_State*, int, int, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_gettop, int, "\x8B\x4C\x24\x04\x8B\x41\x14\x2B\x41\x10\xC1\xF8\x03\xC3", "xxxxxxxxxxxxxx", 0, lua_State*)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_settop, void, "\x8B\x44\x24\x08\x85\xC0\x78\x5B\x53\x56\x8B\x74\x24\x0C\x57\x8B", "xxxxxxxxxxxxxxxx", 0, lua_State*, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_toboolean, int, "\xFF\x74\x24\x08\xFF\x74\x24\x08\xE8\x00\x00\x00\x00\x83\xC4\x08\x83\x78\x04\xFE\x1B\xC0\xF7\xD8\xC3", "xxxxxxxxx????xxxxxxxxxxxx", 0, lua_State*, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_tointeger, size_t, "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x08\xFF\x75\x0C\xFF\x75\x08\xE8\x00\x00\x00\x00\x8B\x48\x04\x83\xC4\x08\x83\xF9\xF2\x73\x0C\xF2\x0F\x10\x00\xF2\x0F\x2C\xC0\x8B\xE5\x5D\xC3\x83\xF9\xFB\x75\x26", "xxxxxxxxxxxxxxxx????xxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_tonumber, lua_Number, "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x08\xFF\x75\x0C\xFF\x75\x08\xE8\x00\x00\x00\x00\x8B\x48\x04\x83\xC4\x08\x83\xF9\xF2\x77\x06\xDD", "xxxxxxxxxxxxxxxx????xxxxxxxxxxxx", 0, lua_State*, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_tolstring, const char*, "\x83\xEC\x24\xA1\x00\x00\x00\x00\x33\xC4\x89\x44\x24\x20\x53\x8B\x5C\x24\x2C\x56\x8B\x74\x24\x34", "xxxx????xxxxxxxxxxxxxxxx", 0, lua_State*, int, size_t*)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_objlen, size_t, "\x83\xEC\x24\xA1\x00\x00\x00\x00\x33\xC4\x89\x44\x24\x20\x8B\x44", "xxxx????xxxxxxxx", 0, lua_State*, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_touserdata, void*, "\xFF\x74\x24\x08\xFF\x74\x24\x08\xE8\x83\xD2\xFE\xFF******\x83\xF9\xF3\x75\x06\x8B\x00\x83\xC0\x18\xC3\x83\xF9", "xxxxxxxxxxxxx??????xxxxxxxxxxxxx", 0, lua_State*, int)
// This is actually luaL_loadfilex() (as per Lua 5.2) now. The new parameter corresponds to mode, and specifying NULL causes Lua
// to default to "bt", i.e. 'binary and text'
// https://www.lua.org/manual/5.2/manual.html#luaL_loadfilex
// https://www.lua.org/manual/5.2/manual.html#pdf-load
CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_loadfilex, int, "\x81\xEC\x08\x02\x00\x00\xA1\x00\x00\x00\x00\x33\xC4\x89\x84\x24", "xxxxxxx????xxxxx", 0, lua_State*, const char*, const char*)
CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_loadstring, int, "\x8B\x54\x24\x08\x83\xEC\x08\x8B\xC2\x56\x8D\x70\x01\x8D\x49\x00", "xxxxxxxxxxxxxxxx", 0, lua_State*, const char*)
//CREATE_NORMAL_CALLABLE_SIGNATURE(lua_load, int, "\x8B\x4C\x24\x10\x33\xD2\x83\xEC\x18\x3B\xCA", "xxxxxxxxxxx", 0, lua_State*, lua_Reader, void*, const char*)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_getfield, void, "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x08\x56\x8B\x75\x08\x57\xFF\x75", "xxxxxxxxxxxxxxxx", 0, lua_State*, int, const char*)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_setfield, void, "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x10\x56\x8B\x75\x08\x57\xFF\x75", "xxxxxxxxxxxxxxxx", 0, lua_State*, int, const char*)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_createtable, void, "\x56\x8B\x74\x24\x08\x8B\x4E\x08\x8B\x41\x14\x3B\x41\x18\x72\x07\x8B\xCE\xE8\x00\x00\x00\x00\x8B\x44\x24\x10\x85\xC0\x74\x12\x83", "xxxxxxxxxxxxxxxxxxx????xxxxxxxxx", 0, lua_State*, int, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_newuserdata, void*, "\x56\x8B\x74\x24\x08\x8B\x4E\x08\x8B\x41\x14\x3B\x41\x18\x72\x07\x8B\xCE\xE8\x00\x00\x00\x00\x8B\x4C\x24\x0C\x81\xF9\x00\xFF\xFF", "xxxxxxxxxxxxxxxxxxx????xxxxxxxxx", 0, lua_State*, size_t)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_insert, void, "\x8B\x4C\x24\x08\x56\x57\x85\xC9\x7E\x21\x8B\x54\x24\x0C\x8D\x71", "xxxxxxxxxxxxxxxx", 0, lua_State*, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_replace, void, "\x56\x8B\x74\x24\x08\xFF\x74\x24\x0C\x8B\x46\x14\x83\xE8\x08\x50\x56\xE8\x00\x00\x00\x00\x83\x46\x14\xF8\x83\xC4\x0C\x5E\xC3", "xxxxxxxxxxxxxxxxxx????xxxxxxxxx", 0, lua_State*, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_remove, void, "\x8B\x4C\x24\x08\x56\x57\x85\xC9\x7E\x21\x8B\x7C\x24\x0C\x8B\x47\x10\x8B\x57\x14\x8D\x77\x14\x8D\x04\xC8\x83\xC0\xF8\x3B\xC2\x72", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_newstate, lua_State*, "\x53\x8B\x5C\x24\x0C\x55\x8B\x6C\x24\x0C\x56\x57\x68\x40\x10\x00\x00\x6A\x00\x6A\x00\x53\xFF\xD5\x8B\xF0\x83\xC4\x10\x8D\x7E\x30", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_Alloc, void*)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_close, void, "\x8B\x44\x24\x04\x53\x56\x57\x8B\x78\x08\x8B\x77\x74\x56\xE8", "xxxxxxxxxxxxxxx", 0, lua_State*)

// Reviving lua_settable() since the function exists again, and because the Crimefest 2015 alternative relied upon internal Lua
// VM functions, which do not apply to LuaJIT
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_gettable, void, "\x56\xFF\x74\x24\x0C\x8B\x74\x24\x0C\x56\xE8\x00\x00\x00\x00\x8B\x4E\x14\x83\xE9\x08\x51\x50\x56\xE8\x00\x00\x00\x00\x8B\xD0\x83", "xxxxxxxxxxx????xxxxxxxxxx????xxx", 0, lua_State*, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_settable, void, "\x56\xFF\x74\x24\x0C\x8B\x74\x24\x0C\x56\xE8\x00\x00\x00\x00\x8B\x4E\x14\x83\xE9\x10\x51\x50\x56", "xxxxxxxxxxx????xxxxxxxxx", 0, lua_State*, int)
//                                                                            that 0x08 vs 0x10 is where gettable and settable differ  ^^^^
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_setmetatable, int, "\x53\x55\x56\x57\xFF\x74\x24\x18\x8B\x7C\x24\x18\x57\xE8\x00\x00\x00\x00\x8B\x77\x14\x83\xC4\x08", "xxxxxxxxxxxxxx????xxxxxx", 0, lua_State*, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_getmetatable, int, "\x56\xFF\x74\x24\x0C\x8B\x74\x24\x0C\x56\xE8\x00\x00\x00\x00\x8B\x48\x04\x83\xC4\x08\x83\xF9\xF4\x75\x07\x8B\x00\x8B\x48\x10\xEB", "xxxxxxxxxxx????xxxxxxxxxxxxxxxxx", 0, lua_State*, int)

CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushnumber, void, "\x8B\x4C\x24\x04\xF2\x0F\x10\x44\x24\x08\x8B\x41\x14\xF2\x0F\x11\x00\x8B\x51\x14\xF2\x0F\x10\x02\x66\x0F\x2E\xC0\x9F\xF6\xC4\x44", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, lua_Number)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushinteger, void, "\x66\x0F\x6E\x44\x24\x08\x8B\x4C\x24\x04\xF3\x0F\xE6\xC0\x8B\x41", "xxxxxxxxxxxxxxxx", 0, lua_State*, size_t)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushboolean, void, "\x8B\x4C\x24\x04\x33\xC0\x39\x44\x24\x08\xBA\xFE\xFF\xFF\xFF\x0F", "xxxxxxxxxxxxxxxx", 0, lua_State*, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushcclosure, void, "\x56\x8B\x74\x24\x08\x8B\x4E\x08\x8B\x41\x14\x3B\x41\x18\x72\x07\x8B\xCE\xE8\x00\x00\x00\x00\x8B\x46\x10\x8B\x40\xF8\x80\x78\x05", "xxxxxxxxxxxxxxxxxxx????xxxxxxxxx", 0, lua_State*, lua_CFunction, int);
// lua_pushstring()'s signature was found before lua_pushlstring()'s, so I'm leaving it here now since it's valid anyway
// It was used as a quick and dirty - and broken - workaround since most lua_pushlstring() calls are inlined, but it ended up
// breaking HTTP downloads of zip archives due to its sensitivity to premature null characters. A non-inlined signature for
// lua_pushlstring() was found by cross-referencing the string 'loaders' to lj_cf_package_require(), which is part of LuaJIT
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushlstring, void, "\x56\x8B\x74\x24\x08\x8B\x4E\x08\x8B\x41\x14\x3B\x41\x18\x72\x07\x8B\xCE\xE8\x00\x00\x00\x00\xFF", "xxxxxxxxxxxxxxxxxxx????x", 0, lua_State*, const char*, size_t)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushstring, void, "\x56\x8B\x74\x24\x08\x57\x8B\x7C\x24\x10\x85\xFF\x75\x0C\x8B\x46", "xxxxxxxxxxxxxxxx", 0, lua_State*, const char*)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushfstring, const char*, "\x56\x8B\x74\x24\x08\x8B\x4E\x08\x8B\x41\x14\x3B\x41\x18\x72\x07\x8B\xCE\xE8\x00\x00\x00\x00\x8D\x44\x24\x10\x50\xFF\x74\x24\x10", "xxxxxxxxxxxxxxxxxxx????xxxxxxxxx", 0, lua_State*, const char*, ...)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_checkstack, int, "\x8B\x54\x24\x08\x81\xFA\x40\x1F\x00\x00\x7F\x38\x8B\x4C\x24\x04", "xxxxxxxxxxxxxxxx", 0, lua_State*, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushvalue, void, "\x56\x57\xFF\x74\x24\x10\x8B\x7C\x24\x10\x57\xE8\x00\x00\x00\x00\x8B\x10\x8B\x77\x14\x83\xC4\x08\x89\x16\x8B\x40\x04\x89\x46\x04", "xxxxxxxxxxxx????xxxxxxxxxxxxxxxx", 0, lua_State*, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushnil, void, "\x8B\x4C\x24\x04\x8B\x41\x14\xC7\x40\x04\xFF\xFF\xFF\xFF\x83\x41\x14\x08\x8B\x41\x14\x3B\x41\x18\x0F\x83\x00\x00\x00\x00\xC3", "xxxxxxxxxxxxxxxxxxxxxxxxxx????x", 0, lua_State*)

// luaI_openlib() is really luaL_openlib(), see lauxlib.h in Lua 5.1's source code
CREATE_NORMAL_CALLABLE_SIGNATURE(luaI_openlib, void, "\x55\x8B\xEC\x83\xE4\xF8\xF2\x0F\x10\x00\x00\x00\x00\x00\x83\xEC\x08\x56\x8B\x75\x08\x57\x8B\x46\x14\xF2\x0F", "xxxxxxxxx?????xxxxxxxxxxxxx", 0, lua_State*, const char*, const luaL_Reg*, int)
CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_ref, int, "\x55\x8B\xEC\x83\xE4\xF8\x8B\x55\x0C\x83\xEC\x08\x8D\x82\x0F\x27\x00\x00\x56\x8B\x75\x08\x57", "xxxxxxxxxxxxxxxx", 0, lua_State*, int);
// Reviving lua_rawgeti() since the function exists again, and because the Crimefest 2015 alternative relied upon internal Lua VM
// functions, which do not apply to LuaJIT
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_rawget, void, "\x56\x8B\x74\x24\x08\x57\xFF\x74\x24\x10\x56\xE8\x00\x00\x00\x00\x8B\x7E\x14\x83\xC7\xF8\x57\xFF\x30\x56\xE8\x00\x00\x00\x00\x8B", "xxxxxxxxxxxx????xxxxxxxxxxx????x", 0, lua_State*, int);
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_rawset, void, "\x53\x56\x8B\x74\x24\x0C\x57\xFF\x74\x24\x14\x56\xE8\x00\x00\x00\x00\x8B\x7E\x14\x8B\x18\x83\xEF\x10\x57\x53\x56\xE8", "xxxxxxxxxxxxx????xxxxxxxxxxxx", 0, lua_State*, int);
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_rawgeti, void, "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x0C\x56\xFF\x75\x0C\x8B\x75\x08\x56\xE8", "xxxxxxxxxxxxxxxxxx", 0, lua_State*, int, int);
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_rawseti, void, "\x53\x56\x8B\x74\x24\x0C\x57\xFF\x74\x24\x14\x56\xE8\x00\x00\x00\x00\x8B\x38\x8B", "xxxxxxxxxxxxx????xxx", 0, lua_State*, int, int);
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_type, int, "\x56\xFF\x74\x24\x0C\x8B\x74\x24\x0C\x56\xE8\x00\x00\x00\x00\x8B\xD0\x83\xC4\x08\x8B\x4A\x04\x83\xF9\xF2\x77\x07\xB8\x03\x00\x00\x00\x5E\xC3\x8B\x46\x08\x05\x90", "xxxxxxxxxxx????xxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int);
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_typename, const char*, "\x8B\x44\x24\x08\x8B\x00\x00\x00\x00\x00\x00\xC3\xCC", "xxxxx??????xx", 0, lua_State*, int);
CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_unref, void, "\x53\x8B\x5C\x24\x10\x85\xDB\x78\x67\x56\x8B\x74\x24\x0C\x57\x8B", "xxxxxxxxxxxxxxxx", 0, lua_State*, int, int);
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_equal, int, "\x56\x8B\x74\x24\x08\x57\xFF\x74\x24\x10\x56\xE8\x00\x00\x00\x00\xFF\x74\x24\x1C\x8B\xF8\x56\xE8\x00\x00\x00\x00\x8B\x57\x04\x83", "xxxxxxxxxxxx????xxxxxxxx????xxxx", 0, lua_State*, int, int)

CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_newmetatable, int, "\x8B\x54\x24\x08\x53\x56\x8B\x74\x24\x0C\x8B\xCA\x8B\x46\x08\x57", "xxxxxxxxxxxxxxxx", 0, lua_State*, const char*)
CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_checkudata, int, "\x56\x8B\x74\x24\x08\x57\xFF\x74\x24\x10\x56\xE8\x00\x00\x00\x00\x83\xC4\x08\x83\x78\x04\xF3\x75\x49\x8B\x54\x24\x14\x8B\x38\x8B", "xxxxxxxxxxxx????xxxxxxxxxxxxxxxx", 0, lua_State*, int, const char*)
CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_error, int, "\x8D\x44\x24\x0C\x50\xFF\x74\x24\x0C\xFF\x74\x24\x0C\xE8\x00\x00\x00\x00\x83\xC4\x0C\x50\xFF\x74\x24\x08\xE8", "xxxxxxxxxxxxxx????xxxxxxxxx", 0, lua_State*, const char*, ...)
CREATE_NORMAL_CALLABLE_SIGNATURE(lua_error, int, "\x56\x8B\x74\x24\x08\x57\x56\xE8\x00\x00\x00\x00\x83\xC4\x04\x85\xC0\x74\x4A\x8B\x4E\x1C\x8B\x7E\x14\x03\xC8\x8B\x46\x08\x83\xA0", "xxxxxxxx????xxxxxxxxxxxxxxxxxxxx", 0, lua_State*)

#undef IMPORT_FUNC

#ifndef INIT_FUNC
#define PD2HOOK_LOG_LEVEL(msg, level, file, line, ...) pd2_log(msg, (int) level, file, line)
#endif
// Done Including ./sblt_msw32_impl/fptrs.h
// Including ./sblt_msw32_impl/lauxlib.h

// Check and Opt functions, from LAuxLib

#define lua_isnumber(L,n)		(lua_type(L, (n)) == LUA_TNUMBER)
#define lua_isstring(L,n)		(lua_type(L, (n)) == LUA_TSTRING || lua_isnumber(L,n))

#define luaL_getmetatable(L,n) (lua_getfield(L, LUA_REGISTRYINDEX, (n)))

#define luaL_argcheck(L, cond,numarg,extramsg)  \
                ((void)((cond) || luaL_argerror(L, (numarg), (extramsg))))
#define luaL_checkstring(L,n)   (luaL_checklstring(L, (n), NULL))
#define luaL_optstring(L,n,d)   (luaL_optlstring(L, (n), (d), NULL))
#define luaL_checkint(L,n)      ((int)luaL_checkinteger(L, (n)))
#define luaL_optint(L,n,d)      ((int)luaL_optinteger(L, (n), (d)))
#define luaL_checklong(L,n)     ((long)luaL_checkinteger(L, (n)))
#define luaL_optlong(L,n,d)     ((long)luaL_optinteger(L, (n), (d)))

#define luaL_opt(L,f,n,d)       (lua_isnoneornil(L,(n)) ? (d) : f(L,(n)))
#define luaL_typename(L,i)      lua_typename(L, lua_type(L,(i)))

#if defined(__cplusplus)
extern "C" {
#endif

int luaL_argerror(lua_State *L, int narg, const char *extramsg);

int luaL_checkoption(lua_State *L, int narg, const char *def, const char *const lst[]);

int luaL_typerror(lua_State *L, int narg, const char *tname);

void luaL_checktype(lua_State *L, int narg, int t);

void luaL_checkany(lua_State *L, int narg);

const char *luaL_checklstring(lua_State *L, int narg, size_t *len);

const char *luaL_optlstring(lua_State *L, int narg,
	const char *def, size_t *len);

lua_Number luaL_checknumber(lua_State *L, int narg);

lua_Number luaL_optnumber(lua_State *L, int narg, lua_Number def);

lua_Integer luaL_checkinteger(lua_State *L, int narg);

lua_Integer luaL_optinteger(lua_State *L, int narg,
	lua_Integer def);

#if defined(__cplusplus)
}
#endif
// Done Including ./sblt_msw32_impl/lauxlib.h

#elif defined(__GNUC__)

// Including ./unix_impl/unix.h

// Including ./unix_impl/lua.hh

#include <cstddef>

#define LUA_NUMBER double

/* Note: changing the following defines breaks the Lua 5.1 ABI. */
#define LUA_INTEGER  ptrdiff_t
#define LUA_IDSIZE   60 /* Size of lua_Debug.short_src. */

#define LUAL_BUFFERSIZE (BUFSIZ > 16384 ? 8192 : BUFSIZ)

extern "C" {
   #define LUA_API extern
   #define LUALIB_API LUA_API

// Including ./unix_impl/pure_lua.h
/*
** $Id: lua.h,v 1.218.1.5 2008/08/06 13:30:12 roberto Exp $
** Lua - An Extensible Extension Language
** Lua.org, PUC-Rio, Brazil (http://www.lua.org)
** See Copyright Notice at the end of this file
*/


#ifndef lua_h
#define lua_h

#include <stdarg.h>
#include <stddef.h>


// #include "luaconf.h"


#define LUA_VERSION	"Lua 5.1"
#define LUA_RELEASE	"Lua 5.1.4"
#define LUA_VERSION_NUM	501
#define LUA_COPYRIGHT	"Copyright (C) 1994-2008 Lua.org, PUC-Rio"
#define LUA_AUTHORS	"R. Ierusalimschy, L. H. de Figueiredo & W. Celes"


/* mark for precompiled code (`<esc>Lua') */
#define	LUA_SIGNATURE	"\033Lua"

/* option for multiple returns in `lua_pcall' and `lua_call' */
#define LUA_MULTRET	(-1)


/*
** pseudo-indices
*/
#define LUA_REGISTRYINDEX	(-10000)
#define LUA_ENVIRONINDEX	(-10001)
#define LUA_GLOBALSINDEX	(-10002)
#define lua_upvalueindex(i)	(LUA_GLOBALSINDEX-(i))


/* thread status; 0 is OK */
#define LUA_YIELD	1
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRERR	5


typedef struct lua_State lua_State;

typedef int (*lua_CFunction) (lua_State *L);


/*
** functions that read/write blocks when loading/dumping Lua chunks
*/
typedef const char * (*lua_Reader) (lua_State *L, void *ud, size_t *sz);

typedef int (*lua_Writer) (lua_State *L, const void* p, size_t sz, void* ud);


/*
** prototype for memory-allocation functions
*/
typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);


/*
** basic types
*/
#define LUA_TNONE		(-1)

#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8



/* minimum Lua stack available to a C function */
#define LUA_MINSTACK	20


/*
** generic extra include file
*/
#if defined(LUA_USER_H)
#include LUA_USER_H
#endif


/* type of numbers in Lua */
typedef LUA_NUMBER lua_Number;


/* type for integer functions */
typedef LUA_INTEGER lua_Integer;



/*
** state manipulation
*/
LUA_API lua_State *(lua_newstate) (lua_Alloc f, void *ud);
LUA_API void       (lua_close) (lua_State *L);
LUA_API lua_State *(lua_newthread) (lua_State *L);

LUA_API lua_CFunction (lua_atpanic) (lua_State *L, lua_CFunction panicf);


/*
** basic stack manipulation
*/
LUA_API int   (lua_gettop) (lua_State *L);
LUA_API void  (lua_settop) (lua_State *L, int idx);
LUA_API void  (lua_pushvalue) (lua_State *L, int idx);
LUA_API void  (lua_remove) (lua_State *L, int idx);
LUA_API void  (lua_insert) (lua_State *L, int idx);
LUA_API void  (lua_replace) (lua_State *L, int idx);
LUA_API int   (lua_checkstack) (lua_State *L, int sz);

LUA_API void  (lua_xmove) (lua_State *from, lua_State *to, int n);


/*
** access functions (stack -> C)
*/

LUA_API int             (lua_isnumber) (lua_State *L, int idx);
LUA_API int             (lua_isstring) (lua_State *L, int idx);
LUA_API int             (lua_iscfunction) (lua_State *L, int idx);
LUA_API int             (lua_isuserdata) (lua_State *L, int idx);
LUA_API int             (lua_type) (lua_State *L, int idx);
LUA_API const char     *(lua_typename) (lua_State *L, int tp);

LUA_API int            (lua_equal) (lua_State *L, int idx1, int idx2);
LUA_API int            (lua_rawequal) (lua_State *L, int idx1, int idx2);
LUA_API int            (lua_lessthan) (lua_State *L, int idx1, int idx2);

LUA_API lua_Number      (lua_tonumber) (lua_State *L, int idx);
LUA_API lua_Integer     (lua_tointeger) (lua_State *L, int idx);
LUA_API int             (lua_toboolean) (lua_State *L, int idx);
LUA_API const char     *(lua_tolstring) (lua_State *L, int idx, size_t *len);
LUA_API size_t          (lua_objlen) (lua_State *L, int idx);
LUA_API lua_CFunction   (lua_tocfunction) (lua_State *L, int idx);
LUA_API void	       *(lua_touserdata) (lua_State *L, int idx);
LUA_API lua_State      *(lua_tothread) (lua_State *L, int idx);
LUA_API const void     *(lua_topointer) (lua_State *L, int idx);


/*
** push functions (C -> stack)
*/
LUA_API void  (lua_pushnil) (lua_State *L);
LUA_API void  (lua_pushnumber) (lua_State *L, lua_Number n);
LUA_API void  (lua_pushinteger) (lua_State *L, lua_Integer n);
LUA_API void  (lua_pushlstring) (lua_State *L, const char *s, size_t l);
LUA_API void  (lua_pushstring) (lua_State *L, const char *s);
LUA_API const char *(lua_pushvfstring) (lua_State *L, const char *fmt,
                                                      va_list argp);
LUA_API const char *(lua_pushfstring) (lua_State *L, const char *fmt, ...);
LUA_API void  (lua_pushcclosure) (lua_State *L, lua_CFunction fn, int n);
LUA_API void  (lua_pushboolean) (lua_State *L, int b);
LUA_API void  (lua_pushlightuserdata) (lua_State *L, void *p);
LUA_API int   (lua_pushthread) (lua_State *L);


/*
** get functions (Lua -> stack)
*/
LUA_API void  (lua_gettable) (lua_State *L, int idx);
LUA_API void  (lua_getfield) (lua_State *L, int idx, const char *k);
LUA_API void  (lua_rawget) (lua_State *L, int idx);
LUA_API void  (lua_rawgeti) (lua_State *L, int idx, int n);
LUA_API void  (lua_createtable) (lua_State *L, int narr, int nrec);
LUA_API void *(lua_newuserdata) (lua_State *L, size_t sz);
LUA_API int   (lua_getmetatable) (lua_State *L, int objindex);
LUA_API void  (lua_getfenv) (lua_State *L, int idx);


/*
** set functions (stack -> Lua)
*/
LUA_API void  (lua_settable) (lua_State *L, int idx);
LUA_API void  (lua_setfield) (lua_State *L, int idx, const char *k);
LUA_API void  (lua_rawset) (lua_State *L, int idx);
LUA_API void  (lua_rawseti) (lua_State *L, int idx, int n);
LUA_API int   (lua_setmetatable) (lua_State *L, int objindex);
LUA_API int   (lua_setfenv) (lua_State *L, int idx);


/*
** `load' and `call' functions (load and run Lua code)
*/
LUA_API void  (lua_call) (lua_State *L, int nargs, int nresults);
LUA_API int   (lua_pcall) (lua_State *L, int nargs, int nresults, int errfunc);
LUA_API int   (lua_cpcall) (lua_State *L, lua_CFunction func, void *ud);
LUA_API int   (lua_load) (lua_State *L, lua_Reader reader, void *dt,
                                        const char *chunkname);

LUA_API int (lua_dump) (lua_State *L, lua_Writer writer, void *data);


/*
** coroutine functions
*/
LUA_API int  (lua_yield) (lua_State *L, int nresults);
LUA_API int  (lua_resume) (lua_State *L, int narg);
LUA_API int  (lua_status) (lua_State *L);

/*
** garbage-collection function and options
*/

#define LUA_GCSTOP		0
#define LUA_GCRESTART		1
#define LUA_GCCOLLECT		2
#define LUA_GCCOUNT		3
#define LUA_GCCOUNTB		4
#define LUA_GCSTEP		5
#define LUA_GCSETPAUSE		6
#define LUA_GCSETSTEPMUL	7

LUA_API int (lua_gc) (lua_State *L, int what, int data);


/*
** miscellaneous functions
*/

LUA_API int   (lua_error) (lua_State *L);

LUA_API int   (lua_next) (lua_State *L, int idx);

LUA_API void  (lua_concat) (lua_State *L, int n);

LUA_API lua_Alloc (lua_getallocf) (lua_State *L, void **ud);
LUA_API void lua_setallocf (lua_State *L, lua_Alloc f, void *ud);



/*
** ===============================================================
** some useful macros
** ===============================================================
*/

#define lua_pop(L,n)		lua_settop(L, -(n)-1)

#define lua_newtable(L)		lua_createtable(L, 0, 0)

#define lua_register(L,n,f) (lua_pushcfunction(L, (f)), lua_setglobal(L, (n)))

#define lua_pushcfunction(L,f)	lua_pushcclosure(L, (f), 0)

#define lua_strlen(L,i)		lua_objlen(L, (i))

#define lua_isfunction(L,n)	(lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L,n)	(lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L,n)	(lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n)		(lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L,n)	(lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isthread(L,n)	(lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L,n)		(lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n)	(lua_type(L, (n)) <= 0)

#define lua_pushliteral(L, s)	\
	lua_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)

#define lua_setglobal(L,s)	lua_setfield(L, LUA_GLOBALSINDEX, (s))
#define lua_getglobal(L,s)	lua_getfield(L, LUA_GLOBALSINDEX, (s))

#define lua_tostring(L,i)	lua_tolstring(L, (i), NULL)



/*
** compatibility macros and functions
*/

#define lua_open()	luaL_newstate()

#define lua_getregistry(L)	lua_pushvalue(L, LUA_REGISTRYINDEX)

#define lua_getgccount(L)	lua_gc(L, LUA_GCCOUNT, 0)

#define lua_Chunkreader		lua_Reader
#define lua_Chunkwriter		lua_Writer


/* hack */
LUA_API void lua_setlevel	(lua_State *from, lua_State *to);


/*
** {======================================================================
** Debug API
** =======================================================================
*/


/*
** Event codes
*/
#define LUA_HOOKCALL	0
#define LUA_HOOKRET	1
#define LUA_HOOKLINE	2
#define LUA_HOOKCOUNT	3
#define LUA_HOOKTAILRET 4


/*
** Event masks
*/
#define LUA_MASKCALL	(1 << LUA_HOOKCALL)
#define LUA_MASKRET	(1 << LUA_HOOKRET)
#define LUA_MASKLINE	(1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT	(1 << LUA_HOOKCOUNT)

typedef struct lua_Debug lua_Debug;  /* activation record */


/* Functions to be called by the debuger in specific events */
typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar);


LUA_API int lua_getstack (lua_State *L, int level, lua_Debug *ar);
LUA_API int lua_getinfo (lua_State *L, const char *what, lua_Debug *ar);
LUA_API const char *lua_getlocal (lua_State *L, const lua_Debug *ar, int n);
LUA_API const char *lua_setlocal (lua_State *L, const lua_Debug *ar, int n);
LUA_API const char *lua_getupvalue (lua_State *L, int funcindex, int n);
LUA_API const char *lua_setupvalue (lua_State *L, int funcindex, int n);
LUA_API int lua_sethook (lua_State *L, lua_Hook func, int mask, int count);
LUA_API lua_Hook lua_gethook (lua_State *L);
LUA_API int lua_gethookmask (lua_State *L);
LUA_API int lua_gethookcount (lua_State *L);

/* From Lua 5.2. */
LUA_API void *lua_upvalueid (lua_State *L, int idx, int n);
LUA_API void lua_upvaluejoin (lua_State *L, int idx1, int n1, int idx2, int n2);
LUA_API int lua_loadx (lua_State *L, lua_Reader reader, void *dt,
		       const char *chunkname, const char *mode);


struct lua_Debug {
  int event;
  const char *name;	/* (n) */
  const char *namewhat;	/* (n) `global', `local', `field', `method' */
  const char *what;	/* (S) `Lua', `C', `main', `tail' */
  const char *source;	/* (S) */
  int currentline;	/* (l) */
  int nups;		/* (u) number of upvalues */
  int linedefined;	/* (S) */
  int lastlinedefined;	/* (S) */
  char short_src[LUA_IDSIZE]; /* (S) */
  /* private part */
  int i_ci;  /* active function */
};

/* }====================================================================== */


/******************************************************************************
* Copyright (C) 1994-2008 Lua.org, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/


#endif

// Done Including ./unix_impl/pure_lua.h
// Including ./unix_impl/pure_lauxlib.h
/*
** $Id: lauxlib.h,v 1.88.1.1 2007/12/27 13:02:25 roberto Exp $
** Auxiliary functions for building Lua libraries
** See Copyright Notice in lua.h
*/


#ifndef lauxlib_h
#define lauxlib_h


#include <stddef.h>
#include <stdio.h>

// #include "lua.h"


#define luaL_getn(L,i)          ((int)lua_objlen(L, i))
#define luaL_setn(L,i,j)        ((void)0)  /* no op! */

/* extra error code for `luaL_load' */
#define LUA_ERRFILE     (LUA_ERRERR+1)

typedef struct luaL_Reg {
  const char *name;
  lua_CFunction func;
} luaL_Reg;

LUALIB_API void (luaL_openlib) (lua_State *L, const char *libname,
                                const luaL_Reg *l, int nup);
LUALIB_API void (luaL_register) (lua_State *L, const char *libname,
                                const luaL_Reg *l);
LUALIB_API int (luaL_getmetafield) (lua_State *L, int obj, const char *e);
LUALIB_API int (luaL_callmeta) (lua_State *L, int obj, const char *e);
LUALIB_API int (luaL_typerror) (lua_State *L, int narg, const char *tname);
LUALIB_API int (luaL_argerror) (lua_State *L, int numarg, const char *extramsg);
LUALIB_API const char *(luaL_checklstring) (lua_State *L, int numArg,
                                                          size_t *l);
LUALIB_API const char *(luaL_optlstring) (lua_State *L, int numArg,
                                          const char *def, size_t *l);
LUALIB_API lua_Number (luaL_checknumber) (lua_State *L, int numArg);
LUALIB_API lua_Number (luaL_optnumber) (lua_State *L, int nArg, lua_Number def);

LUALIB_API lua_Integer (luaL_checkinteger) (lua_State *L, int numArg);
LUALIB_API lua_Integer (luaL_optinteger) (lua_State *L, int nArg,
                                          lua_Integer def);

LUALIB_API void (luaL_checkstack) (lua_State *L, int sz, const char *msg);
LUALIB_API void (luaL_checktype) (lua_State *L, int narg, int t);
LUALIB_API void (luaL_checkany) (lua_State *L, int narg);

LUALIB_API int   (luaL_newmetatable) (lua_State *L, const char *tname);
LUALIB_API void *(luaL_checkudata) (lua_State *L, int ud, const char *tname);

LUALIB_API void (luaL_where) (lua_State *L, int lvl);
LUALIB_API int (luaL_error) (lua_State *L, const char *fmt, ...);

LUALIB_API int (luaL_checkoption) (lua_State *L, int narg, const char *def,
                                   const char *const lst[]);

LUALIB_API int (luaL_ref) (lua_State *L, int t);
LUALIB_API void (luaL_unref) (lua_State *L, int t, int ref);

LUALIB_API int (luaL_loadfile) (lua_State *L, const char *filename);
LUALIB_API int (luaL_loadbuffer) (lua_State *L, const char *buff, size_t sz,
                                  const char *name);
LUALIB_API int (luaL_loadstring) (lua_State *L, const char *s);

LUALIB_API lua_State *(luaL_newstate) (void);


LUALIB_API const char *(luaL_gsub) (lua_State *L, const char *s, const char *p,
                                                  const char *r);

LUALIB_API const char *(luaL_findtable) (lua_State *L, int idx,
                                         const char *fname, int szhint);

/* From Lua 5.2. */
LUALIB_API int luaL_fileresult(lua_State *L, int stat, const char *fname);
LUALIB_API int luaL_execresult(lua_State *L, int stat);
LUALIB_API int (luaL_loadfilex) (lua_State *L, const char *filename,
				 const char *mode);
LUALIB_API int (luaL_loadbufferx) (lua_State *L, const char *buff, size_t sz,
				   const char *name, const char *mode);
LUALIB_API void luaL_traceback (lua_State *L, lua_State *L1, const char *msg,
				int level);


/*
** ===============================================================
** some useful macros
** ===============================================================
*/

#define luaL_argcheck(L, cond,numarg,extramsg)	\
		((void)((cond) || luaL_argerror(L, (numarg), (extramsg))))
#define luaL_checkstring(L,n)	(luaL_checklstring(L, (n), NULL))
#define luaL_optstring(L,n,d)	(luaL_optlstring(L, (n), (d), NULL))
#define luaL_checkint(L,n)	((int)luaL_checkinteger(L, (n)))
#define luaL_optint(L,n,d)	((int)luaL_optinteger(L, (n), (d)))
#define luaL_checklong(L,n)	((long)luaL_checkinteger(L, (n)))
#define luaL_optlong(L,n,d)	((long)luaL_optinteger(L, (n), (d)))

#define luaL_typename(L,i)	lua_typename(L, lua_type(L,(i)))

#define luaL_dofile(L, fn) \
	(luaL_loadfile(L, fn) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define luaL_dostring(L, s) \
	(luaL_loadstring(L, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define luaL_getmetatable(L,n)	(lua_getfield(L, LUA_REGISTRYINDEX, (n)))

#define luaL_opt(L,f,n,d)	(lua_isnoneornil(L,(n)) ? (d) : f(L,(n)))

/*
** {======================================================
** Generic Buffer manipulation
** =======================================================
*/



typedef struct luaL_Buffer {
  char *p;			/* current position in buffer */
  int lvl;  /* number of strings in the stack (level) */
  lua_State *L;
  char buffer[LUAL_BUFFERSIZE];
} luaL_Buffer;

#define luaL_addchar(B,c) \
  ((void)((B)->p < ((B)->buffer+LUAL_BUFFERSIZE) || luaL_prepbuffer(B)), \
   (*(B)->p++ = (char)(c)))

/* compatibility only */
#define luaL_putchar(B,c)	luaL_addchar(B,c)

#define luaL_addsize(B,n)	((B)->p += (n))

LUALIB_API void (luaL_buffinit) (lua_State *L, luaL_Buffer *B);
LUALIB_API char *(luaL_prepbuffer) (luaL_Buffer *B);
LUALIB_API void (luaL_addlstring) (luaL_Buffer *B, const char *s, size_t l);
LUALIB_API void (luaL_addstring) (luaL_Buffer *B, const char *s);
LUALIB_API void (luaL_addvalue) (luaL_Buffer *B);
LUALIB_API void (luaL_pushresult) (luaL_Buffer *B);


/* }====================================================== */


/* compatibility with ref system */

/* pre-defined references */
#define LUA_NOREF       (-2)
#define LUA_REFNIL      (-1)

#define lua_ref(L,lock) ((lock) ? luaL_ref(L, LUA_REGISTRYINDEX) : \
      (lua_pushstring(L, "unlocked references are obsolete"), lua_error(L), 0))

#define lua_unref(L,ref)        luaL_unref(L, LUA_REGISTRYINDEX, (ref))

#define lua_getref(L,ref)       lua_rawgeti(L, LUA_REGISTRYINDEX, (ref))


#define luaL_reg	luaL_Reg

#endif

// Done Including ./unix_impl/pure_lauxlib.h

   #undef LUALIB_API
   #undef LUA_API
}

/* vim: set ts=3 softtabstop=0 sw=3 expandtab: */
// Done Including ./unix_impl/lua.hh

void pd2_log(const char* message, int level, const char* file, int line); // Defined in unix.h
#define PD2HOOK_LOG_LEVEL(msg, level, file, line, ...) pd2_log(msg, (int) level, file, line)
// Done Including ./unix_impl/unix.h

#else

#error Unsupported platform

#endif

// Done Including ./lua.h
