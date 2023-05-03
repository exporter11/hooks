#pragma once
#include "usercmd.h"
namespace hooks {
	bool __fastcall CreateMoveHk(void* ecx, void* edx, float frametime, CUserCMD* pCmd);
}