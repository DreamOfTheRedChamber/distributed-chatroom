/*
 * The Spread Toolkit.
 *     
 * The contents of this file are subject to the Spread Open-Source
 * License, Version 1.0 (the ``License''); you may not use
 * this file except in compliance with the License.  You may obtain a
 * copy of the License at:
 *
 * http://www.spread.org/license/
 *
 * or in the file ``license.txt'' found in this distribution.
 *
 * Software distributed under the License is distributed on an AS IS basis, 
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
 * for the specific language governing rights and limitations under the 
 * License.
 *
 * The Creators of Spread are:
 *  Yair Amir, Michal Miskin-Amir, Jonathan Stanton, John Schultz.
 *
 *  Copyright (C) 1993-2009 Spread Concepts LLC <info@spreadconcepts.com>
 *
 *  All Rights Reserved.
 *
 * Major Contributor(s):
 * ---------------
 *    Ryan Caudy           rcaudy@gmail.com - contributions to process groups.
 *    Claudiu Danilov      claudiu@acm.org - scalable wide area support.
 *    Cristina Nita-Rotaru crisn@cs.purdue.edu - group communication security.
 *    Theo Schlossnagle    jesus@omniti.com - Perl, autoconf, old skiplist.
 *    Dan Schoenblum       dansch@cnds.jhu.edu - Java interface.
 *
 */


#include "net_include.h"

static	char	User[80];
static  char    Spread_name[80];
char RecoveryFile[FILE_NAME_LENGTH];
static  char    Private_group[MAX_GROUP_NAME];
static  mailbox Mbox;
static	int	    Num_sent;

char AnotherPrivate[MAX_GROUP_NAME];
static  int     To_exit = 0;
struct Server localServer;
struct MultiServerStates multiServer;
int localGroupCounter = 0;
int globalCounter = 1;
int numUserPartition = 0;
int antiEntroMessageNum = 0;
int antiEntroMatrix[SERVER_NUM][SERVER_NUM];
#define MAX_MESSLEN     102400
#define MAX_VSSETS      10
#define MAX_MEMBERS     100

static	void	Read_message();
static  void	Bye();
void GroupidUserAddress(char* joinGroupName);
void initializeServer(char* joinGroupName);

int addNewChatRoom(struct Server* serverPointer, int targetChatRoomID, char userName[USER_NAME_LENGTH]);
int addNewUserChatRoom(struct Server* serverPointer, int targetChatRoomID, char userName[USER_NAME_LENGTH], char serverName[USER_NAME_LENGTH], int recoverFlag);
void printServerChatRoomUser(struct Server* serverPointer);
int removeUserChatRoom(struct Server* serverPointer, char* userName, int currentChatRoomID, char serverName[HOST_NAME_LENGTH], int recoverFlag);
int existChatRoom(struct Server* serverPointer, int targetChatRoomID);
int addMessage(struct Server* serverPointer, char userName[USER_NAME_LENGTH], int currentChatRoomID, char messagePayLoad[MESSAGE_LENGTH], struct Lamport packetLamport);
int replyMessageList(struct Server* serverPointer, normalPacket recPacket, mailbox* MboxPointer, char sender[MAX_GROUP_NAME], int private);
int likeMessage(struct Server* serverPointer, char* userName, int currentChatRoomID, int lineNum, struct Lamport* lamportPointer);
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
void printAntiEntroMatrix();
int writeMultiServerStates(normalPacket recPacket, char fileName[FILE_NAME_LENGTH]);
int recoverServerStates(char fileName[FILE_NAME_LENGTH], mailbox* MboxPointer, char Private_group[MAX_GROUP_NAME]);
int customizeChatRoomID(char inputGroupName[MAX_GROUP_NAME], char currentServer[HOST_NAME_LENGTH]);
int replyUserList(struct Server* serverPointer, normalPacket recPacket, mailbox* MboxPointer, char sender[MAX_GROUP_NAME]);
int customizeReceivedChatRoomID(struct Server* serverPointer, normalPacket* recPacketPointer);
int sendSelfChatRoom(char targetChatRoomString[GROUP_NAME_LENGTH], struct Server* serverPointer, normalPacket recPacket, mailbox* MboxPointer);
int serverLikeMessage(struct Server* serverPointer, char* userName, int currentChatRoomID, struct Lamport likedMessageLamport);
int resetUserActive(struct Server* serverPointer);
int muteAllUser(struct Server* serverPointer);

int main( int argc, char *argv[] )
{
	int	ret;
        int     mver, miver, pver;
        sp_time test_timeout;

	char    joinGroupName[GROUP_NAME_LENGTH];
	char    serversGroupName[GROUP_NAME_LENGTH];
	FILE* fTemp;
        test_timeout.sec = 5;
        test_timeout.usec = 0;
	memset(&multiServer, 0, sizeof(struct MultiServerStates));	
	localServer.serverView[0] = 1;
	localServer.serverView[1] = 1;	
	localServer.serverView[2] = 1;
	localServer.serverView[3] = 1;
	localServer.serverView[4] = 1;
	
        if (!SP_version( &mver, &miver, &pver)) 
        {
	          printf("main: Illegal variables passed to SP_version()\n");
	          Bye();
	}
	printf("Spread library version is %d.%d.%d\n", mver, miver, pver);

	GroupidUserAddress(joinGroupName);
	sprintf(serversGroupName, "100006" );    

	initializeServer(joinGroupName);
	
	printf("SpreadName = %s, User = %s\n", Spread_name, User);
	ret = SP_connect_timeout( Spread_name, User, 0, 1, &Mbox, Private_group, test_timeout );   
	if( ret != ACCEPT_SESSION ) 
	{
		printf("error here\n");
		SP_error( ret );
		Bye();
	}
	printf("User: connected to %s with private group %s\n", Spread_name, Private_group );

	if(argc == 2 && strncmp(argv[1], "r", strlen("r")) == 0)
	{
		recoverServerStates(RecoveryFile, &Mbox, Private_group);
	}
	else
	{
		fTemp = fopen(RecoveryFile, "w");
		fclose(fTemp);
	}

	E_init();
	    
	E_attach_fd( Mbox, READ_FD, Read_message, 0, NULL, HIGH_PRIORITY );

	ret = SP_join(Mbox, joinGroupName);
	if(ret < 0)
	{
		SP_error(ret);
 	}
	ret = SP_join(Mbox, serversGroupName);
	if(ret < 0)
	{
		SP_error(ret);
	}
	printf("server join AnotherPrivate %s\n", AnotherPrivate);
	ret = SP_join(Mbox, AnotherPrivate);
	if(ret < 0)
	{
		SP_error(ret);
	}
	printf("\nUser> ");
	fflush(stdout);

	Num_sent = 0;

	E_handle_events();

	return( 0 );
}

void initializeServer(char* joinGroupName)
{
	localServer.groupID = atoi(joinGroupName);
	localServer.numChatRoom = 0;
	localServer.chatRoomListHead = NULL;
}

void GroupidUserAddress(char* joinGroupName)
{
	char hostName[HOST_NAME_LENGTH];
	gethostname(hostName, HOST_NAME_LENGTH);
	if (strncmp(hostName, "ugrad13.cs.jhu.edu", strlen("ugrad13.cs.jhu.edu")) == 0)
	{
		sprintf( User, "Server1" );
	/*	sprintf( Spread_name, "10253@128.220.224.88");*/
		sprintf(Spread_name, SERVER1_SPREADNAME);
		sprintf(joinGroupName, SERVER1_PUB_GROUP);
		sprintf(RecoveryFile, SERVER1_RECOVER_FILE);
		sprintf(AnotherPrivate, SERVER1_PRIV_GROUP);
	}
	else if (strncmp(hostName, "ugrad14.cs.jhu.edu", strlen("ugrad14.cs.jhu.edu")) == 0)
	{
		sprintf( User, "Server2" );
	/*	sprintf( Spread_name, "10253@128.220.224.89");*/
		sprintf(Spread_name, SERVER2_SPREADNAME);
		sprintf(joinGroupName, SERVER2_PUB_GROUP);	
		sprintf(RecoveryFile, SERVER2_RECOVER_FILE);
		sprintf(AnotherPrivate, SERVER2_PRIV_GROUP);
	}
	else if (strncmp(hostName, "ugrad15.cs.jhu.edu", strlen("ugrad15.cs.jhu.edu")) == 0)
	{
		sprintf( User, "Server3" );
	/*	sprintf( Spread_name, "10253@128.220.224.90");*/
		sprintf(Spread_name, SERVER3_SPREADNAME);
		sprintf(joinGroupName, SERVER3_PUB_GROUP);
		sprintf(RecoveryFile, SERVER3_RECOVER_FILE);
		sprintf(AnotherPrivate, SERVER3_PRIV_GROUP);
	}
	else if (strncmp(hostName, "ugrad17.cs.jhu.edu", strlen("ugrad17.cs.jhu.edu")) == 0)
	{
		sprintf( User, "Server4" );
	/*	sprintf( Spread_name, "10253@128.220.224.92");*/
		sprintf(Spread_name, SERVER4_SPREADNAME);
		sprintf(joinGroupName, SERVER4_PUB_GROUP);
		sprintf(RecoveryFile, SERVER4_RECOVER_FILE);
		sprintf(AnotherPrivate, SERVER4_PRIV_GROUP);
	}
	else if (strncmp(hostName, "ugrad19.cs.jhu.edu", strlen("ugrad19.cs.jhu.edu")) == 0)
	{
		sprintf( User, "Server5" );
	/*	sprintf( Spread_name, "10253@128.220.224.94");*/
		sprintf(Spread_name, SERVER5_SPREADNAME);
		sprintf(joinGroupName, SERVER5_PUB_GROUP);
		sprintf(RecoveryFile, SERVER5_RECOVER_FILE);
		sprintf(AnotherPrivate, SERVER5_PRIV_GROUP);
	}
	else
	{
		printf("server run on the incorrect machine\n");
	}
	
}


/* FIXME: The user.c code does not use memcpy()s to avoid bus errors when
 *        dereferencing a pointer into a potentially misaligned buffer */

static	void	Read_message()
{

static	char		 mess[MAX_MESSLEN];
        char		 sender[MAX_GROUP_NAME];
        char		 target_groups[MAX_MEMBERS][MAX_GROUP_NAME];
	char             selfChatRoom[GROUP_NAME_LENGTH];
        membership_info  memb_info;
        vs_set_info      vssets[MAX_VSSETS];
        unsigned int     my_vsset_index;
        int              num_vs_sets;
        char             members[MAX_MEMBERS][MAX_GROUP_NAME];
        int		 num_groups;
        int		 service_type;
        int16		 mess_type;
        int		 endian_mismatch;
        int		 i,j;
        int		 ret;
	normalPacket     recPacket;
	memset(&recPacket, 0, sizeof(normalPacket));

        service_type = 0;

/*
	ret = SP_receive( Mbox, &service_type, sender, 100, &num_groups, target_groups, 
		&mess_type, &endian_mismatch, sizeof(mess), mess );
*/
	ret = SP_receive(Mbox, &service_type, sender, 100, &num_groups, target_groups, 
		&mess_type, &endian_mismatch, sizeof(normalPacket), (char*)(&recPacket));

	/*if the multicast packet come from the server itself, just ignore it*/
	if(comeFromSelf(sender, Private_group) && recPacket.command != 'p' && recPacket.recoverStates != 1 )
	{
		printf("sender=%s, private_group = %s\n", sender, Private_group);
		return;
	}
	if(comeFromSelf(sender, Private_group) && recPacket.recoverStates == 1)
	{
		printf("receive recovery packets, recPacket.command = %c\n", recPacket.command);
	}
	printf("\n============================\n");
	if( ret < 0 ) 
	{
                if ( (ret == GROUPS_TOO_SHORT) || (ret == BUFFER_TOO_SHORT) ) {
                        service_type = DROP_RECV;
                        printf("\n========Buffers or Groups too Short=======\n");
                        ret = SP_receive( Mbox, &service_type, sender, MAX_MEMBERS, &num_groups, target_groups, 
                                          &mess_type, &endian_mismatch, sizeof(mess), mess );
                }
        }
        if (ret < 0 )
        {
		if( ! To_exit )
		{
			SP_error( ret );
			printf("\n============================\n");
			printf("\nBye.\n");
		}
		exit( 0 );
	}
	if( Is_regular_mess( service_type ) )
	{

		if(recPacket.command == 'c')
		{
			if(recPacket.currentChatRoomID != 0)
			{
				removeUserChatRoom(&localServer, recPacket.userName, recPacket.currentChatRoomID, recPacket.previousServerName, recPacket.recoverStates);
				addNewUserChatRoom(&localServer, recPacket.currentChatRoomID, recPacket.userName, recPacket.serverName, recPacket.recoverStates);
			}	
			if(!comeFromServer(sender))
			{
				replyUserList(&localServer, recPacket, &Mbox, sender);
				addLamportStamp(&recPacket, &globalCounter, localServer.groupID);
				syncServers(recPacket, &Mbox, SERVER_GROUPS);	
			}
			updateMultiServerStates(&multiServer, recPacket);
			writeMultiServerStates(recPacket, RecoveryFile);	
	
			printServerChatRoomUser(&localServer);
			printMultiServerStates(&multiServer);	
		}
		else if(recPacket.command == 'u')
		{
			if(recPacket.currentChatRoomID != 0)
			{
				removeUserChatRoom(&localServer, recPacket.switchUserName, recPacket.currentChatRoomID, recPacket.serverName, recPacket.recoverStates);
				addNewUserChatRoom(&localServer, recPacket.currentChatRoomID, recPacket.userName, recPacket.serverName, recPacket.recoverStates);
			}
			if(!comeFromServer(sender))
			{
				replyUserList(&localServer, recPacket, &Mbox, sender);
				addLamportStamp(&recPacket, &globalCounter, localServer.groupID);
	
				syncServers(recPacket, &Mbox, SERVER_GROUPS);

			}

			updateMultiServerStates(&multiServer, recPacket);
			writeMultiServerStates(recPacket, RecoveryFile);	
	
			printServerChatRoomUser(&localServer);
			printMultiServerStates(&multiServer);
		}	
		else if(recPacket.command == 'j')
		{
			/***************basic program structure************
			if(recPacket.currentChatRoomID != 0)
			{
				removeUserChatRoom(struct Server* server_pointer, char* userName, int currentChatRoomID);
			}
			
			if(!existChatRoom(struct Server* server_pointer, int targetChatRoomID))
			{
				addNewChatRoom(struct Server* server_pointer, int targetChatRoomID);
			}
			addUserChatRoom(struct Server* server_pointer, char* userName, int targetChatRoomID);
			***************************************************/			
			

			if(comeFromServer(sender))
			{
				customizeReceivedChatRoomID(&localServer, &recPacket);
			}			
	
			if(recPacket.currentChatRoomID != 0)
			{
				printf("server.c: currentChatRoomID = %d, needs to remove user from previous chat room\n", recPacket.currentChatRoomID);	
				removeUserChatRoom(&localServer, recPacket.userName, recPacket.currentChatRoomID, recPacket.serverName, recPacket.recoverStates);
			}
			
			if(!existChatRoom(&localServer, recPacket.joinChatRoomID))
			{
				printf("ChatRoom does not exist, create one\n");
				addNewChatRoom(&localServer, recPacket.joinChatRoomID, recPacket.userName);
			}
			
			addNewUserChatRoom(&localServer, recPacket.joinChatRoomID, recPacket.userName, recPacket.serverName, recPacket.recoverStates);

			addLamportStamp(&recPacket, &globalCounter, localServer.groupID);
			updateMultiServerStates(&multiServer, recPacket);
			
			writeMultiServerStates(recPacket, RecoveryFile);	
			
			/*only if message comes from clients, send replies*/
			
			if(!comeFromServer(sender))
			{	
				printf("come into reply user list and chat room list\n");
				syncServers(recPacket, &Mbox, SERVER_GROUPS);
				replyUserList(&localServer, recPacket, &Mbox, sender);				
				replyMessageList(&localServer, recPacket, &Mbox, sender, USE_SENDER);	
			}
			printServerChatRoomUser(&localServer);
			printMultiServerStates(&multiServer);
		}
		else if(recPacket.command == 'a')
		{
			/**************basic program structure************
			addMessage(struct Server* server_pointer, char* userName, int currentChatRoomID, char* messagePayload)
			replyMessage(struct Server* serverPointer, nomralPacket recPacket);
			*/
			printf("!!!! message comes from %s !!!\n", sender);
			printf("recPacket.currentChatRoomID = %d\n", recPacket.currentChatRoomID);
			printf("recPacket.messagePayLoad = %s\n", recPacket.messagePayLoad);
			printf("recPacket.userName = %s\n", recPacket.userName);	
			if(comeFromServer(sender))
			{
				customizeReceivedChatRoomID(&localServer, &recPacket);
			}			
	
			if(!comeFromServer(sender))
			{
				addLamportStamp(&recPacket, &globalCounter, localServer.groupID);
			}
			
			updateMultiServerStates(&multiServer, recPacket);
			
			writeMultiServerStates(recPacket, RecoveryFile);	
			
			addMessage(&localServer, recPacket.userName, recPacket.currentChatRoomID, recPacket.messagePayLoad, recPacket.packetLamport);	
		
			if(!comeFromServer(sender))
			{
				syncServers(recPacket, &Mbox, SERVER_GROUPS);
				sendSelfChatRoom(selfChatRoom, &localServer, recPacket, &Mbox);
				replyMessageList(&localServer, recPacket, &Mbox, selfChatRoom, USE_SENDER);
			}
			if(comeFromServer(sender))
			{
				sendSelfChatRoom(selfChatRoom, &localServer, recPacket, &Mbox);
				replyMessageList(&localServer, recPacket, &Mbox, selfChatRoom, USE_SENDER);
			}

			printServerChatRoomUser(&localServer);
			printMultiServerStates(&multiServer);
		}
		else if(recPacket.command == 'l')
		{
			if(comeFromServer(sender))
			{
				customizeReceivedChatRoomID(&localServer, &recPacket);
			}			
			if(!comeFromServer(sender))
			{
				addLamportStamp(&recPacket, &globalCounter, localServer.groupID);	
			}

			if(!comeFromServer(sender))
			{
				likeMessage(&localServer, recPacket.userName, recPacket.currentChatRoomID, recPacket.likeLineNum, &(recPacket.likedMessageLamport));	
			}
			else
			{
				serverLikeMessage(&localServer, recPacket.userName, recPacket.currentChatRoomID, recPacket.likedMessageLamport);
			}
			
			updateMultiServerStates(&multiServer, recPacket);
			
			writeMultiServerStates(recPacket, RecoveryFile);	
			
			if(!comeFromServer(sender))
			{
				syncServers(recPacket, &Mbox, SERVER_GROUPS);
				sendSelfChatRoom(selfChatRoom, &localServer, recPacket, &Mbox);
				replyMessageList(&localServer, recPacket, &Mbox, selfChatRoom, USE_SENDER);
			}
			if(comeFromServer(sender))
			{
				sendSelfChatRoom(selfChatRoom, &localServer, recPacket, &Mbox);
				replyMessageList(&localServer, recPacket, &Mbox, selfChatRoom, USE_SENDER);
			}
	
			printServerChatRoomUser(&localServer);
			printMultiServerStates(&multiServer);
			/**************basic program structure************
			likeMessage(struct Server* serverPointer,  char* userName, int currentChatRoomID, int lineNum);
			*************************************************/
		}
		else if(recPacket.command == 'r')
		{
			/***********basic program structure***********
 			* dislikeMessage(struct Server* serverPointer, char* userName, int currentChatRoomID, int lineNum); 		
 			*********************************************/ 
			if(comeFromServer(sender))
			{
				customizeReceivedChatRoomID(&localServer, &recPacket);
			}			
			if(!comeFromServer(sender))
			{
				addLamportStamp(&recPacket, &globalCounter, localServer.groupID);
			}

			if(!comeFromServer(sender))
			{
				dislikeMessage(&localServer, recPacket.userName, recPacket.currentChatRoomID, recPacket.likeLineNum, &(recPacket.likedMessageLamport));
			}
			else
			{
				serverDislikeMessage(&localServer, recPacket.userName, recPacket.currentChatRoomID, recPacket.likedMessageLamport);
			}

			updateMultiServerStates(&multiServer, recPacket);
			
			writeMultiServerStates(recPacket, RecoveryFile);	
			

			if(!comeFromServer(sender))
			{
				syncServers(recPacket, &Mbox, SERVER_GROUPS);
				sendSelfChatRoom(selfChatRoom, &localServer, recPacket, &Mbox);	
				replyMessageList(&localServer, recPacket, &Mbox, selfChatRoom, USE_SENDER);
			}
			if(comeFromServer(sender))
			{
				sendSelfChatRoom(selfChatRoom, &localServer, recPacket, &Mbox);
				replyMessageList(&localServer, recPacket, &Mbox, selfChatRoom, USE_SENDER);
			}
	
			printServerChatRoomUser(&localServer);
			printMultiServerStates(&multiServer);	
		}
		else if(recPacket.command == 'h')
		{
			replyEntireMessageList(&localServer, recPacket, &Mbox, sender);
			printServerChatRoomUser(&localServer);
		}
		else if(recPacket.command == 'v')
		{
			replyServerView(&localServer, &Mbox, sender);
			printServerChatRoomUser(&localServer);	
		}
		else if(recPacket.command == 'p')
		{
			/***************basic program structure************
			numAntiEntro++;
			int saveAntiEntroVector( int antiEntroMatrix[5][5], normalPacket* recPacket, char sender[MAX_GROUP_NAME]);
			if(numAntiEntro == num_of_users in current partition)
			{
				int analyzeAndSend(int antiEntroMatrix[5][5], struct MultiServerStates* multiServer, mailbox* MboxPointer);
			}
			***************************************************/	
			antiEntroMessageNum++;
			
			saveAntiEntroVector(antiEntroMatrix, &recPacket, sender);
			/*
			printAntiEntroMatrix(antiEntroMatrix);		
			*/
			if(antiEntroMessageNum == numUserPartition)
			{
				printf("received all vectors:\n");
				analyzeSavedVector(&localServer, antiEntroMatrix, &multiServer, &Mbox, Private_group);
				
				printAntiEntroMatrix(antiEntroMatrix);
			}
		}
	}
	else if( Is_membership_mess( service_type ) )
        {
                ret = SP_get_memb_info( mess, service_type, &memb_info );
                if (ret < 0) {
                        printf("BUG: membership message does not have valid body\n");
                        SP_error( ret );
                        exit( 1 );
                }
		if     ( Is_reg_memb_mess( service_type ) )
		{
/*
			printf("Received REGULAR membership for group %s with %d members, where I am member %d:\n",
				sender, num_groups, mess_type );
					printf("grp id is %d %d %d\n",memb_info.gid.id[0], memb_info.gid.id[1], memb_info.gid.id[2] );
*/
			if( Is_caused_join_mess( service_type ) )
			{
				/*
				printf("Due to the JOIN of %s\n", memb_info.changed_member );
				*/
			}
			else if( Is_caused_leave_mess( service_type ) )
			{
				printf("Due to the LEAVE of %s\n", memb_info.changed_member );
			}
			else if( Is_caused_disconnect_mess( service_type ) )
			{
				printf("Due to the DISCONNECT of %s\n", memb_info.changed_member );
			}
			else if( Is_caused_network_mess( service_type ) )
			{
				memcpy(localServer.previousServerView, localServer.serverView, sizeof(localServer.serverView));
				memset(localServer.serverView, 0, sizeof(localServer.serverView));
				memset(antiEntroMatrix, -1, sizeof(antiEntroMatrix));
				numUserPartition = num_groups;
				antiEntroMessageNum = 0;
				for( i=0; i < num_groups; i++ )
				{
					/*
					printf("\t%s\n", &target_groups[i][0] );
					*/
					if(strncmp(&target_groups[i][0], SERVER1_MEMBERNAME, strlen(SERVER1_MEMBERNAME)) == 0)
					{
						localServer.serverView[0] = 1;
					}
					else if(strncmp(&target_groups[i][0], SERVER2_MEMBERNAME, strlen(SERVER2_MEMBERNAME)) == 0)
					{
						localServer.serverView[1] = 1;
					}
					else if(strncmp(&target_groups[i][0], SERVER3_MEMBERNAME, strlen(SERVER3_MEMBERNAME)) == 0)
					{
						localServer.serverView[2] = 1;
					}
					else if(strncmp(&target_groups[i][0], SERVER4_MEMBERNAME, strlen(SERVER4_MEMBERNAME)) == 0)
					{
						localServer.serverView[3] = 1;
					}
					else if(strncmp(&target_groups[i][0], SERVER5_MEMBERNAME, strlen(SERVER5_MEMBERNAME)) == 0)
					{
						localServer.serverView[4] = 1;
					}
				}
				
				/*when there are network change and current serverView is different from the previous
				 * execute the consistency check program*/
				/*
				sendAntiEntroVector(struct MultiServerStates* multiServer, mailbox* MboxPointer);
				*/
				resetUserActive(&localServer);
				sendAntiEntroVector(&multiServer, &Mbox);

				printf("Due to NETWORK change with %u VS sets\n", memb_info.num_vs_sets);
                                num_vs_sets = SP_get_vs_sets_info( mess, &vssets[0], MAX_VSSETS, &my_vsset_index );
                                if (num_vs_sets < 0)
				{
                                        printf("BUG: membership message has more then %d vs sets. Recompile with larger MAX_VSSETS\n", MAX_VSSETS);
                                        SP_error( num_vs_sets );
                                        exit( 1 );
                                }
                                for( i = 0; i < num_vs_sets; i++ )
                                {
                                        printf("%s VS set %d has %u members:\n",
                                               (i  == my_vsset_index) ?
                                               ("LOCAL") : ("OTHER"), i, vssets[i].num_members );
                                        ret = SP_get_vs_set_members(mess, &vssets[i], members, MAX_MEMBERS);
                                        if (ret < 0) 
					{
                                                printf("VS Set has more then %d members. Recompile with larger MAX_MEMBERS\n", MAX_MEMBERS);
                                                SP_error( ret );
                                                exit( 1 );
                                        }
                                        for( j = 0; j < vssets[i].num_members; j++ )
                                                printf("\t%s\n", members[j] );
                                }
			}
		}
		else if( Is_transition_mess(   service_type ) ) 
		{
			printf("received TRANSITIONAL membership for group %s\n", sender );
		}
		else if( Is_caused_leave_mess( service_type ) )
		{
			printf("received membership message that left group %s\n", sender );
		}
		else printf("received incorrecty membership message of type 0x%x\n", service_type );
        } 
	else if ( Is_reject_mess( service_type ) )
        {
		printf("REJECTED message from %s, of servicetype 0x%x messtype %d, (endian %d) to %d groups \n(%d bytes): %s\n",
			sender, service_type, mess_type, endian_mismatch, num_groups, ret, mess );
	}
	else printf("received message of unknown message type 0x%x with ret %d\n", service_type, ret);


	printf("\n");
	/*
	printf("User> ");
	*/
	fflush(stdout);
}


static  void	Bye()
{
	To_exit = 1;

	printf("\nBye.\n");

	SP_disconnect( Mbox );

	exit( 0 );
}
