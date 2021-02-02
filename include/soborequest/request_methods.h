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


#ifndef REQ_MTH_H
#define REQ_MTH_H
/**
 * @brief  
 * @note   
 * @retval None
 */
struct PutMethodData
{
    char *data;
    size_t len;
};


int check_res_ok(CURLcode res);
CURLcode set_method_post(CURL *curl, char *data);
CURLcode set_method_get(CURL *curl);
CURLcode set_method_delete(CURL *curl);
CURLcode set_method_put(CURL *curl, char *data);
CURLcode set_method_patch(CURL *curl, char *data);

size_t read_function_put_method(char *ptr, size_t size, 
    size_t nmemb, void *user_data);

#endif