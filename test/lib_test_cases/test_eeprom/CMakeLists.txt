project(test_eeprom)

IF(UNIX)
    SET(UNIT_TEST_SRC test_eeprom.c)
    INSTALL_TEST_CASE(${UNIT_TEST_SRC})
    target_compile_definitions(${PROJECT_NAME} PRIVATE TRACE)
ENDIF(UNIX)