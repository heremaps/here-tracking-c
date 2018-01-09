/**************************************************************************************************
* Copyright (C) 2017 HERE Europe B.V.                                                             *
* All rights reserved.                                                                            *
*                                                                                                 *
* MIT License                                                                                     *
* Permission is hereby granted, free of charge, to any person obtaining a copy                    *
* of this software and associated documentation files (the "Software"), to deal                   *
* in the Software without restriction, including without limitation the rights                    *
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell                       *
* copies of the Software, and to permit persons to whom the Software is                           *
* furnished to do so, subject to the following conditions:                                        *
*                                                                                                 *
* The above copyright notice and this permission notice shall be included in all                  *
* copies or substantial portions of the Software.                                                 *
*                                                                                                 *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR                      *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,                        *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE                     *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                          *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,                   *
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE                   *
* SOFTWARE.                                                                                       *
**************************************************************************************************/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/net.h>
#include <mbedtls/ssl.h>

#if defined MBEDTLS_DEBUG_C
#include <stdio.h>
#include <mbedtls/debug.h>
#endif

#include "here_tracking_log.h"
#include "here_tracking_tls.h"
#include "here_tracking_tls_cert.h"

/**************************************************************************************************/

typedef struct
{
    mbedtls_ctr_drbg_context ctr_drbg_ctx;
    mbedtls_entropy_context entropy_ctx;
    mbedtls_net_context net_ctx;
    mbedtls_ssl_context ssl_ctx;
    mbedtls_ssl_config ssl_conf;
    mbedtls_ssl_session ssl_session;
    mbedtls_x509_crt crt_ctx;
} here_tracking_tls_mbedtls;

/**************************************************************************************************/

#if defined MBEDTLS_DEBUG_C && HERE_TRACKING_LOG_LEVEL <= HERE_TRACKING_LOG_LEVEL_ERROR

static void here_tracking_tls_debug_cb(void* ctx,
                                       int level,
                                       const char* file,
                                       int line,
                                       const char* msg)
{
    switch(level)
    {
        case 1:
            here_tracking_log(HERE_TRACKING_LOG_LEVEL_ERROR, file, line, msg);
            break;
        case 2:
            /* fall through */
        case 3:
            here_tracking_log(HERE_TRACKING_LOG_LEVEL_INFO, file, line, msg);
            break;
        default:
            break;
    }
}

#endif

/**************************************************************************************************/

here_tracking_error here_tracking_tls_init(here_tracking_tls* tls)
{
    here_tracking_error err = HERE_TRACKING_ERROR;

    if(tls != NULL)
    {
        here_tracking_tls_mbedtls* tls_ctx = malloc(sizeof(here_tracking_tls_mbedtls));

        if(tls_ctx != NULL)
        {
            int res;

            mbedtls_ctr_drbg_init(&(tls_ctx->ctr_drbg_ctx));
            mbedtls_entropy_init(&(tls_ctx->entropy_ctx));
            mbedtls_net_init(&(tls_ctx->net_ctx));
            mbedtls_ssl_init(&(tls_ctx->ssl_ctx));
            mbedtls_ssl_config_init(&(tls_ctx->ssl_conf));
            mbedtls_ssl_session_init(&(tls_ctx->ssl_session));
            mbedtls_x509_crt_init(&(tls_ctx->crt_ctx));
            res = mbedtls_ctr_drbg_seed(&(tls_ctx->ctr_drbg_ctx),
                                        mbedtls_entropy_func,
                                        &(tls_ctx->entropy_ctx),
                                        NULL,
                                        0);

            if(res == 0)
            {
                res = mbedtls_x509_crt_parse(&(tls_ctx->crt_ctx),
                                    (unsigned char*)here_tracking_tls_cert_verisign_universal_root,
                                    strlen(here_tracking_tls_cert_verisign_universal_root) + 1);
            }

            if(res == 0)
            {
                *tls = (here_tracking_tls)tls_ctx;
                err = HERE_TRACKING_OK;
            }
            else
            {
                mbedtls_x509_crt_free(&(tls_ctx->crt_ctx));
                mbedtls_ssl_session_free(&(tls_ctx->ssl_session));
                mbedtls_ssl_config_free(&(tls_ctx->ssl_conf));
                mbedtls_ssl_free(&(tls_ctx->ssl_ctx));
                mbedtls_net_free(&(tls_ctx->net_ctx));
                mbedtls_entropy_free(&(tls_ctx->entropy_ctx));
                mbedtls_ctr_drbg_free(&(tls_ctx->ctr_drbg_ctx));
                free(tls_ctx);
            }
        }
    }
    else
    {
        err = HERE_TRACKING_ERROR_INVALID_INPUT;
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_tls_free(here_tracking_tls* tls)
{
    here_tracking_error err = HERE_TRACKING_ERROR;

    if(tls != NULL)
    {
        if(*tls != NULL)
        {
            here_tracking_tls_mbedtls* tls_ctx = (here_tracking_tls_mbedtls*)(*tls);

            here_tracking_tls_close(*tls);
            mbedtls_entropy_free(&(tls_ctx->entropy_ctx));
            mbedtls_ctr_drbg_free(&(tls_ctx->ctr_drbg_ctx));
            mbedtls_x509_crt_free(&(tls_ctx->crt_ctx));
            mbedtls_ssl_session_free(&(tls_ctx->ssl_session));
            free(tls_ctx);
            *tls = NULL;
            err = HERE_TRACKING_OK;
        }
    }
    else
    {
        err = HERE_TRACKING_ERROR_INVALID_INPUT;
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_tls_connect(here_tracking_tls tls,
                                              const char* host,
                                              uint16_t port)
{
    here_tracking_error err = HERE_TRACKING_ERROR;

    if(tls != NULL && host != NULL && strlen(host) > 0)
    {
        here_tracking_tls_mbedtls* tls_ctx = (here_tracking_tls_mbedtls*)tls;
        char port_string[6];
        int res;

        snprintf(port_string, 6, "%u", port);
        res = mbedtls_net_connect(&(tls_ctx->net_ctx), host, port_string, MBEDTLS_NET_PROTO_TCP);

        if(res == 0)
        {
            res = mbedtls_ssl_config_defaults(&(tls_ctx->ssl_conf),
                                              MBEDTLS_SSL_IS_CLIENT,
                                              MBEDTLS_SSL_TRANSPORT_STREAM,
                                              MBEDTLS_SSL_PRESET_DEFAULT);
#if defined MBEDTLS_DEBUG_C
            /*
             * mbedtls debug levels are defined and mapped to HERE_TRACKING_LOG_LEVEL as follows:
             *    0 - none         - HERE_TRACKING_LOG_LEVEL_NONE, HERE_TRACKING_LOG_LEVEL_FATAL
             *    1 - error        - HERE_TRACKING_LOG_LEVEL_ERROR
             *    2 - state change - HERE_TRACKING_LOG_LEVEL_INFO
             *    3 - info         - HERE_TRACKING_LOG_LEVEL_INFO
             *    4 - verbose      - Never enabled
             */
#if HERE_TRACKING_LOG_LEVEL <= HERE_TRACKING_LOG_LEVEL_ERROR
            mbedtls_ssl_conf_dbg(&(tls_ctx->ssl_conf), here_tracking_tls_debug_cb, NULL);
#if HERE_TRACKING_LOG_LEVEL == HERE_TRACKING_LOG_LEVEL_ERROR
            mbedtls_debug_set_threshold(1);
#elif HERE_TRACKING_LOG_LEVEL == HERE_TRACKING_LOG_LEVEL_WARNING
            mbedtls_debug_set_threshold(1);
#elif HERE_TRACKING_LOG_LEVEL == HERE_TRACKING_LOG_LEVEL_INFO
            mbedtls_debug_set_threshold(3);
#endif
#endif /* HERE_TRACKING_LOG_LEVEL != HERE_TRACKING_LOG_LEVEL_NONE */
#endif /* MBEDTLS_DEBUG_C */
        }

        if(res == 0)
        {
            mbedtls_ssl_conf_ca_chain(&(tls_ctx->ssl_conf), &(tls_ctx->crt_ctx), NULL);
            mbedtls_ssl_conf_rng(&(tls_ctx->ssl_conf),
                                 mbedtls_ctr_drbg_random,
                                 &(tls_ctx->ctr_drbg_ctx));
            res = mbedtls_ssl_setup(&(tls_ctx->ssl_ctx), &(tls_ctx->ssl_conf));
        }

        if(res == 0)
        {
            res = mbedtls_ssl_set_hostname(&(tls_ctx->ssl_ctx), host);
        }

        if(res == 0)
        {
            res = mbedtls_ssl_set_session(&(tls_ctx->ssl_ctx), &(tls_ctx->ssl_session));
        }

        if(res == 0)
        {
            mbedtls_ssl_set_bio(&(tls_ctx->ssl_ctx),
                                &(tls_ctx->net_ctx),
                                mbedtls_net_send,
                                mbedtls_net_recv,
                                NULL);

            while((res = mbedtls_ssl_handshake(&(tls_ctx->ssl_ctx))) != 0)
            {
                if(res != MBEDTLS_ERR_SSL_WANT_READ && res != MBEDTLS_ERR_SSL_WANT_WRITE)
                {
                    break;
                }
            }
        }

        if(res == 0)
        {
            err = HERE_TRACKING_OK;
        }
        else
        {
            mbedtls_ssl_free(&(tls_ctx->ssl_ctx));
            mbedtls_ssl_config_free(&(tls_ctx->ssl_conf));
            mbedtls_net_free(&(tls_ctx->net_ctx));
        }
    }
    else
    {
        err = HERE_TRACKING_ERROR_INVALID_INPUT;
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_tls_close(here_tracking_tls tls)
{
    here_tracking_error err = HERE_TRACKING_ERROR;

    if(tls != NULL)
    {
        here_tracking_tls_mbedtls* tls_ctx = (here_tracking_tls_mbedtls*)tls;
        mbedtls_ssl_close_notify((&tls_ctx->ssl_ctx));
        mbedtls_ssl_get_session(&(tls_ctx->ssl_ctx), &(tls_ctx->ssl_session));
        mbedtls_ssl_free(&(tls_ctx->ssl_ctx));
        mbedtls_ssl_config_free(&(tls_ctx->ssl_conf));
        mbedtls_net_free(&(tls_ctx->net_ctx));
        err = HERE_TRACKING_OK;
    }
    else
    {
        err = HERE_TRACKING_ERROR_INVALID_INPUT;
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_tls_read(here_tracking_tls tls, char* data, uint32_t* data_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR;

    if(tls != NULL && data != NULL && data_size != NULL && (*data_size) > 0)
    {
        here_tracking_tls_mbedtls* tls_ctx = (here_tracking_tls_mbedtls*)tls;
        uint32_t read = 0;

        while(true)
        {
            int res = mbedtls_ssl_read(&(tls_ctx->ssl_ctx),
                                       (unsigned char*)(data + read),
                                       (*data_size) - read);

            if(res == MBEDTLS_ERR_SSL_WANT_READ || res == MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                continue;
            }

            if(res == 0 || res == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
            {
                (*data_size) = read;
                err = HERE_TRACKING_OK;
                break;
            }

            if(res < 0)
            {
                break;
            }

            read += res;

            if(read == (*data_size))
            {
                err = HERE_TRACKING_OK;
                break;
            }
        }
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_tls_write(here_tracking_tls tls,
                                            const char* data,
                                            uint32_t* data_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR;

    if(tls != NULL && data != NULL && data_size != NULL && (*data_size) > 0)
    {
        here_tracking_tls_mbedtls* tls_ctx = (here_tracking_tls_mbedtls*)tls;
        uint32_t written = 0;

        while(true)
        {
            int res = mbedtls_ssl_write(&(tls_ctx->ssl_ctx),
                                        (unsigned char*)(data + written),
                                        (*data_size) - written);

            if(res == MBEDTLS_ERR_SSL_WANT_READ || res == MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                continue;
            }

            if(res <= 0)
            {
                break;
            }

            written += res;

            if(written == (*data_size))
            {
                err = HERE_TRACKING_OK;
                break;
            }
        }
    }

    return err;
}
