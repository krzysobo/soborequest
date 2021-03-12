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
    int num_errors = 0;

    /*
    * C89 compatibility - looks weird (can't use function
    * vars at struct initialization)
    */
    struct ConnData cd;
    cd.url = url;
    cd.port = port;
    cd.method = m;

    for (i = 0; i < num; i++) {
        printf("\nMaking request %d/%d to: %s port: %lu method: %d (%s)...",
            i + 1, num, cd.url, cd.port, cd.method, methodName(cd.method));
        res = make_request(&cd, data, NULL, NULL, NULL, &resp);

        /*
        * I don't want to use assert here, because I don't want the program to quit on the
        * first failed conneciton. I'd rather like to gather the info about how often it happens.
        */
        if (res == CURLE_OK) {
            printf(" - OK!\n");
        }
        else {
            printf(" - FAILED! Error Code: %d. \n", res);
            num_errors++;
            continue;
        }

        printf("\n\n TEST=> RESP CODE %lu size: %lu CONTENTS: %s "
            "CONTENT-TYPE: %s\n",
            (unsigned long)resp->status_code,
            (unsigned long)resp->size,
            resp->contents,
            resp->content_type
        );

        if (test_contents != NULL) {
            printf("\nTESTING: RESP cnt: '%s'\nTEST cnt: '%s'. Is equal?",
                resp->contents, test_contents);

            if (strcmp(resp->contents, test_contents) == 0) {
                printf(" -- YES, OK! \n");
            }
            else {
                printf(" -- FAILED! \n");
                num_errors++;
                continue;
            }
        }
        if (test_ct != NULL) {
            printf("\nTESTING: RESP ct: '%s'\nTEST ct: '%s'. Is equal?",
                resp->content_type, test_ct);
            if (strcmp(resp->content_type, test_ct) == 0) {
                printf(" -- YES, OK! \n");
            }
            else {
                printf(" -- FAILED! \n");
                num_errors++;
                continue;
            }
        }
    }

    if (num_errors > 0) {
        printf("\nThere were %d error(s) out of %d tests.\n", num_errors, num);
    }
    else {
        printf("\nAll %d tests have been performed successfully.\n", num);
    }
    printf("================================================\n\n");
}


int main(int argc, char **argv)
{
    int status = 0;
    char *url = (argc > 0 && argv[1]) ? argv[1] : "http://127.0.0.1";
    long port = 5000;
    int num = 10;
    printf("\ntest_request testing has been started... \n\n");

    test_mass(url, port, METHOD_GET, num, "\"GET IS OK\"\n", NULL);
    test_mass(url, port, METHOD_POST, num, "\"POST IS OK\"\n", NULL);
    test_mass(url, port, METHOD_DELETE, num, "\"DELETE IS OK\"\n", NULL);
    test_mass(url, port, METHOD_PATCH, num, "\"PATCH IS OK\"\n", NULL);

    printf("\ntest_request testing has been finished... \n\n");

    return status;
}

