/*------------------------------------------------------------------------------
 * os.h
 *------------------------------------------------------------------------------
 * OS dependend stuff for the simple httpd - header file (OS/2)
 * (c) 1999 by Dirk Ohme
 *------------------------------------------------------------------------------
 */

/*---| prevent double load |--------------------------------------------------*/
#ifndef __H_OS
#define __H_OS

/*---| definitions |----------------------------------------------------------*/
#define OS2
#define OS_STRING                       "OS/2 32 Bit"


   /*---[ OpenSSL detected ]---*/
#define HAS_SSL "OpenSSL"
#define HAS_OPENSSL
   /*---[ Oracle RDBMS detected ]---*/
#define DB_ORACLE "OCI"

                                        /*---| ZLIB data compression |--------*/
#define HAS_ZLIB

                                        /*---| includes and macros |----------*/
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define USE_STDARG_H
#undef  USE_VARARG_H

#define PROC_API                        _System
#define PROC_CALL                       _Optlink
#define PROC_CALL_PTR                   * _System
#define PROC_START(p,d,i)               i = _beginthread(p, NULL, 32768, d)
#define PROC_WAIT(p)                    wait(p)

#define MODULE_EXTENSION                ".dll"
#define MODULE_HANDLE                   HMODULE

#define STRERROR(e)                     strerror(e)

#ifdef INCLUDE_SIGNAL
#  define PROC_TERM(i)                  return
#  include <signal.h>
#  define SIGNAL_ABORT(f)               signal(SIGABRT,  f);\
                                        signal(SIGBREAK, f);\
                                        signal(SIGINT,   f)
#  define SIGNAL_ERROR(f)               signal(SIGILL,   f);\
                                        signal(SIGSEGV,  f)
#  define SIGNAL_INIT(f)                signal(SIGUSR1,  f)
#  define SIGNAL_TERM(f)                signal(SIGTERM,  f)
#  define SIGNAL_ISCHILD(s)             (0)
#  define SIGNAL_ISERROR(s)             (SIGILL   == s || SIGSEGV == s)
#  define SIGNAL_ISINIT(s)              (SIGUSR1  == s)
#  define SIGNAL_ISTERM(s)              (SIGBREAK == s || SIGINT == s ||\
                                         SIGTERM  == s ||     0  == s )
#endif

#ifdef INCLUDE_STRING
#  include <string.h>
#  define STRCASECMP(s,t)               stricmp(s,t)
#  define STRNCASECMP(s,t,n)            strnicmp(s,t,n)
#endif

#ifdef INCLUDE_TCPIP
#  define INCL_BASE
#  define INCL_DOS
#  define INCL_DOSMODULEMGR
#  define INCL_DOSMISC
#  define INCL_DOSPROCESS
#  include <os2.h>
#  define BSD_SELECT
#  include <memory.h>
#  include <types.h>
#  include <sys/select.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/nameser.h>
#  include <netdb.h>
#  include <nerrno.h>
#  include <sys/time.h>
#  define TCPIP_DATA_HOSTENT            struct hostent
#  define TCPIP_DATA_SOCKADDR           struct sockaddr
#  define TCPIP_DATA_SOCKADDR_IN        struct sockaddr_in
#  define TCPIP_ACCEPT(s)               accept(s,NULL,NULL)
#  define TCPIP_BIND(s,a,l)             bind(s,a,l)
#  define TCPIP_BZERO(a,l)              bzero(a,l)
#  define TCPIP_CLOSE(s)                soclose(s)
#  define TCPIP_CONNECT(s,a,l)          connect(s,a,l)
#  define TCPIP_GETHOSTBYADDR(a,l)      gethostbyaddr(a,l,AF_INET)
#  define TCPIP_GETHOSTBYNAME(n)        gethostbyname(n)
#  define TCPIP_GETPEERNAME(s,n,l)      getpeername(s,n,l)
#  define TCPIP_HTONS(i)                htons((unsigned short) i)
#  define TCPIP_INET_NTOA(a)            inet_ntoa(a)
#  define TCPIP_INIT()                  sock_init()
#  define TCPIP_LISTEN(s,n)             listen(s,n)
#  define TCPIP_READ(s,p,l)             recv(s,p,l,0)
#  define TCPIP_REUSESOCKET(s)\
        { int opt; opt = 1;\
          setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(int));\
        }
#  define TCPIP_SOCKET()                socket(AF_INET, SOCK_STREAM, 0)
#  define TCPIP_SELECT(s,r,w,e,t)       bsdselect(s,r,w,e,t)
#  define TCPIP_SHUTDOWN(s,o)           shutdown(s,o)
#  define TCPIP_SLEEP(s)                DosSleep(1000ul * s)
#  define TCPIP_WRITE(s,p,l)            send(s,p,l,0)
#endif

/*---| prevent double load |--------------------------------------------------*/
#endif

/*===| end of file 'os.h' |===================================================*/
