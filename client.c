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

#define int32u unsigned int

static	char	User[80];
static  char    Spread_name[80];
char    CurrentGroup[80];
char    ServerGroup[80];
char    randomTime[TIME_LENGTH];
char    realUser[80];
char    AnotherPrivate[80];
static  char    Private_group[MAX_GROUP_NAME];
static  mailbox Mbox;
static	int	    Num_sent;

static  int     To_exit = 0;
sp_time test_timeout;

#define MAX_MESSLEN     102400
#define MAX_VSSETS      10
#define MAX_MEMBERS     100

static	void	Print_menu();
static	void	User_command();
static	void	Read_message();
static  void	Bye();

void ClientSendPacket(char* messAddress,int* Num_sent);
void DisplayChatroomMessage(normalPacket* recPacket);
void DisplayServerView(normalPacket* recPacket);
int customizeChatRoomID(char inputGroupName[MAX_GROUP_NAME], char currentServer[HOST_NAME_LENGTH]);
int randomTimeString(char randomTimeBuffer[TIME_LENGTH]);
void DisplayChatroomUser(normalPacket* recPacketPointer);

int main( int argc, char *argv[] )
{
	int	ret;
        int     mver, miver, pver;
	test_timeout.sec = 5;
	test_timeout.usec = 0;
	memset(realUser, 0, sizeof(realUser));
        if (!SP_version( &mver, &miver, &pver)) 
        {
	          printf("main: Illegal variables passed to SP_version()\n");
	          Bye();
  	 }
	 printf("Spread library version is %d.%d.%d\n", mver, miver, pver);

	/*Initialize CurrentGroup*/
	sprintf(CurrentGroup, "0");

	/**************this section should be deleted because client should not connect to any server before client specifies
 	*************** but there will be an error SP_error -18 if not handled*********************************/
/*
	ret = SP_connect_timeout( Spread_name, User, 0, 1, &Mbox, Private_group, test_timeout );
*
*/
	randomTimeString(randomTime);
	printf("Main randomTime = %s", randomTime);
	ret = SP_connect_timeout( SERVER1_SPREADNAME, NULL, 0, 1, &Mbox, Private_group, test_timeout );
	printf("Main Spread_name = %s, User = %s\n", Spread_name, User);	   
	if( ret != ACCEPT_SESSION ) 
	{
	    SP_error( ret );
	    Bye();
	}
	/*printf("User: connected to %s with private group %s\n", Spread_name, Private_group );
	*/
	E_init();

	E_attach_fd( 0, READ_FD, User_command, 0, NULL, LOW_PRIORITY );

	E_attach_fd( Mbox, READ_FD, Read_message, 0, NULL, HIGH_PRIORITY );

	Print_menu();
/*
	printf("\nUser> ");
*/
	fflush(stdout);

	Num_sent = 0;

	E_handle_events();

	return( 0 );
}

void DisplayServerView(normalPacket* recPacket)
{
	int counter;
	printf("Same Segmentation:\n");
	for(counter = 0; counter < SERVER_NUM; counter++)
	{
		if(recPacket->serverView[counter] == 1)
		{
			printf("\t");
			if(counter == 0)
			{
				printf(SERVER1_MEMBERNAME);
			}
			else if(counter == 1)
			{
				printf(SERVER2_MEMBERNAME);
			}
			else if(counter == 2)
			{
				printf(SERVER3_MEMBERNAME);
			}
			else if(counter == 3)
			{
				printf(SERVER4_MEMBERNAME);
			}
			else if(counter == 4)
			{
				printf(SERVER5_MEMBERNAME);
			}
			printf("\n");
		}
	}
}

void DisplayChatroomMessage(normalPacket* recPacketPointer)
{
	printf("LINENUM %d>   ", recPacketPointer->replyLineNum);
	printf("%s:", recPacketPointer->userName);
	printf("%s\tLikeness:%d\n", recPacketPointer->messagePayLoad, recPacketPointer->likeness);
}

void DisplayChatroomUser(normalPacket* recPacketPointer)
{
	printf("USER> %s", recPacketPointer->userName);
	if(strncmp(recPacketPointer->serverName, SERVER1_PUB_GROUP, strlen(SERVER1_PUB_GROUP)) == 0)
	{
		printf(" connected to %s\n", SERVER1);
	}
	else if(strncmp(recPacketPointer->serverName, SERVER2_PUB_GROUP, strlen(SERVER2_PUB_GROUP)) == 0)
	{
		printf(" connected to %s\n", SERVER2);
	}
	else if(strncmp(recPacketPointer->serverName, SERVER3_PUB_GROUP, strlen(SERVER3_PUB_GROUP)) == 0)
	{
		printf(" connected to %s\n", SERVER3);
	}
	else if(strncmp(recPacketPointer->serverName, SERVER4_PUB_GROUP, strlen(SERVER4_PUB_GROUP)) == 0)
	{
		printf(" connected to %s\n", SERVER4);
	}
	else if(strncmp(recPacketPointer->serverName, SERVER5_PUB_GROUP, strlen(SERVER5_PUB_GROUP)) == 0)
	{
		printf(" connected to %s\n", SERVER5);
	}
}

void ClientSendPacket(char* messAddress, int* Num_sent)
{
	int ret;
	ret = SP_multicast(Mbox, AGREED_MESS, ServerGroup, 1, sizeof(normalPacket), messAddress);
	if(ret < 0)
	{
		printf("place1\n");
		SP_error(ret);
		Bye();
	}
	/*
	printf("User %s send multicast message to Group %s\n", User, ServerGroup);
	*/
	(*Num_sent)++;
}

static	void	User_command()
{
	char	command[130];
	char	mess[MAX_MESSLEN];
	char	group[80];
	unsigned int	mess_len;
	int	ret;
	int	i;
	int     lineNum;
	

	normalPacket clientServerPacket;
	memset(&clientServerPacket, 0, sizeof(normalPacket));
	for( i=0; i < sizeof(command); i++ ) command[i] = 0;
	if( fgets( command, 130, stdin ) == NULL ) 
            Bye();
	switch( command[0] )
	{
		case 'c':
			ret = sscanf(&command[2], "%s", group);
			printf("group = %s\n", group);
			if(ret < 1)
			{
				printf("invalid group \n");
				break;
			}
			if(strncmp(ServerGroup, "0", strlen("0")) != 0)
			{
				if(strncmp(ServerGroup, SERVER1_PUB_GROUP, strlen(SERVER1_PUB_GROUP)) == 0)
				{
					SP_leave(Mbox, SERVER1_PRIV_GROUP);
				}
				else if(strncmp(ServerGroup, SERVER2_PUB_GROUP, strlen(SERVER2_PUB_GROUP)) == 0)
				{
					SP_leave(Mbox, SERVER2_PRIV_GROUP);
				}
				else if(strncmp(ServerGroup, SERVER3_PUB_GROUP, strlen(SERVER3_PUB_GROUP)) == 0)
				{
					SP_leave(Mbox, SERVER3_PRIV_GROUP);
				}
				else if(strncmp(ServerGroup, SERVER4_PUB_GROUP, strlen(SERVER4_PUB_GROUP)) == 0)
				{
					SP_leave(Mbox, SERVER4_PRIV_GROUP);
				}
				else if(strncmp(ServerGroup, SERVER5_PUB_GROUP, strlen(SERVER5_PUB_GROUP)) == 0)
				{
					SP_leave(Mbox, SERVER5_PRIV_GROUP);
				}
			}
			memcpy(clientServerPacket.previousServerName, ServerGroup, strlen(ServerGroup)+1);

			if(strncmp(group, "1", strlen("1")) == 0)
			{
				sprintf(ServerGroup, SERVER1_PUB_GROUP);
				sprintf(Spread_name, SERVER1_SPREADNAME);
			}
			else if(strncmp(group, "2", strlen("2")) == 0)
			{
				sprintf(ServerGroup, SERVER2_PUB_GROUP);
				sprintf(Spread_name, SERVER2_SPREADNAME);
			}
			else if(strncmp(group, "3", strlen("3")) == 0 )
			{
				sprintf(ServerGroup, SERVER3_PUB_GROUP);
				sprintf(Spread_name, SERVER3_SPREADNAME);
			}
			else if(strncmp(group, "4", strlen("4")) == 0)
			{
				sprintf(ServerGroup, SERVER4_PUB_GROUP);
				sprintf(Spread_name, SERVER4_SPREADNAME);
			}
			else if(strncmp(group, "5", strlen("5")) == 0)
			{
				sprintf(ServerGroup, SERVER5_PUB_GROUP);
				sprintf(Spread_name, SERVER5_SPREADNAME);
			}
			else
			{
				printf("Please input a valid server number\n");
				break;
			}
			SP_disconnect(Mbox);
			ret = SP_connect_timeout(Spread_name, User, 0, 1, &Mbox, Private_group, test_timeout);
			if(ret != ACCEPT_SESSION)
			{
				printf("place2\n");
				SP_error(ret);
				Bye();
			}
			if(strncmp(group, "1", strlen("1")) == 0)
			{
				SP_join(Mbox, SERVER1_PRIV_GROUP);
				sprintf(AnotherPrivate, SERVER1_PRIV_GROUP);
				printf("join server private group %s\n", SERVER1_PRIV_GROUP);
			}
			else if(strncmp(group, "2", strlen("2")) == 0)
			{
				SP_join(Mbox, SERVER2_PRIV_GROUP);
				sprintf(AnotherPrivate, SERVER2_PRIV_GROUP);
				printf("join server private group %s\n", SERVER2_PRIV_GROUP);
			}
			else if(strncmp(group, "3", strlen("3")) == 0 )
			{
				SP_join(Mbox, SERVER3_PRIV_GROUP);
				sprintf(AnotherPrivate, SERVER3_PRIV_GROUP);
				printf("join server private group %s\n", SERVER3_PRIV_GROUP);
			}
			else if(strncmp(group, "4", strlen("4")) == 0)
			{
				SP_join(Mbox, SERVER4_PRIV_GROUP);
				sprintf(AnotherPrivate, SERVER4_PRIV_GROUP);
				printf("join server private group %s\n", SERVER4_PRIV_GROUP);
			}
			else if(strncmp(group, "5", strlen("5")) == 0)
			{
				SP_join(Mbox, SERVER5_PRIV_GROUP);
				sprintf(AnotherPrivate, SERVER5_PRIV_GROUP);
				printf("join server private group %s\n", SERVER5_PRIV_GROUP);
			}
			else
			{
				printf("Please input a valid server number\n");
				break;
			}
	

			printf("User: connected to %s with private group %s\n", Spread_name, Private_group);
			printf("ServerGroup=%s\n", ServerGroup);
			
			clientServerPacket.command = 'c';
			clientServerPacket.currentChatRoomID = atoi(CurrentGroup);
			memcpy(clientServerPacket.userName, realUser, strlen(realUser));
			memcpy(clientServerPacket.serverName, ServerGroup, strlen(ServerGroup));	
			printf("###clientServerPacket.userName= %s, clientServerPacket.serverName=%s\n", clientServerPacket.userName, clientServerPacket.serverName);
	
			ClientSendPacket((char*)(&clientServerPacket), &Num_sent);
	
			break;

		case 'u':
			ret = sscanf( &command[2], "%s", group);
			if(ret < 1)
			{
				printf(" invalid group\n");
				break;
			}
			/*
			sprintf(User, group);
			*/
			clientServerPacket.command = 'u';
			clientServerPacket.currentChatRoomID = atoi(CurrentGroup);
			memcpy(clientServerPacket.userName, group, strlen(group)+1);
			memcpy(clientServerPacket.switchUserName, realUser, strlen(realUser)+1);
			memcpy(clientServerPacket.serverName, ServerGroup, strlen(ServerGroup)+1);			
			randomTimeString(randomTime);
			
			/*reset realUser field*/
			sprintf(realUser, group);
			sprintf(User, randomTime);
			SP_leave(Mbox, AnotherPrivate);
			SP_disconnect(Mbox);
			printf("inside u: Spread_name = %s, User = %s", Spread_name, User);
			ret = SP_connect_timeout(Spread_name, User, 0, 1, &Mbox, Private_group, test_timeout);

			if(ret != ACCEPT_SESSION)
			{
				printf("place3\n");
				SP_error(ret);
				Bye();
			}
			SP_join(Mbox, AnotherPrivate);
			printf("User: connected to %s with private group %s\n", Spread_name, Private_group);
			/*send the packet*/
			ClientSendPacket((char*)(&clientServerPacket), &Num_sent);
	
			break;

		case 'j':
			ret = sscanf( &command[2], "%s", group );
			if( ret < 1 ) 
			{
				printf(" invalid group \n");
				break;
			}
		
			/*customize chatRoomID*/
			customizeChatRoomID(group, ServerGroup);
	
			/*prepare the packet*/
			clientServerPacket.command = 'j';
			clientServerPacket.currentChatRoomID = atoi(CurrentGroup) ;
			clientServerPacket.joinChatRoomID = atoi(group) ;
			memcpy(clientServerPacket.userName, realUser, strlen(realUser)+1 );
			memcpy(clientServerPacket.serverName, ServerGroup, strlen(ServerGroup)+1);	
			/*send the packet*/
			ClientSendPacket((char*)(&clientServerPacket), &Num_sent);
	
			/*if CurrentGroup != 0, a client needs to first leave the group*/
			if(strncmp(CurrentGroup, "0", strlen("0")) != 0)
			{
				ret = SP_leave(Mbox, CurrentGroup);
				printf("leave CurrentGroup=%s\n", CurrentGroup);
				if(ret < 0) SP_error(ret);
			}
			/*a client joins the new group*/	
			ret = SP_join( Mbox, group );
			if( ret < 0 ) SP_error( ret );
			printf("client join group %s\n", group);
			
			/*reset CurrentGroup value*/
			memcpy(CurrentGroup, group, strlen(group)+1);	
		
			break;

		case 'a':
			ret = sscanf(&command[2], "%s", mess );
			if(ret < 1)
			{
				SP_error(ret);
				printf("place4\n");
				Bye();
			}
			/*prepare the packet*/
			mess_len = strlen( mess )+1;
			clientServerPacket.command = 'a';
			clientServerPacket.currentChatRoomID = atoi(CurrentGroup);
			memcpy(clientServerPacket.messagePayLoad, mess, mess_len);
			memcpy(clientServerPacket.userName, realUser, strlen(realUser)+1);

			/*send the packet*/
			ClientSendPacket((char*)(&clientServerPacket), &Num_sent);
			
			break;
		
		case 'l':
			ret = sscanf(&command[2], "%d", &lineNum );
			if(ret < 1)
			{
				SP_error(ret);
				printf("place5\n");
				Bye();
			}	
			/*prepare the packet*/		
			clientServerPacket.command = 'l';
			clientServerPacket.likeLineNum = lineNum;
			clientServerPacket.currentChatRoomID = atoi(CurrentGroup);
			memcpy(clientServerPacket.userName, realUser, strlen(realUser)+1);
			/*send the packet*/
			ClientSendPacket((char*)(&clientServerPacket), &Num_sent);

			break;

		case 'r':
			ret = sscanf(&command[2], "%d", &lineNum );
			if(ret < 1)
			{
				SP_error(ret);
				printf("place5\n");
				Bye();
			}	
	
			/*prepare the packet*/		
			clientServerPacket.command = 'r';
			clientServerPacket.likeLineNum = lineNum;
			clientServerPacket.currentChatRoomID = atoi(CurrentGroup);
			memcpy(clientServerPacket.userName, realUser, strlen(realUser)+1);
			/*send the packet*/
			ClientSendPacket((char*)(&clientServerPacket), &Num_sent);

			break;

		case 'h':
			clientServerPacket.command = 'h';			
			clientServerPacket.currentChatRoomID = atoi(CurrentGroup);
			memcpy(clientServerPacket.userName, realUser, strlen(realUser)+1);
	
			ClientSendPacket((char*)(&clientServerPacket), &Num_sent);

			break;				

		case 'v':
			clientServerPacket.command = 'v';
			ClientSendPacket((char*)(&clientServerPacket), &Num_sent);
			
			break;

		default:
			printf("\nUnknown commnad\n");
			Print_menu();

			break;
	}


	fflush(stdout);

}

static	void	Print_menu()
{
	printf("\n");
	printf("==========\n");
	printf("User Menu:\n");
	printf("----------\n");
	printf("\n");
	printf("\tc <server> -- connect to a server\n");
	printf("\tu <username> -- specify username\n");
	printf("\tj room<chatRoom> -- join a chatRoom\n");
	printf("\n");
	printf("\ta <message> -- send a message in current chatRoom\n");
    	printf("\tl <lineNumber> -- like the message associated with lineNumber\n");
	printf("\tr <lineNumber> -- dislike the message associated with lineNumber\n");
	printf("\n");
	printf("\th -- print history of the current chat room \n");
	printf("\tv -- print the view of chat servers in current chat room \n");
	printf("\n");
	printf("\tq -- quit\n");
	fflush(stdout);
}

/* FIXME: The user.c code does not use memcpy()s to avoid bus errors when
 *        dereferencing a pointer into a potentially misaligned buffer */

static	void	Read_message()
{

static	char		 mess[MAX_MESSLEN];
        char		 sender[MAX_GROUP_NAME];
        char		 target_groups[MAX_MEMBERS][MAX_GROUP_NAME];
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
	normalPacket	 recPacket;
	memset(&recPacket, 0, sizeof(normalPacket));

        service_type = 0;
	ret = SP_receive( Mbox, &service_type, sender, 100, &num_groups, target_groups, 
		&mess_type, &endian_mismatch, sizeof(normalPacket), (char*)(&recPacket) );

	if( !((strncmp(ServerGroup, SERVER1_PUB_GROUP, strlen(SERVER1_PUB_GROUP)) == 0 && strncmp(sender, SERVER1_MEMBERNAME, strlen(SERVER1_MEMBERNAME)) == 0) 
	|| (strncmp(ServerGroup, SERVER2_PUB_GROUP, strlen(SERVER2_PUB_GROUP)) == 0 && strncmp(sender, SERVER2_MEMBERNAME, strlen(SERVER2_MEMBERNAME)) == 0) 
	|| (strncmp(ServerGroup, SERVER3_PUB_GROUP, strlen(SERVER3_PUB_GROUP)) == 0 && strncmp(sender, SERVER3_MEMBERNAME, strlen(SERVER3_MEMBERNAME)) == 0) 
	|| (strncmp(ServerGroup, SERVER4_PUB_GROUP, strlen(SERVER4_PUB_GROUP)) == 0 && strncmp(sender, SERVER4_MEMBERNAME, strlen(SERVER4_MEMBERNAME)) == 0) 
	|| (strncmp(ServerGroup, SERVER5_PUB_GROUP, strlen(SERVER5_PUB_GROUP)) == 0 && strncmp(sender, SERVER5_MEMBERNAME, strlen(SERVER5_MEMBERNAME)) == 0) 
	) 		
	&& strncmp(AnotherPrivate, sender, strlen(sender)) != 0)
	{
		printf("ServerGroup = %s, sender = %s\n", ServerGroup, sender);
		return;
	}
	if( ret < 0 ) 
	{
		return;
	}
	if( Is_regular_mess( service_type ) )
	{
		/*
		mess[ret] = 0;
                if     ( Is_unreliable_mess( service_type ) ) printf("received UNRELIABLE ");
                else if( Is_reliable_mess(   service_type ) ) printf("received RELIABLE ");
                else if( Is_fifo_mess(       service_type ) ) printf("received FIFO ");
                else if( Is_causal_mess(     service_type ) ) printf("received CAUSAL ");
                else if( Is_agreed_mess(     service_type ) ) printf("received AGREED ");
                else if( Is_safe_mess(       service_type ) ) printf("received SAFE ");
                printf("message from %s, of type %d, (endian %d) to %d groups \n(%d bytes): %s\n",
                        sender, mess_type, endian_mismatch, num_groups, ret, mess );
		*/
		/*
		printf("receive: recPacket.command = %c\n", recPacket.command);	
		*/
		if(recPacket.command == 'j')
		{
			DisplayChatroomMessage(&recPacket);
		}
		else if(recPacket.command == 'u')
		{
			DisplayChatroomUser(&recPacket);
		}	
		else if(recPacket.command == 'a')
		{
			DisplayChatroomMessage(&recPacket);
		}
		else if(recPacket.command == 'l')
		{
			DisplayChatroomMessage(&recPacket);
		}
		else if(recPacket.command == 'r')
		{
			DisplayChatroomMessage(&recPacket);
		}
		else if(recPacket.command == 'h')
		{
			DisplayChatroomMessage(&recPacket);
		}
		else if(recPacket.command == 'v')
		{
			DisplayServerView(&recPacket);	
		}	
	}
	else if( Is_membership_mess( service_type ) )
        {
                ret = SP_get_memb_info( mess, service_type, &memb_info );
                if (ret < 0) {
                        printf("BUG: membership message does not have valid body\n");
      			printf("place9\n");
	                SP_error( ret );
                        exit( 1 );
                }
		if     ( Is_reg_memb_mess( service_type ) )
		{
/*
			printf("Received REGULAR membership for group %s with %d members, where I am member %d:\n",
				sender, num_groups, mess_type );
*/
			ret = 0;
			for( i=0; i < num_groups; i++ )
			{	
				printf("\t%s\n", &target_groups[i][0] );
				if(
					(strncmp(&target_groups[i][0], SERVER1_MEMBERNAME, strlen(&target_groups[i][0])) == 0) ||
					(strncmp(&target_groups[i][0], SERVER2_MEMBERNAME, strlen(&target_groups[i][0])) == 0) ||
					(strncmp(&target_groups[i][0], SERVER3_MEMBERNAME, strlen(&target_groups[i][0])) == 0) ||
					(strncmp(&target_groups[i][0], SERVER4_MEMBERNAME, strlen(&target_groups[i][0])) == 0) ||
					(strncmp(&target_groups[i][0], SERVER5_MEMBERNAME, strlen(&target_groups[i][0])) == 0)
				  )
				{
					ret = 1;
				}
			}
/*
			printf("grp id is %d %d %d\n",memb_info.gid.id[0], memb_info.gid.id[1], memb_info.gid.id[2] );
	*/
			if(ret == 0  && strncmp(sender, AnotherPrivate, strlen(AnotherPrivate)) == 0)
			{
				printf("connected server is down, please reconnect to another server\n");	
			}
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
			/*	printf("Due to the DISCONNECT of %s\n", memb_info.changed_member );
			*/
			}
			else if( Is_caused_network_mess( service_type ) ){
				printf("Due to NETWORK change with %u VS sets\n", memb_info.num_vs_sets);
                                num_vs_sets = SP_get_vs_sets_info( mess, &vssets[0], MAX_VSSETS, &my_vsset_index );
                                if (num_vs_sets < 0) {
                                        printf("BUG: membership message has more then %d vs sets. Recompile with larger MAX_VSSETS\n", MAX_VSSETS);
                                        SP_error( num_vs_sets );
                                 	printf("place10\n");
				        exit( 1 );
                                }
                                for( i = 0; i < num_vs_sets; i++ )
                                {
                                        printf("%s VS set %d has %u members:\n",
                                               (i  == my_vsset_index) ?
                                               ("LOCAL") : ("OTHER"), i, vssets[i].num_members );
                                        ret = SP_get_vs_set_members(mess, &vssets[i], members, MAX_MEMBERS);
                                        if (ret < 0) {
                                                printf("VS Set has more then %d members. Recompile with larger MAX_MEMBERS\n", MAX_MEMBERS);
                                                SP_error( ret );
                                                exit( 1 );
                                        }
                                        for( j = 0; j < vssets[i].num_members; j++ )
                                                printf("\t%s\n", members[j] );
                                }
			}
		}else if( Is_transition_mess(   service_type ) ) 
		{	/*
			printf("received TRANSITIONAL membership for group %s\n", sender );
			*/
		}
		else if( Is_caused_leave_mess( service_type ) ){
			printf("received membership message that left group %s\n", sender );
		}else printf("received incorrecty membership message of type 0x%x\n", service_type );
        } else if ( Is_reject_mess( service_type ) )
        {
		printf("REJECTED message from %s, of servicetype 0x%x messtype %d, (endian %d) to %d groups \n(%d bytes): %s\n",
			sender, service_type, mess_type, endian_mismatch, num_groups, ret, mess );
	}else printf("received message of unknown message type 0x%x with ret %d\n", service_type, ret);


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
