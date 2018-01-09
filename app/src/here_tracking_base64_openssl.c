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

#include <string.h>

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

#include "here_tracking_base64.h"

/**************************************************************************************************/

here_tracking_error here_tracking_base64_enc(const char* in,
                                             uint32_t in_size,
                                             char* out,
                                             uint32_t* out_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR;

    if(in != NULL && in_size > 0 && out != NULL && out_size != NULL)
    {
        uint32_t enc_size = here_tracking_base64_enc_size(in_size);

        if((*out_size) >= enc_size)
        {
            BIO* b64 = BIO_new(BIO_f_base64());

            if(b64 != NULL)
            {
                BIO* bio = BIO_new(BIO_s_mem());

                if(bio != NULL)
                {
                   bio = BIO_push(b64, bio);
                   BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

                   if(BIO_write(bio, in, in_size) == in_size)
                   {
                       BUF_MEM* buf;
                       BIO_flush(bio);
                       BIO_get_mem_ptr(bio, &buf);

                       if(buf->data != NULL && buf->length > 0 && buf->length <= (*out_size))
                       {
                           memcpy(out, buf->data, buf->length);
                           (*out_size) = buf->length;
                           err = HERE_TRACKING_OK;
                       }

                       BIO_set_close(bio, BIO_CLOSE);
                       BIO_free_all(bio);
                   }
                }
                else
                {
                    BIO_free(b64);
                }
            }
        }
        else
        {
            err = HERE_TRACKING_ERROR_BUFFER_TOO_SMALL;
        }
    }
    else
    {
        err = HERE_TRACKING_ERROR_INVALID_INPUT;
    }

    return err;
}
