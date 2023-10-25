#pragma once

#ifndef __PACKET_H__ 
#define __PACKET_H__

enum class EPacket
{
	None                  = 0,

	ProgramStart          = 1,

	// 시작 시 분기 확인
	C2S_IsLogin           = 2,
	S2C_IsLogin           = 3,
	C2S_IsJoin            = 4,
	S2C_IsJoin            = 5,

	// 로그인
	C2S_ReqCheckID        = 101,
	S2C_ID_Success        = 102,
	S2C_ID_Failed         = 103,

	C2S_ReqCheckPW        = 111,
	S2C_PW_Success        = 112,
	S2C_PW_Failed         = 113,

	// 회원 가입
	C2S_Make_NewID        = 201,
	S2C_Make_ID_Success   = 202,
	S2C_Make_ID_Failed    = 203,

	C2S_Make_NewPW        = 204,
	S2C_Make_PW_Success   = 205,
	S2C_Make_PW_Failed    = 206,

	C2S_Join_Success      = 207,

	// 채팅 상황
	C2S_IsCanChat         = 300,
	S2C_CanChat           = 301,

	C2S_Chat              = 302,
	S2C_Chat              = 303,

	Max,
};

#pragma pack(1)
#pragma pack()

#endif