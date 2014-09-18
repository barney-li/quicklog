#pragma once
#include <string>
using namespace std;
#define NULL 0
#define null NULL

typedef signed char TYPE_CHAR;
typedef	signed char	TYPE_INT8;
typedef	short		TYPE_INT16;
typedef	int		TYPE_INT32;
typedef long long	TYPE_INT64;

typedef	unsigned char	TYPE_UINT8;
typedef	unsigned short	TYPE_UINT16;
typedef	unsigned int	TYPE_UINT32;
typedef unsigned long long TYPE_UINT64;

typedef	unsigned char	TYPE_UCHAR;
typedef unsigned short	TYPE_USHORT;
typedef	unsigned int	TYPE_UINT;
typedef unsigned long	TYPE_ULONG;

typedef enum ORDER_DIRECTION
{
	BUY=0,
	SELL=1
};

typedef enum ORDER_TYPE
{
	OPEN=0,
	CLOSE=1,
	CLOSE_TODAY
};
#define CMD_LEN 50
typedef enum 
{
	cmdInitializeProcess		=	0,
	cmdReqConnect				=	1,
	cmdReqOrderInsert			=	5,
	cmdReqOrderActionChange		=	6,
	cmdReqQryInstrument			=	7,
	cmdReqOrderAction			=	8,
	cmdReqQryOrder				=	9,
	cmdReqQryInvestorPosition	=	10,
	cmdReqOrderInsertArbitrage	=	11
}CommandType;
// order identifier, used for order action, qoury
typedef struct 
{
	// any order can be identified with three parameters below
	int sessionId;
	int frontId;
	string orderRef;
	string exchangeId;
	string orderSysId;
	string instrument;
}ORDER_INDEX_TYPE;

