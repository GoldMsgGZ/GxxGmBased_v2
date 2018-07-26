#include <iostream>
#include <string>
#include "framework.h"
#include "GSDefs.h"
#include "GSSysApi.h"


static void ShowMenu( void )
{
	gs_printf("\n enter 'q' to exit...  \n");
	gs_printf("enter 'dump' to getdump...  \n");

}

static void WaitForCommand( void )
{
	std::string strIn("");
	while ( 1 )
	{
		getline(std::cin, strIn);
#ifdef _LINUX
		if (strIn.empty()) 
		{
			std::cin.clear();
			::sleep(100);
			continue;
		}
#endif

		if ( strIn == "q" )
			break;
		else if ( strIn == "dump" )
			CFrameWork::GetInstance()->GetDump();
		else
			CFrameWork::GetInstance()->PassCmdParam( strIn.c_str() );
		//ShowMenu();
	}

}

int main(int argc, char* argv[])
{
	int errCode = CFrameWork::GetInstance()->Init("28181MediaServerConfig.ini");
	if (errCode == 0)
	{
		errCode = CFrameWork::GetInstance()->Start();
		if (errCode != 0)
		{
			gs_printf( "CFrameWork Start fail! \n");
		}
	}
	else
	{
		gs_printf( "CFrameWork Init fail! \n\n程序在10s后关闭...");
		GSSleep(10000);
		exit(0);
	}

	ShowMenu();
	WaitForCommand();


	CFrameWork::GetInstance()->Stop();
	CFrameWork::GetInstance()->Cleanup();
}