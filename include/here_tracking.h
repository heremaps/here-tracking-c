/**************************************************************************************************
 * Copyright (C) 2017-2019 HERE Europe B.V.                                                       *
 * All rights reserved.                                                                           *
 *                                                                                                *
 * MIT License                                                                                    *
 * Permission is hereby granted, free of charge, to any person obtaining a copy                   *
 * of this software and associated documentation files (the "Software"), to deal                  *
 * in the Software without restriction, including without limitation the rights                   *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell                      *
 * copies of the Software, and to permit persons to whom the Software is                          *
 * furnished to do so, subject to the following conditions:                                       *
 *                                                                                                *
 * The above copyright notice and this permission notice shall be included in all                 *
 * copies or substantial portions of the Software.                                                *
 *                                                                                                *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR                     *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,                       *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE                    *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                         *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,                  *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE                  *
 * SOFTWARE.                                                                                      *
 **************************************************************************************************/

/**
 * @file here_tracking.h
 *
 * @brief Interface definition for the HERE Tracking client.
 *
 * The HERE Tracking client is the main component of the HERE Tracking library. After you initialize
 * the HERE Tracking client with all the necessary parameters, you're ready to start interacting
 * with the **HERE Tracking** service. For example, you might start with a common task such as
 * sending telemetry data from a device. To send telemetry data, use the function
 * here_tracking_send().
 *
 * @defgroup client_if Client interface
 * @{
 *
 * @brief Interface definition for the HERE Tracking client.
 *
 * The HERE Tracking client is the main component of the HERE Tracking library. After you initialize
 * the HERE Tracking client with all the necessary parameters, you're ready to start interacting
 * with the **HERE Tracking** service. For example, you might start with a common task such as
 * sending telemetry data from a device. To send telemetry data, use the function
 * here_tracking_send().
 *
 */

#ifndef HERE_TRACKING_H
#define HERE_TRACKING_H

#include <stdint.h>

#include "here_tracking_error.h"
#include "here_tracking_tls.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Macro for the maximum size of the access token in bytes. A null-terminator is required for
 *        the given parameter so the size includes the null-terminator.
 */
#define HERE_TRACKING_ACCESS_TOKEN_SIZE 1024

/**
 * @brief Macro for the maximum size of the base URL in bytes. A null-terminator is required for the
 *        given parameter so the size includes the null-terminator. In this context, "base URL"
 *        refers to the base URL of the HERE Tracking service.
 */
#define HERE_TRACKING_BASE_URL_SIZE 32

/**
 * @brief The size of the device ID in bytes. A null-terminator is not necessary for the given
 *        parameter so the size does not include a null-terminator. */
#define HERE_TRACKING_DEVICE_ID_SIZE 36

/**
 * @brief The size of the device secret in bytes. A null-terminator is not necessary for the given
 *        parameter so the size does not include a null-terminator.
 */
#define HERE_TRACKING_DEVICE_SECRET_SIZE 43


/**
 * @brief HERE Tracking request data format
 */
typedef enum
{
    /**
     * @brief Data in JSON format
     */
    HERE_TRACKING_REQ_DATA_JSON,

    /**
     * @brief Data in protobuf format
     */
    HERE_TRACKING_REQ_DATA_PROTOBUF
} here_tracking_req_type;

/**
 * @brief HERE Tracking response types for send operation.
 */
typedef enum
{
    /**
     * @brief Receive detailed response data in JSON format.
     */
    HERE_TRACKING_RESP_WITH_DATA = 0,

    /**
     * @brief Receive detailed response data in JSON format.
     */
    HERE_TRACKING_RESP_WITH_DATA_JSON = HERE_TRACKING_RESP_WITH_DATA,

    /**
     * @brief Receive only status code in response and no additional data.
     */
    HERE_TRACKING_RESP_STATUS_ONLY = 1,

    /**
     * @brief Receive detailed response data in protobuf format.
     */
    HERE_TRACKING_RESP_WITH_DATA_PROTOBUF = 2
} here_tracking_resp_type;

/**
 * @deprecated Will be removed in ::HERE_TRACKING_VERSION_MAJOR 2.
 *
 * @brief Callback function that is called when response to send operation is received from
 *        HERE Tracking.
 *
 * Possible error codes:
 * - ::HERE_TRACKING_OK - HERE Tracking client received the data successfully.
 * - ::HERE_TRACKING_ERROR_BUFFER_TOO_SMALL - HERE Tracking client received the data successfully
 *   but it doesn't fit into the data buffer provided in here_tracking_send(). The data will be
 *   truncated to the size of the provided buffer and the @p data_size parameter will be set to the
 *   size of the received data before truncation.
 *
 * @param[in] err The error code for the data callback.
 * @param[in] data The data buffer.
 * @param[in] data_size The size of data buffer in bytes.
 * @param[in] user_data User data passed from here_tracking_set_recv_data_cb().
 */
typedef void (*here_tracking_recv_data_cb)(here_tracking_error err,
                                           const char* data,
                                           uint32_t data_size,
                                           void* user_data);

/**
 * @brief Callback function that is called when HERE Tracking client is sending data to
 * HERE Tracking server.
 *
 * @param[out] data Pointer to buffer containing data to be sent.
 *                  Set to NULL to indicate that there is no more data to be sent.
 * @param[out] data_size Number of bytes to send.
 *                       Set to 0 to indicate that there is no more data to be sent.
 * @param[in] user_data User data.
 * @return ::HERE_TRACKING_OK in case of success.
 * @return Other error code if error occured while handling the callback.
 */
typedef here_tracking_error (*here_tracking_send_cb)(const uint8_t** data,
                                                     size_t* data_size,
                                                     void* user_data);

/**
 * @brief Receive events.
 */
typedef enum
{
    /**
     * @brief Event informing the total size of response in bytes.
     */
    HERE_TRACKING_RECV_EVT_RESP_SIZE     = 0,

    /**
     * @brief Event informing that (part of) response data has been received.
     */
    HERE_TRACKING_RECV_EVT_RESP_DATA     = 1,

    /**
     * @brief Event informing that response is complete.
     */
    HERE_TRACKING_RECV_EVT_RESP_COMPLETE = 2
} here_tracking_recv_evt;

typedef struct
{
    /**
     * @brief Event type
     */
    here_tracking_recv_evt evt;

    /**
     * @brief Error status of the event.
     *
     * Set for all event types.
     */
    here_tracking_error err;

    /**
     * @brief Pointer buffer containing received response data.
     *
     * Set only for event ::HERE_TRACKING_IO_RECV_EVT_RESP_DATA.
     */
    uint8_t* data;

    /**
     * @brief Size of data in bytes.
     *
     * - For event ::HERE_TRACKING_IO_RECV_EVT_RESP_SIZE set to the total number of bytes in the
     * response.
     * - For event ::HERE_TRACKING_IO_RECV_EVT_RESP_DATA set to the number of bytes in the buffer
     * @link here_tracking_io_recv_data::data @endlink .
     */
    size_t data_size;
} here_tracking_recv_data;

/**
 * @brief Receive event callback.
 *
 * @param[in] data Receive event data.
 * @param[in] user_data User data.
 * @return ::HERE_TRACKING_OK if the event was processed without errors.
 * @return Other error code if error occured while processing the event.
 */
typedef here_tracking_error (*here_tracking_recv_cb)(const here_tracking_recv_data* data,
                                                     void* user_data);

/**
 * @brief The HERE Tracking Client Structure.
 */
typedef struct
{
    /** @brief Access token for HERE Tracking. You must terminate the string with `\0`. */
    char access_token[HERE_TRACKING_ACCESS_TOKEN_SIZE];

    /**
     * @brief The base URL of the HERE Tracking service. You must terminate the string with `\0`.
     */
    char base_url[HERE_TRACKING_BASE_URL_SIZE];

    /** @brief HERE Device ID. A null-terminator is not required. */
    char device_id[HERE_TRACKING_DEVICE_ID_SIZE];

    /** @brief HERE Device Secret. A null-terminator is not required. */
    char device_secret[HERE_TRACKING_DEVICE_SECRET_SIZE];

    /**
     * @brief Time difference in seconds between the device time and the time on the HERE Tracking
     *        server.
     */
    int32_t srv_time_diff;

    /** @brief The expiry time of the access token. */
    uint32_t token_expiry;

    /** @brief The TLS connection handle used by the client. */
    here_tracking_tls tls;

    /**
     * @brief Data callback function that has been set in here_tracking_set_recv_data_cb().
     *        NULL if the callback hasn't been set.
     */
    here_tracking_recv_data_cb data_cb;

    /** @brief User data to pass back in the data callback. */
    void* data_cb_user_data;

    /** @brief Correlation id set by the user. You must terminate the string with `\0`.*/
    const char* correlation_id;

    /** @brief User agent set by the user. You must terminate the string with `\0`. */
    const char* user_agent;

    /** @brief Indicates time when client can make requests again after being rate-limited. */
    uint32_t retry_after;

} here_tracking_client;

/**
 * @brief Initializes the HERE Tracking client.
 *
 * @param[in] client Pointer to the HERE Tracking client structure.
 * @param[in] device_id The ID of the device. A null-terminator is not required.
 *                      The length of the ID must match the size defined in
 *                      #HERE_TRACKING_DEVICE_ID_SIZE.
 * @param[in] device_secret The device secret. A null-terminator is not required.
 *                          The length of the secret must match the size defined in
 *                          #HERE_TRACKING_DEVICE_SECRET_SIZE.
 * @param[in] base_url The base URL of the HERE Tracking service. You must terminate the string
 *                     with `\0`. The length of the URL must not exceed the size defined in
 *                     #HERE_TRACKING_BASE_URL_SIZE (including 0-terminator).
 * @return ::HERE_TRACKING_OK The HERE Tracking client initialized successfully.
 * @return ::HERE_TRACKING_ERROR_INVALID_INPUT One or more input parameters were invalid.
 */
here_tracking_error here_tracking_init(here_tracking_client* client,
                                       const char* device_id,
                                       const char* device_secret,
                                       const char* base_url);

/**
 * @brief Releases all system resources that are held by the HERE Tracking client.
 *
 * @param[in] client Pointer to the initialized client structure.
 * @return ::HERE_TRACKING_OK The system resources were successfully released.
 */
here_tracking_error here_tracking_free(here_tracking_client* client);

/**
 * @deprecated Will be removed in ::HERE_TRACKING_VERSION_MAJOR 2.
 *
 * @brief Sets the callback method to be invoked when data is received from HERE Tracking.
 *
 * Subsequent calls to this method will replace the previously set callback. Passing NULL in @p cb
 * will remove the callback if one has been set.
 *
 * @param[in] client Pointer to the initialized client structure.
 * @param[in] cb Data callback method.
 * @param[in] user_data User data to pass in data callback.
 * @return ::HERE_TRACKING_OK The HERE Tracking client successfully received data.
 * @return ::HERE_TRACKING_ERROR_INVALID_INPUT One or more input parameters were invalid.
 */
here_tracking_error here_tracking_set_recv_data_cb(here_tracking_client* client,
                                                   here_tracking_recv_data_cb cb,
                                                   void* user_data);

/**
 * @brief Requests an access token for your device from HERE Tracking.
 *
 * If the device already has an access token, this method invalidates the existing access token and
 * requests a new one. If a new access token is successfully received, the access token is written
 * to the @link here_tracking_client::access_token access_token @endlink property of the client
 * structure.
 *
 * @param[in] client Pointer to the initialized client structure.
 * @return ::HERE_TRACKING_OK The access token was successfully received.
 * @return ::HERE_TRACKING_ERROR_INVALID_INPUT One or more input parameters were invalid.
 * @return ::HERE_TRACKING_ERROR_TIME_MISMATCH The time on the device doesn't match the time on the
 *         HERE Tracking server.
 * @return ::HERE_TRACKING_ERROR An unknown error occurred.
 */
here_tracking_error here_tracking_auth(here_tracking_client* client);

/**
 * @deprecated Will be removed in ::HERE_TRACKING_VERSION_MAJOR 2.
 *
 * @brief Sends data to HERE Tracking.
 *
 * This method also requests a new access token if there isn't one available yet or if the current
 * one has expired. After the data in the given buffer is sent, the same buffer is used to receive
 * the response data. The caller must wait for the data callback before using or releasing this
 * buffer.
 *
 * @param[in] client Pointer to client structure with valid access token.
 * @param[in,out] data The data buffer.
 * @param[in] send_size The size of the data to send in bytes.
 * @param[in] recv_size Maximum number of bytes to receive.
 * @return ::HERE_TRACKING_OK HERE Tracking client has successfully sent the data to HERE Tracking.
 * @return ::HERE_TRACKING_ERROR_INVALID_INPUT One or more input parameters were invalid.
 * @return ::HERE_TRACKING_ERROR_TIME_MISMATCH The time on the device doesn't match the time on the
 *         HERE Tracking server.
 * @return ::HERE_TRACKING_ERROR An unknown error occurred.
 */
here_tracking_error here_tracking_send(here_tracking_client* client,
                                       char* data,
                                       uint32_t send_size,
                                       uint32_t recv_size);

/**
 * @brief Sends data to HERE Tracking.
 *
 * This method also requests a new access token if there isn't one available yet or if the current
 * one has expired.
 *
 * @param[in] client Pointer to the initialized client structure.
 * @param[in] send_cb Callback function that will be called by the library to request data for
 *                    sending.
 * @param[in] recv_cb Callback function that will be called by the library when response data is
 *                    received from HERE Tracking server.
 * @param[in] req_type Format of data that will be sent to HERE Tracking server.
 * @param[in] resp_type Response type to use.
 * @param[in] user_data User data to pass back as an argument in send and recv callbacks.
 * @return ::HERE_TRACKING_OK HERE Tracking client has successfully sent the data to HERE Tracking.
 * @return ::HERE_TRACKING_ERROR_INVALID_INPUT One or more input parameters were invalid.
 * @return ::HERE_TRACKING_ERROR_TIME_MISMATCH The time on the device doesn't match the time on the
 *         HERE Tracking server.
 * @return ::HERE_TRACKING_ERROR An unknown error occurred.
 */
here_tracking_error here_tracking_send_stream(here_tracking_client* client,
                                              here_tracking_send_cb send_cb,
                                              here_tracking_recv_cb recv_cb,
                                              here_tracking_req_type req_type,
                                              here_tracking_resp_type resp_type,
                                              void* user_data);

#ifdef __cplusplus
}
#endif

#endif /* HERE_TRACKING_H */

/** @} */
