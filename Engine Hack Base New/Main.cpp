#include "Main.h"
#include "SetupHooks.h"

char g_szBaseDir[256];
local_s g_Local;
HINSTANCE hInstance;
SCREENINFO g_Screen;
GameInfo_s BuildInfo;
cl_clientfunc_t *g_pClient = nullptr;
cl_clientfunc_t g_Client;
cl_enginefunc_t *g_pEngine = nullptr;
cl_enginefunc_t g_Engine;
engine_studio_api_t *g_pStudio = nullptr;
engine_studio_api_t g_Studio;

string szDirFile( char* pszName )
{
	string szRet = g_szBaseDir;
	return (szRet + pszName);
}

BOOL __declspec(dllexport) APIENTRY DllMain( HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved )
{
	if( dwReason == DLL_PROCESS_ATTACH )
	{
		if ( GetLastError() != ERROR_ALREADY_EXISTS )
		{
			GetModuleFileName( hModule, g_szBaseDir, sizeof( g_szBaseDir ) );
			char* pos = g_szBaseDir + strlen( g_szBaseDir );
			while( pos >= g_szBaseDir && *pos!='\\' ) --pos; pos[ 1 ]=0;
			hInstance = hModule;
			DisableThreadLibraryCalls((HINSTANCE)hModule);
			SetupHooks* Hook = new SetupHooks;
			Hook->StartThread(&SetupHooks::Initialize, 0);
			delete Hook;
		}
	}
	return TRUE;
}