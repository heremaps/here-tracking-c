
find_package(MbedTLS)
find_package(OpenSSL)

if(MBEDTLS_FOUND)
  set(TEST_BASE64_MBEDTLS_NO_MOCK_SOURCES
      ${CMAKE_SOURCE_DIR}/app/src/here_tracking_base64_mbedtls.c
      test_here_tracking_base64_mbedtls_no_mock.c)
  add_executable(test_here_tracking_base64_mbedtls_no_mock ${TEST_BASE64_MBEDTLS_NO_MOCK_SOURCES})
  target_link_libraries(test_here_tracking_base64_mbedtls_no_mock
                        ${MBEDTLS_LIBRARIES}
                        ${CHECK_LDFLAGS})
  add_test(NAME
           test_here_tracking_base64_mbedtls_no_mock
           COMMAND
           test_here_tracking_base64_mbedtls_no_mock)

  set(TEST_HMAC_SHA_MBEDTLS_NO_MOCK_SOURCES
      ${CMAKE_SOURCE_DIR}/app/src/here_tracking_hmac_sha_mbedtls.c
      test_here_tracking_hmac_sha_mbedtls_no_mock.c)
  add_executable(test_here_tracking_hmac_sha_mbedtls_no_mock
                 ${TEST_HMAC_SHA_MBEDTLS_NO_MOCK_SOURCES})
  target_link_libraries(test_here_tracking_hmac_sha_mbedtls_no_mock
                        ${MBEDTLS_LIBRARIES}
                        ${CHECK_LDFLAGS})
  add_test(NAME
           test_here_tracking_hmac_sha_mbedtls_no_mock
           COMMAND
           test_here_tracking_hmac_sha_mbedtls_no_mock)
endif()

if(OPENSSL_FOUND)
  set(TEST_BASE64_OPENSSL_NO_MOCK_SOURCES
      ${CMAKE_SOURCE_DIR}/app/src/here_tracking_base64_openssl.c
      test_here_tracking_base64_openssl_no_mock.c)
  add_executable(test_here_tracking_base64_openssl_no_mock ${TEST_BASE64_OPENSSL_NO_MOCK_SOURCES})
  target_link_libraries(test_here_tracking_base64_openssl_no_mock
                        ${OPENSSL_LIBRARIES}
                        ${CHECK_LDFLAGS})
  add_test(NAME
           test_here_tracking_base64_openssl_no_mock
           COMMAND
           test_here_tracking_base64_openssl_no_mock)

  set(TEST_HMAC_SHA_OPENSSL_NO_MOCK_SOURCES
      ${CMAKE_SOURCE_DIR}/app/src/here_tracking_hmac_sha_openssl.c
      test_here_tracking_hmac_sha_openssl_no_mock.c)
  add_executable(test_here_tracking_hmac_sha_openssl_no_mock
                 ${TEST_HMAC_SHA_OPENSSL_NO_MOCK_SOURCES})
  target_link_libraries(test_here_tracking_hmac_sha_openssl_no_mock
                        ${OPENSSL_LIBRARIES}
                        ${CHECK_LDFLAGS})
  add_test(NAME
           test_here_tracking_hmac_sha_openssl_no_mock
           COMMAND
           test_here_tracking_hmac_sha_openssl_no_mock)
endif()

set(TEST_TIME_SOURCES ${CMAKE_SOURCE_DIR}/app/src/here_tracking_time.c test_here_tracking_time.c)
add_executable(test_here_tracking_time ${TEST_TIME_SOURCES})
target_link_libraries(test_here_tracking_time ${CHECK_LDFLAGS})
add_test(NAME test_here_tracking_time COMMAND test_here_tracking_time)
