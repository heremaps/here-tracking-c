
set(TEST_TRACKING_SOURCES
    ${CMAKE_SOURCE_DIR}/src/here_tracking.c
    mocks/mock_here_tracking_http.c
    mocks/mock_here_tracking_time.c
    mocks/mock_here_tracking_tls.c
    test_here_tracking.c)
add_executable(test_here_tracking ${TEST_TRACKING_SOURCES})
target_link_libraries(test_here_tracking ${CHECK_LDFLAGS})
add_test(NAME test_here_tracking COMMAND test_here_tracking)

set(TEST_TRACKING_DATA_BUFFER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/here_tracking_data_buffer.c
    test_here_tracking_data_buffer.c)
add_executable(test_here_tracking_data_buffer ${TEST_TRACKING_DATA_BUFFER_SOURCES})
target_link_libraries(test_here_tracking_data_buffer ${CHECK_LDFLAGS})
add_test(NAME test_here_tracking_data_buffer COMMAND test_here_tracking_data_buffer)

set(TEST_TRACKING_HTTP_SOURCES
    ${CMAKE_SOURCE_DIR}/src/here_tracking_http.c
    ${CMAKE_SOURCE_DIR}/src/here_tracking_http_defs.c
    ${CMAKE_SOURCE_DIR}/src/here_tracking_http_parser.c # Not currently mocking HTTP parser
    ${CMAKE_SOURCE_DIR}/src/here_tracking_utils.c
    mocks/mock_here_tracking_data_buffer.c
    mocks/mock_here_tracking_log.c
    mocks/mock_here_tracking_time.c
    mocks/mock_here_tracking_tls.c
    mocks/mock_here_tracking_tls_writer.c
    mocks/mock_here_tracking_uuid_gen.c
    test_here_tracking_http.c)
add_executable(test_here_tracking_http ${TEST_TRACKING_HTTP_SOURCES})
target_link_libraries(test_here_tracking_http ${CHECK_LDFLAGS})
add_test(NAME test_here_tracking_http COMMAND test_here_tracking_http)

set(TEST_TRACKING_HTTP_PARSER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/here_tracking_http_parser.c
    ${CMAKE_SOURCE_DIR}/src/here_tracking_utils.c
    test_here_tracking_http_parser.c)
add_executable(test_here_tracking_http_parser ${TEST_TRACKING_HTTP_PARSER_SOURCES})
target_link_libraries(test_here_tracking_http_parser ${CHECK_LDFLAGS})
add_test(NAME test_here_tracking_http_parser COMMAND test_here_tracking_http_parser)

set(TEST_TRACKING_OAUTH_SOURCES
    ${CMAKE_SOURCE_DIR}/src/here_tracking_http_defs.c
    ${CMAKE_SOURCE_DIR}/src/here_tracking_oauth.c
    ${CMAKE_SOURCE_DIR}/src/here_tracking_utils.c
    mocks/mock_here_tracking_data_buffer.c
    mocks/mock_here_tracking_log.c
    mocks/mock_here_tracking_time.c
    test_here_tracking_oauth.c)
add_executable(test_here_tracking_oauth ${TEST_TRACKING_OAUTH_SOURCES})
target_link_libraries(test_here_tracking_oauth ${CHECK_LDFLAGS})
add_test(NAME test_here_tracking_oauth COMMAND test_here_tracking_oauth)

set(TEST_TRACKING_TLS_WRITER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/here_tracking_tls_writer.c
    mocks/mock_here_tracking_data_buffer.c
    mocks/mock_here_tracking_tls.c
    test_here_tracking_tls_writer.c)
add_executable(test_here_tracking_tls_writer ${TEST_TRACKING_TLS_WRITER_SOURCES})
target_link_libraries(test_here_tracking_tls_writer ${CHECK_LDFLAGS})
add_test(NAME test_here_tracking_tls_writer COMMAND test_here_tracking_tls_writer)

set(TEST_TRACKING_UTILS_SOURCES
    ${CMAKE_SOURCE_DIR}/src/here_tracking_utils.c
    test_here_tracking_utils.c)
add_executable(test_here_tracking_utils ${TEST_TRACKING_UTILS_SOURCES})
target_link_libraries(test_here_tracking_utils ${CHECK_LDFLAGS})
add_test(NAME test_here_tracking_utils COMMAND test_here_tracking_utils)

set(TEST_TRACKING_UUID_GEN_SOURCES
    ${CMAKE_SOURCE_DIR}/src/here_tracking_uuid_gen.c
    mocks/mock_here_tracking_time.c
    test_here_tracking_uuid_gen.c)
add_executable(test_here_tracking_uuid_gen ${TEST_TRACKING_UUID_GEN_SOURCES})
target_link_libraries(test_here_tracking_uuid_gen ${CHECK_LDFLAGS})
add_test(NAME test_here_tracking_uuid_gen COMMAND test_here_tracking_uuid_gen)

set(TEST_TRACKING_VERSION_SOURCES
${CMAKE_SOURCE_DIR}/src/here_tracking_version.c
test_here_tracking_version.c)
add_executable(test_here_tracking_version ${TEST_TRACKING_VERSION_SOURCES})
target_link_libraries(test_here_tracking_version ${CHECK_LDFLAGS})
add_test(NAME test_here_tracking_version COMMAND test_here_tracking_version)
