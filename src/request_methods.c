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
#include "soborequest/request_methods.h"
#include "soborequest/request_cb_functions.h"


/* TODO - the method under construction */
size_t read_function_put_method(char *ptr, size_t size, 
    size_t nmemb, void *user_data)
{  
    struct PutMethodData *userdata;
    
    userdata = calloc(1, sizeof userdata);
    
    userdata = (struct PutMethodData*)user_data;
    
    printf("\nuserdata->len, %lu", userdata->len);

    /*
     size_t to_copy = (userdata->len < curl_size)? userdata->len: curl_size;
     memcpy(ptr, userdata->data, to_copy);
     userdata->len -= to_copy;
     userdata->data += to_copy;
     return to_copy;
    */
    return 0;
}


/**
 * @brief checks if a result code is OK (only CURLE_OK / 0 is OK)
 * @note   
 * @param  res: 
 * @retval 1 (true) or 0 (false)
 */
int check_res_ok(CURLcode res)
{
    /* CURLE_OK = 0 IS OK, anything else is an error. We're re-using 
     * the CURL codes for error tracing/logging 
     */
    return (res == CURLE_OK);
}


/**
 * @brief  sets the POST method
 * @note   
 * @param  *curl: 
 * @param  *data: 
 * @retval 
 */
CURLcode set_method_post(CURL *curl, char *data) 
{
    CURLcode res;
    long len_data;

    if (data != NULL) {
        len_data = strlen(data);
        if (len_data > 2000000000) {
            res = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, 
                (curl_off_t)len_data);
        } else {
            res = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, len_data);
        }
    } else {
        res = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0);
    }
    if (!check_res_ok(res))
        return res;

    res = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    return res;
}


/**
 * @brief  sets the POST method
 * @note   
 * @param  *curl: 
 * @param  *data: 
 * @retval 
 */
CURLcode set_method_patch(CURL *curl, char *data) 
{
    CURLcode res;
    long len_data;

    if (data != NULL) {
        len_data = strlen(data);
        if (len_data > 2000000000) {
            res = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, 
                (curl_off_t)len_data);
        } else {
            res = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, len_data);
        }
    } else {
        res = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0);
    }
    if (!check_res_ok(res))
        return res;

    return curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    if (!check_res_ok(res))
        return res;

    res = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    return res;
}


/**
 * @brief  sets the GET method
 * @note   
 * @param  *curl: 
 * @retval 
 */
CURLcode set_method_get(CURL *curl)
{
    return curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
}


/**
 * @brief  sets the DELETE method
 * @note   
 * @param  *curl: 
 * @retval 
 */
CURLcode set_method_delete(CURL *curl)
{
    return curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
}


/**
 * @brief  sets the PUT method (warning; PUT = CURLOPT_UPLOAD)
 * @note   
 * @param  *curl: 
 * @param  *data: 
 * @retval 
 */
CURLcode set_method_put(CURL *curl, char *data)
{
    CURLcode res;
    long len_data;

    struct PutMethodData put_method_data;
    put_method_data.data = data;



    res = curl_easy_setopt(curl, CURLOPT_READFUNCTION, 
        read_function_put_method);
    if (!check_res_ok(res))
        return res;

    res = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    if (!check_res_ok(res))
        return res;

    res = curl_easy_setopt(curl, CURLOPT_READDATA, &put_method_data);
    if (!check_res_ok(res))
        return res;

    len_data = strlen(data);
    if (len_data > 2000000000) {
        res = curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, len_data);
    } else {
        res = curl_easy_setopt(curl, CURLOPT_INFILESIZE, len_data);
    }
    return res;
}
