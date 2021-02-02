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
#include <unistd.h>
#include <assert.h>
#include <limits.h>
#include "curl/curl.h"
#include "soborequest/request.h"
#include "soborequest/request_methods.h"
#include "soborequest/request_cb_functions.h"


#ifndef PATH_MAX
#define PATH_MAX 1024
#endif


/**
 * @brief  mass-testing
 * @note   
 * @param  *url: 
 * @param  port: 
 * @param  m: 
 * @param  num: 
 * @param  *test_contents: 
 * @param  *test_ct: 
 * @retval None
 */
void test_mass(char *url, long port, enum METHOD m, int num, 
    char *test_contents, char *test_ct)
{
    CURLcode res;
    char *data = "abcd";
    int i = 0;

    struct ResponseData *resp;
    // resp = calloc(1, sizeof (struct ResponseData *));

    for(i=0; i < num; i++) {
        res = make_request(url, m, data, port, NULL, NULL, NULL, &resp);
        printf("\n\nMAKE_REQUEST RES: %d", res);
        printf("\n\n TEST=> RESP CODE %lu size: %lu CONTENTS: %s "
               "CONTENT-TYPE: %s\n",
            resp->status_code, resp->size, resp->contents, resp->content_type
        );
        continue;
        if(test_contents != NULL) {
            printf("\nTESTING: RESP cnt: '%s'\nTEST cnt: '%s'. Is equal?", 
                resp->contents, test_contents);
            assert(strcmp(resp->contents, test_contents) == 0);
            printf(" -- YES, OK! \n");
        }
        if(test_ct != NULL) {
            printf("\nTESTING: RESP ct: '%s'\nTEST ct: '%s'. Is equal?", 
                resp->content_type, test_ct);
            assert(strcmp(resp->content_type, test_ct) == 0);
            printf(" -- YES, OK! \n");
        }
    }
}



int main(int argc, char **argv)
{
    int status;
    // CURLcode res;
    char *url = (argc > 0 && argv[1])?argv[1]: "http://127.0.0.1";
    status = 0;
    long port = 5000;
    int num = 1000;    
    printf("\ntest_request testing has been started... \n\n");
    
    test_mass(url, port, METHOD_GET, num, "\"GET IS OK\"\n", NULL);
    test_mass(url, port, METHOD_POST, num, "\"POST IS OK\"\n", NULL);
    test_mass(url, port, METHOD_DELETE, num, "\"DELETE IS OK\"\n", NULL);
    test_mass(url, port, METHOD_PATCH, num, "\"PATCH IS OK\"\n", NULL);
    
    printf("\ntest_request testing has been finished... \n\n");

    return status;
}

