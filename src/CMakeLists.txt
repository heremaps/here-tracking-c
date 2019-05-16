

set(LIB_SOURCES
    here_tracking.c
    here_tracking_data_buffer.c
    here_tracking_http.c
    here_tracking_http_defs.c
    here_tracking_http_parser.c
    here_tracking_oauth.c
    here_tracking_tls_writer.c
    here_tracking_utils.c
    here_tracking_uuid_gen.c
    here_tracking_version.c)

add_library(heretrackingc STATIC ${LIB_SOURCES})
