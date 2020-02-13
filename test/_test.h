#ifndef __test___h
#define __test___h


#ifndef ok
#define ok test_ok
#endif

#ifndef is
#define is test_is
#endif

#ifndef isnt
#define isnt test_isnt
#endif

#ifndef diag
#define diag test_diag
#endif

#ifndef debug
#define debug test_debug
#endif

#ifndef cleanup
#define cleanup test_cleanup
#endif

/**
 * Todo: Idea: Add assert-level: If failed continue with current test but skip all tests/suites after that
 * Todo: Idea: Provide macro which shows if something went wrong overall (based on _test_failed[0] only). Usage: Prevent to do the cleanup step after something went wrong. For files usually temprary but which shall remain undeleted for debugging purposes.
 * Todo: Idea: Also provide the same only focusing the current test-function (based on _test_failed[0] and also _test_failed[1].
 */

#ifdef __TEST__
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <stdio.h>

static FILE  * _test_file = 0;
static jmp_buf _test_jmp_buf[2];
static int _test_plan = -1;        /**< -1: No plan (default). >=0: Plan exists, _test_plan represents number of planned assertions. */

/** Assert level
 * 0: After failed assertion just continue (default);
 * 1: After failed assertion skip all statements in current test-function, continue with next test function,
 * 2: Full abortion of test-run after first failed assertion */
static int _test_assertlevel = 0;
static int const _test_verbose_min = 0;
static int const _test_verbose_max = 3;
static int       _test_verbose = 1;   /**< See matrix below */
static const struct {
   char diag;
   char assert[2];
   char begin[2];
   char end[2];
   char summary;
   char debug;
} _test_verbose_matrix[/*_test_verbose*/] = {
/*               || diag ||      assert     ||      begin      ||       end       || summary || debug || */
/* _test_verbose ||      || passed | failed || passed | failed || passed | failed ||         ||       || */
      /* 0  */    { 0,    { 0,       1     },{ 0,       0     },{ 0,       1     },  1,         1     },
      /* 1  */    { 1,    { 0,       1     },{ 0,       0     },{ 0,       1     },  1,         1     }, /* <- default */
      /* 2  */    { 1,    { 0,       1     },{ 1,       1     },{ 1,       1     },  1,         1     },
      /* 3  */    { 1,    { 1,       1     },{ 1,       1     },{ 1,       1     },  1,         1     }
};

static int _debug_level = 0;

static int _test_no[2]     = { 0, 0 };
static int _test_failed[2] = { 0, 0 };
#define _test_str_buf_size (512)
static char _test_str_buf[_test_str_buf_size];

#define test_ok(cond, ...)\
   do{\
      if(0 == _test_file)\
         _test_file = stderr;\
      if(stderr == _test_file)\
         setvbuf(_test_file, NULL, _IONBF, 0);\
      ++_test_no[0];\
      snprintf(_test_str_buf, _test_str_buf_size, __VA_ARGS__);\
      if(cond){\
         if(_test_verbose_matrix[_test_verbose].assert[0])\
            fprintf(_test_file, "ok %4i - [%s] %s\n", _test_no[0], #cond, _test_str_buf);\
      }else{\
         if(_test_verbose_matrix[_test_verbose].assert[1])\
            fprintf(_test_file, "%s:%i:1: error: not ok %4i - [%s] %s\n", __FILE__, __LINE__, _test_no[0], #cond, _test_str_buf);\
         ++_test_failed[0];\
         if(0<_test_assertlevel)\
            longjmp(_test_jmp_buf[_test_assertlevel-1],1);\
      }\
   }while(0)

#define test_is(a, b, ...)\
   do{\
      if(0 == _test_file)\
         _test_file = stderr;\
      if(stderr == _test_file)\
         setvbuf(_test_file, NULL, _IONBF, 0);\
      ++_test_no[0];\
      snprintf(_test_str_buf, _test_str_buf_size, __VA_ARGS__);\
      if((a) == (b)){\
         if(_test_verbose_matrix[_test_verbose].assert[0])\
            fprintf(_test_file, "ok %4i - [%s == %s] %s\n", _test_no[0], #a, #b, _test_str_buf);\
      }else{\
         if(_test_verbose_matrix[_test_verbose].assert[1])\
            fprintf(_test_file, "%s:%i:1: error: not ok %4i - [%s == %s] %s\n", __FILE__, __LINE__, _test_no[0], #a, #b, _test_str_buf);\
         ++_test_failed[0];\
         if(0<_test_assertlevel)\
            longjmp(_test_jmp_buf[_test_assertlevel-1],1);\
      }\
   }while(0)

#define test_isnt(a, b, ...)\
   do{\
      if(0 == _test_file)\
         _test_file = stderr;\
      if(stderr == _test_file)\
         setvbuf(_test_file, NULL, _IONBF, 0);\
      ++_test_no[0];\
      snprintf(_test_str_buf, _test_str_buf_size, __VA_ARGS__);\
      if((a) != (b)){\
         if(_test_verbose_matrix[_test_verbose].assert[0])\
            fprintf(_test_file, "ok %4i - [%s != %s] %s\n", _test_no[0], #a, #b, _test_str_buf);\
      }else{\
         if(_test_verbose_matrix[_test_verbose].assert[1])\
            fprintf(_test_file, "%s:%i:1: error: not ok %4i - [%s != %s] %s\n", __FILE__, __LINE__, _test_no[0], #a, #b, _test_str_buf);\
         ++_test_failed[0];\
         if(0<_test_assertlevel)\
            longjmp(_test_jmp_buf[_test_assertlevel-1],1);\
      }\
   }while(0)


#define test_diag(...)\
   do{\
      snprintf(_test_str_buf, _test_str_buf_size, __VA_ARGS__);\
      if(0 == _test_file)\
         _test_file = stderr;\
      if(stderr == _test_file)\
         setvbuf(_test_file, NULL, _IONBF, 0);\
      if(_test_verbose_matrix[_test_verbose].diag)\
         fprintf(_test_file, "%s\n", _test_str_buf);\
   }while(0)

#define test_debug(dl, ...)\
   do{\
      if(_test_debug_level >= (dl))\
         test_diag(__VA_ARGS__);\
   }while(0)

#define test_cleanup \
   if(0 == (_test_failed[0] - _test_failed[1]))

#define test_result\
   ((fprintf(_test_file, "\n%s:1:1: %s: ####################################\n%s:2:1: %s: #  %s (%4i/%4i) (plan: %4i) #\n%s:3:1: %s: ####################################\n", __FILE__, (((_test_plan != -1)&&(_test_plan != _test_no[0]))||_test_failed[0]?"error":"info"), __FILE__, (((_test_plan != -1)&&(_test_plan != _test_no[0]))||_test_failed[0]?"error":"info"), (_test_failed[0]?"[FAIL]":"[PASS]"), (_test_failed[0]?_test_failed[0]:_test_no[0]), _test_no[0], _test_plan, __FILE__, (((_test_plan != -1)&&(_test_plan != _test_no[0]))||_test_failed[0]?"error":"info") )),(((_test_plan != -1)&&(_test_plan != _test_no[0]))||_test_failed[0]?1:0))

#define test_file(f)\
   do{\
      _test_file = fopen((f), "w+");\
   }while(0)

#define test_verboselevel(l)\
   do{\
      _test_verbose = ((l)>_test_verbose_max?_test_verbose_max:((l)<_test_verbose_min?_test_verbose_min:(l)));\
   }while(0)

#define test_plan(p)\
   do{\
      _test_plan = (p<-1?-1:(p));\
   }while(0)

#define test_assertlevel(l)\
   do{\
      _test_assertlevel = ((l)<0?(0):((l>2)?2:(l)));\
   }while(0)

#define test_debuglevel(dl)\
   do{\
      _test_debuglevel = (dl);\
   }while(0)

#define test_def(func)\
   static void func##__test(void)

#define test_call(func)\
   do{\
      _test_failed[1] = _test_failed[0];\
      _test_no[1] = _test_no[0];\
      if ( !setjmp(_test_jmp_buf[0]) ){\
         if(_test_verbose_matrix[_test_verbose].begin[(_test_failed[0] - _test_failed[1])>0])\
            test_diag("### test %s =>", #func);\
         func##__test();\
         if(_test_verbose_matrix[_test_verbose].end[(_test_failed[0] - _test_failed[1])>0])\
            test_diag("### <= test %s %s (%i/%i)", #func, ((_test_failed[0] - _test_failed[1])?"[FAIL]":"[PASS]"), ((_test_failed[0] - _test_failed[1])?(_test_failed[0] - _test_failed[1]):(_test_no[0] - _test_no[1])), (_test_no[0] - _test_no[1]));\
      }else{\
         test_diag("### <= test %s [FAIL] and aborted fatally with first failing assertion  (assertlevel = %i)", #func, _test_assertlevel);\
      }\
   }while(0)


void __attribute__ (( constructor )) _test_entry(void);

#define test_entry(entry)\
   void _test_entry(void){\
      if ( !setjmp(_test_jmp_buf[1]) ){\
         test_call(entry);\
         _mcleanup();\
      }else{\
         test_diag("### <= complete test run [FAIL] and aborted fatally with first failing assertion (assertlevel = %i).", _test_assertlevel);\
      }\
      exit(test_result);\
   }

#define test_main()\
   int main(int argc, char *argv[]){ return 0; }

#else /* __TEST__ */

#define test_ok(cond, ...)
#define test_okf(cond, ...)
#define test_is(a, b, ...)
#define test_isf(a, b, ...)
#define test_isnt(a, b, ...)
#define test_isntf(a, b, ...)
#define test_diag(...)
#define test_debug(dl, ...)

#endif /* __TEST__ */

#endif /* __test___h */
