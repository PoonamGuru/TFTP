#include"client.h"
int main()
{
    int socket_fd, i ,ret_val,w_ret_val,fd, block_no;
    char command[100];

    //err err_packet;
    Data_f data_packet;
    Ack_f  ack_packet;
    Req_f  req_packet;


    struct sockaddr_in server_addr , client_addr;
    server_addr.sin_family      = AF_INET;          //------------------IPv4
    server_addr.sin_port        = 69;               //------------------TFTP
    socklen_t server_size = sizeof(server_addr);
    data_packet.data[512] = '\0';


    /*cerate a socket for udp*/
    socket_fd  = socket(AF_INET , SOCK_DGRAM, 0);

    /*error checking */
    if(socket_fd == -1)
    {
	perror("socket creation");
	exit (0);
    }

    while(1)
    {

	printf("\nTFTP :> ");
	scanf("\n%[^\n]s", command);
	
	if(strncmp(command , "connect" , 7) == 0)
	{
            server_addr.sin_addr.s_addr = inet_addr(command+8);//-------------Loop back
            break;
	}

	else
	{
	    puts("doesn't make any coonection please connect to network");
	}
    }

    while(1)
    {

	printf("\nTFTP :> ");
	scanf("\n%[^\n]s", command);
	puts(command);

	char buffer[20];


	if(strncmp(command , "get" , 3) == 0)
	{
	    /*form the frame (rrq) */
	    req_packet.opcode  = RRQ;
	    strcpy(req_packet.filename , command+4);

	    /*send to the server*/
	    sendto(socket_fd , &req_packet , sizeof(req_packet) , 0 , (struct sockaddr *) &server_addr , sizeof(server_addr));
	    printf("Message sent to server successfully\n");

	    //printf("%d %s\n", req_packet.opcode , req_packet.filename);

	    fd = open(req_packet.filename , O_WRONLY | O_CREAT , 0777);
	    if(fd == -1)
	    {
		perror("open");
		exit(0);
	    }
	    /*recieve from */
	    do
	    {
		/*recieve the data from the server*/
		ret_val  = recvfrom(socket_fd , &data_packet , sizeof(data_packet) , 0 , (struct sockaddr *) &server_addr , &server_size);
		if(ret_val != -1) 
		{
		    /*if data packet is recieved*/
		    if(data_packet.opcode == DATA)
		    {
			printf("Recieved data packet block %d",data_packet.block_no);




			/*create ack frame*/
			ack_packet.opcode = ACK;
			ack_packet.block_no = data_packet.block_no; //..................put the received packet block no
			sendto(socket_fd , &ack_packet , sizeof(ack_packet) , 0 , (struct sockaddr *) &server_addr , sizeof(server_addr));
			printf("	Sending Acknowledge packet\n");




			/*write it into the file*/
			w_ret_val = write(fd , data_packet.data , strlen(data_packet.data));
			if(w_ret_val == -1)
			{
			    perror("write");
			    exit(0);
			}
			//		printf("bn :%d	rv : %d\n" , data_packet.block_no, strlen(data_packet.data));
		    }


		    /*ERROR packet*/
		    if(data_packet.opcode == ERR)
		    {
			printf("ERROR : %s\n",data_packet.data);
		    }


		}



	    }while(strlen(data_packet.data) == 512);

	    printf("received completly");
	    close(fd); //close the fd


	}

	else if(strncmp(command , "put" , 3) == 0)
	{


	    req_packet.opcode  = WRQ;
	    strcpy(req_packet.filename , command+4);

	    /*send to the server*/
	    if(sendto(socket_fd , &req_packet , sizeof(req_packet) , 0 , (struct sockaddr *) &server_addr , sizeof(server_addr))!= -1)
		printf("Message sent to server successfully\n");
	    /*recieve the data from the server*/
	    ret_val  = recvfrom(socket_fd , &data_packet , sizeof(data_packet) , 0 , (struct sockaddr *) &server_addr , &server_size);
	    if(ret_val != -1) 
	    {
		printf("opcode : %d\n",data_packet.opcode);

		/*ERROR packet*/
		if(data_packet.opcode == ERR)
		{

		    printf("ERROR : %s\n",data_packet.data);
		}


		else if(data_packet.opcode == ACK)
		{
		    block_no = 0;
		    int r_val = 0, send_ret;
		    printf("ack received from server %d\n" , data_packet.block_no);
		    /*open the file in read mode*/
		    fd = open(req_packet.filename, O_RDONLY);
		    if(fd > 1)
		    {
			data_packet.opcode = DATA;
			/*read the 512 bytes of data and transfer it to server*/
			do
			{
			    block_no++;
			    r_val = read(fd , data_packet.data ,  512);
			    if(r_val == -1)
			    {
				perror("read");
				exit(0);
			    }

			    data_packet.data[r_val] = '\0';
//			    printf("sd : %ld\n",strlen(data_packet.data));
			    data_packet.block_no = block_no;

			    /*send the data to the server*/
			    send_ret = sendto(socket_fd , &data_packet , sizeof(data_packet) , 0 , (struct sockaddr *) &server_addr , sizeof(server_addr));
//			    printf("bn : %d  ret : %d sendto : %d\n",data_packet.block_no, r_val,send_ret); 
			    printf("Sending the data packet with block_no %d",data_packet.block_no);


			    /*recieve the acknowlege from receiver */
			    recvfrom(socket_fd , &ack_packet , sizeof(ack_packet) ,0, (struct sockaddr *) &server_addr , &server_size);
			    if(ack_packet.opcode == ACK && ack_packet.block_no == block_no)
			    {
				printf("            Received the ack for block_no %d\n", ack_packet.block_no);
				
			    }


			}while(r_val == 512);


		    }
		}

	    }





	    puts(command);
	}

	else if(strncmp(command , "quit" , 3) == 0)
	{
	    puts(command);
	    puts("application closed");
	    break;
	}

	else if(strncmp(command , "help" , 4) == 0)
	{
	    puts("----------------------------------------help menu------------------------------");
	    puts("connect ->  To get the ip address");
	    puts("get     ->  To recieve a file from server");
	    puts("put     ->  To send a file to the server");
	    puts("quit    ->  To exit from the application");
	}


	else
	    printf("undifned command\ntype help for futher details\n");




    }


    return 0;
}







