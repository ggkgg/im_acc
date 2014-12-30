/** �� �� ��: im_msg_handle.c
** Copyright (c), imove(SHENTHEN) Co., Ltd.
** ��    ��: 2014-12-28
** ��    ��:
** ��    ��:
** �޸���ʷ:
** 2014-12-28   �ű󴴽����ļ���
##############################################################################*/

#include "im_wideband_speedup.h"
#include "im_json_parse.h"
#include <pthread.h>

#define HTTP_TIMEOUT		30		//30S
#define HEARTBEAT_TIMEOUT	110		//��λ��

pthread_t heartbeat_pid;
static int cnt_time = 0 ;

extern int im_http_sendmsg(char *url, char *postdata, char *httpresp, int timeout);
extern int im_udp_sendmsg(char *msg);


void im_brocast_to_terminal(char *p_msg, int errcode, char *json_data)
{
	char udp_msg[2048] = {0};
	char udp_info[1024] = {0};
	int ret = 0;
	
	//���첢���͹㲥��Ϣ֪ͨ�����ն�
	sprintf(udp_info, "\"%s\":%s, \"%s\":%s", 
			jason_key[JSON_SEND_INFORM_ID], s_im_speedup.im_dev_name,
			jason_key[JSON_SEND_INFORM_MSG], json_data);
	ret = im_json_make_udp_brocast(p_msg, errcode, udp_info, udp_msg);
	if (ret == MSG_CODE_SUCESS)
	{
		im_udp_sendmsg(udp_msg);
	}
	else
	{
		IM_DEBUG("im_json_make_udp_brocast fail");
	}
}

/*********************************************
function:  ��¼�ƶ�
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/

int im_cloud_login(char *url)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	S_IM_MSG_HEADER s_im_cmd_header;
	int ret = MSG_CODE_SUCESS;
	
	im_json_make_cmd_login(&s_im_login_cmd, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}
	
	ret = im_http_sendmsg(url, msg, httpresp, HTTP_TIMEOUT);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
		return ret;
	}

	printf("httpresp:%s\n", httpresp);
	ret = im_json_parse_cloud(httpresp, &s_im_cmd_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_parse_cloud fail");
	}

	return ret;
}

/*********************************************
function:  �˳��ƶ�
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/

int im_cloud_logout(char *url)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	S_IM_MSG_HEADER s_im_cmd_header;
	int ret = MSG_CODE_SUCESS;
	
	im_json_make_cmd_logout(&s_im_login_resp, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}
	
	ret = im_http_sendmsg(url, msg, httpresp, HTTP_TIMEOUT);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
	}

	ret = im_json_parse_cloud(httpresp, &s_im_cmd_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_parse_cloud fail");
	}
	return ret;
}


/*********************************************
function:  ���ƶ˵�����
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/

int im_cloud_heartbeat(char *url)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	S_IM_MSG_HEADER s_im_cmd_header;
	int ret = MSG_CODE_SUCESS;
	
	im_json_make_cmd_heartbeat(&s_im_login_resp, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}
	
	ret = im_http_sendmsg(url, msg, httpresp, HEARTBEAT_TIMEOUT);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
	}

	ret = im_json_parse_cloud(httpresp, &s_im_cmd_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_parse_cloud fail");
	}
	return ret;
}

/*********************************************
function:  ��ѯ�Ƿ�ɼ���
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/

int im_cloud_isaac(char *url, S_IM_MSG_HEADER *s_im_cmd_header)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	int ret = MSG_CODE_SUCESS;
	
	im_json_make_cmd_isacc(&s_im_acc, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}
	
	ret = im_http_sendmsg(url, msg, httpresp, HTTP_TIMEOUT);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
	}

	ret = im_json_parse_cloud(httpresp, s_im_cmd_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_parse_cloud fail");
	}

	return ret;
}

/*********************************************
function:  ��������
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/

int im_cloud_startaac(char *url)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	S_IM_MSG_HEADER s_im_cmd_header;
	int ret = MSG_CODE_SUCESS;
	
	im_json_make_cmd_startacc(&s_im_acc, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}
	
	ret = im_http_sendmsg(url, msg, httpresp, HTTP_TIMEOUT);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
	}

	ret = im_json_parse_cloud(httpresp, &s_im_cmd_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_parse_cloud fail");
	}
	return ret;
}

/*********************************************
function:  ֹͣ����
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/

int im_cloud_stopaac(char *url)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	S_IM_MSG_HEADER s_im_cmd_header;
	int ret = MSG_CODE_SUCESS;
	
	im_json_make_cmd_stopacc(&s_im_login_resp, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}
	
	ret = im_http_sendmsg(url, msg, httpresp, HTTP_TIMEOUT);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
	}

	ret = im_json_parse_cloud(httpresp, &s_im_cmd_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_parse_cloud fail");
	}
	return ret;
}

/*********************************************
function:  ͳ�Ƽ�������
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/

int im_cloud_countflow(char *url)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	S_IM_MSG_HEADER s_im_cmd_header;
	int ret = MSG_CODE_SUCESS;
	
	im_json_make_cmd_countflow(&s_im_countflow_cmd, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}
	
	ret = im_http_sendmsg(url, msg, httpresp, HTTP_TIMEOUT);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
	}

	ret = im_json_parse_cloud(httpresp, &s_im_cmd_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_parse_cloud fail");
	}
	return ret;
}

/*********************************************
function:  ��ȡ���������ӿ�
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/

int im_cloud_getaacflow(char *url)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	S_IM_MSG_HEADER s_im_cmd_header;
	int ret = MSG_CODE_SUCESS;
	
	im_json_make_cmd_getaacflow(&s_im_getaacflow_cmd, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}
	
	ret = im_http_sendmsg(url, msg, httpresp, HTTP_TIMEOUT);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
	}

	ret = im_json_parse_cloud(httpresp, &s_im_cmd_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_parse_cloud fail");
	}
	return ret;
}

/*********************************************
function:  ��ȡ�����������
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/

int im_cloud_getflowinfo(char *url)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	S_IM_MSG_HEADER s_im_cmd_header;
	int ret = MSG_CODE_SUCESS;
	
	im_json_make_cmd_flowinfo(&s_im_login_resp, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}
	
	ret = im_http_sendmsg(url, msg, httpresp, HTTP_TIMEOUT);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
	}

	ret = im_json_parse_cloud(httpresp, &s_im_cmd_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_parse_cloud fail");
	}
	return ret;
}

/*********************************************
function:  ��ȡ������ʷ
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/


int im_cloud_getacclist(char *url)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	S_IM_MSG_HEADER s_im_cmd_header;
	int ret = MSG_CODE_SUCESS;
	
	im_json_make_cmd_acclist(&s_im_login_resp, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}
	
	ret = im_http_sendmsg(url, msg, httpresp, HTTP_TIMEOUT);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
	}

	ret = im_json_parse_cloud(httpresp, &s_im_cmd_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_parse_cloud fail");
	}
	return ret;
}

/*********************************************
function:  ��ȡ��������
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/

int im_cloud_getaccinfo(char *url)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	S_IM_MSG_HEADER s_im_cmd_header;
	int ret = MSG_CODE_SUCESS;
	
	im_json_make_cmd_accinfo(&s_im_login_resp, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}
	
	ret = im_http_sendmsg(url, msg, httpresp, HTTP_TIMEOUT);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
	}

	ret = im_json_parse_cloud(httpresp, &s_im_cmd_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_parse_cloud fail");
	}

	
	return ret;
}


void *im_heartbeat_thread(void *arg)
{
	
	im_cloud_heartbeat(CLOUD_URL);
	
	sleep(300); // 5����һ������
	
	
	if(!s_im_keepacc.im_is_keepacc)	
	{
		cnt_time++;
		if (cnt_time >= 24) // 120 ����
		{
			cnt_time = 0;
			return NULL;
		}
	}
}

void im_start_heartbeat_cloud(void)
{

	int ret = 0;
	
	ret = pthread_create(&heartbeat_pid, NULL, im_heartbeat_thread, NULL);
	if(ret != 0)
	{
		IM_DEBUG("can't create thread: %s\n", strerror(ret));
		return;
	}
	
	pthread_detach(heartbeat_pid);
}

void im_stop_heartbeat_cloud(void)
{
	int err = 0;
	
	err = pthread_cancel(heartbeat_pid);
	if (err != 0)
	{
		IM_DEBUG("pthread_cancel FAIL");
	}
	cnt_time = 0;
}

/*********************************************
function:   �����ն˷�����������
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/
int im_handle_cmd_terminal(char *p_msg, int acc_socket)
{
	S_IM_MSG_HEADER s_im_cloud_header;	//�ƶ˷��ص�ͷ�ṹ
	int ret = MSG_CODE_SUCESS;
	char json_data[1024] = {0};
	int errcode = MSG_CODE_SUCESS; //������Ϣͷ��CODE��ֵ
	
	switch(s_im_cmd_header.im_cmd)
	{
		case TERM_CMD_SPEEDUP_INFO:
			im_json_make_termres_speedupinfo(json_data);
			break;
		case TERM_CMD_GETFLOW:
			break;
		case TERM_CMD_KEEPACC:
			break;
		case TERM_CMD_START_ACC:
			ret = im_cloud_startaac(CLOUD_URL);
			if (ret != MSG_CODE_SUCESS)
			{
				IM_DEBUG("im_cloud_startaac fail");
				errcode = MSG_CODE_UNKNOWN;
				break;
			}
			
			errcode = s_im_cloud_header.im_code;
			//���췵�ص�DATA
			if (s_im_cloud_header.im_code == MSG_CODE_SUCESS)
			{
				im_json_make_termres_startacc(json_data);
				
				//��ʼ��������Ϣ���
				s_im_speedupinfo_resp.im_is_open = 1;
				s_im_speedupinfo_resp.im_isp = s_im_startacc_resp.im_isp;
				strcpy(s_im_speedupinfo_resp.im_cur_bandwidth, s_im_startacc_resp.im_cur_bandwidth);
				strcpy(s_im_speedupinfo_resp.im_bandwidth, s_im_startacc_resp.im_bandwidth);
				s_im_speedupinfo_resp.im_remaintime = s_im_startacc_resp.im_remaintime;

				im_brocast_to_terminal(p_msg, errcode, json_data);

				
			}
			break;
		case TERM_CMD_STOP_ACC:
			ret = im_cloud_stopaac(CLOUD_URL);
			if (ret != MSG_CODE_SUCESS)
			{
				IM_DEBUG("im_cloud_startaac fail");
				errcode = MSG_CODE_UNKNOWN;
				break;
			}
			s_im_speedupinfo_resp.im_is_open = 0;
			break;
		case TERM_CMD_IS_ACC:
			ret = im_cloud_isaac(CLOUD_URL, &s_im_cloud_header);
			if (ret != MSG_CODE_SUCESS)
			{
				IM_DEBUG("im_cloud_startaac fail");
				errcode = MSG_CODE_UNKNOWN;
				break;
			}
			//���췵�ص�DATA
			if (s_im_cloud_header.im_code == MSG_CODE_SUCESS)
			{
				sprintf(json_data, "\"%s\":%d, \"%s\":%d, \"%s\":\"%s\", \"%s\":\"%s\"", 
							jason_key[JSON_IS_ACC], s_im_isacc_resp.im_is_acc,
							jason_key[JSON_ISP], s_im_isacc_resp.im_isp,
							jason_key[JSON_CUR_BANDWIDTH], s_im_isacc_resp.im_cur_bandwidth,
							jason_key[JSON_MAX_BANDWIDTH], s_im_isacc_resp.im_max_bandwidth);
			}
			
			break;
		case TERM_CMD_GETFLOW_INFO:
			break;
		case TERM_CMD_UNGETTEDFLOW:
			break;
		case TERM_CMD_PEAKFLOW:
			break;
		case TERM_CMD_GET_ACCLIST:
			ret = im_cloud_getacclist(CLOUD_URL);
			if (ret != MSG_CODE_SUCESS)
			{
				IM_DEBUG("im_cloud_startaac fail");
				errcode = MSG_CODE_UNKNOWN;
				break;
			}
			break;
		default:
			IM_DEBUG("unkonow cmd");
			errcode = MSG_CODE_FORMAT_ERR;
			ret = MSG_CODE_FORMAT_ERR;
			break;
	}


	ret = im_json_make_respone_send(p_msg, errcode, json_data, acc_socket);  
	if (ret != MSG_CODE_SUCESS) //����ʧ��
	{
		IM_DEBUG("im_json_make_respone_send fail");
		
	}

	return ret;
}


/*********************************************
function:   ������Ϣ������ִ������
params:
[IN] p_msg:	JSON��Ϣ
[IN]acc_socket: ��ǰ��Ϣ��SOCKET
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/
int im_handle_msg(char *p_msg, int acc_socket)
{
	int ret = MSG_CODE_SUCESS;
	if (p_msg == NULL)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_FORMAT_ERR;
	}
	ret = im_json_parse_terminal(p_msg);
	if (ret == 	MSG_CODE_SUCESS)
	{
		ret = im_handle_cmd_terminal(p_msg, acc_socket);
	}
	else
	{
		IM_DEBUG("im_json_parse error");
	}
#if 0	
	im_json_make_respone_send(p_msg, ret, NULL, acc_socket);  //��������ǰ�Ȼظ�
	if (ret != MSG_CODE_SUCESS) //����ʧ��
	{
		IM_DEBUG("im_json_parse fail");
		
		return MSG_CODE_FORMAT_ERR;
	}

#endif	
	return MSG_CODE_SUCESS;
	
}


