#pragma once

typedef void (*PreS_DynamicSound_t)(int, DWORD, char *, float *, DWORD, DWORD, DWORD, DWORD);

class Client
{
private:
	static void InitHack(void);
	static void HUD_Frame(double time);
	static void HUD_Redraw(float time, int intermission);
public:
	static void PreS_DynamicSound(int entid, DWORD u, char *szSoundFile, float *fOrigin, DWORD dont, DWORD know, DWORD ja, DWORD ck);
	void HookEngine(void);
	void HookStudio(void);
	void HookClient(void);
};

extern PreS_DynamicSound_t PreS_DynamicSound_s;