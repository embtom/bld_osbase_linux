/* ****************************************************************************************************
 * test_prj
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:     armv6
 *  author:	    Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */
#include <stdio.h>
#include <stdlib.h>

/* system */
#include <lib_thread.h>
#include <lib_thread_wakeup.h>
#include <libStreamRouter.h>
#include <lib_serial.h>
#include <lib_serial_linux.h>
#include <lib_console.h>
#include <lib_console_factory.h>
#include <lib_convention__errno.h>

/* frame */
#include <app_test.h>

/* project */

static struct StreamDeviceMap s_streamMap[STREAM_CNT] = {
    M_STREAM_MAPPING_ENTRY(STREAMDEVICE_unix),  /* TTYSTREAM_critical */\
    M_STREAM_MAPPING_ENTRY(STREAMDEVICE_unix),  /* TTYSTREAM_error */\
    M_STREAM_MAPPING_ENTRY(STREAMDEVICE_unix),  /* TTYSTREAM_warning */\
    M_STREAM_MAPPING_ENTRY(STREAMDEVICE_unix),  /* TTYSTREAM_info */\
    M_STREAM_MAPPING_ENTRY(STREAMDEVICE_unix),  /* TTYSTREAM_info */\
    M_STREAM_MAPPING_ENTRY(STREAMDEVICE_unix),  /*TTYSTREAM_control*/
};


//		new_TestFixture("test_lib_can__mscp_client_cleanup", test_lib_can__mscp_client_cleanup),
/* *************************************************************************************************
 * \brief			Main
 * \description		Creates and executes the test runner instance
 * \return			int
 * *************************************************************************************************/
int main() 
{
    int ret;
    unsigned int running;
    lib_serial_hdl uart2Hdl = NULL;
    console_hdl_t consoleHdl = NULL;

    lib_thread__init(PROCESS_SCHED_rr,10);
    lib_thread__wakeup_init();

    libStreamRouter_Init(&s_streamMap[0], sizeof(s_streamMap));

    app_test__init();
    
    running = 1;
    while(running) {
        lib_thread__msleep(5000);
    }


}
