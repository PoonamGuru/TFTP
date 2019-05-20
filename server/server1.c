#include"server.h"



int main()
{

    int socket_fd, client_len, fd, ret_val;
    char ser_buf[512];
    char * temp;
    err err_packet;                            //---------error packet
    Data_f data_packet;                        //---------data packet
    Req_f req_packet;
    Ack_f ack_packet;
    int block_no = 0,rd_rval;                          //---------count the no of packet sent
    struct sockaddr_in server_addr , client_addr;
    server_addr.sin_family       = AF_INET;			//---------internet protocol
    server_addr.sin_port         = 69;				//---------tftp
    server_addr.sin_addr.s_addr  = inet_addr("127.0.01");	//---------loop back ip address




    /*create a socket at the server end */
    socket_fd = socket(AF_INET , SOCK_DGRAM , 0);		//---------using UDP socket

    /*error checking*/
    if(socket_fd == -1)
    {
	perror("socket creation:");
	exit(0);
    }


    /*bind the ip address and port no*/
    if(bind(socket_fd , (struct sockaddr *) &server_addr , sizeof(server_addr))== -1)
    {
	perror("bind");
	exit(0);
    }


    socklen_t client_size = sizeof(client_addr);

    while(1)
    {
	/*Recieve the packet from the client*/
	if(recvfrom(socket_fd ,&req_packet ,sizeof(req_packet)  , 0 , (struct sockaddr *) &client_addr , &client_size) == -1)
	{
	    perror("recvfrom:");
	    exit(0);
	}


	printf("%d %s\n", req_packet.opcode , req_packet.filename);
	switch(req_packet.opcode)
	{

	    //get command
	    case RRQ:
		block_no = 0;
		puts(req_packet.filename);
		/*check whether the file is present or not*/
		fd = open(req_packet.filename , O_RDONLY);
		if(fd == -1)
		{

		    /*error packet*/
		    data_packet.opcode = ERR;
		    strcpy(data_packet.data,"file not found");
		    /*send the error packet to the client*/
		    sendto(socket_fd , &data_packet , sizeof(data_packet) , 0 , (struct sockaddr *) &client_addr , sizeof(client_addr));
		}
		else if(fd)
		{
		    data_packet.opcode = DATA; //..................DATA = 3
		    /*read the 512 byte*/
		    do
		    {
			block_no++;
			data_packet.block_no = block_no; //.................send first packet as the ack
			rd_rval = read(fd , data_packet.data , 512);
			if(rd_rval == -1)
			{
			    perror("read");
			    break;
			}


			data_packet.data[rd_rval] = '\0';
			printf("sd : %ld\n",strlen(data_packet.data));



			/*send the data to the client*/
			sendto(socket_fd , &data_packet , sizeof(data_packet) , 0 , (struct sockaddr *) &client_addr , sizeof(client_addr));
			printf("bn : %d  ret : %d\n",data_packet.block_no, rd_rval);	

			printf("Sednding the packet block_no %d",block_no);	    

			/*recieve the acknowlege from receiver */
			recvfrom(socket_fd , &ack_packet , sizeof(ack_packet) ,0, (struct sockaddr *) &client_addr , &client_size);
			if(ack_packet.block_no == block_no)
			{
			    printf("		received the ack for block_no %d\n", ack_packet.block_no);
			    continue;
			}


		    }while(rd_rval == 512);
		    puts("finished transver");



		}

		break;



	    case WRQ:
		block_no = 0;

		fd = open(req_packet.filename,O_WRONLY);
		printf("fd = %d \n",fd);
		if(fd > 1)
		{
		    data_packet.opcode = ERR;
		    strcpy(data_packet.data , "File already present");
		    /*send error packet to client*/
		    sendto(socket_fd , &data_packet , sizeof(data_packet) , 0 , (struct sockaddr *) &client_addr , sizeof(client_addr));
		    break;
		}

		else if(fd == -1)
		{	/*send ack packet, block no as 0*/
		    char buff[30];
		    data_packet.opcode = ACK;
		    data_packet.block_no = 0;
		    /*send ack packet to client(0)*/
		    sendto(socket_fd , &data_packet , sizeof(data_packet) , 0 , (struct sockaddr *) &client_addr , sizeof(client_addr));
		}
		/*create a file to store the received data from the client*/
		if((fd = open(req_packet.filename, O_WRONLY | O_CREAT , 0777)) != -1)
		{ 
		    do
		    {
			/*recieve the data from the client*/
			ret_val  = recvfrom(socket_fd , &data_packet , sizeof(data_packet) , 0 , (struct sockaddr *) &client_addr , &client_size);
			if(ret_val != -1)
			{
			    /*if data packet is recieved*/
			    if(data_packet.opcode == DATA)
			    {
				printf("bn :%d  rv : %d\n" , data_packet.block_no, strlen(data_packet.data));
				//				printf("Recieved data packet block %d\n",data_packet.block_no);
				/*write it into the file*/
				if(write(fd , data_packet.data , strlen(data_packet.data)) == -1)
				{
				    perror("write");
				    exit(0);
				}



				/*create ack frame*/
				ack_packet.opcode = ACK;
				ack_packet.block_no = data_packet.block_no; //..................put the received packet block no
				sendto(socket_fd , &ack_packet , sizeof(ack_packet) , 0 , (struct sockaddr *) &client_addr , sizeof(client_addr));
				printf("        Sending Acknowledge packet\n");

			    }

			}	    
		    }while(strlen(data_packet.data) == 512);


		    puts("received completely");
		    close(fd);
		}
		break;

	}
    }

    return 0;
}
