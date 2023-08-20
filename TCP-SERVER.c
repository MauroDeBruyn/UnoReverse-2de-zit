#ifdef _WIN32
	#define _WIN32_WINNT _WIN32_WINNT_WIN7
	#include <winsock2.h> //for all socket programming
	#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <stdint.h>
	#include <stdio.h>
	#include <ctype.h>
	#include <string.h> //for memset
<<<<<<< HEAD
	#include <time.h>
=======
	#include<time.h> //for displaying time
>>>>>>> cfd0f984f42e736a198b7bfe685e1817bfb1cc62
	void OSInit( void )
	{
		WSADATA wsaData;
		int WSAError = WSAStartup( MAKEWORD( 2, 0 ), &wsaData );
		if( WSAError != 0 )
		{
			fprintf( stderr, "WSAStartup errno = %d\n", WSAError );
			exit( -1 );
		}
	}
	void OSCleanup( void )
	{
		WSACleanup();
	}

#else
	#include <sys/socket.h> //for sockaddr, socket, socket
	#include <sys/types.h> //for size_t
	#include <netdb.h> //for getaddrinfo
	#include <netinet/in.h> //for sockaddr_in
	#include <arpa/inet.h> //for htons, htonl, inet_pton, inet_ntop
	#include <errno.h> //for errno
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
	#include<time.h> //for displaying time
	void OSInit( void ) {}
	void OSCleanup( void ) {}

#endif

char* inet_ntop(int af, const void* src, char* dst, socklen_t size);
int initialization(int flag);
int connection( int internet_socket,const char * client_address_str, int size );
void execution( int internet_socket,FILE * filePointer, char client_address_str[INET6_ADDRSTRLEN] );
void HTTPclient(const char * client_address_str,FILE *filePointer);
void cleanup( int internet_socket );
//void logFiles(char buffer[1000]);

int main( int argc, char * argv[] )
{
	//////////////////
	//Initialization//
	//////////////////

	FILE *filePointer =fopen( "attack_.log", "a" );

	OSInit();

	int internet_socket = initialization(0);

	char client_address_str[INET6_ADDRSTRLEN];//string to store IP address

	//////////////
	//Connection//
	//////////////

	int client_internet_socket = connection( internet_socket, client_address_str, sizeof(client_address_str) );

	/////////////
	//Execution//
	/////////////

	execution( client_internet_socket, filePointer, client_address_str );

	////////////
	//Clean up//
	////////////

	fclose(filePointer);

	cleanup( internet_socket );

	OSCleanup();

	return 0;
}

char* inet_ntop(int af, const void* src, char* dst, socklen_t size)
{
    if (af == AF_INET)
		{
        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_addr = (struct in_addr)src;
        if (inet_ntop(AF_INET, &(sa.sin_addr), dst, size) == NULL)
				{
            perror("inet_ntop");
            return NULL;
        }
        return dst;
    }
		else if (af == AF_INET6)
		{
        struct sockaddr_in6 sa;
        sa.sin6_family = AF_INET6;
        sa.sin6_addr = (struct in6_addr)src;
        if (inet_ntop(AF_INET6, &(sa.sin6_addr), dst, size) == NULL)
				{
            perror("inet_ntop");
            return NULL;
        }
        return dst;
    }
    fprintf(stderr, "Unsupported address family\n");
    return NULL;
}

int initialization(int flag)
{
	//Step 1.1
	struct addrinfo internet_address_setup;
	struct addrinfo * internet_address_result;
	memset( &internet_address_setup, 0, sizeof internet_address_setup );
	internet_address_setup.ai_family = AF_UNSPEC;
	internet_address_setup.ai_socktype = SOCK_STREAM;
	internet_address_setup.ai_flags = AI_PASSIVE;
	if (flag == 0)
	{
		int getaddrinfo_return = getaddrinfo( NULL, "22", &internet_address_setup, &internet_address_result );
		if( getaddrinfo_return != 0 )
		{
			fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
			exit( 1 );
		}

		int internet_socket = -1;
		struct addrinfo * internet_address_result_iterator = internet_address_result;
		while( internet_address_result_iterator != NULL )
		{
			//Step 1.2
			internet_socket = socket( internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol );
			int mode = 0;
			setsockopt(internet_socket, AF_UNSPEC, AF_UNSPEC, (char*)&mode, sizeof(mode));
			if( internet_socket == -1 )
			{
				perror( "socket" );
			}
			else
			{
				//Step 1.3
				int bind_return = bind( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
				if( bind_return == -1 )
				{
					perror( "bind" );
					close( internet_socket );
				}
				else
				{
					//Step 1.4
					int listen_return = listen( internet_socket, 1 );
					if( listen_return == -1 )
					{
						close( internet_socket );
						perror( "listen" );
					}
					else
					{
						break;
					}
				}
			}
			internet_address_result_iterator = internet_address_result_iterator->ai_next;
		}

		freeaddrinfo( internet_address_result );

		if( internet_socket == -1 )
		{
			fprintf( stderr, "socket: no valid socket address found\n" );
			exit( 2 );
		}

		return internet_socket;
	}
	else if (flag == 1)
	{
		int getaddrinfo_return = getaddrinfo( "ip-api.com", "80", &internet_address_setup, &internet_address_result );
		if( getaddrinfo_return != 0 )
		{
			fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
			exit( 1 );
		}

		int internet_socket = -1;
		struct addrinfo * internet_address_result_iterator = internet_address_result;
		while( internet_address_result_iterator != NULL )
		{
			//Step 1.2
			internet_socket = socket( internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol );
			if( internet_socket == -1 )
			{
				perror( "socket" );
			}
			else
			{
				//Step 1.3
				int connect_return = connect( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
				if( connect_return == -1 )
				{
					perror( "connect" );
					close( internet_socket );
				}
				else
				{
					break;
				}
			}
			internet_address_result_iterator = internet_address_result_iterator->ai_next;
		}

		freeaddrinfo( internet_address_result );

		if( internet_socket == -1 )
		{
			fprintf( stderr, "socket: no valid socket address found\n" );
			exit( 2 );
		}

		return internet_socket;
	}
}

int connection( int internet_socket, const char * client_address_str, int size )
{
	//Step 2.1
	struct sockaddr_storage client_internet_address;
	socklen_t client_internet_address_length = sizeof client_internet_address;
	int client_socket = accept( internet_socket, (struct sockaddr *) &client_internet_address, &client_internet_address_length );
	if( client_socket == -1 )
	{
		perror( "accept" );
		close( internet_socket );
		exit( 3 );
	}

	//get IP address from client
	if (client_internet_address.ss_family == AF_INET) {
			// IPv4 address
			struct sockaddr_in* s = (struct sockaddr_in*)&client_internet_address;
			inet_ntop(AF_INET, &s->sin_addr, client_address_str, size);
	}
	else
	{ // AF_INET6
			// IPv6 address
			struct sockaddr_in6* s = (struct sockaddr_in6*)&client_internet_address;
			inet_ntop(AF_INET6, &s->sin6_addr, client_address_str, size);
	}

	printf("Attacker IP address: %s\n", client_address_str);

	return client_socket;
}

void execution( int internet_socket, FILE * filePointer, char client_address_str[INET6_ADDRSTRLEN] )
{
	//Step 3.1
	int number_of_bytes_received = 0;
	char buffer[1000];
	number_of_bytes_received = recv( internet_socket, buffer, ( sizeof buffer ) - 1, 0 );
	if( number_of_bytes_received == -1 )
	{
		perror( "recv" );
	}
	else
	{
			buffer[number_of_bytes_received] = '\0';
			printf("Received : %s\n", buffer);
	}

	HTTPclient(client_address_str, filePointer);

	fputs(" Received: ",filePointer);
	fputs(buffer, filePointer);
	fputs("\n",filePointer);

	int number_of_bytes_send = 0;
	int totalBytesSend = 0;

	char totalBytesSendStr[20];

	int randInt;
	srand(time(NULL)); // seed the random number generator with the current time
	char attackString[16];

	while(1)
	{

			//Counter attack
      randInt = rand() % 999999 + 1; // generate a random integer between 1 and 999999
			sprintf(attackString, "%d", randInt);
			number_of_bytes_send = send( internet_socket, "attackString", 16, 0 );

			//check if client left and put number of bytes send in the log file
			if( number_of_bytes_send == -1 )
			{
					printf("Client left. Server sent %d bytes\n",totalBytesSend);
					sprintf(totalBytesSendStr, "%d", totalBytesSend);
					fputs(" Total bytes send: ",filePointer);
					fputs(totalBytesSendStr, filePointer);
					fputs("\n",filePointer);
					break;
			}
			else
			{
					totalBytesSend +=number_of_bytes_send;
					sleep(100000);
			}
	}
	cleanup(internet_socket);
}

void cleanup( int internet_socket )
{
	#ifdef _WIN32 //windows
	    int shutdown_return = shutdown( internet_socket, SD_RECEIVE );
	    if( shutdown_return == -1 )
	    {
	        perror( "shutdown" );
	    }
	#else //linux
	    int shutdown_return = shutdown( internet_socket, SHUT_RD );
	    if( shutdown_return == -1 )
	    {
	        perror( "shutdown" );
	    }
	#endif

	close( internet_socket );
}

void HTTPclient(const char * client_address_string,FILE *filePointer)
{
	int internet_socket = initialization(1);

	fputs("IP address:",filePointer);
	fputs(client_address_string,filePointer);
	fputs("\n",filePointer);

	char buffer[1000];
	char HTTPrequest[100] ={0};

	sprintf(HTTPrequest,"GET /json/%s HTTP/1.0\r\nHost: ip-api.com\r\nConnection: close\r\n\r\n", client_address_string);
	printf("HTTP request = %s",HTTPrequest);

	int number_of_bytes_send = 0;
	number_of_bytes_send = send( internet_socket, HTTPrequest, 16, 0 );
	if( number_of_bytes_send == -1 )
	{
		perror( "send" );
	}

	int number_of_bytes_received = 0;
	number_of_bytes_received = recv( internet_socket, buffer, ( sizeof buffer ) - 1, 0 );
	if( number_of_bytes_received == -1 )
	{
		perror( "recv" );
	}
	else
	{
		buffer[number_of_bytes_received] = '\0';
		printf( "Received : %s\n", buffer );
	}

	char* jsonFile = strchr(buffer,'{');

	if( jsonFile == NULL)
	{
			number_of_bytes_received = recv( internet_socket, buffer, ( sizeof buffer ) - 1, 0 );

			if( number_of_bytes_received == -1 )
			{
					perror( "recv" );
			}
			else
			{
					buffer[number_of_bytes_received] = '\0';
					printf( "Received : %s\n", buffer );
			}

			//put geolocation in file
			fputs(" Geolocation = ",filePointer);
			fputs( buffer , filePointer );
			fputs("\n",filePointer);
	}
	else
	{
			//put geolocation in file
			fputs(" Geolocation = ",filePointer);
			fputs(jsonFile, filePointer);
			fputs("\n",filePointer);
	}
	cleanup(internet_socket);
}
/*
void logFiles(char buffer[1000]) //Function that will log the files
{
	time_t tm;
	time(&tm);

	FILE *f;
	f = fopen("attack_.log", "a");
	fprintf(f, "Attacker log %s%s\n\n", ctime(&tm), buffer);
	fclose(f);
}
*/
