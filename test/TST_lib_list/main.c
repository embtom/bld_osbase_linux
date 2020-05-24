#include <embUnit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libStreamRouter.h>
#include <test_cases.h>
#include <lib_convention__errno.h>
#include <lib_convention__macro.h>
#include <lib_thread.h>
#include <lib_thread_wakeup.h>
#include <lib_list.h>

static struct StreamDeviceMap s_streamMap[STREAM_CNT] = {
    M_STREAM_MAPPING_ENTRY(STREAMDEVICE_unix),  /* TTYSTREAM_critical */\
    M_STREAM_MAPPING_ENTRY(STREAMDEVICE_unix),  /* TTYSTREAM_error */\
    M_STREAM_MAPPING_ENTRY(STREAMDEVICE_unix),  /* TTYSTREAM_warning */\
    M_STREAM_MAPPING_ENTRY(STREAMDEVICE_unix),  /* TTYSTREAM_info */\
    M_STREAM_MAPPING_ENTRY(STREAMDEVICE_unix),  /* TTYSTREAM_info */\
    M_STREAM_MAPPING_ENTRY(STREAMDEVICE_unix),  /*TTYSTREAM_control*/
};

int main() 
{
    struct queue_attr       *testCase_list;
	struct list_node        *testCase_node;
    test_case_instance_t    *testCase;

    lib_thread__init(PROCESS_SCHED_rr,10);
    lib_thread__wakeup_init();
     libStreamRouter_Init(&s_streamMap[0], sizeof(s_streamMap));

    int ret = test_cases__init();
	if (ret < EOK) {
		return EXIT_FAILURE;
	}

	testCase_list = test_cases__get_list();
	if (testCase_list == NULL) {
		return EXIT_FAILURE;
	}

    ret = lib_list__get_begin(testCase_list ,&testCase_node, 0, NULL);
	if (ret < EOK) {
		return EXIT_FAILURE;
	}

    do {
	    testCase = (struct test_case_instance*)GET_CONTAINER_OF(testCase_node, struct test_case_instance, node);
        if(!strcmp(testCase->name, "lib_list")) {
            break;
        }
	}while(ret = lib_list__get_next(testCase_list,&testCase_node, 0, NULL), (ret == LIB_LIST__EOK));

    if (ret < EOK) {
        return EXIT_FAILURE;
    }

	TestRunner_start();
    TestRunner_runTest((*testCase->embunitTest)());
    TestRunner_end();

    return EXIT_SUCCESS;
}