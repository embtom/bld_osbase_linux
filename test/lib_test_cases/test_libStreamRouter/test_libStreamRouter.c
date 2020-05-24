/* ****************************************************************************************************
 * test_lib_list.c within the following project: lib_thread
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:     armv6
 *  author:	    Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/


/* *******************************************************************
 * includes
 * ******************************************************************/

/*c-runime */

/*frame*/
#include <lib_convention__errno.h>
#include <lib_log.h>

/*project*/
#include <embUnit.h>
#include <test_cases.h>
#include <test_libStreamRouter.h>
#include <libStreamRouter.h>

/* *******************************************************************
 * Defines
 * ******************************************************************/
#define M_TEST_MODULE_NAME 		"test_lib_ttyportmux"

/* *******************************************************************
 * Global Functions
 * ******************************************************************/

static void setUp(void)
{
}

static void tearDown(void)
{
}

static void test_libStreamRouter_getDevices(void)
{
	char test_buffer[] = "test_message\n";
	char test_inBuffer[20] = {0};

	int ret;
	ret = libStreamRouter_StreamDeviceCount();

	struct StreamDeviceInfo* deviceInfo;
	struct list_node *itr_node;
	struct list_node *first_node = libStreamRouter_getStreamDevice_listentry();

	libStreamRouter_listentryForeach(itr_node, first_node)
	{
		deviceInfo = libStreamRouter_getStreamDevice_info(itr_node);
		TEST_INFO("name %s\n",deviceInfo->deviceName);
	}

	TEST_INFO("Number of %u\n", ret);
}

static void test_libStreamRouter_getStreams()
{
	int ret;
	int streamsCount= libStreamRouter_getStreamCount();
	struct StreamDeviceMap mapping[8];
	ret = libStreamRouter_getStreamDeviceMap(&mapping[0], sizeof(mapping));
	mapping[STREAM_error].deviceType	= STREAMDEVICE_syslog;
	ret = libStreamRouter_setStreamDeviceMap(&mapping[0], sizeof(mapping));

}

static TestRef lib_ttyportmux_test(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures){
		// TEST: test cases
		new_TestFixture("test_libStreamRouter_getDevices", test_libStreamRouter_getDevices),
		new_TestFixture("test_libStreamRouter_getStreams", test_libStreamRouter_getStreams)
	};
	EMB_UNIT_TESTCALLER(lib_ttyportmux__test, "lib_ttyportmux__test", setUp, tearDown, fixtures);

	return (TestRef)&lib_ttyportmux__test;
}

TEST_CASE_INIT(libStreamRouter, test_lib_ttyportmux__start , &lib_ttyportmux_test);


