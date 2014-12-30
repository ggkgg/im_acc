/** �� �� ��: im_wideband_speedup.c
** Copyright (c), imove(SHENTHEN) Co., Ltd.
** ��    ��: 2014-12-27
** ��    ��:
** ��    ��:
** �޸���ʷ:
** 2014-12-27   �ű󴴽����ļ���
##############################################################################*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "msg.h"
#include "im_wideband_speedup.h"
//#include "im_queue.h"
//#include "im_json_parse.h"



#define SOCKET_MODULE		"im_wideband_acc"
#define WAN_IFNAME			"eth1"
#define LAN_IFNAME			"br-lan"

extern int im_cloud_login(char *url);
extern int im_get_local_ipv4(char *ifname, char *local_ip);
extern int im_handle_msg(char *p_msg, int acc_socket);

const char *jason_key[]=
{
	"header",
	"cmd",
	"ver",
	"seq",
	"device",
	"code",
	"sessionid",
	"sign",
	"data",
	"dev_name",
	"isp",
	"cur_bandwidth",
	"acc_bandwidth",
	"acc_remaintime",
	"is_open",
	"max_bandwidth",
	"is_acc",
	"total_flow",
	"remain_flow",
	"get_flow",
	"get_flow_id",
	"get_flow_info",
	"peak_flow",
	"is_keepacc",
	"acc_list",
	"use_time",
	"use_flow",
	"open_acc_time",
	"send_inform",
	"send_inform_Id",
	"send_inform_msg",
	"acc_time",
	"acc_url"
	
	
};

S_IM_MSG_HEADER s_im_cmd_header;
S_IM_TERMSG_DATA_SPEEDUP_CMD s_im_speedup;
S_IM_TERMSG_DATA_GETFLOW_CMD s_im_getflow;
S_IM_TERMSG_DATA_KEEPACC_CMD s_im_keepacc;

S_IM_CLOUD_DATA_LOGIN_CMD	 s_im_login_cmd;
S_IM_CLOUD_DATA_LOGIN_RESP	 s_im_login_resp;
S_IM_CLOUD_DATA_ACC_CMD		 s_im_acc;
S_IM_TERMSG_DATA_ISACC_RESP  s_im_isacc_resp;
S_IM_TERMSG_DATA_STARTACC_RESP s_im_startacc_resp;
S_IM_TERMSG_DATA_FLOW_INFO_RESP s_im_flowinfo_resp;
S_IM_CLOUD_DATA_ACCSPEC_RESP s_im_accspec;
S_IM_CLOUD_DATA_COUNTFLOW_CMD s_im_countflow_cmd;
S_IM_CLOUD_DATA_GETAACFLOW_CMD s_im_getaacflow_cmd;
S_IM_TERMSG_DATA_SPEEDUP_INFO_RESP s_im_speedupinfo_resp;


void im_init_session(void)
{
	strcpy(s_im_acc.im_session_id, s_im_login_resp.im_session_id);
	strcpy(s_im_countflow_cmd.im_session_id, s_im_login_resp.im_session_id);
	strcpy(s_im_getaacflow_cmd.im_session_id, s_im_login_resp.im_session_id);
}

int im_windband_init(void)
{
	int ret = 0;
	
	im_init_session();

//��ʼ����ѯ/�������ٽṹ��
	strcpy(s_im_acc.im_dev_id, s_im_login_cmd.im_dev_id);
	strcpy(s_im_acc.im_ip, s_im_login_cmd.im_ip);

	
	return ret;
}

void im_check_internet_connect(void)
{
	int ret = 0;
	char ipaddr[MAX_IP_LEN] = {0};
	
	//���������ӳɹ�������ִ��
	do
	{
		ret = im_get_local_ipv4(WAN_IFNAME, ipaddr);
		
		if (ret != MSG_CODE_SUCESS)
		{
			sleep(10);
		}
		
	}while(ret != MSG_CODE_SUCESS);
	
}

void im_start_login_cloud(char *url)
{
	int ret = MSG_CODE_SUCESS;

	ret = im_get_local_ipv4(LAN_IFNAME, s_im_login_cmd.im_ip);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_get_local_ipv4 lanip fail");
		return MSG_CODE_UNKNOWN;
	}
	
	strcpy(s_im_login_cmd.im_dev_id, "imove_test 123456");
	
	do
	{
		ret = im_cloud_login(url);
		if (ret != MSG_CODE_SUCESS)
		{
			sleep(10);
		}
		
	}while(ret != MSG_CODE_SUCESS);
}

int main (int argc, char *argv[])
{
	int server_socket = -1;
	int accept_socket = -1;
	int ret = -1;
	
	//char *url = "http://172.16.2.99:8000";
	/*��Ϣ��ʽΪ:{"header":{"cmd":%d, "ver":%d, "seq":%d, 
	 "device":%d, "appid": %d,"code":%d, "sessionid":"%s", "sign":"%s"}, 
	 "data":[{""}]}
	*/
	
	char *p_msg = NULL;

	im_check_internet_connect();
	im_start_login_cloud(CLOUD_URL);
	im_windband_init();
	
	server_socket = IM_DomainServerInit(SOCKET_MODULE);  //��ʼ������ͨ��socket
	if (server_socket < 0)
	{
		IM_DEBUG("IM_DomainServerInit error");
		return -1;
	}
	

	while(1)
    {
    	accept_socket = IM_ServerAcceptClient(server_socket);
		if (accept_socket < 0 )
		{
			if (accept_socket == EINTR)
			{
				continue;
			}
			else
			{
				perror("IM_ServerAcceptClient ERROR");
				return -1;
			}
		}

		ret = IM_MsgReceive(accept_socket, &p_msg, NULL);	
		if (ret < 0)
        {
            IM_DEBUG("msg receive failed accept_socket = %d!\n",  accept_socket);
            IM_DomainServerDeinit(accept_socket);
			continue;
        }

		ret = im_handle_msg(p_msg, accept_socket); //������Ϣ����ʶ�������
		if (ret != MSG_CODE_SUCESS)
		{
			IM_DEBUG("im_handle_msg fail");
		}
		
		IM_FREE(p_msg);
	    IM_DomainServerDeinit(accept_socket);
	}


	return 0;
}
