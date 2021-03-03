/*  
*  Copyright (c) 2020-2021 Krzysztof Sobolewski <krzysztof.sobolewski@gmail.com>
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:

*  The above copyright notice and this permission notice shall be included in all
*  copies or substantial portions of the Software.

*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*  SOFTWARE.
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


int send_via_ssl(char *host, char *port, char *text_out, char **ret_buf)
{    
    BIO *sbio;
    SSL_CTX *ctx;
    SSL *ssl;
    int part_i = 0;
    int part_actual_len = 0 ;
    int total_size = 0, buf_out_size = 0;
    int part_size = 1000;

    buf_out_size = part_size * 3;

    char *tmpbuf = NULL;
    char *buf_out = NULL;
    tmpbuf = calloc(part_size + 1, 1);
    buf_out = calloc(buf_out_size + 1, 1);

    SSL_load_error_strings();
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    ctx = SSL_CTX_new(SSLv23_client_method());
    sbio = BIO_new_ssl_connect(ctx);
    BIO_get_ssl(sbio, &ssl);

    if (!ssl) {
        printf("\nERROR: SSL not initialized. Quitting.\n\n");
        return -1;
    }

    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    // /* https://www.openssl.org/docs/man1.0.2/man3/BIO_get_ssl.html */
    BIO_set_conn_hostname(sbio, host);
    BIO_set_conn_port(sbio, port);

    BIO_puts(sbio, text_out);

    while ((part_actual_len = BIO_read(sbio, tmpbuf, part_size)) > 0) {
        total_size += part_actual_len;
        if (buf_out_size < part_i * part_size + part_actual_len) {
            buf_out_size = part_i * part_size + part_size * 3;
            buf_out = realloc(buf_out, buf_out_size + 1);
            // printf("\n\nrealloc at part %d\n\n", part_i);
        }
        memcpy(buf_out + part_i * part_size, tmpbuf, part_actual_len);

        part_i++;
        // printf("\npart_actual_len: %d LALA: %s", part_actual_len, tmpbuf);
        if((strstr(tmpbuf,"\n") != NULL) || 
                (part_actual_len < part_size)) {
            break;
        }
    }

    // printf("\nFINAL BUF OUT: %s\n\n\n", buf_out);
    free(tmpbuf);
    BIO_free(sbio);
    *ret_buf = buf_out;

    return 0;
}
