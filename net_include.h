#include <sys/types.h>
#include <ifaddrs.h>
#include <unistd.h>
#include "sp.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define int32u unsigned int
#define FILE_NAME_LENGTH 20
#define HOST_NAME_LENGTH 20
#define GROUP_NAME_LENGTH 10
#define USER_NAME_LENGTH 20
#define TIME_LENGTH 50
#define MESSAGE_LENGTH 30
#define SERVER_NUM 5
#define DISPLAY_LINE 25
#define USE_SENDER 1
#define NOTUSE_SENDER 0
#define SERVER1_SPREADNAME "10255@128.220.224.88"
#define SERVER2_SPREADNAME "10255@128.220.224.89"
#define SERVER3_SPREADNAME "10255@128.220.224.90"
#define SERVER4_SPREADNAME "10255@128.220.224.92"
#define SERVER5_SPREADNAME "10255@128.220.224.94"
#define SERVER1_MEMBERNAME "#Server1#ugrad13"
#define SERVER2_MEMBERNAME "#Server2#ugrad14"
#define SERVER3_MEMBERNAME "#Server3#ugrad15"
#define SERVER4_MEMBERNAME "#Server4#ugrad17"
#define SERVER5_MEMBERNAME "#Server5#ugrad19"
#define SERVER1_RECOVER_FILE "Server1_Rec.txt" 
#define SERVER2_RECOVER_FILE "Server2_Rec.txt"
#define SERVER3_RECOVER_FILE "Server3_Rec.txt"
#define SERVER4_RECOVER_FILE "Server4_Rec.txt"
#define SERVER5_RECOVER_FILE "Server5_Rec.txt"
#define SERVER1_PUB_GROUP "100001"
#define SERVER2_PUB_GROUP "100002"
#define SERVER3_PUB_GROUP "100003"
#define SERVER4_PUB_GROUP "100004"
#define SERVER5_PUB_GROUP "100005"
#define SERVER1_PRIV_GROUP "200001"
#define SERVER2_PRIV_GROUP "200002"
#define SERVER3_PRIV_GROUP "200003"
#define SERVER4_PRIV_GROUP "200004"
#define SERVER5_PRIV_GROUP "200005"
#define SERVER1 "server1"
#define SERVER2 "server2"
#define SERVER3 "server3"
#define SERVER4 "server4"
#define SERVER5 "server5"
#define SERVER_GROUPS "100006"

struct Lamport
{
	int globalCounter;
	int serverIndex;	
};

struct LikeNode
{
	char userName[USER_NAME_LENGTH];
	struct Lamport likedMessageLamport;	
	struct LikeNode* nextLikeNode;
};

struct MessageNode
{
	int messageID;
	struct Lamport messageLamport;
	char messagePayLoad[MESSAGE_LENGTH];
	int numLike;
	char userName[USER_NAME_LENGTH];
	struct LikeNode* likeListHead;
	struct MessageNode* nextMessage;
	struct MessageNode* previousMessage;
};

struct UserNode
{
	char userName[USER_NAME_LENGTH];
	char serverName[HOST_NAME_LENGTH];
	struct UserNode* nextUser;
	int active;
};

struct ChatRoomNode
{
	int groupID;
	int numUser;
	int numMessage;
	struct ChatRoomNode* nextChatRoom;
	struct UserNode* userListHead;
	struct MessageNode* messageListHead;
	struct MessageNode* messageListTail;
};

struct Server
{
	int groupID;
	int numChatRoom;
	int serverView[SERVER_NUM];
	int previousServerView[SERVER_NUM];
	struct ChatRoomNode* chatRoomListHead;
};

typedef struct 
{
	char command;
	int likeLineNum;    /*line number associated with liked messaged*/
	int replyLineNum;   /*display line number, used on client side*/
	int likeness;
	int joinChatRoomID; 
	int recoverStates;
	
	int currentChatRoomID;	
	struct Lamport packetLamport;
	struct Lamport likedMessageLamport;
	char serverName[HOST_NAME_LENGTH];
	char previousServerName[HOST_NAME_LENGTH];
	char userName[USER_NAME_LENGTH];
	char switchUserName[USER_NAME_LENGTH];
	char messagePayLoad[MESSAGE_LENGTH];
	int serverView[SERVER_NUM];
	int antiEntroVector[SERVER_NUM];	
}normalPacket;

struct UpdateStateMessage
{
	normalPacket updateStateMessage;
	struct UpdateStateMessage* nextUpdate;
};

struct MultiServerStates
{
	struct UpdateStateMessage* serverStates[SERVER_NUM];	
	int antiEntroVector[SERVER_NUM];
};


