#include "SvcMessage.h"

#pragma warning(disable:4996)

// SVC Hook by _or_75

// ���� ��������� �����/������� , ������ �� �� ������ � ��� ���������
static const char* blockList[] = 
{
	"exit","quit","bind","unbind","unbindall","kill","exec","alias",
	"clear","motdfile","motd_write","writecfg","cd","echo","cmd",
	"developer","fps_max","fps_modem","disconnect","connect","motd","name",
	"csx_setcvar","set","screenshot"
};

// ���� ��������� ����� , ������ �������� �� �� ������
static const char* blockListCvar[] = 
{
	"developer","fps_max","fps_modem"
};

#define MAX_CMD_LINE 1024

int* MSG_ReadCount = 0;
int* MSG_CurrentSize = 0;
int* MSG_BadRead = 0;
void* MSG_Base = 0;
int MSG_SavedReadCount = 0;

char com_token[1024];

HL_MSG_ReadByte MSG_ReadByte;
HL_MSG_ReadShort MSG_ReadShort;
HL_MSG_ReadLong MSG_ReadLong;
HL_MSG_ReadString MSG_ReadString;

HL_MSG_CBuf_AddText CBuf_AddText_Orign;

TEmptyCallback SVC_StuffText_Orign;
TEmptyCallback SVC_SendCvarValue_Orign;
TEmptyCallback SVC_SendCvarValue2_Orign;

TEmptyCallback HookServerMsg(const unsigned Index, void* CallBack,AutoOffset* Offset)
{
	TEmptyCallback Call = (TEmptyCallback)Offset->SVCBase[Index]->Callback;
	Offset->SVCBase[Index]->Callback = (DWORD)CallBack;
	return Call;
}

void MSG_SaveReadCount()
{
	MSG_SavedReadCount = *MSG_ReadCount;
}

void MSG_RestoreReadCount()
{
	*MSG_ReadCount = MSG_SavedReadCount;
}

void CBuf_AddText(char* text)
{
	char str[1024];
	strncpy(str, text, sizeof(str));
	str[sizeof(str) - 1] = 0;

	if (SanitizeCommands(str))
		return;

	CBuf_AddText_Orign(text);
}

void SVC_StuffText() // server send cmd
{
	MSG_SaveReadCount();
	char* commands = MSG_ReadString();

	char str[1024];
	strncpy(str, commands, sizeof(str));
	str[sizeof(str) - 1] = 0;

	if (SanitizeCommands(str))
		return;

	MSG_RestoreReadCount();
	SVC_StuffText_Orign();
}

void SVC_SendCvarValue()
{
	MSG_SaveReadCount();
	char* cvar = MSG_ReadString();
	
	bool isGood = IsCvarGood(cvar);
	if (!isGood)
	{
		g_Engine.Con_Printf("\t\t\t\t\t\t\t\t\t[HLR] Server tried to request blocked cvar: %s\n", cvar);
		return;
	}
	else
		g_Engine.Con_Printf("\t\t\t\t\t\t\t\t\t[HLR] Server tried to request not blocked cvar: %s\n", cvar);

	MSG_RestoreReadCount();	
	SVC_SendCvarValue_Orign();
}

void SVC_SendCvarValue2()
{
	MSG_SaveReadCount();
	char* cvar = MSG_ReadString();

	bool isGood = IsCvarGood(cvar);
	if (!isGood)
	{
		g_Engine.Con_Printf("\t\t\t\t\t\t\t\t\t[HLR] Server tried to request blocked cvar: %s\n", cvar);
		return;
	}
	else
		g_Engine.Con_Printf("\t\t\t\t\t\t\t\t\t[HLR] Server tried to request not blocked cvar: %s\n", cvar);

	MSG_RestoreReadCount();	
	SVC_SendCvarValue2_Orign();
}

char ToUpper(char ch)
{
    return (char)(ch+32);
}

bool ParseListCvar(const char* str)
{
	DWORD temp = 0;
	DWORD len = sizeof(str)-1;
	for(DWORD i = 0;i < _countof(blockListCvar);i++)
	{
		for(DWORD x = 0;x < len;x++)
		{
			if ( blockListCvar[i][x] == str[x] || blockListCvar[i][x] == ToUpper(str[x]) )
			{
				temp++;
				if ( temp == len )
					return true;
			}
			else temp = 0;
		}
	}
	return false;
}

bool ParseList(const char* str)
{
	DWORD temp = 0;
	DWORD len = sizeof(str)-1;
	for(DWORD i = 0;i < _countof(blockList);i++)
	{
		for(DWORD x = 0;x < len;x++)
		{
			if ( blockList[i][x] == str[x] || blockList[i][x] == ToUpper(str[x]) )
			{
				temp++;
				if ( temp == len )
					return true;
			}
			else temp = 0;
		}
	}
	return false;
}

bool IsCvarGood(const char *str)
{
	if (str[0] == 0)
		return true;

	if (ParseListCvar(str))
		return false;

	return true;
}

bool IsCommandGood(const char *str)
{
	char *ret = g_Engine.COM_ParseFile((char *)str, com_token);
	if (ret == NULL || com_token[0] == 0)
		return true;
	if ((ParseList(com_token)))
		return false;
	return true;
}

bool SanitizeCommands(char *str)
{
	bool changed = false;
	char *text = str;
	char command[MAX_CMD_LINE];
	int i, quotes;
	int len = strlen(str);

	while (text[0] != 0)
	{
		quotes = 0;
		for (i = 0; i < len; i++)
		{
			if (text[i] == '\"') quotes++;
			if (!(quotes & 1) && text[i] == ';')
				break;
			if (text[i] == '\n')
				break;
			if (text[i] == 0x00)
				break;
		}
		if (i >= MAX_CMD_LINE)
			i = MAX_CMD_LINE;

		strncpy(command, text, i);
		command[i] = 0;

		bool isGood = IsCommandGood(command);

		int log = 1;
		if (log > 0)
		{
			char *c = command;
			char *a = isGood ? 
				"\t\t\t\t\t\t\t\t\t[HLR] Server execute command: %s\n": 
				"\t\t\t\t\t\t\t\t\t[HLR] Server blocked command: %s\n";
			g_Engine.Con_Printf(a,c);
		}
		len -= i;
		if (!isGood)
		{
			strncpy(text, text + i, len);
			text[len] = 0;
			text++;
			changed = true;
		}
		else
		{
			text += i + 1;
		}
	}
	return changed;
}