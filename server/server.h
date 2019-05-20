#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <stdlib.h>
#include<string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>


#define RRQ   1
#define WRQ   2
#define DATA  3
#define ACK   4
#define ERR   5

typedef struct request
{
      char filename[50];
      char opcode;
}Req_f;

typedef struct err_t
{
    char opcode;
    char err_code;
    char err_msg[30];
    unsigned  int end : 1; 
}err;

typedef struct data
{
    int block_no;
    char opcode;
    char data[513];
}Data_f;

typedef struct ack
{
    char opcode;
    int block_no;
}Ack_f;








#endif

