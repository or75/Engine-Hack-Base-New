#pragma once

#include <windows.h>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <list>
#include <map>

#include "cvar.h"
#include "config.h"
#include "stringfinder.h"
#include "interpreter.h"
#include "ModuleSecurity.h"

#include "ValveSDK/engine/wrect.h"
#include "ValveSDK/engine/cl_dll.h"
#include "ValveSDK/engine/r_studioint.h"
#include "ValveSDK/engine/cl_entity.h"
#include "ValveSDK/misc/com_model.h"
#include "ValveSDK/engine/triangleapi.h"
#include "ValveSDK/engine/pmtrace.h"
#include "ValveSDK/engine/pm_defs.h"
#include "ValveSDK/engine/pm_info.h"
#include "ValveSDK/common/ref_params.h"
#include "ValveSDK/common/studio_event.h"
#include "ValveSDK/common/net_api.h"
#include "ValveSDK/common/r_efx.h"
#include "ValveSDK/engine/cvardef.h"
#include "ValveSDK/engine/util_vector.h"
#include "ValveSDK/misc/parsemsg.h"
#include "ValveSDK/engine/studio.h"
#include "ValveSDK/engine/event_args.h"
#include "ValveSDK/engine/event_flags.h"
#include "ValveSDK/common/event_api.h"
#include "ValveSDK/common/screenfade.h"
#include "ValveSDK/engine/keydefs.h"
#include "ValveSDK/common/engine_launcher_api.h"

using namespace std;

static bool Init = true;

typedef struct cl_clientfuncs_s
{
	int ( *Initialize ) ( cl_enginefunc_t *pEnginefuncs, int iVersion );
	int ( *HUD_Init ) ( void );
	int ( *HUD_VidInit ) ( void );
	void ( *HUD_Redraw ) ( float time, int intermission );
	int ( *HUD_UpdateClientData ) ( client_data_t *pcldata, float flTime );
	int ( *HUD_Reset ) ( void );
	void ( *HUD_PlayerMove ) ( struct playermove_s *ppmove, int server );
	void ( *HUD_PlayerMoveInit ) ( struct playermove_s *ppmove );
	char ( *HUD_PlayerMoveTexture ) ( char *name );
	void ( *IN_ActivateMouse ) ( void );
	void ( *IN_DeactivateMouse ) ( void );
	void ( *IN_MouseEvent ) ( int mstate );
	void ( *IN_ClearStates ) ( void );
	void ( *IN_Accumulate ) ( void );
	void ( *CL_CreateMove ) ( float frametime, struct usercmd_s *cmd, int active );
	int ( *CL_IsThirdPerson ) ( void );
	void ( *CL_CameraOffset ) ( float *ofs );
	struct kbutton_s *( *KB_Find ) ( const char *name );
	void ( *CAM_Think ) ( void );
	void ( *V_CalcRefdef ) ( struct ref_params_s *pparams );
	int ( *HUD_AddEntity ) ( int type, struct cl_entity_s *ent, const char *modelname );
	void ( *HUD_CreateEntities ) ( void );
	void ( *HUD_DrawNormalTriangles ) ( void );
	void ( *HUD_DrawTransparentTriangles ) ( void );
	void ( *HUD_StudioEvent ) ( const struct mstudioevent_s *event, const struct cl_entity_s *entity );
	void ( *HUD_PostRunCmd ) ( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed );
	void ( *HUD_Shutdown ) ( void );
	void ( *HUD_TxferLocalOverrides ) ( struct entity_state_s *state, const struct clientdata_s *client );
	void ( *HUD_ProcessPlayerState ) ( struct entity_state_s *dst, const struct entity_state_s *src );
	void ( *HUD_TxferPredictionData ) ( struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd );
	void ( *Demo_ReadBuffer ) ( int size, unsigned char *buffer );
	int ( *HUD_ConnectionlessPacket ) ( struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size );
	int ( *HUD_GetHullBounds ) ( int hullnumber, float *mins, float *maxs );
	void ( *HUD_Frame ) ( double time );
	int ( *HUD_Key_Event ) ( int down, int keynum, const char *pszCurrentBinding );
	void ( *HUD_TempEntUpdate ) ( double frametime, double client_time, double cl_gravity, struct tempent_s **ppTempEntFree, struct tempent_s **ppTempEntActive, int ( *Callback_AddVisibleEntity )( struct cl_entity_s *pEntity ), void ( *Callback_TempEntPlaySound )( struct tempent_s *pTemp, float damp ) );
	struct cl_entity_s *( *HUD_GetUserEntity ) ( int index );
	int ( *HUD_VoiceStatus ) ( int entindex, qboolean bTalking );
	int ( *HUD_DirectorMessage ) ( unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags );
	int ( *HUD_GetStudioModelInterface ) ( int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio );
	void ( *HUD_CHATINPUTPOSITION_FUNCTION ) ( int *x, int *y );
	int ( *HUD_GETPLAYERTEAM_FUNCTION ) ( int iplayer );
	void ( *CLIENTFACTORY ) ( void );
} cl_clientfunc_t;

struct GameInfo_s
{
	char* GameName;
	char* GameVersion;
	BYTE Protocol;
	DWORD Build;
};
extern GameInfo_s BuildInfo;

typedef struct server_msg_t
{
	int index;
	char* Name;
	DWORD Callback;
} *server_msg_s;

typedef struct sizebuf_t
{
	int AllowOverflow, Overflowed;
	BYTE* Data;
	int MaxSize, CurrentSize;
} *sizebuf_s;

typedef server_msg_t server_msg_array_t[1];
typedef server_msg_array_t* server_msg_array_s;

struct local_s
{
	int iClip;
	int iWeaponID;
	int iIndex;
	int iTeam;
};

struct player_s
{
	int iTeam;
};

extern player_s g_Player[33];
extern char g_szBaseDir[256];
extern local_s g_Local;
extern HINSTANCE hInstance;
extern SCREENINFO g_Screen;
extern cl_clientfunc_t *g_pClient;
extern cl_clientfunc_t g_Client;
extern cl_enginefunc_t *g_pEngine;
extern cl_enginefunc_t g_Engine;
extern engine_studio_api_t *g_pStudio;
extern engine_studio_api_t g_Studio;

#define	SVC_BAD                   0
#define	SVC_NOP                   1
#define	SVC_DISCONNECT            2
#define	SVC_EVENT                 3
#define	SVC_VERSION               4
#define	SVC_SETVIEW               5
#define	SVC_SOUND                 6
#define	SVC_TIME                  7
#define	SVC_PRINT                 8
#define	SVC_STUFFTEXT             9
#define	SVC_SETANGLE              10
#define	SVC_SERVERINFO            11
#define	SVC_LIGHTSTYLE            12
#define	SVC_UPDATEUSERINFO        13
#define	SVC_DELTADESCRIPTION      14
#define	SVC_CLIENTDATA            15
#define	SVC_STOPSOUND             16
#define	SVC_PINGS                 17
#define	SVC_PARTICLE              18
#define	SVC_DAMAGE                19
#define	SVC_SPAWNSTATIC           20
#define	SVC_EVENT_RELIABLE        21
#define	SVC_SPAWNBASELINE         22
#define	SVC_TEMPENTITY            23
#define	SVC_SETPAUSE              24
#define	SVC_SIGNONNUM             25
#define	SVC_CENTERPRINT           26
#define	SVC_KILLEDMONSTER         27
#define	SVC_FOUNDSECRET           28
#define	SVC_SPAWNSTATICSOUND      29
#define	SVC_INTERMISSION          30
#define	SVC_FINALE                31
#define	SVC_CDTRACK               32
#define	SVC_RESTORE               33
#define	SVC_CUTSCENE              34
#define	SVC_WEAPONANIM            35
#define	SVC_DECALNAME             36
#define	SVC_ROOMTYPE              37
#define	SVC_ADDANGLE              38
#define	SVC_NEWUSERMSG            39
#define	SVC_PACKETENTITIES        40
#define	SVC_DELTAPACKETENTITIES   41
#define	SVC_CHOKE                 42
#define	SVC_RESOURCELIST          43
#define	SVC_NEWMOVEVARS           44
#define	SVC_RESOURCEREQUEST       45
#define	SVC_CUSTOMIZATION         46
#define	SVC_CROSSHAIRANGLE        47
#define	SVC_SOUNDFADE             48
#define	SVC_FILETXFERFAILED       49
#define	SVC_HLTV                  50
#define	SVC_DIRECTOR              51
#define	SVC_VOICEINIT             52
#define	SVC_VOICEDATA             53
#define	SVC_SENDEXTRAINFO         54
#define	SVC_TIMESCALE             55
#define	SVC_RESOURCELOCATION      56
#define	SVC_SENDCVARVALUE         57
#define	SVC_SENDCVARVALUE2        58

string szDirFile( char* pszName );
int pfnHookUserMsg( char *szMsgName, pfnUserMsgHook pfn );