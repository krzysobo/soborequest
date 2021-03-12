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
#include "soborequest/request.h"
#include "soborequest/request_methods.h"
#include "soborequest/request_cb_functions.h"


/**
 * @brief
 * @note
 * @param  *curl:
 * @param  **headers:
 * @param  num_headers:
 * @param  *headers_list:
 * @param  *headers_temp:
 * @retval
 */


const char *methodName(enum METHOD m)
{
    static const char *names[] = {
        "GET", "POST", "PUT", "PATCH", "DELETE", "HEAD",
        "CONNECT", "OPTIONS", "TRACE" };
    static int size_names = 9;
    int m_ob0 = m - 1;
    if (m_ob0 < size_names) {
        return names[m_ob0];
    }

    return "-n/a-";
}


int set_headers(CURL *curl, char **headers,
    int num_headers,
    struct curl_slist *headers_list,
    struct curl_slist *headers_temp)
{
    int res = CURLE_OK;
    int i;

    if ((headers == NULL) || (num_headers < 1)) {
        return res;
    }

    for (i = 0; i < num_headers; i++) {
        headers_temp = curl_slist_append(headers_list, headers[i]);
        if (headers_temp == NULL) {
            return RES_SLIST_ERROR;
        }
        headers_list = headers_temp;
    }
    res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers_list);

    return res;
}


/**
 * @brief  makes a request
 * @note
 * @param  *cd:
 * @param  *data:
 * @param  *header_data:
 * @param  *auth_data:
 * @param  *cb_data:
 * @param  **resp: ResponseData if retval==CURLE_OK, NULL otherwise
 *         (do not try to display the  members of NULL, because it will cause segmentation fault!)
 * @retval
 */
int make_request(struct ConnData *cd, char *data,
    struct HeaderData *header_data, struct AuthData *auth_data,
    struct CallbackData *cb_data, struct ResponseData **resp)
{
    CURL *curl = curl_easy_init();
    CURLcode res;
    char *resp_cont_type;
    long resp_code;

    struct curl_slist *headers_list = NULL;
    struct curl_slist *headers_temp = NULL;
    /* char *resp_method = NULL; */
    /* char *resp_url = NULL; */
    struct Memory chunk_write;
    struct Memory chunk_read;

    init_memory_chunk(&chunk_write);
    init_memory_chunk(&chunk_read);


    if (curl == NULL) {  /* something went wrong, don't go further */
        return CURLE_FAILED_INIT;  /* re-using the codes from CURL */
    }
    if (!cd->method) {
        return CURLE_BAD_FUNCTION_ARGUMENT;
    }

    res = curl_easy_setopt(curl, CURLOPT_URL, cd->url);
    if (!check_res_ok(res))
        return res;

    if (cd->port) {
        res = curl_easy_setopt(curl, CURLOPT_PORT, cd->port);
        if (!check_res_ok(res))
            return res;
    }

    if (header_data != NULL) {
        if (header_data->follow_redirects) {
            res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION,
                header_data->follow_redirects);
            if (!check_res_ok(res))
                return res;
        }
        if (header_data->use_autoreferer) {
            res = curl_easy_setopt(curl, CURLOPT_AUTOREFERER,
                header_data->use_autoreferer);
            if (!check_res_ok(res))
                return res;
        }
        else if ((header_data->referer != NULL) &&
            (strlen(header_data->referer))) {
            res = curl_easy_setopt(curl, CURLOPT_REFERER,
                header_data->referer);
            if (!check_res_ok(res))
                return res;
        }

        if (header_data->headers != NULL) {
            res = set_headers(curl, header_data->headers,
                header_data->num_headers, headers_list, headers_temp);
            if (!check_res_ok(res))
                return res;
        }
    }

    if (auth_data != NULL) {
        if ((auth_data->user != NULL) && (auth_data->password != NULL)) {
            res = curl_easy_setopt(curl, CURLOPT_USERNAME,
                auth_data->user);
            if (!check_res_ok(res))
                return res;

            res = curl_easy_setopt(curl, CURLOPT_PASSWORD,
                auth_data->password);
            if (!check_res_ok(res))
                return res;
        }
    }

    if ((cb_data != NULL) && (cb_data->write_function != NULL)) {
        res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
            cb_data->write_function);
        if (!check_res_ok(res))
            return res;
        res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk_write);
        if (!check_res_ok(res))
            return res;
    }
    else {
        res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function_common);
        if (!check_res_ok(res))
            return res;
        res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk_write);
        if (!check_res_ok(res))
            return res;
    }

    if ((cb_data != NULL) && (cb_data->read_function != NULL)) {
        res = curl_easy_setopt(curl, CURLOPT_READFUNCTION, cb_data->read_function);
        if (!check_res_ok(res))
            return res;

        res = curl_easy_setopt(curl, CURLOPT_READDATA, (void *)&chunk_read);
        if (!check_res_ok(res))
            return res;
    }
    if (!check_res_ok(res))
        return res;

    switch (cd->method) {
    case METHOD_GET:
        res = set_method_get(curl);
        break;
    case METHOD_POST:
        res = set_method_post(curl, data);
        break;
    case METHOD_PUT:
        res = set_method_put(curl, data);
        break;
    case METHOD_PATCH:
        res = set_method_patch(curl, data);
        break;
    case METHOD_DELETE:
        res = set_method_delete(curl);
        break;
    case METHOD_HEAD:
        res = RES_NOT_IMPLEMENTED_YET;
        break;
    case METHOD_CONNECT:
        res = RES_NOT_IMPLEMENTED_YET;
        break;
    case METHOD_OPTIONS:
        res = RES_NOT_IMPLEMENTED_YET;
        break;
    case METHOD_TRACE:
        res = RES_NOT_IMPLEMENTED_YET;
        break;
    default:
        res = CURLE_BAD_FUNCTION_ARGUMENT;
    }
    if (!check_res_ok(res))
        return res;

    res = curl_easy_perform(curl);
    /*
     * TODO: implemented only in 7.72.0 and later
     * https://curl.se/libcurl/c/CURLINFO_EFFECTIVE_METHOD.html
     * curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_METHOD, &resp_method);
     * curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &resp_url);
    */
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp_code);
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &resp_cont_type);

    *resp = make_response_data(resp_code, chunk_write.size,
        chunk_write.response, resp_cont_type);

    if (headers_temp != NULL)
        curl_slist_free_all(headers_temp);

    if (headers_list != NULL)
        curl_slist_free_all(headers_list);

    if (!check_res_ok(res))
        return res;

    return CURLE_OK;
}
