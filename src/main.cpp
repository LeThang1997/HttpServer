#include <iostream>
#include <stdio.h>
#include "json.h"
#include "HttpServer.hpp"



int main()
{
	printf("ThangLMb is testing server anpr \n");
	HttpServer* httpServer = NULL;
	httpServer = new HttpServer();
	while(1)
	httpServer->loop();
	return 0;
}
