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


#include <unistd.h>  /* write, sleep itd */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "sobologger/logger.h"
#include "soborequest/socket_functions.h"


struct timeval timeout_read = {5, 0};
struct timeval timeout_write = {10, 0};


/**
 * @brief  
 * @note   
 * @param  *srv_addr: 
 * @param  srv_port: 
 * @retval 
 */
int connect_to_server(const char *srv_addr, const uint16_t srv_port)
{
    int sockfd; 
    struct sockaddr_in servaddr; 

    /* socket create and varification */
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        logger("socket creation failed.", LOG_ERROR); 
        return -1;
    } 
    else
        logger("Socket successfully created.", LOG_DEBUG);
    
    memset(&servaddr, 0, sizeof(servaddr));
    
    /* assign IP, PORT */
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(srv_addr); 
    servaddr.sin_port = htons(srv_port); 
  

    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout_read, sizeof(timeout_read)) < 0) {
        logger("Can't set socket option for recv.", LOG_ERROR); 
        return -1;
    }
    if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout_write, sizeof(timeout_write)) < 0) {
        logger("Can't set socket option for send.", LOG_ERROR); 
        return -1;
    }

    /* connect the client socket to server socket */
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) { 
        logger("connection with the server failed.", LOG_ERROR); 
        return -1;
    } 
    else
        logger("connected to the server.", LOG_INFO); 

    return sockfd;
}


/**
 * @brief  
 * @note   
 * @param  sockfd: 
 * @retval 
 */
int close_connection(int sockfd)
{
    int res;
    res = close(sockfd);
    return res;
}


/**
 * @brief  
 * @note   
 * @param  sockfd: 
 * @param  *buf: 
 * @param  *len: 
 * @retval 
 */
int send_all_data(int sockfd, char *buf, int *len)
{
    int total = 0;        /* how many bytes we've sent */
    int bytesleft = *len; /* how many we have left to send */
    int n;

    while(total < *len) {
        n = send(sockfd, buf + total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }
    *len = total; /* return number actually sent here */

    return (n == -1)? -1: 0; /* return -1 on failure, 0 on success */
}


/**
 * @brief  
 * @note   
 * @param  *msg_ret: 
 * @retval 
 */
int read_all_data(int sockfd, void *msg_ret, int len_msg_ret)
{
    int res;
    res = read(sockfd, (void *)msg_ret, len_msg_ret);

    return res;
}
