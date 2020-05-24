/*
 * This file is part of the EMBTOM project
 * Copyright (c) 2018-2020 Thomas Willetal 
 * (https://github.com/embtom)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* system */
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*frame*/
#include <lib_log.h>
#include <lib_clock.h>
#include <libStreamRouter.h>
#include <test_cases.h>
#include <lib_convention__errno.h>

/* *******************************************************************
 * Defines
 * ******************************************************************/
#define M_TEST_MODULE_NAME 				"test_lib_log"
#define M_TEST_FRAM__NAME				"TEST_FRAM"
#define M_TEST_FRAM__SIZE				8192
#define M_TEST_FRAM__SYSFS				"/sys/class/spi_master/spi0/spi0.0/eeprom"

/* *******************************************************************
 * Static Variables
 * *******************************************************************/
static unsigned int fram_fd = 0;
static unsigned int fram_size = 0;
static uint8_t *fram_mirror = NULL;

/* *******************************************************************
 * Static Function Prototypes
 * ******************************************************************/
// Test cases
static void test_fram__case_device_open(void);
static void test_fram__case_device_status(void);
static void test_fram__case_cycle_device_read(void);
static void test_fram__case_cycle_device_write_pattern(void);
static void test_fram__case_seek_read(void);
static void test_fram__case_seek_write(void);
static void test_fram__case_device_close_open(void);
static void test_fram__case_device_close(void);

// Helper functions
static void test_fram__help_write_hexdump(const char *_modulename, const char *_dumpname, const uint8_t *_buf, unsigned int _len, unsigned int _byte_per_line);
static void test_fram__help_generate_test_data(uint8_t * const _buf, unsigned int _length);
static int test_fram__help_check_test_data(const uint8_t * const _buf1, const uint8_t * const _buf2,  unsigned int _length);
static int test_fram__help_get_all(int _fd, uint8_t * const _buf, unsigned int _size, unsigned int *_cycle_count);
static int test_fram__help_set_all(int _fd, const uint8_t * const _buf, unsigned int _size, unsigned int *_cycle_count);

/* *******************************************************************
 * Global Functions
 * ******************************************************************/

/* *************************************************************************************************
 * \brief			Test Case open device
 * \description		Open of the FRAM device
 * *************************************************************************************************/
static void test_fram__case_device_open(void)
{
	int ret = EOK;
	msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' start", __FUNCTION__);

	fram_fd = open(M_TEST_FRAM__SYSFS, O_RDWR);
	if (fram_fd < 0) {
		msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' failed with errno %i", __FUNCTION__, errno);
		ret = -errno;
	}
	TEST_ASSERT_EQUAL_INT(EOK, ret)
}

/* *************************************************************************************************
 * \brief		    Test Case size
 * \description		Check if the FRRAM device size has the expetced size
 * *************************************************************************************************/
static void test_fram__case_device_status(void)
{
	int ret;
	struct stat fram_stat;
	msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' start", __FUNCTION__);

	ret = fstat(fram_fd, &fram_stat);
	if(ret < EOK) {
		msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' failed with errno %i", __FUNCTION__, errno);
		ret = -errno;
	}
	else {
		msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' device with size %u found", __FUNCTION__,fram_size);
	}

	TEST_ASSERT_EQUAL_INT(EOK, ret);
	fram_size = fram_stat.st_size;
	TEST_ASSERT_EQUAL_INT(fram_size,M_TEST_FRAM__SIZE);


}

/* *************************************************************************************************
 * \brief		    Test Read FRAM content
 * \description		Check if the FRRAM device size has the expetced size
 * *************************************************************************************************/
static void test_fram__case_cycle_device_read(void)
{
	int ret;
	unsigned int cycle_count;

	msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' start", __FUNCTION__);

	fram_mirror = (uint8_t*)calloc(fram_size, sizeof(uint8_t));
	TEST_ASSERT_NOT_NULL(fram_mirror);

	/*Set of the read write pointer*/
	ret = lseek(fram_fd, 0, SEEK_SET);
	TEST_ASSERT_EQUAL_INT(EOK, ret);

	ret = test_fram__help_get_all(fram_fd, fram_mirror, fram_size, &cycle_count);
	TEST_ASSERT_EQUAL_INT(fram_size, ret);

	msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' number of read bytes %u at cycles %u",__FUNCTION__, ret, cycle_count);
	//lib_log__hexdump(M_TEST_FRAM__NAME,__FUNCTION__, fram_mirror, fram_size, 32);
	test_fram__help_write_hexdump(M_TEST_FRAM__NAME,__FUNCTION__,fram_mirror, fram_size, 32);

	free(fram_mirror);
}

/* *************************************************************************************************
 * \brief		    Check if
 * \description		Check if the FRRAM device size has the expected size
 * *************************************************************************************************/
static void test_fram__case_cycle_device_write_pattern(void)
{
	unsigned int cycle_count;
	int ret;
	uint8_t *write_fram_data;
	uint8_t	*read_fram_data;
	int error_count;
	uint64_t time_diff;


	msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' start", __FUNCTION__);

	write_fram_data = (uint8_t*)calloc(fram_size, sizeof(uint8_t));
	read_fram_data = (uint8_t*)calloc(fram_size, sizeof(uint8_t));

	test_fram__help_generate_test_data(write_fram_data, fram_size);

	/*Set of the read write pointer*/
	ret = lseek(fram_fd, 0, SEEK_SET);
	TEST_ASSERT_EQUAL_INT(EOK, ret);

	time_diff = lib_clock__get_time_us();
	ret = test_fram__help_set_all(fram_fd, write_fram_data, fram_size, &cycle_count);
	TEST_ASSERT_EQUAL_INT(fram_size, ret);
	time_diff = lib_clock__get_time_us() - time_diff;


	msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' number of written bytes %u at cycles %u",__FUNCTION__, ret, cycle_count);
	msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' Required write Time of %llu",__FUNCTION__, time_diff);

	/*Set of the read write pointer*/
	ret = lseek(fram_fd, 0, SEEK_SET);

	ret = test_fram__help_get_all(fram_fd, read_fram_data, fram_size, &cycle_count);
	TEST_ASSERT_EQUAL_INT(fram_size, ret);

	error_count = test_fram__help_check_test_data(write_fram_data, read_fram_data, fram_size);
	TEST_ASSERT_EQUAL_INT(0, error_count);

	//lib_log__hexdump(M_TEST_FRAM__NAME, __FUNCTION__, fram_size, 32);
	test_fram__help_write_hexdump(M_TEST_FRAM__NAME,__FUNCTION__, read_fram_data, fram_size, 32);

	msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' number of read bytes %u at cycles %u",__FUNCTION__, ret, cycle_count);
}

/* *************************************************************************************************
 * \brief		    Test SEEK FRAM read at specific locations
 * \description		Check if the FRRAM device size has the expetced size
 * *************************************************************************************************/
#define SEEK_POS_READ_INCREMENT 0x80
#define SEEK_POS_READ_COUNT		20
static void test_fram__case_seek_read(void)
{
	uint8_t read_buf[SEEK_POS_READ_COUNT] = {0};
	int ret, check_count;
	int fram_seek_pos;
	int error_count= 0;
	uint8_t test_var;

	msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' start", __FUNCTION__);

	for (fram_seek_pos = 0; fram_seek_pos < fram_size; fram_seek_pos += SEEK_POS_READ_INCREMENT)
	{
		ret = lseek(fram_fd, fram_seek_pos, SEEK_SET);
		TEST_ASSERT_EQUAL_INT(fram_seek_pos, ret);

		ret = read(fram_fd, &read_buf[0], sizeof(read_buf));
		TEST_ASSERT_EQUAL_INT(SEEK_POS_READ_COUNT, ret);

		test_var = (uint8_t)fram_seek_pos;
		if(read_buf[0] != test_var) {
			error_count++;
		}

		for(check_count=0; check_count < (SEEK_POS_READ_COUNT-1); check_count++) {
			if((read_buf[check_count]+1) != read_buf[check_count +1]) {
				error_count++;
			}
		}
	}

	TEST_ASSERT_EQUAL_INT(EOK, error_count);
}

/* *************************************************************************************************
 * \brief		    Test FRAM seek write seek read at specific locations
 * \description		Fram SEEK WRITE then SEEK READ at incremented locations
 * *************************************************************************************************/
#define SEEK_POS_READ_WRITE_INCREMENT 0x100
#define SEEK_POS_READ_WRITE_COUNT		20
#define SEEK_POS_TEST_BYTE			  0xAA
static void test_fram__case_seek_write(void)
{
	uint8_t check_buf[SEEK_POS_READ_WRITE_COUNT] = {0};
	int ret, check_count;
	int fram_seek_pos;
	int error_count= 0;
	uint8_t *fram_mirror;

	memset(&check_buf[0],0xAA, SEEK_POS_READ_WRITE_COUNT);
	//flush(fram_fd);
	msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' start", __FUNCTION__);

	for (fram_seek_pos = 0; fram_seek_pos < fram_size; fram_seek_pos += SEEK_POS_READ_WRITE_INCREMENT)
	{
		ret = lseek(fram_fd, fram_seek_pos, SEEK_SET);
		TEST_ASSERT_EQUAL_INT(fram_seek_pos, ret);

		memcpy(&check_buf[0],&fram_seek_pos,sizeof(fram_seek_pos));

		ret = write(fram_fd, &check_buf[0], sizeof(check_buf));
		TEST_ASSERT_EQUAL_INT(SEEK_POS_READ_WRITE_COUNT, ret);
	}


	for (fram_seek_pos = 0; fram_seek_pos < fram_size; fram_seek_pos += SEEK_POS_READ_WRITE_INCREMENT)
	{
		ret = lseek(fram_fd, fram_seek_pos, SEEK_SET);
		TEST_ASSERT_EQUAL_INT(fram_seek_pos, ret);

		ret = read(fram_fd, &check_buf[0], sizeof(check_buf));
		TEST_ASSERT_EQUAL_INT(SEEK_POS_READ_WRITE_COUNT, ret);

		if(*((int*)&check_buf[0])!=fram_seek_pos) {
			error_count++;
		}

		for(check_count=4; check_count < (SEEK_POS_READ_WRITE_COUNT); check_count++) {
			if(check_buf[check_count] != SEEK_POS_TEST_BYTE) {
				error_count++;
			}
		}
	}

	fram_mirror = (uint8_t*)calloc(fram_size, sizeof(uint8_t));
	ret = test_fram__help_get_all(fram_fd, fram_mirror, fram_size, NULL);
	TEST_ASSERT_EQUAL_INT(fram_size, ret);

	test_fram__help_write_hexdump(M_TEST_FRAM__NAME,__FUNCTION__, &fram_mirror[0], fram_size, 32);
	free(fram_mirror);

	TEST_ASSERT_EQUAL_INT(EOK, error_count);
}

/* *************************************************************************************************
 * \brief		    Test Case Close device again
 * \description		Check if the FRRAM device size has the expected size
 * *************************************************************************************************/
#define EMPTY_STREAM	"deadbeaf"
static void test_fram__case_device_close_open(void)
{
	int ret, count, write_val;
	int error_count = 0;
	char*read_ptr=EMPTY_STREAM;
	msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' start", __FUNCTION__);

	fram_mirror = (uint8_t*)calloc(fram_size, sizeof(uint8_t));

	for(count=0; count<fram_size; count+=sizeof(EMPTY_STREAM)) {

		if(count + sizeof(EMPTY_STREAM) >= fram_size)
			write_val = fram_size-count-1;
		else
			write_val = sizeof(EMPTY_STREAM);
		memcpy(fram_mirror+count,read_ptr,write_val);
	}

	ret = test_fram__help_set_all(fram_fd, fram_mirror, fram_size, NULL);
	TEST_ASSERT_EQUAL_INT(fram_size, ret);

	ret = close(fram_fd);
	if(ret < EOK) {
		ret = -errno;
	}

	TEST_ASSERT_EQUAL_INT(EOK, ret);
	memset(fram_mirror, 0, fram_size);

	fram_fd = open(M_TEST_FRAM__SYSFS, O_RDWR);
	if(ret < EOK) {
		ret = -errno;
	}
	TEST_ASSERT_EQUAL_INT(EOK, ret);

	ret = test_fram__help_get_all(fram_fd, fram_mirror, fram_size, NULL);
	if(ret < EOK) {
		ret = -errno;
	}
	TEST_ASSERT_EQUAL_INT(fram_size, ret);

	for(count=0; count<fram_size; count+=sizeof(EMPTY_STREAM))
	{
		if(count + sizeof(EMPTY_STREAM) >= fram_size)
			write_val = fram_size-count-1;
		else
			write_val = sizeof(EMPTY_STREAM);

		if (strncmp((char*)(fram_mirror+count), read_ptr, write_val) != 0) {
			error_count++;
		}
	}

	TEST_ASSERT_EQUAL_INT(EOK, error_count);
	test_fram__help_write_hexdump(M_TEST_FRAM__NAME,__FUNCTION__, &fram_mirror[0], fram_size, 32);
	free(fram_mirror);
}

/* *************************************************************************************************
 * \brief		    Test finished
 * \description		Test finished close device
 * *************************************************************************************************/
static void test_fram__case_device_close(void)
{
	int ret;
	msg(LOG_LEVEL_info, M_TEST_FRAM__NAME, "Test '%s()' start", __FUNCTION__);

	ret = close(fram_fd);
	if(ret < EOK) {
		ret = -errno;
	}

	TEST_ASSERT_EQUAL_INT(EOK, ret);
}

/* *************************************************************************************************
 * \brief		    Test finished
 * \description		Test finished close device
 * *************************************************************************************************/
#define BASE_LOG_PATH 	"/tmp"
static void test_fram__help_write_hexdump(const char *_modulename, const char *_dumpname, const uint8_t *_buf, unsigned int _len, unsigned int _byte_per_line)
{
	FILE *file_dump = NULL;
	char file_path[256];
	snprintf(file_path, sizeof(file_path), "%s/%s__%s.txt",BASE_LOG_PATH,_modulename, _dumpname);
	/* Open File with override */
	file_dump = fopen(file_path, "w+");
	if(file_dump == NULL){
		TEST_INFO ("'%s()' errno %u\n",errno);
		return;
	}

	//lib_log__set_stream(file_dump);
	//lib_log__hexdump(_modulename,_dumpname, _buf, _len, _byte_per_line);
	//lib_log__set_stream(stdout);
	//fclose(file_dump);
}

/* *************************************************************************************************
 * \brief		    Generate a pattern to FRAM write
 * \description		Generate a test pattern to write
 * *************************************************************************************************/
static void test_fram__help_generate_test_data(uint8_t * const _buf, unsigned int _length)
{
	unsigned int count;
	TEST_ASSERT_NOT_NULL(_buf);

	for(count = 0; count < _length; count++) {
		_buf[count] = count;
	}
	return;
}

/* *************************************************************************************************
 * \brief		    Check if two buffers are equal
 * \description		Used to check the test patterns
 * *************************************************************************************************/
static int test_fram__help_check_test_data(const uint8_t * const _buf1, const uint8_t * const _buf2,  unsigned int _length)
{
	int error_count = 0;
	int count;

	for (count = 0; count < _length; count++) {
		if (_buf1[count] != _buf2[count]) {
			error_count++;
		}
	}

	return error_count;
}

/* *************************************************************************************************
 * \brief		    Helper Function to get the whole FRAM
 * \description		The complete FRAM size
 * *************************************************************************************************/
static int test_fram__help_get_all(int _fd, uint8_t * const _buf, unsigned int _size, unsigned int *_cycle_count)
{
	int ret;
	unsigned int read_bytes, cycle_count;

	if (_buf== NULL)
		return -EPAR_NULL;

	lseek(_fd, 0, SEEK_SET);

	for(read_bytes = 0, cycle_count = 0; read_bytes < _size; cycle_count++ )
	{
		ret = read(_fd, (_buf+read_bytes), (_size-read_bytes));
		if(ret < EOK) {
			ret = -errno;
			break;
		}
		read_bytes = read_bytes+ ret;
	}

	if(_cycle_count!= NULL)
		*_cycle_count = cycle_count;

	return read_bytes;
}

/* *************************************************************************************************
 * \brief		    Helper Function to write the whole FRAM
 * \description		The complete FRAM size
 * *************************************************************************************************/
static int test_fram__help_set_all(int _fd, const uint8_t * const _buf, unsigned int _size, unsigned int *_cycle_count)
{
	int ret;
	unsigned int write_bytes, cycle_count;

	uint8_t *write_ptr;
	write_ptr = (uint8_t*)_buf;

	if (_buf== NULL)
		return -EPAR_NULL;

	lseek(_fd, 0, SEEK_SET);

	for(write_bytes = 0, cycle_count = 0; write_bytes < _size; cycle_count++ )
	{
		ret = write(_fd, (write_ptr+write_bytes), (_size-write_bytes));
		if(ret < EOK) {
			ret = -errno;
			break;
		}
		write_bytes = write_bytes+ ret;
	}

	if(_cycle_count!= NULL)
		*_cycle_count = cycle_count;

	return write_bytes;
}


static void setUp(void)
{
}

static void tearDown(void)
{
}

static TestRef eeprom_test(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
    	new_TestFixture("test_fram__device_open", test_fram__case_device_open),
    	new_TestFixture("test_fram__case_device_status", test_fram__case_device_status),
		new_TestFixture("test_fram__case_cycle_device_read", test_fram__case_cycle_device_read),
		new_TestFixture("test_fram__case_cycle_device_write_pattern", test_fram__case_cycle_device_write_pattern),
		new_TestFixture("test_fram__case_seek_read", test_fram__case_seek_read),
		new_TestFixture("test_fram__case_seek_write", test_fram__case_seek_write),
		new_TestFixture("test_fram__case_device_close_open", test_fram__case_device_close_open),
		new_TestFixture("test_fram__case_device_close", test_fram__case_device_close)
    };
    EMB_UNIT_TESTCALLER(test_fram, "test_fram", setUp, tearDown, fixtures);

    return (TestRef)&test_fram;
}

TEST_CASE_INIT(test_eeprom, test_eeprom__start , &eeprom_test);


