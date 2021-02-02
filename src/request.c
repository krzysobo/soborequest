// Copyright (c) 2020-2021 Krzysztof Sobolewski <krzysztof.sobolewski@gmail.com>

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "soborequest/request.h"
#include "soborequest/request_methods.h"
#include "soborequest/request_cb_functions.h"


int set_headers(CURL *curl, char **headers, 
    int num_headers,
    struct curl_slist *headers_list, 
    struct curl_slist *headers_temp)
{
    int res = CURLE_OK;
    int i;

    if((headers == NULL) || (num_headers < 1)) {
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


int make_request(char *url, enum METHOD method, char *data, long port,
    struct HeaderData *header_data, struct AuthData *auth_data,
    struct CallbackData *cb_data, struct ResponseData **resp)
{
    CURL *curl = curl_easy_init();
    CURLcode res;
    struct curl_slist *headers_list = NULL;
    struct curl_slist *headers_temp = NULL;
    // char *resp_method = NULL;
    // char *resp_url = NULL;
    struct Memory chunk_write;
    struct Memory chunk_read;

    init_memory_chunk(&chunk_write);
    init_memory_chunk(&chunk_read);


    if (curl == NULL) {  // something went wrong, don't go further
        return CURLE_FAILED_INIT;  // I am re-using the codes from CURL
    }
    if (!method) {
        return CURLE_BAD_FUNCTION_ARGUMENT;
    }

    res = curl_easy_setopt(curl, CURLOPT_URL, url);
    if (!check_res_ok(res))
        return res;

    if (port) {
        res = curl_easy_setopt(curl, CURLOPT_PORT, port);
        if (!check_res_ok(res))
            return res;
    }

    if (header_data != NULL ) {
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
        } else if ((header_data->referer != NULL) && 
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
        if (auth_data->auth_type) {
            res = curl_easy_setopt(curl, CURLOPT_HTTPAUTH, 
                (long)auth_data->auth_type);
            if (!check_res_ok(res))
                return res;
        }

        if (auth_data->user_pwd != NULL) {
            res = curl_easy_setopt(curl, CURLOPT_USERPWD, 
                auth_data->user_pwd);
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
    } else {
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

    switch (method) {
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
    curl_slist_free_all(headers_temp);
    curl_slist_free_all(headers_list);
    if (!check_res_ok(res))
        return res;


    // TODO: implemented only in 7.72.0 and later https://curl.se/libcurl/c/CURLINFO_EFFECTIVE_METHOD.html
    // curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_METHOD, &resp_method); 
    // curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &resp_url);

    char *resp_cont_type;
    long resp_code;


    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp_code);
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &resp_cont_type);
  
    // struct ResponseData *resp_in;
    *resp = make_response_data(resp_code, chunk_write.size, chunk_write.response, resp_cont_type);
    // *resp = resp_in;
    // printf("\nRESPC CODE: %lu SIZE: %lu \n", resp_code, chunk_write.size);
    // printf("\nRESP_IN CODE: %lu SIZE: %lu CONT: %s TYPE: %s \n", 
    //     resp_in->status_code, resp_in->size, resp_in->contents, 
    //     resp_in->content_type);

    // // resp_cont_type is accidentally cleared after cleanup. Don't use it after curl_easy_cleanup,
    // // and if anything, go for memcpy
    // resp = resp_in;
    // curl_easy_cleanup(curl);
    // printf("\nAFTER CLEANUP RESP_IN CODE: %lu SIZE: %lu CONT: %s TYPE: %s \n",
    //     resp->status_code, resp->size, resp->contents, 
    //     resp->content_type);

    return CURLE_OK;
}
