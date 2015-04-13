/*------------------------------------------------------------------------------
 * mod_test.c
 *------------------------------------------------------------------------------
 * sample and testing module
 * (c) 1999 by Dirk Ohme
 *------------------------------------------------------------------------------
 * general notes:
 *      - Storage of module variables is done within a shared memory block,
 *        which is allocated during initialization and free during clearing
 *        of the module.
 *------------------------------------------------------------------------------
 */

/*---| definitions |----------------------------------------------------------*/
        /* note:
         *      The next line is necessary so the right section of <http.h>
         *      is selected. MAIN_MODLE should be set in the *.c file contai-
         *      ning the modInit() routine and mustn't be set in any other *.c
         *      file using <http.h>!
         */
#define MAIN_MODULE

/*---| includes |-------------------------------------------------------------*/
#define INCLUDE_STRING
#include <os.h>
#include <http.h>



/*---| module definitions |---------------------------------------------------*/
typedef struct s_TestData {             /* test module data definitions:      */
        unsigned char szLineParam[256]; /*  - string variable                 */
        int           iNumberParam;     /*  - number variable                 */
} TestData, *PTestData;                 /*------------------------------------*/



/*---| module variables |-----------------------------------------------------*/
static PTestData         pTestData      = NULL;



/*---| routines |-------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * modAbort()
 * module abortion routine - is called, whenever an abort has to be done. The
 * reason for the abort is given by fReason. Before a server shutdown,
 * modAbort() is called, when a connection is still established to a client
 * and has to be terminated. Afterwards, modClear() is executed.
 * input:  PHttpConnection pConn        - connection data structure
 *         HttpModAbort fReason         - reason for abort, see http.h
 * output: int          == 0            = okay
 *                      <  0            = error during abort
 * notes:
 *      - This routine may not only be called with the right PID!
 *        On time-outs, the PID is the one of the time-out handler
 *        routine. The proper PID can be get by pConn->iPID.
 *      - So be careful on doing jobs here in!
 *-----------------------------------------------------------------------------
 */
int PROC_API modAbort( PHttpConnection pConn, HttpModAbort fReason )
{
        /*---| log access |---*/
        HttpLog( pConn, "mod_test::modAbort(%d) --> %s", fReason,\
                 ((HTTP_MOD_ABORT_UNKNOWN   == fReason) ? "general abort"   :\
                  (HTTP_MOD_TIMEOUT         == fReason) ? "timeout"         :\
                  (HTTP_MOD_CONNECTION_LOST == fReason) ? "connection lost" :\
                  (HTTP_MOD_SHUTDOWN        == fReason) ? "server shutdown" :\
                                                          "unknown reason") );
        HttpPrintf( pConn, "<br><hr><h3>Abort!</h3>%s<hr><br>\r\n",\
                    ((HTTP_MOD_ABORT_UNKNOWN  ==fReason) ? "general abort"   :\
                     (HTTP_MOD_TIMEOUT        ==fReason) ? "timeout"         :\
                     (HTTP_MOD_CONNECTION_LOST==fReason) ? "connection lost" :\
                     (HTTP_MOD_SHUTDOWN       ==fReason) ? "server shutdown" :\
                                                           "unknown reason") );
        /*---| return success |---*/
        return 0;

} /* modAbort() */



/*-----------------------------------------------------------------------------
 * modClear()
 * module destructor routine, called when module is removed from memory. This
 * routine is also called during re-initialization, before modInit().
 * input:  --none--
 * output: int          == 0            = okay
 *                      <  0            = error during destruction
 *-----------------------------------------------------------------------------
 */
int PROC_API modClear( void )
{
        /*---| drop module variables |---*/
        memShmFree( ((void **) &pTestData), sizeof(TestData) );

        /*---| return success |---*/
        return 0;

} /* modClear() */



/*-----------------------------------------------------------------------------
 * modConfig()
 * module configuration routine, called by the administration instance in
 * order to give the module the possibility of a configuration by browser.
 * input:  PHttpConnection pConn        - connection data structure
 * output: int          == 0            = okay
 *                      <  0            = error during configuration
 * note:
 *      1) The module's name within the configuration list is given in the
 *         pConn->szTemp area at position 8128 .. 8191 (max. 64 chars.).
 *-----------------------------------------------------------------------------
 */
int PROC_API modConfig( PHttpConnection pConn, PTwoWayLinkedList *pCfg )
{
        unsigned char            szModName[64];
        register unsigned char  *psz;

        /*---| check parameter |---*/
        if( NULL == pCfg ) {
                /*---| no configuration! |---*/
                HttpPuts( pConn, "<p><font color=\"red\">Missing configu"\
                          "ration data!</font></p>" );
                return -1;
        }

        /*---| copy module's name |---*/
        strncpy( szModName, &(pConn->szTemp[8128]), 64 );
        szModName[63] = '\0';

        /*---| display data |---*/
        /* notes:
         *      1) The update information is send via HTTP method GET or POST
         *         to the WWW server, using the <FORM ...> ... </FORM> tags.
         *      2) A sequence number must be specified within this request -
         *         it can be gathered by HttpAdmGetSequence(). The sequence
         *         number is only valid for one session and is updated every
         *         time the configuration changes.
         *      3) The parameters are:
         *              a) sequence number                      @=x
         *              b) divider character (optional)         D=x
         *              c) group name (= module's name)         G=x
         *              d) search string for replace (opt.)     S=x
         *              e) new line to be set                   L=x
         *              f) action code: add new line            A=A
         *                              delete line             A=D
         *                              update line             A=U
         *              ... repetition at b) ...
         *      4) Type e) may be repeated several times - then, each e) is
         *         added left-to-right to one line, using b) as dividing
         *         character. If b) is not given, ':' is assumed.
         *      5) When adding a new line or deleting an existing line, the
         *         line data (to be searched for) are given in e).
         *         When updating an existing line, the line start is set in d)
         *         and the new line data is set in e).
         *         When updating a not existing line, the line data in e) is
         *         added to the end of the list.
         *      6) When updating a line and d) is not given, the first e)
         *         entry plus b) is used for searching, too (i.e. (e)="test",
         *         (b)=":" --> (d)="test:").
         */
        /*---| ... display entries with a HTML table |---*/
        HttpPuts( pConn, "<div align=\"center\"><p>"\
                  "<table border=1 width=\"90%\">" );

        /*---| ... update of parameter #1 |---*/
        HttpPuts( pConn, "<tr><td>parameter #1</td>" );
        HttpPrintf( pConn, "<td><form action=\"%s\" method=\"GET\">"\
                    "<!-- begin of update sequence -->\r\n",\
                    pConn->pszLocation );
        HttpPrintf( pConn, "<input type=\"hidden\" name=\"@\" value=\"%s"\
                    "\"><!-- sequence number -->\r\n", HttpAdmGetSequence() );
        HttpPrintf( pConn, "<!-- divider=default ':' -->\r\n" );
        HttpPrintf( pConn, "<input type=\"hidden\" name=\"g\" value=\"%s"\
                    "\"><!-- group name = module name -->\r\n", szModName );
        HttpPrintf( pConn, "<input type=\"hidden\" name=\"l\" value=\"%s"\
                    "\"><!-- first part of line and search string for update"\
                    " -->\r\n", "parameter_1" );
        HttpPrintf( pConn, "<input name=\"l\" value=\"%s\" size=20><!-- "\
                    "string parameter (#1) -->\r\n",\
                    pTestData->szLineParam );
        HttpPrintf( pConn, "<input type=\"hidden\" name=\"a\" value=\"u\">"\
                    "<!-- action = update -->\r\n" );
        HttpPrintf( pConn, "</td><td><input type=\"submit\" value=\"update\""\
                    "></form><!-- submit (update) button -->\r\n" );

        /*---| ... update of parameter #2 |---*/
        HttpPuts( pConn, "</tr><tr><td>parameter #2</td>" );
        HttpPrintf( pConn, "<td><form action=\"%s\" method=\"GET\">"\
                    "<!-- begin of update sequence -->\r\n",\
                    pConn->pszLocation );
        HttpPrintf( pConn, "<input type=\"hidden\" name=\"@\" value=\"%s"\
                    "\"><!-- sequence number -->\r\n", HttpAdmGetSequence() );
        HttpPrintf( pConn, "<!-- divider=default ':' -->\r\n" );
        HttpPrintf( pConn, "<input type=\"hidden\" name=\"g\" value=\"%s"\
                    "\"><!-- group name = module name -->\r\n", szModName );
        HttpPrintf( pConn, "<input type=\"hidden\" name=\"l\" value=\"%s"\
                    "\"><!-- first part of line and search string for update"\
                    " -->\r\n", "parameter_2" );
        HttpPrintf( pConn, "<input name=\"l\" value=\"%d\" size=20><!-- "\
                    "numerical parameter (#2) -->\r\n",\
                    pTestData->iNumberParam );
        HttpPrintf( pConn, "<input type=\"hidden\" name=\"a\" value=\"u\">"\
                    "<!-- action = update -->\r\n" );
        HttpPrintf( pConn, "</td><td><input type=\"submit\" value=\"update\""\
                    "></form><!-- submit (update) button -->\r\n" );

        /*---| ... close HTML table |---*/
        HttpPuts( pConn, "</tr></table></div></p>" );

        /*---| return success |---*/
        return 0;

} /* modConfig() */



/*-----------------------------------------------------------------------------
 * modHandler()
 * process a given request
 * input:  PHttpConnection pConn        - connection data structure
 * output: int  ==  0                   = okay  --> HTTP_RC_OK
 *              >   0                   = HTTP response code
 *              <   0                   = error --> HTTP_RC_INTERNAL_ERROR
 *-----------------------------------------------------------------------------
 */
int PROC_API modHandler( PHttpConnection pConn )
{
        /*---| set HTTP header |---*/
        HttpSendHeader( pConn, 0 );

        /*---| return parameters on a testing page |---*/
        HttpPuts( pConn, "<html><head><title>mod_test</title></head>" );
        HttpPrintf( pConn, "<body bgcolor=\"white\"><h1>mod_test</h1>\n"\
                    "<a name=\"test\"></a><a name=\"mod_test\"></a>\n"\
                    "<p>This is the output of the sample module. The "\
                    "configuration parameters are<ul>\n<li>string para"\
                    "meter = &quot;<tt>%s</tt>&quot;\n<li>numerical "\
                    "parameter = &quot;<tt>%d</tt>&quot;\n</ul></p>\n",\
                    (pTestData->szLineParam), (pTestData->iNumberParam) );
        HttpPrintf( pConn, "<p>You are user <tt>%s@%s</tt></p>\n",\
                    ((NULL != pConn->pszAuthUser) ? pConn->pszAuthUser\
                                                  : "<i>unknown</i>"),\
                    pConn->szRemoteHost );
        HttpPuts( pConn, "<p><a href=\"/\">Back</a> to home page.</p>"\
                  "</body></html>" );

        /*---| return success |---*/
        return 0;

} /* modHandler() */



/*-----------------------------------------------------------------------------
 * modInit()
 * module initialization routine, called when module is loaded into memory.
 * This is also called during re-initialization, after modClear().
 * input:  PHttpRoutines pRoutines      - pointer to list with routines
 *         PTwoWayLinkedList *pCfg      - two-way linked list with configura-
 *                                        tion data (or empty)
 *         int fNew     == 0            - a configuration is given, you may
 *                                        use it
 *                      != 0            - a configuration is not given, use
 *                                        default values and store it to the
 *                                        newly created linked list (*pCfg).
 *                                        It will be auto-saved afterwards!
 * output: int          == 0            = okay
 *                      <  0            = error during initialization
 *-----------------------------------------------------------------------------
 */
int PROC_API modInit( PHttpRoutines pRoutines, PTwoWayLinkedList *pCfg,
                      int fNew )
{
        register unsigned char  *psz;

        /*---| initialize internal data structures of module |---*/
        /* note:
         *      This is required, so use of the Http...() and List...()
         *      macros is possible. See also the MAIN_MODULE definition
         *      in the beginning of this file!
         */
        HttpInit( pRoutines );

        /*---| do we need to set up default values? |---*/
        if( 0 != fNew ) {
                /*---| yes, do it! |---*/
                ListAdd( pCfg, "parameter_1:Test Line" );
                ListAdd( pCfg, "parameter_2:1" );
        }

        /*---| allocate shared memory for module data |---*/
        if( NULL == (pTestData=memShmAlloc(sizeof(TestData))) )
                return -1;

        /*---| get module variables |---*/
        /* note:
         *      The parameters returned by ListFind() contain the full
         *      string. So we need to separate the line 'name:value'
         *      into its 'name:' and 'value' parts. The easiest way is
         *      to find the first occurrence of the colon (':') in the
         *      string.
         * note:
         *      It is yours to define the syntax! You may use an equal
         *      sign ('=') instead of the colon, too.
         */
        strncpy( pTestData->szLineParam, "<i>undefined</i>",
                 sizeof(pTestData->szLineParam) );
        pTestData->iNumberParam = -1;
        if( NULL != (psz=ListFind(pCfg, "parameter_1:")) ) {
                /*---| get pointer to ':' and step to next character |---*/
                psz = strchr( psz, ':' );
                strncpy( pTestData->szLineParam, ++psz,
                         sizeof(pTestData->szLineParam) );
        }
        if( NULL != (psz=ListFind(pCfg, "parameter_2:")) ) {
                /*---| get pointer to ':' and convert from next char |---*/
                psz = strchr( psz, ':' );
                pTestData->iNumberParam = atoi( ++psz );
        }

        /*---| return success |---*/
        return 0;

} /* modInit() */



/*===| end of file 'mod_test.c' |=============================================*/
