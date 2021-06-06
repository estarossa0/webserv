#include "websrv.h"
#define PORT 80

int main()
{
	char buffer[1024] = {0};
	char response[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 5\n";
	server sv(PORT);

	while (1)
	{
		sv.connect();
		sv.read(buffer, sizeof(buffer));
		log buffer line;
		sv.send((char *) response, strlen(response));	// response header
		sv.send((void *)"\n", 1);						//indicate end of header
		sv.send((void *)"hello", 5);					// body message
		printf("Hello message sent\n");
		sv.close();
	}
	return 0;
}