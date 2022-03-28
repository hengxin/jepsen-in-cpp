/* Headers */
/* #undef HAVE_UNISTD_H */
#define HAVE_INTTYPES_H 1
#define HAVE_STDLIB_H 1
/* #undef HAVE_SYS_SELECT_H */
/* #undef HAVE_SYS_SOCKET_H */
/* #undef HAVE_SYS_TIME_H */
/* #undef HAVE_ARPA_INET_H */
/* #undef HAVE_NETINET_IN_H */
#define HAVE_WINSOCK2_H 1

/* Functions */
/* #undef HAVE_STRCASECMP */
#define HAVE__STRICMP 1
#define HAVE_SNPRINTF 1
#define HAVE__SNPRINTF 1

/* Workaround for platforms without POSIX strcasecmp (e.g. Windows) */
#ifndef HAVE_STRCASECMP
# ifdef HAVE__STRICMP
# define strcasecmp _stricmp
# define HAVE_STRCASECMP
# endif
#endif

/* Symbols */
/* #undef HAVE___FUNC__ */
#define HAVE___FUNCTION__ 1

/* Workaround for platforms without C90 __func__ */
#ifndef HAVE___FUNC__
# ifdef HAVE___FUNCTION__
# define __func__ __FUNCTION__
# define HAVE___FUNC__
# endif
#endif

/* Workaround for platforms without gcc __PRETTY_FUNCTION__ */
#ifndef HAVE___PRETTY_FUNCTION__
# define __PRETTY_FUNCTION__  __FUNCSIG__
# define HAVE___PRETTY_FUNCTION__
#endif