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
#include <fcntl.h>
#include <assert.h>
#include <limits.h>
#include <sys/socket.h>

#include "sobologger/logger.h"
#include "soborequest/socket_functions.h"


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
void test_wrong_addr_port(char *url, long port, int num,
    char *test_contents)
{
    int i = 0;
    int num_errors = 0;
    int sockfd;

    for (i = 0; i < num; i++) {
        sockfd = connect_to_server(url, port);
        printf("\nChecking whether connect_to_server returns -1 "
            "for the wrong address or port...");
        if (sockfd == -1) {
            printf(" - OK!\n");
        }
        else {
            printf(" - FAILED!\n");
            num_errors++;
            continue;
        }
    }
}


void test_correct_addr_port(char *url, long port, int num,
    char *test_contents)
{
    int i = 0;
    int num_errors = 0;
    int sockfd;


    for (i = 0; i < num; i++) {
        sockfd = connect_to_server(url, port);
        printf("\nChecking whether connection to the correct address"
            " (%s:%lu) succeeds...", url, port);
        if (sockfd >= 0) {
            printf(" - OK!\n");
        }
        else {
            printf(" - FAILED! \n");
            num_errors++;
            continue;
        }
        printf("\nSOCKET FD: %d \n", sockfd);

        close(sockfd);
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
    char *url = (argc > 0 && argv[1]) ? argv[1] : "127.0.0.1";
    long port = 50001;
    int num = 10;
    setup_logger("TESTSOCK", "./logs-socket.log");
    printf("\ntest_request testing has been started... \n\n");

    test_wrong_addr_port("non-existent?!!!URL?", 0, num, "AAA");
    test_correct_addr_port(url, port, num, "AAA");

    printf("\ntest_request testing has been finished... \n\n");

    return status;
}

