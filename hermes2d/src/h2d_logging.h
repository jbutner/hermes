/** \addtogroup g_logging Event Logging
 *  \{
 *  \brief Functions and support for logging of events.
 *
 *  Hermes2D controls even logging through:
 *  - compiler directives (e.g. ::H2D_REPORT_INFO). Directives has to be included prior including Hermes2D header files.
 *  - boolean variables (e.g. ::__h2d_report_info). These variables can be set anytime but their direct use is discouraged
 *    because they are integeded to be used by Python wreappers. Initial settings of these variables is given by the compiler
 *    directives.
 *
 *  By default, all logs are written into a file 'hermes2d.log' created in the current directory.
 *  A logs created by the application can be directed to a separate file. The file is specified through
 *  a compiler directive ::H2D_REPORT_FILE, i.e.,\code
    #define H2D_REPORT_FILE "application.log"
 *  \endcode
 *  In a case, the application is a test (i.e., a directive ::H2D_TEST is defined),
 *  the output is directed to a file 'test.log'.
 *  The output to a file can be suppressed specifying ::H2D_REPORT_NO_FILE.
 *
 *  \section s_example Supported Directives
 *  The following list contains directives that controls even logging.
 *  - ::H2D_REPORT_WARN: Define to allow warning.
 *  - ::H2D_REPORT_WARN_INTRO: Define to allow warning about integration issues.
 *  - ::H2D_REPORT_INFO: Define to allow info.
 *  - ::H2D_REPORT_VERBOSE: Define to allow verbose.
 *  - ::H2D_REPORT_TIME: Define to allow time measurement reports.
 *  - ::H2D_REPORT_TRACE: Define to allow execution tracing.
 *  - ::H2D_REPORT_FILE "my_file.log": Define to direct output of a file \c my_file_log.
 *  - ::H2D_REPORT_NO_FILE: Define to avoid any output file. It always overrides ::H2D_REPORT_FILE.
 *  - ::H2D_REPORT_REPORT_ALL: Define to allow all events to be reported except integration warnings (::H2D_REPORT_WARN_INTRO). It overrides all settings.
 *  - ::H2D_REPORT_RUNTIME_CONTROL: Define to allow controling of event logging through boolean variables.
 *    Notice this will enforce evaluation of all parameters of logging macros even though a logging of a given event
 *    is not enabled.
 *  - ::H2D_NO_LOGO: Define to disable logo message. This directive has to be defined at the compilation time of Hermes2D library.
 *
 *  \section s_usage Usage Guidelines
 *  - Do \b not put any computation (e.g., \c it++) into parameters if the result of the computation is
 *    used outside the macro. If a given event is not logged, no code may be generated and therefore
 *    your computation will never be executed.
 *  - Do \b not use new line characters (i.e. \c \\n or \c \\r) inside the message. Used a space or an exclamation
 *    mark instead, see below.
 *  - Use an exclamation mark at the beginning of the message to emphasize the message.
 *  - Use a space at the beginning of the message to generate a sub-info.
 *  - Use the event logging wisely since every line is flushed to a file if file output is enabled. This allows
 *    to obtain all logged vents in a case a SIGSEGV happens on a remote machine, e.g., if an application is
 *    executed using a batch system on a cluster.
 *  - The format of the message is similar to the function printf().
 *
 *  \section s_example Examples of Use
 *  The following code \code
    #define H2D_REPORT_WARN
    #define H2D_REPORT_INFO
    #define H2D_REPORT_VERBOSE
    #define H2D_REPORT_FILE "application.log"
    #include <hermes2d.h>
    #include <solver_umfpack.h>
 *  \endcode will enable logging of events warning, info and verbose in the application and it
 *  copies the output to a file \c application.log.
 *
 *  The following code \code
    info("Result is %d", 32);
    info(" Probability of error is %g", 0.1);
    trace("Computation is done.");
    info("!Done");
 *  \endcode will generate \verbatim
    I Result is 32
      Probability of error is 0.1
    R Computation is done.

    I Done. \endverbatim on screen if all events are enabled.
 */
 
#ifndef __H2D_LOGGING_H_
#define __H2D_LOGGING_H_

#include "../../hermes_common/logging.h"

/* log file */
#undef H2D_LOG_FILE
#ifdef H2D_REPORT_NO_FILE
#  define H2D_LOG_FILE NULL
#else
# ifdef H2D_REPORT_FILE
#  define H2D_LOG_FILE H2D_REPORT_FILE
# else
#  ifndef H2D_TEST
#    define H2D_LOG_FILE "hermes2d.log" // default filename for a library
#  else
#    define H2D_LOG_FILE "test.log" // default filename for a library test
#  endif
# endif
#endif

/// Builds info about an event. \internal
#define H2D_BUILD_LOG_INFO(__event) HermesLogEventInfo(__event, H2D_LOG_FILE, __CURRENT_FUNCTION, __FILE__, __LINE__)

/* error and assert macros */
#define error(...) hermes_exit_if(hermes_log_message_if(true, H2D_BUILD_LOG_INFO(HERMES_EC_ERROR), __VA_ARGS__))
#define error_if(__cond, ...) hermes_exit_if(hermes_log_message_if(__cond, H2D_BUILD_LOG_INFO(HERMES_EC_ERROR), __VA_ARGS__))
#ifndef NDEBUG
# define assert_msg(__cond, ...) assert(!hermes_log_message_if(!(__cond), H2D_BUILD_LOG_INFO(HERMES_EC_ASSERT), __VA_ARGS__))
#else
# define assert_msg(__cond, ...)
#endif

/* reporting macros */
#ifdef H2D_REPORT_ALL
# undef H2D_REPORT_WARNING
# define H2D_REPORT_WARNING
# undef H2D_REPORT_NO_INTR_WARNING
# undef H2D_REPORT_INFO
# define H2D_REPORT_INFO
# undef H2D_REPORT_VERBOSE
# define H2D_REPORT_VERBOSE
# undef H2D_REPORT_TRACE
# define H2D_REPORT_TRACE
# undef H2D_REPORT_TIME
# define H2D_REPORT_TIME
#endif
/** \def H2D_RCTR(__var)
 *  \brief Defines a condition that can control whether logging of a given event is enabled. \internal
 *  An argument \a __var spefies a variable which can control a logging of a given event during
 *  runtime if runtime control is enabled through a preprocessor directive ::H2D_REPORT_RUNTIME_CONTROL. */
#ifdef H2D_REPORT_RUNTIME_CONTROL
# define H2D_RCTR(__var) __var /* reports will be controled also by runtime report control variables */
extern HERMES_API bool __h2d_report_warn;
extern HERMES_API bool __h2d_report_warn_intr;
extern HERMES_API bool __h2d_report_info;
extern HERMES_API bool __h2d_report_verbose;
extern HERMES_API bool __h2d_report_trace;
extern HERMES_API bool __h2d_report_time;
extern HERMES_API bool __h2d_report_debug;
#else
# define H2D_RCTR(__var) true /* reports will be controled strictly by preprocessor directives */
#endif

#if defined(H2D_REPORT_WARNING) || defined(H2D_REPORT_RUNTIME_CONTROL)
# define warn(...) hermes_log_message_if(true && H2D_RCTR(__h2d_report_warn), H2D_BUILD_LOG_INFO(HERMES_EC_WARNING), __VA_ARGS__)
# define warn_if(__cond, ...) hermes_log_message_if((__cond) && H2D_RCTR(__h2d_report_warn), H2D_BUILD_LOG_INFO(HERMES_EC_WARNING), __VA_ARGS__)
#else
# define warn(...)
# define warn_if(__cond, ...)
#endif
#if defined(H2D_REPORT_INTR_WARNING) || defined(H2D_REPORT_RUNTIME_CONTROL)
# define warn_intr(...) hermes_log_message_if(H2D_RCTR(__h2d_report_warn_intr), H2D_BUILD_LOG_INFO(HERMES_EC_WARNING), __VA_ARGS__)
#else
# define warn_intr(...)
#endif
#if defined(H2D_REPORT_INFO) || defined(H2D_REPORT_RUNTIME_CONTROL)
# define info(...) hermes_log_message_if(true  && H2D_RCTR(__h2d_report_info), H2D_BUILD_LOG_INFO(HERMES_EC_INFO), __VA_ARGS__)
# define info_if(__cond, ...) hermes_log_message_if((__cond) && H2D_RCTR(__h2d_report_warn), H2D_BUILD_LOG_INFO(HERMES_EC_INFO), __VA_ARGS__)
#else
# define info(...)
# define info_if(__cond, ...)
#endif
#if defined(H2D_REPORT_VERBOSE) || defined(H2D_REPORT_RUNTIME_CONTROL)
# define verbose(...) hermes_log_message_if(true && H2D_RCTR(__h2d_report_verbose), H2D_BUILD_LOG_INFO(HERMES_EC_VERBOSE), __VA_ARGS__)
#else
# define verbose(...)
#endif
#if defined(H2D_REPORT_TRACE) || defined(H2D_REPORT_RUNTIME_CONTROL)
# define trace(...) hermes_log_message_if(true && H2D_RCTR(__h2d_report_trace), H2D_BUILD_LOG_INFO(HERMES_EC_TRACE), __VA_ARGS__)
#else
# define trace(...)
#endif
#if defined(H2D_REPORT_TIME) || defined(H2D_REPORT_RUNTIME_CONTROL)
# define report_time(...) hermes_log_message_if(true && H2D_RCTR(__h2d_report_time), H2D_BUILD_LOG_INFO(HERMES_EC_TIME), __VA_ARGS__)
#else
# define report_time(...)
#endif
#if defined(_DEBUG) || !defined(NDEBUG) || defined(H2D_REPORT_RUNTIME_CONTROL)
# define debug_log(...) hermes_log_message_if(true && H2D_RCTR(__h2d_report_debug), H2D_BUILD_LOG_INFO(HERMES_EC_DEBUG), __VA_ARGS__)
#else
# define debug_log(...)
#endif

/** \def error(...)
 *  \brief Logs an error and quits the application. For details see \ref s_usage. */
/** \def error_if(__cond, ...)
 *  \brief If \a __cond is true, it logs an error and quits the application.
 *
 *  For usage guidelines, see \ref s_usage. */

/** \def assert_msg(__cond, ...)
 *  \brief If \a __cond is false, it logs a message and invokes assert().
 *
 *  Similar to the function assert() if ::NDEBUG is defined, no code may be generated for the macro.
 *  For usage guidelines, see \ref s_usage. */

/** \def warn(...)
 *  \brief Logs a warning.
 *
 *  If ::H2D_REPORT_WARN is not defined, no code may be generated for the macro.
 *  For usage guidelines, see \ref s_usage. */

/** \def warn_if(__cond, ...)
 *  \brief If \a __cond is true, it logs a warning.
 *
 *  If ::H2D_REPORT_WARN is not defined, no code may be generated for the macro.
 *  For usage guidelines, see \ref s_usage. */

/** \def warn_intr(...)
 *  \brief Logs an warning about integration. This is used to report integration issues which
 *  may occur frequently.
 *
 *  If ::H2D_REPORT_INTR_WARNING is not defined, no code may be generated for the macro.
 *  For usage guidelines, see \ref s_usage. */

/** \def info(...)
 *  \brief Logs info about a result of an operation.
 *
 *  If ::H2D_REPORT_INFO is not defined, no code may be generated for the macro.
 *  For usage guidelines, see \ref s_usage. */

/** \def info_if(__cond, ...)
 *  \brief If \a __cond is true, it logs info about a result of na operation.
 *
 *  If ::H2D_REPORT_WARN is not defined, no code may be generated for the macro.
 *  For usage guidelines, see \ref s_usage. */

/** \def verbose(...)
 *  \brief Logs detailed info about a result of an operation. It should be used as a second level
 *  of the macro info().
 *
 *  If ::H2D_REPORT_VERBOSE is not defined, no code may be generated for the macro.
 *  For usage guidelines, see \ref s_usage. */

/** \def report_time(...)
 *  \brief Logs information about measured time.
 *
 *  If ::H2D_REPORT_TIME is not defined, no code may be generated for the macro.
 *  For usage guidelines, see \ref s_usage. */

/** \def trace(...)
 *  \brief Logs information about executed code portions.
 *
 *  If ::H2D_REPORT_TRACE is not defined, no code may be generated for the macro.
 *  For usage guidelines, see \ref s_usage. */

/** \def debug_log(...)
 *  \brief Logs a general debugging information.
 *
 *  This macro should be used for debugging outputs and it is suggested to remove
 *  almost all of them an debugged issue is solved.
 *  If ::NDEBUG is defined, no code may be generated for the macro.
 *  For usage guidelines, see \ref s_usage. */

/** \} */

#define hermes2d_fwrite(ptr, size, nitems, stream) \
      __hermes_fwrite((ptr), (size), (nitems), (stream), H2D_BUILD_LOG_INFO(HERMES_EC_ERROR))

#define hermes2d_fread(ptr, size, nitems, stream) \
      __hermes_fread((ptr), (size), (nitems), (stream), H2D_BUILD_LOG_INFO(HERMES_EC_ERROR))

#endif
