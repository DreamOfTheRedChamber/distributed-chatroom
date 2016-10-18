#include "net_include.h"
int addNewChatRoom(struct Server* serverPointer, int targetChatRoomID, char userName[USER_NAME_LENGTH]);
int addNewUserChatRoom(struct Server* serverPointer, int targetChatRoomID, char userName[USER_NAME_LENGTH], char serverName[HOST_NAME_LENGTH], int recoverFlag);
void printServerChatRoomUser(struct Server* serverPointer);
int removeUserChatRoom(struct Server* serverPointer, char* userName, int currentChatRoomID, char serverName[HOST_NAME_LENGTH], int recoverFlag);
int existChatRoom(struct Server* serverPointer, int targetChatRoomID);
int addMessage(struct Server* serverPointer, char userName[USER_NAME_LENGTH], int currentChatRoomID, char messagePayLoad[MESSAGE_LENGTH], struct Lamport packetLamport);
int replyMessageList(struct Server* serverPointer, normalPacket recPacket, mailbox* MboxPointer, char sender[MAX_GROUP_NAME], int private);

int likeMessage(struct Server* serverPointer, char* userName, int currentChatRoomID, int lineNum, struct Lamport* lamportPointer);
int serverLikeMessage(struct Server* serverPointer, char* userName, int currentChatRoomID, struct Lamport likedMessageLamport);
int dislikeMessage(struct Server* serverPointer, char* userName, int currentChatRoomID, int lineNum, struct Lamport* lamportPointer);
int serverDislikeMessage(struct Server* serverPointer, char* userName, int currentChatRoomID, struct Lamport dislikeMessageLamport);

int replyEntireMessageList(struct Server* serverPointer, normalPacket recPacket, mailbox* MboxPointer, char sender[MAX_GROUP_NAME]);
int replyServerView(struct Server* serverPointer, mailbox* MboxPointer, char sender[MAX_GROUP_NAME]);
int updateMultiServerStates(struct MultiServerStates* multiServerPointer, normalPacket recPacket);
int comeFromServer(char sender[MAX_GROUP_NAME]);
int comeFromSelf(char sender[MAX_GROUP_NAME], char Private_group[MAX_GROUP_NAME]);
int syncServers(normalPacket recPacket, mailbox* MboxPointer, char destGroup[MAX_GROUP_NAME]);
int addLamportStamp(normalPacket* recPacketPointer, int* globalCounter, int serverIndex);
void printMultiServerStates(struct MultiServerStates* multiServer);
int sendAntiEntroVector(struct MultiServerStates* multiServer, mailbox* MboxPointer);
int saveAntiEntroVector(int antiEntroMatrix[][SERVER_NUM], normalPacket* recPacket, char sender[MAX_GROUP_NAME]);
int analyzeSavedVector(struct Server* serverPointer, int antiEntroMatrix[][SERVER_NUM], struct MultiServerStates* multiServer, mailbox* MboxPointer, char sender[MAX_GROUP_NAME]);
int determineServerIndex(char sender[MAX_GROUP_NAME]);
void printAntiEntroMatrix(int antiEntroMatrix[SERVER_NUM][SERVER_NUM]);
int compareLamport(struct Lamport destLamport, struct Lamport sourceLamport);
int writeMultiServerStates(normalPacket recPacket, char fileName[FILE_NAME_LENGTH]);
int recoverServerStates(char fileName[FILE_NAME_LENGTH], mailbox* MboxPointer, char Private_group[MAX_GROUP_NAME]);
int customizeChatRoomID(char inputGroupName[MAX_GROUP_NAME], char currentServer[HOST_NAME_LENGTH]);
int randomTimeString(char randomTimeBuffer[TIME_LENGTH]);
int replyUserList(struct Server* serverPointer, normalPacket recPacket, mailbox* MboxPointer, char sender[MAX_GROUP_NAME]);
int sendSelfChatRoom(char targetChatRoomString[GROUP_NAME_LENGTH], struct Server* serverPointer, normalPacket recPacket, mailbox* MboxPointer);
int customizeReceivedChatRoomID(struct Server* serverPointer, normalPacket* recPacketPointer);
int resetUserActive(struct Server* serverPointer);
int muteAllUsers(struct Server* serverPionter);

int muteAllUsers(struct Server* serverPointer)
{
	struct ChatRoomNode* cycleChatRoomNode = serverPointer->chatRoomListHead;
	struct UserNode* cycleUserNode = NULL;

	if(cycleChatRoomNode == NULL)
	{
		printf("there is no chatroom created\n");
		return 0;
	}
	while(cycleChatRoomNode != NULL)
	{
		cycleUserNode = cycleChatRoomNode->userListHead;

		while(cycleUserNode != NULL)
		{
			cycleUserNode->active = 0;
			cycleUserNode = cycleUserNode->nextUser;
		}

		cycleChatRoomNode = cycleChatRoomNode->nextChatRoom;
	}
	return 1;
}



int resetUserActive(struct Server* serverPointer)
{
	struct ChatRoomNode* cycleChatRoomNode = serverPointer->chatRoomListHead;
	struct UserNode* cycleUserNode = NULL;
	int calculatedView;

	if(cycleChatRoomNode == NULL)
	{
		printf("there is no chatroom created\n");
		return 0;
	}
	while(cycleChatRoomNode != NULL)
	{
		cycleUserNode = cycleChatRoomNode->userListHead;

		while(cycleUserNode != NULL)
		{
			if(strncmp(cycleUserNode->serverName, SERVER1_PUB_GROUP, strlen(SERVER1_PUB_GROUP)) == 0)
			{
				calculatedView = 0;
			}
			else if(strncmp(cycleUserNode->serverName, SERVER2_PUB_GROUP, strlen(SERVER2_PUB_GROUP)) == 0)
			{
				calculatedView = 1;
			}
			else if(strncmp(cycleUserNode->serverName, SERVER3_PUB_GROUP, strlen(SERVER3_PUB_GROUP)) == 0)
			{
				calculatedView = 2;
			}
			else if(strncmp(cycleUserNode->serverName, SERVER4_PUB_GROUP, strlen(SERVER4_PUB_GROUP)) == 0)
			{
				calculatedView = 3;
			}
			else if(strncmp(cycleUserNode->serverName, SERVER5_PUB_GROUP, strlen(SERVER5_PUB_GROUP)) == 0)
			{
				calculatedView = 4;
			}
			if(serverPointer->serverView[calculatedView] == 0)
			{
				cycleUserNode->active = 0;
			}
			if(serverPointer->serverView[calculatedView] == 1)
			{
				cycleUserNode->active = 1;
			}
			cycleUserNode = cycleUserNode->nextUser;
		}

		cycleChatRoomNode = cycleChatRoomNode->nextChatRoom;
	}
	return 1;
}

int customizeReceivedChatRoomID(struct Server* serverPointer, normalPacket* recPacketPointer)
{
	int serverGroupID = 0;
	serverGroupID = serverPointer->groupID;

	if(serverGroupID == atoi(SERVER1_PUB_GROUP))
	{
		if(recPacketPointer->currentChatRoomID != 0)
		{
			recPacketPointer->currentChatRoomID = recPacketPointer->currentChatRoomID % 1000;
		}
		if(recPacketPointer->joinChatRoomID != 0)
		{
			recPacketPointer->joinChatRoomID = recPacketPointer->joinChatRoomID % 1000;
		}
	}
	else if(serverGroupID == atoi(SERVER2_PUB_GROUP))
	{
		if(recPacketPointer->currentChatRoomID != 0)
		{
			recPacketPointer->currentChatRoomID = recPacketPointer->currentChatRoomID % 1000 + 1000;
		}
		if(recPacketPointer->joinChatRoomID != 0)
		{
			recPacketPointer->joinChatRoomID = recPacketPointer->joinChatRoomID % 1000 + 1000;	
		}
	}
	else if(serverGroupID == atoi(SERVER3_PUB_GROUP))
	{
		if(recPacketPointer->currentChatRoomID != 0)
		{
			recPacketPointer->currentChatRoomID = recPacketPointer->currentChatRoomID % 1000 + 2000;
		}
		if(recPacketPointer->joinChatRoomID != 0)
		{
			recPacketPointer->joinChatRoomID = recPacketPointer->joinChatRoomID % 1000 + 2000;
		}
	}
	else if(serverGroupID == atoi(SERVER4_PUB_GROUP))
	{
		if(recPacketPointer->currentChatRoomID != 0)
		{
			recPacketPointer->currentChatRoomID = recPacketPointer->currentChatRoomID % 1000 + 3000;
		}
		if(recPacketPointer->joinChatRoomID != 0)
		{
			recPacketPointer->joinChatRoomID = recPacketPointer->joinChatRoomID % 1000 + 3000;
		}
	}
	else if(serverGroupID == atoi(SERVER5_PUB_GROUP))
	{
		if(recPacketPointer->currentChatRoomID != 0)
		{
			recPacketPointer->currentChatRoomID = recPacketPointer->currentChatRoomID % 1000 + 4000;
		}
		if(recPacketPointer->joinChatRoomID != 0)
		{
			recPacketPointer->joinChatRoomID = recPacketPointer->joinChatRoomID % 1000 + 4000;
		}
	}

	return 1;
}

int sendSelfChatRoom(char targetChatRoomString[GROUP_NAME_LENGTH], struct Server* serverPointer, normalPacket recPacket, mailbox* MboxPointer)
{
	int serverGroupID = 0;
	int targetChatRoomID = 0;
	serverGroupID = serverPointer->groupID;

	if(serverGroupID == atoi(SERVER1_PUB_GROUP))
	{
		targetChatRoomID = recPacket.currentChatRoomID % 1000;
	}
	else if(serverGroupID == atoi(SERVER2_PUB_GROUP))
	{
		targetChatRoomID = recPacket.currentChatRoomID % 1000 + 1000;
	}
	else if(serverGroupID == atoi(SERVER3_PUB_GROUP))
	{
		targetChatRoomID = recPacket.currentChatRoomID % 1000 + 2000;
	}
	else if(serverGroupID == atoi(SERVER4_PUB_GROUP))
	{
		targetChatRoomID = recPacket.currentChatRoomID % 1000 + 3000;
	}
	else if(serverGroupID == atoi(SERVER5_PUB_GROUP))
	{
		targetChatRoomID = recPacket.currentChatRoomID % 1000 + 4000;
	}
	sprintf(targetChatRoomString, "%d", targetChatRoomID);
 	/*
	ret = SP_multicast(*MboxPointer, AGREED_MESS, targetChatRoomString, 1, sizeof(normalPacket), (char*)(&recPacket));
	if(ret < 0)
	{
		SP_error(ret);
	}	
	*/
	return 1;
}

int replyUserList(struct Server* serverPointer, normalPacket recPacket, mailbox* MboxPointer, char sender[MAX_GROUP_NAME])
{
	/*First find the chatRoom*/
	struct ChatRoomNode* targetNode = serverPointer->chatRoomListHead;
	struct UserNode* cycleUserNode = NULL;
	normalPacket replyClientPacket;
	int ret;
	if(targetNode == NULL)
	{
		printf("in replyUserList, chat room list is empty\n");
		return 0;
	}
	/*find the chat room to list users*/
	while(targetNode != NULL)
	{
		printf("in replyUserList, targetNode->groupID = %d, recPacket.joinChatRoomID = %d\n", targetNode->groupID, recPacket.joinChatRoomID);
		if(targetNode->groupID == recPacket.joinChatRoomID)
		{
			break;
		}
		targetNode = targetNode->nextChatRoom;
	}

	if(targetNode == NULL)
	{
		printf("in replyUserList, does not find chatRoom->groupID by joinChatRoomID\n");

		return 0;
	}
	else
	{
		cycleUserNode = targetNode->userListHead;
		if(cycleUserNode == NULL)
		{
			printf("in replyUserList, user list is empty\n");
		}
		else
		{
		
			while(cycleUserNode != NULL)
			{
				if(cycleUserNode->active == 1)
				{
					memset(&replyClientPacket, 0, sizeof(normalPacket));
	
					replyClientPacket.command = 'u';
					memcpy(replyClientPacket.userName, cycleUserNode->userName, strlen(cycleUserNode->userName) + 1);
					memcpy(replyClientPacket.serverName, cycleUserNode->serverName, strlen(cycleUserNode->serverName)+1);
					ret = SP_multicast(*MboxPointer, AGREED_MESS, sender, 1, sizeof(normalPacket), (char*)(&replyClientPacket));
					if(ret < 0)
					{
						SP_error(ret);
					}
				}
				cycleUserNode = cycleUserNode->nextUser;
			}
			
		}
	}
	return 1;
}

int randomTimeString(char randomTimeBuffer[TIME_LENGTH])
{
	/*
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	strftime(randomTimeBuffer, sizeof(randomTimeBuffer), "%Y-%m-%d %H:%M", t);
	printf("currentDate: %s", randomTimeBuffer);
	*/
	char            fmt[64], buf[64];
    	struct timeval  tv;
    	struct tm       *tm;

	gettimeofday(&tv, NULL);
 	if((tm = localtime(&tv.tv_sec)) != NULL)
	{
	/*
            strftime(fmt, sizeof fmt, "%Y-%m-%d%H:%M:%S.%%06u%z", tm);
	*/
	    strftime(fmt, sizeof fmt, "%H%M%S%z", tm);
            snprintf(buf, sizeof buf, fmt, tv.tv_usec);
      /*
	    printf("'%s'\n", buf); 
	*/
    	}
	memcpy(randomTimeBuffer, buf, strlen(buf)+1);
	return 1;
}


int customizeChatRoomID(char inputGroupName[MAX_GROUP_NAME], char currentServer[HOST_NAME_LENGTH])
{
        /*for a chatRoom ID input by the user, calculate the final server it should be in*/
        int targetGroupID = 0;
        char targetGroupIDTemp[GROUP_NAME_LENGTH];
        if(strncmp(currentServer, SERVER1_PUB_GROUP, strlen(SERVER1_PUB_GROUP)) == 0)
        {
                targetGroupID = atoi(inputGroupName);
        }
        else if(strncmp(currentServer, SERVER2_PUB_GROUP, strlen(SERVER2_PUB_GROUP)) == 0)
        {
                targetGroupID = atoi(inputGroupName) + 1000;
        }
        else if(strncmp(currentServer, SERVER3_PUB_GROUP, strlen(SERVER3_PUB_GROUP)) == 0)
        {
                targetGroupID = atoi(inputGroupName) + 2000;
        }
        else if(strncmp(currentServer, SERVER4_PUB_GROUP, strlen(SERVER4_PUB_GROUP)) == 0)
        {
                targetGroupID = atoi(inputGroupName) + 3000;
        }
        else if(strncmp(currentServer, SERVER5_PUB_GROUP, strlen(SERVER5_PUB_GROUP)) == 0)
        {
                targetGroupID = atoi(inputGroupName) + 4000;
        }

        sprintf(targetGroupIDTemp, "%d", targetGroupID);
        memcpy(inputGroupName, targetGroupIDTemp, strlen(targetGroupIDTemp) + 1);

        return 1;
}

int recoverServerStates(char fileName[FILE_NAME_LENGTH], mailbox* MboxPointer, char Private_group[MAX_GROUP_NAME])
{
	FILE* fp;
	normalPacket testPacket;
	int ret;
	int counter = 0;
	fp = fopen(fileName, "r");

	while(fread(&testPacket, sizeof(normalPacket), 1, fp))
	{
		testPacket.recoverStates = 1;
		printf("send self recovery packets %d\n", counter);
		ret = SP_multicast(*MboxPointer, AGREED_MESS, Private_group, 1, sizeof(normalPacket), (char*)(&testPacket));
		if(ret < 0)
		{
			SP_error(ret);
		}
		counter++;
	}
	fclose(fp);
	return 1;
}

int writeMultiServerStates(normalPacket recPacket, char fileName[FILE_NAME_LENGTH])
{
	/*write a packet to the file*/
	FILE* fp;
	fp = fopen(fileName, "at");
	fwrite(&recPacket, sizeof(normalPacket), 1, fp);	
	fclose(fp);
	return 1;
}

void printAntiEntroMatrix(int antiEntroMatrix[SERVER_NUM][SERVER_NUM])
{
	int i;
	int j;
	printf("AntiEntroMatrix:\n");
	for(i = 0; i < SERVER_NUM; i++)
	{
		for(j = 0; j < SERVER_NUM; j++)
		{
			printf("%d ", antiEntroMatrix[i][j]);
		}
		printf("\n");
	}
}

int analyzeSavedVector(struct Server* serverPointer, int antiEntroMatrix[][SERVER_NUM], struct MultiServerStates* multiServer, mailbox* MboxPointer, char sender[MAX_GROUP_NAME])
{
	/*this function analyze the gathered anti entropy vectors and decide whether and which updates to send*/
	int serverIndex;
	int row;
	int collumn;
	int maxRowPos;
	int minRowPos;
	normalPacket temp;
	int ret = 0;
	int viewCycle = 0;
	int synMessageCycle;
	struct UpdateStateMessage* serverStatesCycle;
	int startRow;
	serverIndex = determineServerIndex(sender);	
	
	/*locate a non-zero position row to start with in each cycle*/
	for(row = 0; row < SERVER_NUM; row++)
	{
		if(antiEntroMatrix[row][0] != -1)
		{
			startRow = row;
			break;
		}
	}
/*
	printf("in analyzeSavedVector, startRow = %d, serverIndex = %d\n", startRow, serverIndex);
*/	
	for(collumn = 0; collumn < SERVER_NUM; collumn++)
	{
		maxRowPos = startRow;
		minRowPos = startRow;
		/*find lowest max and min position inside a collumn*/
		for(row = 0; row < SERVER_NUM; row++)
		{	
			if(antiEntroMatrix[row][collumn] > antiEntroMatrix[maxRowPos][collumn])
			{
				maxRowPos = row;
			}
			if(antiEntroMatrix[row][collumn] < antiEntroMatrix[minRowPos][collumn] && antiEntroMatrix[row][collumn] != -1)	
			{
				minRowPos = row;
			}	
		}
		/*
		printf("in analyzeSavedVector, maxRowPos = %d, minRowPos = %d\n", maxRowPos, minRowPos);		
		*/
		if(antiEntroMatrix[minRowPos][collumn] == antiEntroMatrix[maxRowPos][collumn])
		{
			continue;
		}	
	
		/*if max position equals serverIndex, this server should send packets between antiEntroMatrix[row][minRowPos] to antiEntroMatrix[row][maxRowPos]*/
		if(serverIndex == maxRowPos)
		{
			printf("server %d is responsible for sending updates for #%d\n", serverIndex, collumn);
			printf("	packet from %d to %d\n", antiEntroMatrix[minRowPos][collumn]+1, antiEntroMatrix[maxRowPos][collumn]);
			
			serverStatesCycle = multiServer->serverStates[collumn];
			synMessageCycle = 1;

			/*loop until the packet to be synced among multiple servers*/
			while(synMessageCycle <  antiEntroMatrix[minRowPos][collumn] + 1 && serverStatesCycle != NULL)
			{
				serverStatesCycle = serverStatesCycle->nextUpdate;
				printf("updateStateMessage.packetLamport.globalCounter = %d\n", serverStatesCycle->updateStateMessage.packetLamport.globalCounter);
				synMessageCycle++;
			}			
			if(serverStatesCycle == NULL)
			{
				printf("in analyzeSavedVector, server does not have enough message to send\n");
				return 0;
			}
			/*send packets it should*/
			for(synMessageCycle = antiEntroMatrix[minRowPos][collumn] + 1; synMessageCycle <= antiEntroMatrix[maxRowPos][collumn]; synMessageCycle++)
			{
				/**/
				printf("in analyzeSavedVector, enter send responsible packets\n");	
				memset(&temp, 0, sizeof(normalPacket));
				memcpy(&temp, &(serverStatesCycle->updateStateMessage), sizeof(normalPacket));
				for(viewCycle = 0; viewCycle < SERVER_NUM; viewCycle++)
				{
				/*
					printf("serverPointer->serverView[%d]=%d\n", viewCycle, serverPointer->serverView[viewCycle]);
				*/
					if(serverPointer->previousServerView[viewCycle] == 0)
					{
						if(viewCycle == 0)
						{
							ret = SP_multicast(*MboxPointer, AGREED_MESS, SERVER1_MEMBERNAME, 1, sizeof(normalPacket), (char*)(&temp));	
						}
						else if(viewCycle == 1)
						{
							ret = SP_multicast(*MboxPointer, AGREED_MESS, SERVER2_MEMBERNAME, 1, sizeof(normalPacket), (char*)(&temp));	
						}
						else if(viewCycle == 2)
						{
							ret = SP_multicast(*MboxPointer, AGREED_MESS, SERVER3_MEMBERNAME, 1, sizeof(normalPacket), (char*)(&temp));	
						}
						else if(viewCycle == 3)
						{
							ret = SP_multicast(*MboxPointer, AGREED_MESS, SERVER4_MEMBERNAME, 1, sizeof(normalPacket), (char*)(&temp));	
						}
						else if(viewCycle == 4)
						{
							ret = SP_multicast(*MboxPointer, AGREED_MESS, SERVER5_MEMBERNAME, 1, sizeof(normalPacket), (char*)(&temp));	
						}
				/*	
						printf("serverView = %d\n", viewCycle);
				*/	
						if(ret < 0)
						{
							SP_error(ret);
						}
					}
				}
				serverStatesCycle = serverStatesCycle->nextUpdate;
			}
		}
		
	}	
	
	return 1;	
}

int saveAntiEntroVector(int antiEntroMatrix[][SERVER_NUM], normalPacket* recPacket, char sender[MAX_GROUP_NAME])
{
	int serverIndex = 0;
	int counter = 0;
	serverIndex = determineServerIndex(sender);
	for(counter = 0; counter < SERVER_NUM; counter++)
	{
		antiEntroMatrix[serverIndex][counter] = recPacket->antiEntroVector[counter];
	}

	return 1;
}

int sendAntiEntroVector(struct MultiServerStates* multiServer, mailbox* MboxPointer)
{
	normalPacket temp;
	int counter = 0;
	int ret = 0;
	memset(&temp, 0, sizeof(normalPacket));
	temp.command = 'p';
	for(counter = 0; counter < SERVER_NUM; counter++)
	{
		temp.antiEntroVector[counter] = multiServer->antiEntroVector[counter];
	}
	
	ret = SP_multicast(*MboxPointer, AGREED_MESS, SERVER_GROUPS, 1, sizeof(normalPacket), (char*)(&temp));
	if(ret < 0)
	{
		SP_error(ret);
	}
	return 1;
}

int determineServerIndex(char sender[MAX_GROUP_NAME])
{
	int serverIndex;
	if(strncmp(sender, SERVER1_MEMBERNAME, strlen(SERVER1_MEMBERNAME)) == 0)
	{
		serverIndex = 0;
	}
	else if(strncmp(sender, SERVER2_MEMBERNAME, strlen(SERVER2_MEMBERNAME)) == 0)
	{
		serverIndex = 1;
	}
	else if(strncmp(sender, SERVER3_MEMBERNAME, strlen(SERVER3_MEMBERNAME)) == 0)
	{
		serverIndex = 2;
	}
	else if(strncmp(sender, SERVER4_MEMBERNAME, strlen(SERVER4_MEMBERNAME)) == 0)
	{
		serverIndex = 3;
	}
	else if(strncmp(sender, SERVER5_MEMBERNAME, strlen(SERVER5_MEMBERNAME)) == 0)
	{
		serverIndex = 4;
	}
	else
	{
		printf("serverIndex = -1\n");
		serverIndex = -1;
	}
	return serverIndex;
}

void printMultiServerStates(struct MultiServerStates* multiServer)
{
	if(multiServer == NULL)
	{
		printf("in printMultiServerStates, multiServer pointer = NULL\n");
	}
	struct UpdateStateMessage* cycleStateMessage = NULL;
	int counter = 0;
	printf("---------multi server states---------------\n");
	for(counter = 0; counter < SERVER_NUM; counter++)
	{
		cycleStateMessage = multiServer->serverStates[counter];
		printf("Message from server %d, total message = %d:\n", counter, multiServer->antiEntroVector[counter]);
		
		while(cycleStateMessage != NULL)
		{
			printf("\tlamport global = %d, server index = %d, command = %c\n", ((cycleStateMessage->updateStateMessage).packetLamport.globalCounter), ((cycleStateMessage->updateStateMessage).packetLamport).serverIndex,  ((cycleStateMessage->updateStateMessage).command));
			if((cycleStateMessage->updateStateMessage).command == 'c')
			{
				printf("\t\tuserName = %s, serverName = %s\n", cycleStateMessage->updateStateMessage.userName, cycleStateMessage->updateStateMessage.serverName);		              	      }
			else if((cycleStateMessage->updateStateMessage).command == 'u')
			{
				printf("\t\tswitchUserName = %s, userName = %s\n", cycleStateMessage->updateStateMessage.switchUserName, cycleStateMessage->updateStateMessage.userName);
	
			}
			else if( (cycleStateMessage->updateStateMessage).command == 'j')
			{
				printf("\t\tuserName = %s, joinChatRoomID = %d\n", cycleStateMessage->updateStateMessage.userName, cycleStateMessage->updateStateMessage.joinChatRoomID);
			}
			else if((cycleStateMessage->updateStateMessage).command == 'a')
			{
				printf("\t\tuserName = %s, messagePayLoad = %s\n", cycleStateMessage->updateStateMessage.userName, cycleStateMessage->updateStateMessage.messagePayLoad);
			}
			else if((cycleStateMessage->updateStateMessage).command == 'l')
			{
				printf("\t\tuserName = %s, likeLineNum = %d\n", cycleStateMessage->updateStateMessage.userName, cycleStateMessage->updateStateMessage.likeLineNum);
			}
			else if((cycleStateMessage->updateStateMessage).command == 'r')
			{
				printf("\t\tuserName = %s, likeLineNum = %d\n", cycleStateMessage->updateStateMessage.userName, cycleStateMessage->updateStateMessage.likeLineNum);
			}
			cycleStateMessage = cycleStateMessage->nextUpdate;			
		}
	}
}

int addLamportStamp(normalPacket* recPacketPointer, int* globalCounter, int serverIndex)
{
	if(recPacketPointer->packetLamport.globalCounter == 0 && recPacketPointer->packetLamport.serverIndex == 0)
	{
		recPacketPointer->packetLamport.globalCounter = *globalCounter;
		recPacketPointer->packetLamport.serverIndex = serverIndex % 100000;
		*globalCounter = *globalCounter + 1;
	}
	return 1;
}

int syncServers(normalPacket recPacket, mailbox* MboxPointer, char destGroup[MAX_GROUP_NAME])
{
	int ret;
	ret = SP_multicast(*MboxPointer, AGREED_MESS, destGroup, 1, sizeof(normalPacket), (char*)(&recPacket));
	if(ret < 0)
	{
		SP_error(ret);
	}
	return 1;	
}

int comeFromSelf(char sender[MAX_GROUP_NAME], char Private_group[MAX_GROUP_NAME])
{
	/*judge whether the packet comes from itself*/
	if(strncmp(sender, Private_group, strlen(Private_group)) == 0)
	{
		return 1;
	}
	return 0;
}

int comeFromServer(char sender[MAX_GROUP_NAME])
{
	/*judge whether the packet comes from servers*/
	if((strncmp(sender, SERVER1_MEMBERNAME, strlen(SERVER1_MEMBERNAME)) == 0
	|| strncmp(sender, SERVER2_MEMBERNAME, strlen(SERVER2_MEMBERNAME)) == 0
	|| strncmp(sender, SERVER3_MEMBERNAME, strlen(SERVER3_MEMBERNAME)) == 0
	|| strncmp(sender, SERVER4_MEMBERNAME, strlen(SERVER4_MEMBERNAME)) == 0
	|| strncmp(sender, SERVER5_MEMBERNAME, strlen(SERVER5_MEMBERNAME)) == 0))
	{
		return 1;
	}
	return 0;
}

int updateMultiServerStates(struct MultiServerStates* multiServerPointer, normalPacket recPacket)
{
	struct UpdateStateMessage* temp = NULL;
	struct UpdateStateMessage* listTail = NULL;
	int serverIndex;
	temp = (struct UpdateStateMessage*) malloc(sizeof(struct UpdateStateMessage));
	if(!temp)
	{
		printf("malloc fail\n");
		exit(0);
	}
	memcpy(&(temp->updateStateMessage), &recPacket, sizeof(normalPacket));
	temp->nextUpdate = NULL;
	
	/*packet comes from which server*/
	serverIndex = recPacket.packetLamport.serverIndex;

	if(multiServerPointer == NULL)
	{
		printf("in updateMultiServerStates, multiServerPointer == NULL\n");
		return 0;
	}	
	/*find list tail of the list*/
	listTail = multiServerPointer->serverStates[serverIndex - 1];
	if(listTail == NULL)
	{
		(multiServerPointer->serverStates[serverIndex-1]) = temp;
		printf("Add to head, serverIndex = %d\n", serverIndex-1);
	}
	else
	{
		while(listTail->nextUpdate != NULL)
		{
			listTail = listTail->nextUpdate;
		}
		listTail->nextUpdate = temp;
		printf("Add to tail\n");
	}
	multiServerPointer->antiEntroVector[serverIndex - 1] = multiServerPointer->antiEntroVector[serverIndex - 1] + 1;	
	
	return 1;
}

int serverDislikeMessage(struct Server* serverPointer, char* userName, int currentChatRoomID, struct Lamport dislikeMessageLamport)
{
	struct ChatRoomNode* targetNode = serverPointer->chatRoomListHead;
	struct MessageNode* startMessageNode = NULL;
	struct LikeNode* likeListHead = NULL;
	struct LikeNode* likeListTail = NULL;
	struct LikeNode* previousLikeNode = NULL;
	struct LikeNode* currentLikeNode = NULL;

	if(targetNode == NULL)
	{
		printf("in dislikeMessage, chat room list is empty\n");
		return 0;
	}
	/*find the chat room to add message*/
	while(targetNode != NULL)
	{
		if(targetNode->groupID == currentChatRoomID)
		{
			break;
		}
		targetNode = targetNode->nextChatRoom;
	}
	if(targetNode == NULL)
	{
		printf("in dislikeMessage, does not find currentChatRoomID\n");
		return 0;
	}

	/*find the starting message node to send back to and display on client side*/
	startMessageNode = targetNode->messageListHead;
	if(startMessageNode == NULL)
	{
		printf("Message list is empty\n");
		return 0;
	}	
	while(compareLamport(startMessageNode->messageLamport, dislikeMessageLamport) != -1 && startMessageNode != NULL)
	{
		startMessageNode = startMessageNode->nextMessage;
	}

	/*remove LikeNode associated with that user*/
	likeListHead = startMessageNode->likeListHead;
	likeListTail = startMessageNode->likeListHead;
	if(likeListHead == NULL)
	{
		printf("dislikeMessage: like list is empty\n");
		return 0;
	}
	while(likeListTail->nextLikeNode != NULL)	
	{
		likeListTail = likeListTail->nextLikeNode;
	}
	
	currentLikeNode = likeListHead;
	while(currentLikeNode != NULL)
	{
		if(strncmp(userName, currentLikeNode->userName, strlen(userName)) == 0)
		{
			break;
		}
		currentLikeNode = currentLikeNode->nextLikeNode;
	}	
	
	if(currentLikeNode == NULL)
	{
		printf("could not find likeNode associated with userName\n");
		return 0;
	}

	/*four scenarios*/
	/*first case:*/
	if(currentLikeNode == likeListHead && currentLikeNode == likeListTail)
	{
		free(currentLikeNode);
		startMessageNode->likeListHead = NULL;
	}
	else if(currentLikeNode == likeListHead && currentLikeNode != likeListTail)
	{
		startMessageNode->likeListHead = currentLikeNode->nextLikeNode;
		free(currentLikeNode);
	}
	else if(currentLikeNode != likeListHead && currentLikeNode == likeListTail)
	{
		previousLikeNode = likeListHead;
		while(previousLikeNode != NULL && previousLikeNode->nextLikeNode != currentLikeNode)
		{
			previousLikeNode = previousLikeNode->nextLikeNode;
		}
		free(currentLikeNode);
		if(previousLikeNode != NULL)
		{
			previousLikeNode->nextLikeNode = NULL;
		}
	}
	else if(currentLikeNode != likeListHead && currentLikeNode != likeListTail)
	{
		previousLikeNode = likeListHead;
		while(previousLikeNode != NULL && previousLikeNode->nextLikeNode != currentLikeNode)
		{
			previousLikeNode = previousLikeNode->nextLikeNode;
		}
		previousLikeNode->nextLikeNode = currentLikeNode->nextLikeNode;
		free(currentLikeNode);
	}

	startMessageNode->numLike = startMessageNode->numLike - 1;
	return 1;
}


int dislikeMessage(struct Server* serverPointer, char* userName, int currentChatRoomID, int lineNum, struct Lamport* lamportPointer)
{
	struct ChatRoomNode* targetNode = serverPointer->chatRoomListHead;
	struct MessageNode* startMessageNode = NULL;
	struct LikeNode* likeListHead = NULL;
	struct LikeNode* likeListTail = NULL;
	struct LikeNode* previousLikeNode = NULL;
	struct LikeNode* currentLikeNode = NULL;
	int messageCounter = 0;

	if(targetNode == NULL)
	{
		printf("in dislikeMessage, chat room list is empty\n");
		return 0;
	}
	/*find the chat room to add message*/
	while(targetNode != NULL)
	{
		if(targetNode->groupID == currentChatRoomID)
		{
			break;
		}
		targetNode = targetNode->nextChatRoom;
	}
	if(targetNode == NULL)
	{
		printf("in dislikeMessage, does not find currentChatRoomID\n");
		return 0;
	}

	/*find the starting message node to send back to and display on client side*/
	startMessageNode = targetNode->messageListTail;
	if(startMessageNode == NULL)
	{
		return 0;
	}
	messageCounter = 1;
	while(startMessageNode->previousMessage != NULL && messageCounter < DISPLAY_LINE)
	{
		messageCounter = messageCounter + 1;
		startMessageNode = startMessageNode->previousMessage;
	}

	/*find the MessageNode associated with lineNum*/
	messageCounter = 1;
	while(messageCounter < lineNum && startMessageNode != NULL)
	{
		startMessageNode = startMessageNode->nextMessage;
		messageCounter = messageCounter + 1;
	}		
	if(messageCounter < lineNum && startMessageNode == NULL)
	{
		printf("likeMessage: lineNum is too big\n");
		return 0;
	}

	/*customize the dislike packet*/
	memcpy(lamportPointer, &(startMessageNode->messageLamport), sizeof(struct Lamport));

	/*remove LikeNode associated with that user*/
	likeListHead = startMessageNode->likeListHead;
	likeListTail = startMessageNode->likeListHead;
	if(likeListHead == NULL)
	{
		printf("dislikeMessage: like list is empty\n");
		return 0;
	}
	while(likeListTail->nextLikeNode != NULL)	
	{
		likeListTail = likeListTail->nextLikeNode;
	}
	
	currentLikeNode = likeListHead;
	while(currentLikeNode != NULL)
	{
		if(strncmp(userName, currentLikeNode->userName, strlen(userName)) == 0)
		{
			break;
		}
		currentLikeNode = currentLikeNode->nextLikeNode;
	}	
	
	if(currentLikeNode == NULL)
	{
		printf("could not find likeNode associated with userName\n");
		return 0;
	}

	/*four scenarios*/
	/*first case:*/
	if(currentLikeNode == likeListHead && currentLikeNode == likeListTail)
	{
		free(currentLikeNode);
		startMessageNode->likeListHead = NULL;
	}
	else if(currentLikeNode == likeListHead && currentLikeNode != likeListTail)
	{
		startMessageNode->likeListHead = currentLikeNode->nextLikeNode;
		free(currentLikeNode);
	}
	else if(currentLikeNode != likeListHead && currentLikeNode == likeListTail)
	{
		previousLikeNode = likeListHead;
		while(previousLikeNode != NULL && previousLikeNode->nextLikeNode != currentLikeNode)
		{
			previousLikeNode = previousLikeNode->nextLikeNode;
		}
		free(currentLikeNode);
		if(previousLikeNode != NULL)
		{
			previousLikeNode->nextLikeNode = NULL;
		}
	}
	else if(currentLikeNode != likeListHead && currentLikeNode != likeListTail)
	{
		previousLikeNode = likeListHead;
		while(previousLikeNode != NULL && previousLikeNode->nextLikeNode != currentLikeNode)
		{
			previousLikeNode = previousLikeNode->nextLikeNode;
		}
		previousLikeNode->nextLikeNode = currentLikeNode->nextLikeNode;
		free(currentLikeNode);
	}

	startMessageNode->numLike = startMessageNode->numLike - 1;
	return 1;
}

int serverLikeMessage(struct Server* serverPointer, char* userName, int currentChatRoomID, struct Lamport likedMessageLamport)
{
	struct ChatRoomNode* targetNode = serverPointer->chatRoomListHead;
	struct MessageNode* startMessageNode = NULL;
	struct LikeNode* temp = NULL;
	struct LikeNode* likeListTail = NULL;

	if(targetNode == NULL)
	{
		printf("in serverlikeMessage, chat room list is empty\n");
		return 0;
	}
	/*find the chat room to add message*/
	while(targetNode != NULL)
	{
		if(targetNode->groupID == currentChatRoomID)
		{
			break;
		}
		targetNode = targetNode->nextChatRoom;
	}
	if(targetNode == NULL)
	{
		printf("in serverlikeMessage, does not find currentChatRoomID\n");
		return 0;
	}

	/*find the starting message node to send back to and display on client side*/
	startMessageNode = targetNode->messageListHead;
	if(startMessageNode == NULL)
	{
		printf("in serverLikeMessage, messageList is empty\n");
		return 0;
	}

	/*find the MessageNode associated with lamport stamp*/
	while(compareLamport(startMessageNode->messageLamport, likedMessageLamport) != -1 && startMessageNode != NULL)
	{
		startMessageNode = startMessageNode->nextMessage;
	}
	if( startMessageNode == NULL)
	{
		printf("serverlikeMessage: lineNum is too big\n");
		return 0;
	}

	/*check whether userName is already in like list, if not, add it in*/
	temp = (struct LikeNode*) malloc(sizeof(struct LikeNode));
	if(!temp)
	{
		printf("malloc fail\n");
		return 0;
	}
	memcpy(temp->userName, userName, strlen(userName)+1 );
	memcpy(&(temp->likedMessageLamport), &(startMessageNode->messageLamport), sizeof(struct Lamport));
	temp->nextLikeNode = NULL;

	/*append new created likeNode*/
	if(startMessageNode->likeListHead == NULL)
	{
		startMessageNode->likeListHead = temp;
	}
	else
	{
		likeListTail = startMessageNode->likeListHead;
		while(likeListTail->nextLikeNode != NULL)
		{
			if(strncmp(likeListTail->userName, userName, strlen(userName)) == 0)
			{
				printf("user already liked this message\n");
				free(temp);
				return 0;
			}
			likeListTail = likeListTail->nextLikeNode;
		}
		/*for a special case where the above while loop is not executed*/
		if(strncmp(likeListTail->userName, userName, strlen(userName)) == 0)
		{
			printf("user already liked this message\n");
			free(temp);
			return 0;
		}

		likeListTail->nextLikeNode = temp;
	}
	startMessageNode->numLike = startMessageNode->numLike + 1;
	return 1;
}

int likeMessage(struct Server* serverPointer, char* userName, int currentChatRoomID, int lineNum, struct Lamport* lamportPointer)
{
	struct ChatRoomNode* targetNode = serverPointer->chatRoomListHead;
	struct MessageNode* startMessageNode = NULL;
	struct LikeNode* temp = NULL;
	struct LikeNode* likeListTail = NULL;
	int messageCounter = 0;

	if(targetNode == NULL)
	{
		printf("in likeMessage, chat room list is empty\n");
		return 0;
	}
	/*find the chat room to add message*/
	while(targetNode != NULL)
	{
		if(targetNode->groupID == currentChatRoomID)
		{
			break;
		}
		targetNode = targetNode->nextChatRoom;
	}
	if(targetNode == NULL)
	{
		printf("in likeMessage, does not find currentChatRoomID\n");
		return 0;
	}

	/*find the starting message node to send back to and display on client side*/
	startMessageNode = targetNode->messageListTail;
	if(startMessageNode == NULL)
	{
		return 0;
	}
	messageCounter = 1;
	while(startMessageNode->previousMessage != NULL && messageCounter < DISPLAY_LINE)
	{
		messageCounter = messageCounter + 1;
		startMessageNode = startMessageNode->previousMessage;
	}

	/*find the MessageNode associated with lineNum*/
	messageCounter = 1;
	while(messageCounter < lineNum && startMessageNode != NULL)
	{
		startMessageNode = startMessageNode->nextMessage;
		messageCounter = messageCounter + 1;
	}		
	if(messageCounter < lineNum && startMessageNode == NULL)
	{
		printf("likeMessage: lineNum is too big\n");
		return 0;
	}

	/*customize the like packet*/
	memcpy(lamportPointer, &(startMessageNode->messageLamport), sizeof(struct Lamport));	
	
	/*check whether userName is already in like list, if not, add it in*/
	temp = (struct LikeNode*) malloc(sizeof(struct LikeNode));
	if(!temp)
	{
		printf("malloc fail\n");
		return 0;
	}

	memcpy(temp->userName, userName, strlen(userName)+1 );
	memcpy(&(temp->likedMessageLamport), lamportPointer, sizeof(struct Lamport));
	temp->nextLikeNode = NULL;

	/*append new created likeNode*/	
	if(startMessageNode->likeListHead == NULL)
	{
		startMessageNode->likeListHead = temp;
	}
	else
	{
		likeListTail = startMessageNode->likeListHead;
		while(likeListTail->nextLikeNode != NULL)
		{
			if(strncmp(likeListTail->userName, userName, strlen(userName)) == 0)
			{
				printf("user already liked this message\n");
				free(temp);
				return 0;
			}
			likeListTail = likeListTail->nextLikeNode;
		}
		/*for a special case where the above while loop is not executed*/
		if(strncmp(likeListTail->userName, userName, strlen(userName)) == 0)
		{
			printf("user already liked this message\n");
			free(temp);
			return 0;
		}
	
		likeListTail->nextLikeNode = temp;	
	}
	startMessageNode->numLike = startMessageNode->numLike + 1;
	return 1;
}

int replyServerView(struct Server* serverPointer, mailbox* MboxPointer, char sender[MAX_GROUP_NAME])
{
	normalPacket replyClientPacket;
	int counter = 0;
	int ret = 0;
	replyClientPacket.command = 'v';
	for(counter = 0; counter < SERVER_NUM; counter++)
	{
		printf("replyServerView: serverView[%d]=%d\n", counter, serverPointer->serverView[counter]);
		replyClientPacket.serverView[counter] = serverPointer->serverView[counter];
	}

	ret = SP_multicast(*MboxPointer, AGREED_MESS, sender, 1, sizeof(normalPacket), (char*)(&replyClientPacket) );
	if(ret < 0)
	{
		SP_error(ret);
	}	

	return 1;			
}

int replyEntireMessageList(struct Server* serverPointer, normalPacket recPacket, mailbox* MboxPointer, char sender[MAX_GROUP_NAME])
{
	/* multicast a list of messages in the specified chatRoom
 	* */
	normalPacket replyClientPacket;
	char sourceGroup[MAX_GROUP_NAME];
	int ret;
	int messageCounter = 0;
	struct ChatRoomNode* targetNode = serverPointer->chatRoomListHead;
	struct MessageNode* startMessageNode = NULL;

	if(targetNode == NULL)
	{
		printf("in replyEntireMessageList, chat room list is empty\n");
		return 0;
	}
	/*find the chat room to add message*/
	while(targetNode != NULL)
	{
		if(targetNode->groupID == recPacket.currentChatRoomID)
		{
			break;
		}
		targetNode = targetNode->nextChatRoom;
	}
	if(targetNode == NULL)
	{
		printf("in replyEntireMessageList, does not find chatroom->groupID by currentChatRoomID\n");
		targetNode = serverPointer->chatRoomListHead;
		while(targetNode != NULL)
		{
			if(targetNode->groupID == recPacket.joinChatRoomID)
			{
				break;
			}
			targetNode = targetNode->nextChatRoom;
		}
		if(targetNode == NULL)
		{
			printf("in replyEntireMessageList, does not find chatroom->groupID by joinChatRoomID\n");
			return 0;
		}
	}
	
	/*find the starting message node to send back to and display on client side*/
	startMessageNode = targetNode->messageListHead;
	if(startMessageNode == NULL)
	{
		printf("in replyEntireMessageList, message list is empty\n");
		return 0;
	}
	
	/*multicast message packet*/
	messageCounter = 1;
	while(startMessageNode != NULL)
	{
		/*create the packet*/
		memset(&replyClientPacket, 0, sizeof(normalPacket));
		if(recPacket.currentChatRoomID != 0)
		{
			sprintf(sourceGroup, "%d", recPacket.currentChatRoomID);
		}
		else
		{
			sprintf(sourceGroup, "%d", recPacket.joinChatRoomID);
		}
		replyClientPacket.command = recPacket.command;
		replyClientPacket.replyLineNum = messageCounter;
		replyClientPacket.likeness = startMessageNode->numLike;
		memcpy(replyClientPacket.messagePayLoad, startMessageNode->messagePayLoad, strlen(startMessageNode->messagePayLoad)+1);
		memcpy(replyClientPacket.userName, startMessageNode->userName, strlen(startMessageNode->userName)+1);
/*
		printf("multicast packet to %s, userName = %s, messagePayLoad = %s\n", sourceGroup, replyClientPacket.userName, replyClientPacket.messagePayLoad);
*/			
		ret = SP_multicast(*MboxPointer, AGREED_MESS, sender, 1, sizeof(normalPacket), (char*)(&replyClientPacket) );
		if(ret < 0)
		{
			SP_error(ret);
		}
		startMessageNode = startMessageNode->nextMessage;
		messageCounter = messageCounter + 1;
	}
	return 1;
}




int replyMessageList(struct Server* serverPointer, normalPacket recPacket, mailbox* MboxPointer, char sender[MAX_GROUP_NAME], int private)
{
	/* multicast a list of messages in the specified chatRoom
 	* */
	normalPacket replyClientPacket;
	char sourceGroup[MAX_GROUP_NAME];
	int ret;
	int messageCounter = 0;
	struct ChatRoomNode* targetNode = serverPointer->chatRoomListHead;
	struct MessageNode* startMessageNode = NULL;

	if(targetNode == NULL)
	{
		printf("in replyMessageList, chat room list is empty\n");
		return 0;
	}
	/*find the chat room to add message*/
	while(targetNode != NULL)
	{
		printf("in ReplyMessageList, targetNode->groupID = %d, recPacket.currentChatRoomID = %d\n", targetNode->groupID, recPacket.currentChatRoomID);
		if(targetNode->groupID == recPacket.currentChatRoomID)
		{
			break;
		}
		targetNode = targetNode->nextChatRoom;
	}
	if(targetNode == NULL)
	{
		printf("in replyMessageList, does not find chatroom->groupID by currentChatRoomID\n");
		targetNode = serverPointer->chatRoomListHead;
		while(targetNode != NULL)
		{
			printf("in replyMessageList, targetNode->groupID = %d, recPacket.joinChatRoomID = %d\n", targetNode->groupID, recPacket.joinChatRoomID);
			if(targetNode->groupID == recPacket.joinChatRoomID)
			{
				break;
			}
			targetNode = targetNode->nextChatRoom;
		}
		if(targetNode == NULL)
		{
			printf("in replyMessageList, does not find chatroom->groupID by joinChatRoomID\n");
			return 0;
		}
	}

	
	/*find the starting message node to send back to and display on client side*/
	startMessageNode = targetNode->messageListTail;
	if(startMessageNode == NULL)
	{
		printf("in replyMessageList, message list is empty\n");
		return 0;
	}
	messageCounter = 1;
	while(startMessageNode->previousMessage != NULL && messageCounter <= DISPLAY_LINE)
	{
		messageCounter = messageCounter + 1;
		startMessageNode = startMessageNode->previousMessage;
	}
	
	/*multicast message packet*/
	messageCounter = 1;
	while(startMessageNode != NULL)
	{
		/*create the packet*/
		memset(&replyClientPacket, 0, sizeof(normalPacket));
		if(recPacket.currentChatRoomID != 0)
		{
			sprintf(sourceGroup, "%d", recPacket.currentChatRoomID);
		}
		else
		{
			sprintf(sourceGroup, "%d", recPacket.joinChatRoomID);
		}
		replyClientPacket.command = recPacket.command;
		replyClientPacket.replyLineNum = messageCounter;
		replyClientPacket.likeness = startMessageNode->numLike;
		memcpy(replyClientPacket.messagePayLoad, startMessageNode->messagePayLoad, strlen(startMessageNode->messagePayLoad)+1);
		memcpy(replyClientPacket.userName, startMessageNode->userName, strlen(startMessageNode->userName)+1);
/*
		printf("multicast packet to %s, userName = %s, messagePayLoad = %s\n", sourceGroup, replyClientPacket.userName, replyClientPacket.messagePayLoad);
*/		if(private == 1)
		{	
			ret = SP_multicast(*MboxPointer, AGREED_MESS, sourceGroup, 1, sizeof(normalPacket), (char*)(&replyClientPacket) );
		}
		else
		{
			ret = SP_multicast(*MboxPointer, AGREED_MESS, sender, 1, sizeof(normalPacket), (char*)(&replyClientPacket) );
		}
		if(ret < 0)
		{
			SP_error(ret);
		}
		startMessageNode = startMessageNode->nextMessage;
		messageCounter = messageCounter + 1;
	}
	return 1;
}

int addMessage(struct Server* serverPointer, char userName[USER_NAME_LENGTH], int currentChatRoomID, char messagePayLoad[MESSAGE_LENGTH], struct Lamport packetLamport)
{
	struct ChatRoomNode* targetNode = serverPointer->chatRoomListHead;
	struct MessageNode* temp = NULL;
	struct MessageNode* cycleMessagePos = NULL;
	struct MessageNode* previousMessagePos = NULL;
	struct MessageNode* nextMessagePos = NULL;
	if(targetNode == NULL)
	{
		printf("in addMessage, chat room list is empty\n");
		return 0;
	}
	/*find the chat room to add message*/
	while(targetNode != NULL)
	{
		if(targetNode->groupID == currentChatRoomID)
		{
			break;
		}
		targetNode = targetNode->nextChatRoom;
	}
	if(targetNode == NULL)
	{
		printf("in addMessage, does not find currentChatRoomID\n");
		return 0;
	}

	/*create a new message node*/
	temp = (struct MessageNode*) malloc(sizeof(struct MessageNode));
	temp->messageID = targetNode->numMessage + 1;
	memcpy(temp->messagePayLoad, messagePayLoad, strlen(messagePayLoad) + 1);
	memcpy(temp->userName, userName, strlen(userName) + 1);
	memcpy(&(temp->messageLamport), &packetLamport, sizeof(struct Lamport) );
	temp->numLike = 0;
	temp->nextMessage = NULL;
	temp->previousMessage = NULL;
	temp->likeListHead = NULL;

	printf("addMessage, temp->userName = %s\n", temp->userName);
	printf("addMessage, temp->messagePayLoad = %s\n", temp->messagePayLoad);
	printf("addMessage, temp->messageLamport.globalCounter = %d, temp->messageLamport.serverIndex=%d\n", temp->messageLamport.globalCounter, temp->messageLamport.serverIndex);	

	if(!temp)
	{
		printf("malloc fail\n");
		exit(0);
	}

	if(targetNode->messageListHead != NULL)
	{
           printf("messageListHead timeStamp globalCounter = %d, serverIndex = %d\n", (targetNode->messageListHead->messageLamport).globalCounter, (targetNode->messageListHead->messageLamport).serverIndex);	
           printf("messageListTail timeStamp globalCounter = %d, serverIndex = %d\n", (targetNode->messageListTail->messageLamport).globalCounter, (targetNode->messageListTail->messageLamport).serverIndex);
	}
	/*append the new created node to message list*/
	/*there are four scenarios here*/

	/*first scenario: list is empty*/
	if(targetNode->messageListHead == NULL)
	{
		printf("addMessage, first case\n");
		targetNode->messageListHead = temp;
		targetNode->messageListTail = temp;
	}
	/*second scenario: list not empty, insert in the head*/
	else if(targetNode->messageListHead != NULL && compareLamport((targetNode->messageListHead)->messageLamport, temp->messageLamport) == 0)
	{
		printf("addMessage, second case\n");
		/*
		cycleMessagePos = targetNode->messageListHead;		
		
		targetNode->messageListTail->nextMessage = temp;
		targetNode->messageListTail = temp;
		*/
		temp->nextMessage = targetNode->messageListHead;
		targetNode->messageListHead->previousMessage = temp;
	
		targetNode->messageListHead = temp;
	}
	/*third scenario: list not empty, insert in the tail*/
	else if(targetNode->messageListTail != NULL && compareLamport((targetNode->messageListTail)->messageLamport, temp->messageLamport) == 1)
	{
		printf("addMessage, third case\n");
		targetNode->messageListTail->nextMessage = temp;
		temp->previousMessage = targetNode->messageListTail;

		targetNode->messageListTail = temp;
	}
	/*fourth scenario: insert in the middle*/
	else if(targetNode->messageListHead != NULL
		 &&  compareLamport((targetNode->messageListHead)->messageLamport, temp->messageLamport) == 1
		 &&  compareLamport((targetNode->messageListTail)->messageLamport, temp->messageLamport) == 0)	
	{
		printf("addMessage, fourth case\n");
		cycleMessagePos = targetNode->messageListHead;
		previousMessagePos = targetNode->messageListHead;
		while(compareLamport(cycleMessagePos->messageLamport, temp->messageLamport) == 1)
		{
			previousMessagePos = cycleMessagePos;
			cycleMessagePos = cycleMessagePos->nextMessage;
			if(cycleMessagePos == NULL)
			{
				printf("in addMessage, something wrong happens\n");
				return 0;
			}
		}
		
		if(compareLamport(cycleMessagePos->messageLamport, temp->messageLamport) == -1)
		{	
			printf("in addMessage, message already exists");
			return 0;
		}
		temp->nextMessage = previousMessagePos->nextMessage;
		temp->previousMessage = previousMessagePos;

		nextMessagePos = previousMessagePos->nextMessage;
		previousMessagePos->nextMessage = temp;
		nextMessagePos->previousMessage = temp;
	}
	else
	{
		printf("message already exist in message list\n");
		return 0;
	}

	targetNode->numMessage = targetNode->numMessage + 1;
	return 1;
}

int compareLamport(struct Lamport destLamport, struct Lamport sourceLamport)
{
	if(destLamport.globalCounter < sourceLamport.globalCounter)
	{
		return 1;
	}
	else if(destLamport.globalCounter > sourceLamport.globalCounter)
	{
		return 0;
	}
	else if(destLamport.globalCounter == sourceLamport.globalCounter)
	{
		if(destLamport.serverIndex < sourceLamport.serverIndex)
		{
			return 1;
		}
		else if(destLamport.serverIndex > sourceLamport.serverIndex)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	return -1;
}

int existChatRoom(struct Server* serverPointer, int targetChatRoomID)
{
	struct ChatRoomNode* targetNode = serverPointer->chatRoomListHead;
	while(targetNode != NULL)
	{
		if(targetNode->groupID == targetChatRoomID)
		{
			return 1;
		}
		targetNode = targetNode->nextChatRoom;
	}

	return 0;
}

int removeUserChatRoom(struct Server* serverPointer, char* userName, int currentChatRoomID, char serverName[HOST_NAME_LENGTH], int recoverFlag)
{
	struct ChatRoomNode* targetNode = serverPointer->chatRoomListHead;
	struct UserNode* userListHead = NULL;
	struct UserNode* userListTail = NULL;
	struct UserNode* currentUserNode = NULL;
	struct UserNode* previousUserNode = NULL;

	if(recoverFlag == 1)
	{
		return 0;
	}
	if(targetNode == NULL)
	{
		printf("in removeUserChatRoom, chat room list is empty\n");
		return 0;
	}
	
	/*find the chat room to remove user*/
	while(targetNode != NULL)
	{
		printf("in removeUserChatRoom: targetNode->groupId = %d, currentChatRoomID = %d\n", targetNode->groupID, currentChatRoomID );
		if(targetNode->groupID == currentChatRoomID)
		{
			break;
		}
		targetNode = targetNode->nextChatRoom;
	}
	/*does not find currentChatRoomID*/
	if(targetNode == NULL)
	{
		printf("in removeUserChatRoom, does not find currentChatRoomID\n");
		return 0;
	}

	/*user list empty*/	
	if(targetNode->userListHead == NULL)
	{
		printf("in removeuserChatRoom, userList is empty\n");
		return 0;
	}
	else
	{	
	/*there are four scenarios*/

		/*find head and tail of the list*/
		userListHead = targetNode->userListHead;
		userListTail = targetNode->userListHead;
		currentUserNode = targetNode->userListHead;
		while(currentUserNode != NULL)
		{
			userListTail = currentUserNode;
			currentUserNode = currentUserNode->nextUser;
		}

		/*find target userNode*/
		currentUserNode = targetNode->userListHead;
		while(currentUserNode != NULL)
		{
			printf("userName = %s, currentUserNode->userName = %s\n", userName, currentUserNode->userName);
			printf("serverName = %s, currentUserNode->serverName = %s\n", serverName, currentUserNode->serverName);
			if(strncmp(userName, currentUserNode->userName, strlen(currentUserNode->userName)) == 0 && strncmp(serverName, currentUserNode->serverName, strlen(currentUserNode->serverName)) == 0)
			{
				break;
			}		
			currentUserNode = currentUserNode->nextUser;	
		}

		/*first case ----- delete the only node in the list*/
		/*if  (currentUserNode == userListHead && currentUserNode == userListTail)*/
		if  (currentUserNode == userListHead && currentUserNode == userListTail)
		{
			free(currentUserNode);
			targetNode->userListHead = NULL;			
		}		

		/*second case ----- delete the first node in the list*/
		/*else if ( currentUserNode == userListHead && currentUserNode != userListTail)*/		
		else if ( currentUserNode == userListHead && currentUserNode != userListTail)		
		{
			targetNode->userListHead = currentUserNode->nextUser;
			free(currentUserNode);
		}
		/*third case ---- delete the last node in the list*/
		/*else if ( currentUserNode != userListHead && currentUserNode == userListTail)*/
		else if ( currentUserNode != userListHead && currentUserNode == userListTail)
		{
			previousUserNode = targetNode->userListHead;
			while(previousUserNode != NULL && previousUserNode->nextUser != currentUserNode)
			{
				previousUserNode = previousUserNode->nextUser;
			}
			free(currentUserNode);
			if(previousUserNode != NULL)
			{
				previousUserNode->nextUser = NULL;
			}
		}

		/*fourth case ---- delete a middle node in the list*/
		/*else if ( currentUserNode != userListHead && currentUserNode != userListTail)*/
		else if ( currentUserNode != userListHead && currentUserNode != userListTail)
		{
			previousUserNode = targetNode->userListHead;
			while(previousUserNode != NULL && previousUserNode->nextUser != currentUserNode)
			{
				previousUserNode = previousUserNode->nextUser;
			}
			previousUserNode->nextUser = currentUserNode->nextUser;
			free(currentUserNode);			
		}
		printf("successfully remove user %s from chatroom%d", userName, currentChatRoomID);	
		targetNode->numUser = targetNode->numUser - 1;
	}
	
	return 1;
}


void printServerChatRoomUser(struct Server* serverPointer)
{
	struct ChatRoomNode* cycleChatRoomList;
	struct UserNode* cycleUserList;
	struct MessageNode* cycleMessageList;
	struct LikeNode* likeNodeList;
	cycleChatRoomList = serverPointer->chatRoomListHead;
	while(cycleChatRoomList != NULL)
	{
		printf("\nChatRoom %d ---- numUser = %d, numMessage = %d\n", cycleChatRoomList->groupID , cycleChatRoomList->numUser, cycleChatRoomList->numMessage);
		cycleUserList = cycleChatRoomList->userListHead;
		while(cycleUserList != NULL)
		{
			printf("	userName:%s     connected to %s\n", cycleUserList->userName, cycleUserList->serverName);
			cycleUserList = cycleUserList->nextUser;
		}
		cycleMessageList = cycleChatRoomList->messageListHead;
		while(cycleMessageList != NULL)
		{
			printf("	Message %d: owner: %s like-%d Body-%s\n", cycleMessageList->messageID, cycleMessageList->userName, cycleMessageList->numLike, cycleMessageList->messagePayLoad);
			likeNodeList = cycleMessageList->likeListHead;
			while(likeNodeList != NULL)
			{
				printf("		%s\n", likeNodeList->userName);
				likeNodeList = likeNodeList->nextLikeNode;
			}
			cycleMessageList = cycleMessageList->nextMessage;
		}		
	
		cycleChatRoomList = cycleChatRoomList->nextChatRoom;

	}
}

int addNewChatRoom(struct Server* serverPointer, int targetChatRoomID, char userName[USER_NAME_LENGTH])
{
	struct ChatRoomNode* listTail = serverPointer->chatRoomListHead;
	struct ChatRoomNode* temp = NULL;

	/*malloc a new chatRoom node*/	
	temp = (struct ChatRoomNode*) malloc(sizeof(struct ChatRoomNode));
	if(!temp)
	{
		printf("malloc fail\n");
		return 0;
	}
	temp->groupID = targetChatRoomID ;
	temp->numUser = 0;
	temp->numMessage = 0;
	temp->nextChatRoom = NULL;
	temp->userListHead = NULL;
	temp->messageListHead = NULL;
	temp->messageListTail = NULL;
	printf("addNewChatRoom succeed. %d\n", temp->groupID);

	/*append the new created node to the end of the link list*/
	if(listTail == NULL)
	{
		serverPointer->chatRoomListHead = temp;
	}
	else
	{
		/*while traverse the chatRoom list, check whether the chat room already exists*/
		while(listTail->nextChatRoom != NULL)
		{
			if(targetChatRoomID == listTail->groupID)
			{
				printf("chat room already exists in chat room list\n");
				return 0;
			}
			listTail = listTail->nextChatRoom;
		}
		listTail->nextChatRoom = temp;	
	}

	serverPointer->numChatRoom = serverPointer->numChatRoom + 1;
	return 1;
}


int addNewUserChatRoom(struct Server* serverPointer, int targetChatRoomID, char userName[USER_NAME_LENGTH], char serverName[HOST_NAME_LENGTH], int recoverFlag)
{
	struct ChatRoomNode* targetNode = serverPointer->chatRoomListHead;
	struct UserNode* userListTail = NULL;
	struct UserNode* temp = NULL;
	if(recoverFlag == 1)
	{
		return 0;
	}
	
	printf("server chat room num = %d\n", serverPointer->numChatRoom);
	if(targetNode == NULL)
	{
		printf("in addNewUserChatRoom, chatRoomListHead is empty\n");
		return 0;
	}

	/*find the chat room to add user*/
	while(targetNode != NULL)
	{
		printf("targetNode->groupId = %d, targetChatRoomID = %d\n", targetNode->groupID, targetChatRoomID);
		if(targetNode->groupID == targetChatRoomID )
		{
			break;
		}
		targetNode = targetNode->nextChatRoom;
	}

	if(targetNode == NULL)
	{
		printf("in addNewUserChatRoom, cannot find the target chat room\n");
		return 0;
	}
	
	/*create a new user node*/
	temp = (struct UserNode*)malloc(sizeof(struct UserNode));
	if(!temp)
	{
		printf("malloc fail\n");
		exit(0);
	}
	memcpy(temp->userName, userName, strlen(userName) + 1);
	memcpy(temp->serverName, serverName, strlen(serverName)+1);
	temp->nextUser = NULL;
	temp->active = 1;
	/*append the new created userNode to the end of the link list*/
	/*first scenario: userList empty*/
	if(targetNode->userListHead == NULL)
	{
		targetNode->userListHead = temp;
	}	
	/*second scenarior: traverse through the userList till the end
 	* simultaneously check whether the userName already exists*/
	else
	{
		userListTail = targetNode->userListHead;
		while(userListTail->nextUser != NULL)
		{
			if(strncmp(userName, userListTail->userName, strlen(userListTail->userName)) == 0)
			{
				printf("UserName already exist in UserList");
				return 1;
			}
			userListTail = userListTail->nextUser;
		}
		userListTail->nextUser = temp;
	}
	
	targetNode->numUser = targetNode->numUser + 1;
	return 1;
}


