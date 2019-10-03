#ifndef __MOD_SEC__
#define __MOD_SEC__

void HideModuleFromPEB(HINSTANCE hInstance);
void RemovePeHeader(DWORD ModuleBase);
void HideModule( HANDLE hModule );
void HideModuleXta( HINSTANCE hModule );
bool DestroyModuleHeader(HMODULE hModule);

#endif