/*------------------------------------------------------------------------------
 * http.h
 *------------------------------------------------------------------------------
 * header file containing definitons, data structure and prototype definitions
 * for the HTTP communication and module services
 * (c) 1999 by Dirk Ohme
 *------------------------------------------------------------------------------
 */

/*---| prevent double load (begin) |------------------------------------------*/
#ifndef __H_HTTP
#define __H_HTTP



/*---| definitions |----------------------------------------------------------*/
typedef enum e_HttpProtocol {           /* type of WWW protocol:              */
        HTTP_UNKNOWN_PROTOCOL   = 0,    /*   - unknown type                   */
        HTTP_FTP,                       /*   - file transfer protocol         */
        HTTP_HTTP,                      /*   - hypertext transfer protocol    */
        HTTP_HTTPS                      /*   - HTTP via secure socket layer   */
} HttpProtocol, *PHttpProtocol;         /*------------------------------------*/
                                        /*                                    */
typedef enum e_HttpReqMethod {          /* HTTP request method                */
        HTTP_UNKNOWN_METHOD     = 0,    /*  - unknown request method          */
        HTTP_GET,                       /*  - request method 'GET'            */
        HTTP_HEAD,                      /*  - request method 'HEAD'           */
        HTTP_POST,                      /*  - request method 'POST'           */
        HTTP_PUT,                       /*  - request method 'PUT'            */
        HTTP_DELETE,                    /*  - request method 'DELETE'         */
        HTTP_LINK,                      /*  - request method 'LINK'           */
        HTTP_UNLINK                     /*  - request method 'UNLINK'         */
} HttpReqMethod;                        /*------------------------------------*/
                                        /*                                    */
typedef enum e_HttpModAbort {           /* reasons for aborts on modules:     */
        HTTP_MOD_ABORT_UNKNOWN  = 0,    /*  - reason for abort is unknown     */
        HTTP_MOD_TIMEOUT,               /*  - timeout has occurred            */
        HTTP_MOD_CONNECTION_LOST,       /*  - connection has been aborted     */
        HTTP_MOD_SHUTDOWN,              /*  - shutdown of WWW server          */
        HTTP_MOD_CLOSE_CHANNEL          /*  - close channel (see SSL)         */
} HttpModAbort;                         /*------------------------------------*/
                                        /*                                    */
typedef struct s_TwoWayLinkedList {     /* two-way linked list:               */
        unsigned char   *pszText;       /*  - pointer to text                 */
        struct s_TwoWayLinkedList       /*  - pointer to previous and next    */
                        *pPrev, *pNext; /*    element in list                 */
} TwoWayLinkedList, *PTwoWayLinkedList; /*------------------------------------*/
                                        /*                                    */
typedef struct s_HttpSrvLaunch {        /* launch list entry:                 */
        int              iDay;          /*  - -1:        every day            */
                                        /*     0 .. 365: day of year (0=1.Jan)*/
                                        /*   400 .. 406: at sun, mon, ... sat.*/
        int              iHour;         /*  - -1/0..23:  every hour/at hour   */
        int              iMin;          /*  - -1/0..59:  every minute/at min. */
        void (* PROC_CALL pProc)(void *pArg);/* - pointer to routine          */
        void            *pArg;          /*  - pointer to parameter data       */
} HttpSrvLaunch, *PHttpSrvLaunch;       /*------------------------------------*/
                                        /*                                    */
typedef struct s_HttpServer {           /* server details:                    */
        int              fSched;        /*  - the scheduler's status          */
                                        /*    -1/0/1 : abort/stopped/running  */
        int              cbConnections; /*  - number of communication channels*/
                                        /*    currently open or -1 on destruct*/
        int              iTcpSocket;    /*  - main communication socket       */
        int              iTcpSSLSocket; /*  - main communication socket, SSL  */
        int              iPort;         /*  - port address server is listening*/
        int              iSSLPort;      /*  - port address of SSL server      */
        char             szLocalAddr[32];/* - IP address of WWW server        */
        char             szLocalHost[64];/* - name of WWW server              */
        char             szAdmin[64];   /*  - email address of administrator  */
                                        /*  - scheduler entries               */
#ifdef HTTP_SERVER_MODULE
        HttpSrvLaunch    asScheduler[HTTP_SCHEDULE_ENTRIES];
#else
        HttpSrvLaunch    asScheduler[1];
#endif
} HttpServer, *PHttpServer;             /*------------------------------------*/
                                        /*                                    */
typedef struct s_HttpConnection {       /* connection details:                */
        PHttpServer      psServer;      /*  - ptr. to server details struct.  */
        int (PROC_CALL_PTR pTCPsend)(   /*  - TCP/IP send/write routine       */
                struct s_HttpConnection *pConn, unsigned char *psz, int cb );
        int (PROC_CALL_PTR pTCPrecv)(   /*  - TCP/IP receive/read routine     */
                struct s_HttpConnection *pConn, unsigned char *psz, int cb );
        int              iCommSocket;   /*  - communication socket to client  */
        int              iPID;          /*  - PID on child (iCommSocket != 0) */
        int              iTimeOut;      /*  - time out value (count down ctr.)*/
        int              iReadIdx;      /*  - read index on buffer            */
        int              iWriteIdx;     /*  - write index on buffer           */
        int              cLastChar;     /*  - buffer for HttpUngetChar()      */
        unsigned char    szBuffer[8192];/*  - buffer for TCP/IP reading       */
        unsigned char    szTemp[8192];  /*  - buffer for temporary data       */
        char             szRemoteAddr[32];/*- IP address of WWW client        */
        char             szRemoteHost[64];/*- name of WWW client              */
        PTwoWayLinkedList pHttpSSLInfo; /*  - list with HTTP-SSL details      */
        PTwoWayLinkedList pHttpCallHdr; /*  - list with HTTP calling header   */
        HttpReqMethod    fReqMethod;    /*  - request method                  */
        int              fReqHttpVersion;/* - HTTP version on request         */
        unsigned long    fReqInfo;      /*  - infos about the request         */
        unsigned char   *pszAuthScheme; /*  - authorization scheme   or NULL  */
        unsigned char   *pszAuthUser;   /*  - authorization user     or NULL  */
        unsigned char   *pszAuthPass;   /*  - authorization password or NULL  */
        unsigned char   *pszLocation;   /*  - location of object              */
        unsigned char   *pszPath;       /*  - (translated) path of object     */
        unsigned char   *pszPathInfo;   /*  - additional path information     */
        unsigned char   *pszQueryString;/*  - query string                    */
        unsigned long    cbContentLength;/* - content length                  */
        unsigned long    cbContentRead; /*  - number of bytes read from cont. */
        PTwoWayLinkedList pHttpRespHdr; /*  - list with HTTP response header  */
        int (PROC_CALL_PTR modAbort)(   /*  - abort handler routine           */
                struct s_HttpConnection *pConn, HttpModAbort fReason); /*     */
        void            *pSSL;          /*  - per connection SSL data         */
        void            *psZ;           /*  - per request data compression    */
        void            *pZBuffer;      /*  - data compression: Z buffer      */
        unsigned long    ulZCRC32;      /*  - data compression: CRC-32        */
} HttpConnection, *PHttpConnection;     /*------------------------------------*/
                                        /*                                    */
typedef struct s_HttpRoutines {         /* list of server API: (V 0.01)       */
        unsigned char *\
               (PROC_CALL_PTR HttpBase64Decode)( unsigned char *pszBase64 );
        int    (PROC_CALL_PTR HttpGetChar)( PHttpConnection pConn );
        int    (PROC_CALL_PTR HttpGetLine)( PHttpConnection pConn,
                                            unsigned char *pszBuffer,
                                            int cbBuffer );
#ifdef USE_STDARG_H
        int    (PROC_CALL_PTR HttpLog)( PHttpConnection pConn, ... );
#else
        int    (PROC_CALL_PTR HttpLog)( PHttpConnection pConn, ... );
#endif
        int    (PROC_CALL_PTR HttpParseQueryStr)( unsigned char **ppszQueryStr,
                                                  unsigned char *pszBuffer,
                                                  unsigned long  cbBuffer );
        int    (PROC_CALL_PTR HttpPrintf)( PHttpConnection pConn, ... );
        int    (PROC_CALL_PTR HttpPuts)( PHttpConnection pConn,
                                         unsigned char *pszText );
        int    (PROC_CALL_PTR HttpRead)( PHttpConnection pConn,
                                         unsigned char *pszBuffer,
                                         int cbBuffer );
        int    (PROC_CALL_PTR HttpSendHeader)( PHttpConnection pConn,
                                               int fStatus );
        int    (PROC_CALL_PTR HttpSystem)( int fEnv,
                                           unsigned char *pszCmdLine );
        int    (PROC_CALL_PTR HttpUngetChar)( PHttpConnection pConn,
                                              unsigned char cChar );
        int    (PROC_CALL_PTR HttpWrite)( PHttpConnection pConn,
                                          unsigned char *pszBuffer,
                                          int cbBuffer );
        int    (PROC_CALL_PTR ListAdd)( PTwoWayLinkedList *pList,
                                        unsigned char *pszText );
        int    (PROC_CALL_PTR ListClear)( PTwoWayLinkedList *pList );
        int    (PROC_CALL_PTR ListDelete)( PTwoWayLinkedList *pList,
                                           unsigned char *pszSearchText );
        char * (PROC_CALL_PTR ListFind)( PTwoWayLinkedList *pList,
                                         unsigned char *pszSearchText );
        char * (PROC_CALL_PTR ListGet)( PTwoWayLinkedList *pList );
        int    (PROC_CALL_PTR ListNext)( PTwoWayLinkedList *pList );
        int    (PROC_CALL_PTR ListPrev)( PTwoWayLinkedList *pList );
        int    (PROC_CALL_PTR ListReplace)( PTwoWayLinkedList *pList,
                                            unsigned char *pszSearchText,
                                            unsigned char *pszReplaceText );
        int    (PROC_CALL_PTR ListRewind)( PTwoWayLinkedList *pList );
                                        /*------------------------------------*/
                                        /* list of server API: (V 0.02)       */
        unsigned char *\
               (PROC_CALL_PTR HttpAdmGetSequence)( void );
        int    (PROC_CALL_PTR HttpParseURL)( unsigned char *pszURL,
                                             PHttpProtocol  pfProtocol,
                                             unsigned char *pszHost,
                                             unsigned long  cbHost,
                                             unsigned int  *piPort,
                                             unsigned char *pszPath,
                                             unsigned long cbPath );
        void * (PROC_CALL_PTR memShmAlloc)( unsigned long cbMemSize );
        int    (PROC_CALL_PTR memShmFree)( void **ppMemory,
                                           unsigned long cbMemSize );
                                        /*------------------------------------*/
                                        /* list of server API: (V 0.03)       */
        unsigned char *\
               (PROC_CALL_PTR HttpGetHttpErrorText)( int iHttpRC );
        int    (PROC_CALL_PTR HttpGetPathToBin)( unsigned char *pszPath,
                                                 int cbPath );
        int    (PROC_CALL_PTR HttpSockInit)( void );
        int    (PROC_CALL_PTR HttpScheduleAt)( PHttpSrvLaunch pEntry );
                                        /*------------------------------------*/
                                        /* list of server API: (V 0.04)       */
                                        /*------------------------------------*/
                                        /* list of server API: (V 0.05)       */
        void * (PROC_CALL_PTR memSrvAlloc)( unsigned char *pszFilename,
                                            unsigned long cbLinenumber,
                                            unsigned long cbMemSize );
        int    (PROC_CALL_PTR memSrvFree)( void **ppMemory );
        int    (PROC_CALL_PTR HttpCheckConns)( void );
                                        /*------------------------------------*/
                                        /* list of server API: (V 0.06)       */
                                        /*------------------------------------*/
        PHttpServer (PROC_CALL_PTR HttpGetServerInternalData)( void );
        int    (PROC_CALL_PTR HttpRedirect)( PHttpConnection pConn,
                                             unsigned char *pszURL );

} HttpRoutines, *PHttpRoutines;         /*------------------------------------*/
                                        /* HTTP request info codes:           */
                                        /*------------------------------------*/
                                                /* request for re-use of chn. */
#define HTTP_REQ_KEEP_ALIVE     0x00000001ul
                                                /* signal, to re-use channel  */
#define HTTP_REQ_REUSE_CHANNEL  0x00000002ul
                                                /* data compression is enabled*/
#define HTTP_REQ_GZIP_ACTIVE    0x00002000ul
                                                /* SSL is enabled             */
#define HTTP_REQ_SSL_ACTIVE     0x00004000ul
                                                /* error on processing        */
#define HTTP_REQ_ERROR          0x00008000ul
                                                /* HTTP header sent           */
#define HTTP_RESP_HEADER_SENT   0x80000000ul
                                        /*------------------------------------*/
                                        /* HTTP response codes:               */
                                        /*------------------------------------*/
#define HTTP_RC_CONTINUE               100
#define HTTP_RC_OK                     200
#define HTTP_RC_MULTIPLE_CHOICES       300
#define HTTP_RC_MOVED_PERMANENTLY      301
#define HTTP_RC_MOVED_TEMPORARILY      302
#define HTTP_RC_BAD_REQUEST            400
#define HTTP_RC_UNAUTHORIZED           401
#define HTTP_RC_PAYMENT_REQUIRED       402
#define HTTP_RC_FORBIDDEN              403
#define HTTP_RC_NOT_FOUND              404
#define HTTP_RC_METHOD_NOT_ALLOWED     405
#define HTTP_RC_REQUEST_TIMEOUT        408
#define HTTP_RC_CONFLICT               409
#define HTTP_RC_GONE                   410
#define HTTP_RC_LENGTH_REQUIRED        411
#define HTTP_RC_SSL_REQUIRED           499
#define HTTP_RC_INTERNAL_ERROR         500
#define HTTP_RC_NOT_IMPLEMENTED        501
#define HTTP_RC_BAD_GATEWAY            502
#define HTTP_RC_SERVICE_UNAVAILABLE    503
#define HTTP_RC_GATEWAY_TIME_OUT       504
#define HTTP_RC_UNSUPPORTED_VERSION    505
#define HTTP_RC_VARIANT_ALSO_VARIES    506
#define HTTP_RC_INSUFFICIENT_STORAGE   507
#define HTTP_RC_NOT_EXTENDED           508
                                        /*------------------------------------*/



/*---| local data structures |------------------------------------------------*/
#ifndef HTTP_SERVER_MODULE
        /*---| access structure for server routines |-----------------*/
#  ifdef MAIN_MODULE
       PHttpRoutines     pHttpRoutines          = NULL;
#  else
extern PHttpRoutines     pHttpRoutines;
#  endif
#endif



/*---| prototypes |-----------------------------------------------------------*/
#ifndef HTTP_SERVER_MODULE
        /*---| prototypes for module procedures and functions |-------*/
int     PROC_API  modAbort( PHttpConnection pConn, HttpModAbort fReason );
int     PROC_API  modClear( void );
int     PROC_API  modConfig( PHttpConnection pConn, PTwoWayLinkedList *pCfg );
int     PROC_API  modHandler( PHttpConnection pConn );
int     PROC_API  modInit( PHttpRoutines pRoutines, PTwoWayLinkedList *pCfg,
                           int fNew );

        /*---| macros for accessing server routines |-----------------*/
#define HttpInit(p)             if(NULL == (pHttpRoutines=(p))) return -1
#define HttpAdmGetSequence()    (pHttpRoutines->HttpAdmGetSequence)()
#define HttpBase64Decode(c)     (pHttpRoutines->HttpBase64Decode)(c)
#define HttpCheckConns()        (pHttpRoutines->HttpCheckConns)()
#define HttpGetChar(c)          (pHttpRoutines->HttpGetChar)(c)
#define HttpGetHttpErrorText(i) (pHttpRoutines->HttpGetHttpErrorText)(i)
#define HttpGetLine(c,p,l)      (pHttpRoutines->HttpGetLine)(c,p,l)
#define HttpGetPathToBin(p,l)   (pHttpRoutines->HttpGetPathToBin)(p,l)
#define HttpLog                 (pHttpRoutines->HttpLog)
#define HttpParseQueryStr(i,o,c) (pHttpRoutines->HttpParseQueryStr)(i,o,c)
#define HttpParseURL(a,b,c,d,e,f,g)\
                                (pHttpRoutines->HttpParseURL)(a,b,c,d,e,f,g)
#define HttpPrintf              (pHttpRoutines->HttpPrintf)
#define HttpPuts(c,s)           (pHttpRoutines->HttpPuts)(c,s)
#define HttpRead(c,p,l)         (pHttpRoutines->HttpRead)(c,p,l)
#define HttpRedirect(c,u)       (pHttpRoutines->HttpRedirect)(p,u)
#define HttpScheduleAt(p)       (pHttpRoutines->HttpScheduleAt)(p)
#define HttpSendHeader(c,f)     (pHttpRoutines->HttpSendHeader)(c,f)
#define HttpSendSequence(c)     (pHttpRoutines->HttpPrintf)\
                                    (c,"<input type=\"hidden\" name=\"@\""\
                                       " value=\"%s\">\r\n",\
                                     HttpAdmGetSequence())
#define HttpSockInit()          (pHttpRoutines->HttpSockInit)()
#define HttpSystem(f,c)         (pHttpRoutines->HttpSystem)(f,c)
#define HttpUngetChar(c,x)      (pHttpRoutines->HttpUngetChar)(c,x)
#define HttpWrite(c,p,l)        (pHttpRoutines->HttpWrite)(c,p,l)
#define ListAdd(l,s)            (pHttpRoutines->ListAdd)(l,s)
#define ListClear(l)            (pHttpRoutines->ListClear)(l)
#define ListDelete(l,s)         (pHttpRoutines->ListDelete)(l,s)
#define ListFind(l,s)           (pHttpRoutines->ListFind)(l,s)
#define ListGet(l)              (pHttpRoutines->ListGet)(l)
#define ListNext(l)             (pHttpRoutines->ListNext)(l)
#define ListPrev(l)             (pHttpRoutines->ListPrev)(l)
#define ListReplace(l,s,t)      (pHttpRoutines->ListReplace)(l,s,t)
#define ListRewind(l)           (pHttpRoutines->ListRewind)(l)
#define memShmAlloc(l)          (pHttpRoutines->memShmAlloc)(l)
#define memShmFree(p,l)         (pHttpRoutines->memShmFree)(p,l)
#define memSrvAlloc(l)          (pHttpRoutines->memSrvAlloc)(__FILE__,__LINE__,l)
#define memSrvFree(p)           (pHttpRoutines->memSrvFree)(p)
#endif



/*---| prevent double load (end) |--------------------------------------------*/
#endif

/*===| end of file 'http.h' |=================================================*/
