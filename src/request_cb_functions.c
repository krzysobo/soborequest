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
#include <stdlib.h>
#include <string.h>
#include <soborequest/request_cb_functions.h>


/**
 * @brief  
 * @note   
 * @param  *mem: 
 * @retval None
 */
void init_memory_chunk(struct Memory *mem)
{
    mem->size = 0;
    mem->response = malloc(1);
}


struct ResponseData *make_response_data(
    long status_code, size_t size, char *contents, char *content_type)
{
    struct ResponseData *resp;
    resp = calloc(1, sizeof(struct ResponseData));
    resp->status_code = status_code;
    resp->size = size;
    if((contents != NULL) && (strlen(contents) > 0)) {
        resp->contents = calloc(strlen(contents) + 1, 1);
        memcpy(resp->contents, contents, strlen(contents)); 
    } else {
        resp->contents = calloc(2, 1);
        resp->contents[1] = 0;        
    }

    if((content_type != NULL) && (strlen(content_type) > 0)) {
        resp->content_type = calloc(strlen(content_type) + 1, 1);
        memcpy(resp->content_type, content_type, strlen(content_type)); 
    } else {
        resp->content_type = calloc(2, 1);
        resp->content_type[1] = 0;        
    }
  
    return resp;
}



/**
 * @brief  
 * @note   
 * @param  *data: - data fetched from server 
 * @param  size: size of a single item (eg. char)
 * @param  nitems: number of items (eg. 10 chars in a ten-char string)
 * @param  *userdata: 
 * @retval 
 */
size_t write_function_common(void *data, size_t size, 
    size_t nitems, void *userdata)
{   
    size_t realsize = size * nitems;  /* size of all items together */

    /* casting back to struct Memory (initial casting was at setting 
     * the variable write_chunk)
     */
    struct Memory *mem_userdata = (struct Memory *)userdata;

    char *ptr = realloc(mem_userdata->response, 
        mem_userdata->size + realsize + 1);
    
    if (ptr == NULL) 
        return 0;  /* not enough memory */

    mem_userdata->response = ptr;
    memcpy(&(mem_userdata->response[mem_userdata->size]), data, realsize);
    mem_userdata->size += realsize;
    mem_userdata->response[mem_userdata->size] = 0;

    return realsize;
}
