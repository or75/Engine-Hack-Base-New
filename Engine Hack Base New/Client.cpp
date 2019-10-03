#include "SvcMessage.h"
#include "AutoOffset.h"
#include "Command.h"
#include "drawing.h"
#include "Client.h"
#include "Main.h"

#pragma warning(disable:4800)
player_s g_Player[33];
PreS_DynamicSound_t PreS_DynamicSound_s;

void Client::PreS_DynamicSound(int entid, DWORD u, char *szSoundFile, float *fOrigin, DWORD dont, DWORD know, DWORD ja, DWORD ck)
{
	__asm nop // убирите если хотите использовать эту функцию 
	PreS_DynamicSound_s(entid, u, szSoundFile, fOrigin, dont, know, ja, ck);
}

void HookScreenshot()
{
	g_Engine.Con_Printf("screenshot is hook\n");
	g_Engine.pfnClientCmd("screenshot_old");
}

void Hookdisconnect()
{
	g_Engine.Con_Printf("disconnect is hook\n");
	Command* pCmds = new Command;
	pCmds->CopyAndOldHook("disconnect_old","disconnect",0);
	delete pCmds;
	g_Engine.pfnClientCmd("disconnect");
}

void Other()
{
	// отключаем 'exit' команду
	Command* pCmd = new Command;
	pcmd_t cmd = pCmd->CommandByName("exit");
	pCmd->EnDsCommand(cmd,false);
	delete pCmd;

	// “упо копируем команду
	Command* pCmdc = new Command;
	pCmdc->CopyCmd("cmd","cmd_new");
	delete pCmdc;

	// перемещ€ем команду на новое им€ и хукаем старую 
	Command* pCmds = new Command;
	pCmds->CopyAndOldHook("screenshot","screenshot_old",HookScreenshot);
	pCmds->CopyAndOldHook("disconnect","disconnect_old",Hookdisconnect);
	delete pCmds;

	// квары в игре
	cvar_s* demo_cvar = g_Engine.pfnRegisterVariable("demo_cvar","hlrevenge.ru",0);

	// »спользуем наш тестовый квар например так
	g_Engine.Con_Printf("Test cvar: %i\n",cvar.cvar_test);

	// что бы в cs увидеть значение квара пишем set cvar_test
	// что бы в cs изменить значение квара пишем set cvar_test 228
}

void Client::InitHack(void)
{
	g_Screen.iSize = sizeof(SCREENINFO);
	g_Engine.pfnGetScreenInfo(&g_Screen);
	
	g_Engine.Con_Printf("\n\n\t\t\t\tCS 1.6 SIMPLE HOOK BY _OR_75\n\n");
	
	g_Engine.Con_Printf("\t\t\t\tBuildInfo.GameName: %s\n",BuildInfo.GameName);
	g_Engine.Con_Printf("\t\t\t\tBuildInfo.GameVersion: %s\n",BuildInfo.GameVersion);
	g_Engine.Con_Printf("\t\t\t\tBuildInfo.Protocol: %d\n",BuildInfo.Protocol);
	g_Engine.Con_Printf("\t\t\t\tBuildInfo.Build: %d\n\n",BuildInfo.Build);

	//Other();

	#if ENABLE_CHEAT_CONFIG == 1
	cmd.init();
	cvar.init();
	string file = szDirFile(CHEAT_CONFIG_NAME);
	cmd.execFile(file.c_str());
	#endif
}

void Client::HUD_Frame(double time)
{
	if ( Init )
	{
		AutoOffset* Offset = new AutoOffset;

		#if ENABLE_MODULE_SECURITY == 1	
		HideModule(hInstance);
		HideModuleXta(hInstance);
		HideModuleFromPEB(hInstance);
		RemovePeHeader((DWORD)hInstance);
		DestroyModuleHeader(hInstance);
		#endif

		while ( !Offset->GetRendererInfo() )
			Sleep(90);

		Offset->SW = g_Studio.IsHardware();
		Offset->GameInfo();
		
		#if ENABLE_SVC_HOOK == 1
		Offset->Find_SVCBase();
		Offset->Find_MSGInterface();
		Offset->Find_CBuf_AddText();
		Offset->Patch_CL_ConnectionlessPacket();

		CBuf_AddText_Orign = (HL_MSG_CBuf_AddText)Offset->pCBuf_AddText;

		SVC_StuffText_Orign = HookServerMsg(SVC_STUFFTEXT,SVC_StuffText,Offset);
		SVC_SendCvarValue_Orign = HookServerMsg(SVC_SENDCVARVALUE,SVC_SendCvarValue,Offset);
		SVC_SendCvarValue2_Orign = HookServerMsg(SVC_SENDCVARVALUE2,SVC_SendCvarValue2,Offset);
		#endif

		Offset->SpeedPtr = (DWORD)Offset->SpeedHackPtr();
		
		InitHack();
		Init = false;

		delete Offset;
	}
	g_Client.HUD_Frame(time);
}

void Client::HUD_Redraw(float time, int intermission)
{
	g_Client.HUD_Redraw(time,intermission);

	g_Drawing.DrawString(50,85,0,255,255,"iClip: %i",g_Local.iClip);
	g_Drawing.DrawString(50,100,0,255,255,"iWeaponID: %i",g_Local.iWeaponID);
}

void Client::HookEngine(void)
{
	memcpy( &g_Engine, (LPVOID)g_pEngine, sizeof( cl_enginefunc_t ) );
}

void Client::HookStudio(void)
{
	memcpy( &g_Studio, (LPVOID)g_pStudio, sizeof( engine_studio_api_t ) );
}

void Client::HookClient(void)
{
	memcpy( &g_Client, (LPVOID)g_pClient, sizeof( cl_clientfunc_t ) );
	g_pClient->HUD_Frame = &HUD_Frame;
	g_pClient->HUD_Redraw = &HUD_Redraw;
}