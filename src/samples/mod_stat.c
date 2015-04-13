/*------------------------------------------------------------------------------
 * mod_stat.c
 *------------------------------------------------------------------------------
 * module for access statistics
 * (c) 1999 by Dirk Ohme
 *------------------------------------------------------------------------------
 * general notes:
 *      - Storage of module variables is done within a shared memory block,
 *        which is allocated during initialization and free during clearing
 *        of the module.
 *------------------------------------------------------------------------------
 */

/*---| definitions |----------------------------------------------------------*/
#define MAIN_MODULE
        /*---| name of output file |----------------------------------*/
#define OUTPUT_FILENAME         "mod_stat.lst"
        /*---| max. number of different access objects |--------------*/
#define MAX_STAT_OBJECTS          64
        /*---| size of image data in bytes |--------------------------*/
#define IMAGE_SIZE                43
#define IMAGE_SIZE_STR           "43"



/*---| includes |-------------------------------------------------------------*/
#define INCLUDE_STRING
#include <os.h>
#include <http.h>
#include <time.h>



/*---| module definitions |---------------------------------------------------*/
typedef struct s_StatEntry {            /* statistics entry:                  */
        unsigned long    cHits;         /*  - hit counter                     */
        unsigned long    fLastHit;      /*  - last hit                        */
        unsigned char    szName[16];    /*  - identifier                      */
} StatEntry, *PStatEntry;               /*------------------------------------*/



/*---| module variables |-----------------------------------------------------*/
static unsigned char     szGif[IMAGE_SIZE] = {
        0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x01, 0x00,
        0x01, 0x00, 0x80, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
        0x00, 0x00, 0x00, 0x21, 0xF9, 0x04, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x2C, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x01, 0x00, 0x00, 0x02, 0x02, 0x44,
        0x01, 0x00, 0x3B
};
static int               cEntries       = MAX_STAT_OBJECTS;
static PStatEntry        pEntries       = NULL;



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
        unsigned char    szBuffer[2048];
        register FILE   *ph;

        /*---| write out statistics |---*/
        if( 0 != HttpGetPathToBin(szBuffer, sizeof(szBuffer)) ) {
                szBuffer[0] = '\0';
        }
        strncat( szBuffer, OUTPUT_FILENAME, sizeof(szBuffer) );
        if( NULL != (ph=fopen(szBuffer, "w")) ) {
                register int i;

                /*---| write header |---*/
                fputs( "# Information is stored like this:\n"\
                       "# <no. of hits> <last access time> <name>\n"\
                       "#  |             |                  |\n"\
                       "#  |             +--- time stamp    +--- reference\n"\
                       "#  |                  as hex-decimal     name\n"\
                       "#  |                  seconds since\n"\
                       "#  +--- number of     1970/01/01 0:00\n"\
                       "#       hits\n", ph );

                /*---| process all entries |---*/
                for( i = 0; i < cEntries; i++ ) {
                        if( '\0' != pEntries[i].szName[0] ) {
                                fprintf( ph, "%8u 0x%8X %s\n",
                                         pEntries[i].cHits,
                                         pEntries[i].fLastHit,
                                         pEntries[i].szName );
                        }
                }

                /*---| close file |---*/
                fflush( ph );
                fclose( ph );
        }

        /*---| drop module variables |---*/
        memShmFree( ((void **) &pEntries), cEntries * sizeof(StatEntry) );

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
        register unsigned long   l, cTotal;
        register int             i;

        /*---| check parameter |---*/
        if( NULL == pCfg ) {
                /*---| no configuration! |---*/
                HttpPuts( pConn, "<p><font color=\"red\">Missing configu"\
                          "ration data!</font></p>" );
                return -1;
        }

        /*---| get highest hit number |---*/
        l = cTotal = 0;
        for( i = 0; i < cEntries; i++ ) {
                /*---| get highest number |---*/
                if( '\0' != pEntries[i].szName[0] && l < pEntries[i].cHits )
                        l = pEntries[i].cHits;
        }

        /*---| display statistics |---*/
        HttpPuts( pConn, "<h3>Statistics</h3>\r\n"\
                  "<p>Names, last access times and hit number of web pages,"\
                  " which have been marked for statistics.\r\nThe output is"\
                  " sorted by hits.</p>" );
        HttpPuts( pConn, "<table border=1 width=\"100%\">\r\n"\
                  "<tr><th>Hits<th>Last Access<th>Statistic Name</tr>" );
        for( ; 0 != l; l-- ) {
                /*---| display all entries with that hit rate |---*/
                for( i = 0; i < cEntries; i++ ) {
                        if( '\0' != pEntries[i].szName[0] &&
                               l == pEntries[i].cHits ) {
                                HttpPrintf( pConn, "<tr><td align=right>%u"\
                                            "<td>%s<td><tt>%s</tt></tr>\r\n",\
                                            pEntries[i].cHits,\
                                            ctime((time_t *) &(pEntries[i].fLastHit)),\
                                            pEntries[i].szName );
                                cTotal += pEntries[i].cHits;
                        }
                }
        }
        HttpPrintf( pConn, "<tr><td align=right><b>%u</b><td "\
                    "colspan=2>&lt;-- hits total</tr>\r\n", cTotal );
        HttpPuts( pConn, "</table>" );

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
        register int     i, iMin;

        /*---| set HTTP header |---*/
        ListAdd( &(pConn->pHttpRespHdr), "Content-type: image/gif" );
        ListAdd( &(pConn->pHttpRespHdr), "Content-length: "IMAGE_SIZE_STR );
        ListAdd( &(pConn->pHttpRespHdr), "Expires: now" );
        HttpSendHeader( pConn, 0 );

        /*---| send image data |---*/
        HttpWrite( pConn, &szGif[0], IMAGE_SIZE );

        /*---| set statistics |---*/
        for( i = iMin = 0; i < cEntries; i++ ) {
                /*---| search for unused entry or with lowest hit rate |---*/
                if( '\0' == pEntries[i].szName ) {
                        /*---| okay, we got an unused entry |---*/
                        iMin = i;
                        break;
                } else
                if( NULL != pConn->pszQueryString &&
                       0 == strncmp(pEntries[i].szName,
                                    pConn->pszQueryString,
                                    sizeof(pEntries[i].szName)) ) {
                        /*---| matching entry |---*/
                        time( (time_t *) &(pEntries[i].fLastHit) );
                        pEntries[i].cHits++;
                        iMin = -1;
                        break;
                }

                /*---| check last entry |---*/
                if( pEntries[i].cHits < pEntries[iMin].cHits )
                        iMin = i;
        }

        /*---| write out new data |---*/
        if( 0 <= iMin && NULL != pConn->pszQueryString ) {
                strncpy( pEntries[iMin].szName, pConn->pszQueryString,
                         sizeof(pEntries[iMin].szName) );
                time( (time_t *) &(pEntries[iMin].fLastHit) );
                pEntries[iMin].cHits = 1;
        }

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
        unsigned char    szBuffer[2048];
        register FILE   *ph;

        /*---| initialize internal data structures of module |---*/
        HttpInit( pRoutines );

        /*---| allocate shared memory for module data |---*/
        if( NULL == (pEntries=memShmAlloc(cEntries * sizeof(StatEntry))) )
                return -1;

        /*---| read in statistics |---*/
        if( 0 != HttpGetPathToBin(szBuffer, sizeof(szBuffer)) ) {
                szBuffer[0] = '\0';
        }
        strncat( szBuffer, OUTPUT_FILENAME, sizeof(szBuffer) );
        if( NULL != (ph=fopen(szBuffer, "r")) ) {
                register int i;

                /*---| process all entries |---*/
                for( i = 0; !feof(ph) && (i < cEntries); ) {
                        register unsigned char  *psz;

                        /*---| try to read next line, skip remarks |---*/
                        if( NULL == fgets(szBuffer, sizeof(szBuffer), ph) ||
                             '#' == szBuffer[0] )
                                continue;

                        /*---| strip trailing \n and \r |---*/
                        if( NULL != (psz=strchr(szBuffer, '\n')) ) *psz = '\0';
                        if( NULL != (psz=strchr(szBuffer, '\r')) ) *psz = '\0';

                        /*---| scan line |---*/
                        if( 3 == sscanf(szBuffer, "%u 0x%08X %s",
                                        &(pEntries[i].cHits),
                                        &(pEntries[i].fLastHit),
                                        &(pEntries[i].szName[0])) ) {
                                /*---| we got an entry |---*/
                                i++;
                        }
                }

                /*---| close file |---*/
                fclose( ph );

                /*---| initialize rest of list |---*/
                while( i < cEntries ) {
                        pEntries[i++].szName[0] = '\0';
                }
        }


        /*---| return success |---*/
        return 0;

} /* modInit() */



/*===| end of file 'mod_stat.c' |=============================================*/
