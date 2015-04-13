/*------------------------------------------------------------------------------
 * cgi_prog.c
 *------------------------------------------------------------------------------
 * sample executable showing how to program a CGI
 * (c) 1999 by Dirk Ohme
 *------------------------------------------------------------------------------
 */

/*---| includes, OS dependent |-----------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



/*---| main routine |---------------------------------------------------------*/
int main( int cArgs, unsigned char *paszArgs[] )
{
        register int             i;
        register unsigned char  *psz;

        /*---| send HTTP response and content type lines |---*/
        puts( "HTTP/1.0 200 Okay" );
        puts( "Content-type: text/plain" );
        puts( "" );

        /*---| display parameters |---*/
        printf( "Test-CGI (binary) okay\n\n  no. of parameters: %u\n", cArgs );
        for( i = 0; i < cArgs; i++ ) {
                printf( "  parameter #%2u: '%s'\n", i, paszArgs[i] );
        }
        puts( "" );

        /*---| display environment |---*/
        printf( "Environment:\n" );
        if( NULL == (psz=getenv("PATH")) )                 psz = "<empty>";
        printf( "  PATH                   = '%s'\n", psz );
        if( NULL == (psz=getenv("SERVER_SOFTWARE")) )      psz = "<empty>";
        printf( "  SERVER_SOFTWARE        = '%s'\n", psz );
        if( NULL == (psz=getenv("GATEWAY_INTERFACE")) )    psz = "<empty>";
        printf( "  GATEWAY_INTERFACE      = '%s'\n", psz );
        if( NULL == (psz=getenv("SERVER_MAIN_PROTOCOL")) ) psz = "<empty>";
        printf( "  SERVER_MAIN_PROTOCOL   = '%s'\n", psz );
        if( NULL == (psz=getenv("SERVER_NAME")) )          psz = "<empty>";
        printf( "  SERVER_NAME            = '%s'\n", psz );
        if( NULL == (psz=getenv("SERVER_PORT")) )          psz = "<empty>";
        printf( "  SERVER_PORT            = '%s'\n", psz );
        if( NULL == (psz=getenv("SERVER_ADMIN")) )         psz = "<empty>";
        printf( "  SERVER_ADMIN           = '%s'\n", psz );
        if( NULL == (psz=getenv("SERVER_PROTOCOL")) )      psz = "<empty>";
        printf( "  SERVER_PROTOCOL        = '%s'\n", psz );
        if( NULL == (psz=getenv("REMOTE_ADDR")) )          psz = "<empty>";
        printf( "  REMOTE_ADDR            = '%s'\n", psz );
        if( NULL == (psz=getenv("REMOTE_HOST")) )          psz = "<empty>";
        printf( "  REMOTE_HOST            = '%s'\n", psz );
        if( NULL == (psz=getenv("REMOTE_USER")) )          psz = "<empty>";
        printf( "  REMOTE_USER            = '%s'\n", psz );
        if( NULL == (psz=getenv("AUTH_TYPE")) )            psz = "<empty>";
        printf( "  AUTH_TYPE              = '%s'\n", psz );
        if( NULL == (psz=getenv("REQUEST_METHOD")) )       psz = "<empty>";
        printf( "  REQUEST_METHOD         = '%s'\n", psz );
        if( NULL == (psz=getenv("REQUEST_URI")) )          psz = "<empty>";
        printf( "  REQUEST_URI            = '%s'\n", psz );
        if( NULL == (psz=getenv("SCRIPT_NAME")) )          psz = "<empty>";
        printf( "  SCRIPT_NAME            = '%s'\n", psz );
        if( NULL == (psz=getenv("PATH_TRANSLATED")) )      psz = "<empty>";
        printf( "  PATH_TRANSLATED        = '%s'\n", psz );
        if( NULL == (psz=getenv("SCRIPT_NAME")) )          psz = "<empty>";
        printf( "  SCRIPT_NAME            = '%s'\n", psz );
        if( NULL == (psz=getenv("PATH_INFO")) )            psz = "<empty>";
        printf( "  PATH_INFO              = '%s'\n", psz );
        if( NULL == (psz=getenv("QUERY_STRING")) )         psz = "<empty>";
        printf( "  QUERY_STRING           = '%s'\n", psz );
        if( NULL == (psz=getenv("CONTENT_LENGTH")) )       psz = "<empty>";
        printf( "  CONTENT_LENGTH         = '%s'\n", psz );

        /*---| return success |---*/
        return 0;

} /* main() */


/*===| end of file 'cgi_prog.c' |=============================================*/
