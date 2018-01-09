
option(MbedTLS "Use MbedTLS" ON)

option(OpenSSL "Use OpenSSL" OFF)

if(MbedTLS)
  find_package(MbedTLS REQUIRED)
  include_directories(${MBEDTLS_INCLUDE_DIR})
  set(APPLIB_TLS_SOURCES
      here_tracking_base64_mbedtls.c
      here_tracking_hmac_sha_mbedtls.c
      here_tracking_tls_mbedtls.c)
  set(APPLIB_TLS_LIBS ${MBEDTLS_LIBRARIES})
elseif(OpenSSL)
  find_package(OpenSSL REQUIRED)
  include_directories(${OPENSSL_INCLUDE_DIR})
  set(APPLIB_TLS_SOURCES here_tracking_base64_openssl.c here_tracking_hmac_sha_openssl.c)
  set(APPLIB_TLS_LIBS ${OPENSSL_LIBRARIES})
endif()

set(APPLIB_SOURCES
    here_tracking_log.c
    here_tracking_time.c
    here_tracking_tls_cert.c
    ${APPLIB_TLS_SOURCES})

add_library(heretrackingappc STATIC ${APPLIB_SOURCES})

set(APP_SOURCES here_tracking_app.c)

add_executable(here_tracking_app ${APP_SOURCES})
target_link_libraries(here_tracking_app heretrackingc heretrackingappc ${APPLIB_TLS_LIBS})
