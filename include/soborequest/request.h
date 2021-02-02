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


#include <curl/curl.h>
#include "soborequest/request_cb_functions.h"


#ifndef REQ_H
#define REQ_H


/**
 * @brief  METHODS handled by make_request (not all of them implemented yet! 
 *         See request.c:make_request for details)
 * @note   
 * @retval None
 */
enum METHOD {
    METHOD_GET = 1,
    METHOD_POST,
    METHOD_PUT,
    METHOD_PATCH, 
    METHOD_DELETE,

    METHOD_HEAD,
    METHOD_CONNECT,
    METHOD_OPTIONS,
    METHOD_TRACE
};


/**
 * @brief  returns the name of a HTTP method
 * @note   
 * @param  m: 
 * @retval 
 */
static inline const char * methodName(enum METHOD m)
{
    static const char *names[] = {
        "GET", "POST", "PUT", "PATCH", "DELETE", "HEAD", 
        "CONNECT", "OPTIONS", "TRACE"};
    static int size_names = 9;
    int m_ob0 = m - 1;
    if(m_ob0 < size_names) {
        return names[m_ob0];
    }

    return "-n/a-";
}


/**
 * @brief  the last METHOD handled, used for checking/validation
 * @note   
 * @retval None
 */


struct ConnData {
    char *url;
    long port;
    enum METHOD method;
};


struct HeaderData {
    /**
     * @brief  optional headers - array of strings
     * @note   
     * @retval None
     */
    char **headers;

    /**
     * @brief  number of headers - obligatory if **headers are set
     * @note   
     * @retval None
     */
    int num_headers; 

    /**
     * @brief  true (1L) or false (def. 0)
     * @note   
     * @retval None
     */
    long follow_redirects;

    /**
     * @brief  true (1L) or false (def. 0)
     * @note   
     * @retval None
     */
    long use_autoreferer;

    /**
     * @brief  referer URL
     * @note   
     * @retval None
     */
    char *referer;
};


struct AuthData {
    /**
     * @brief  CURLAUTH_* from curl/curl.h
     * @note   
     * @retval None
     */
    unsigned long auth_type;   // default - CURLAUTH_NONE
    
    /**
     * @brief  user:pwd, eg. "alice:blah!blah666$$#""
     * @note   
     * @retval None
     */
    char *user_pwd; 
};

struct CallbackData {
    void *write_function;
    void *read_function;
};

enum RES {
    RES_OK = CURLE_OK,
    RES_SLIST_ERROR = -99,
    RES_NOT_IMPLEMENTED_YET = -100,
};


int make_request(struct ConnData *cd, char *data,
    struct HeaderData *header_data, struct AuthData *auth_data,
    struct CallbackData *cb_data, struct ResponseData **resp);

#endif
