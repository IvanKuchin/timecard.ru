// #include<stdio.h>
#include<string>
#include "c_bt.h"

using namespace std;

string Test1()
{
	string error_message = "";

	return error_message;
}

int main(void)
{
	string	error_message = "";

	error_message = Test1();

	if(error_message.empty())
		printf("Test passed, but with failure");
	else
		printf("Test failed");

	return(0);
}

