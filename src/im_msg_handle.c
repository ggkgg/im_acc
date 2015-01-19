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
#include <ifaddrs.h>
#include <signal.h>
#include "timer.h"

#define HTTP_TIMEOUT		30		//30S
#define HEARTBEAT_TIMEOUT	110		//��λ��
#define IFF_UP				0x01
#define IFF_RUNNING 		0x40
#define TIMER_5MIN			300

pthread_t rollquery_pid;

static int cnt_time = 0 ;
static unsigned long flow_count = 0;
static timer_handle_t timer_hearbeat = 100;
static timer_handle_t timer_flowcount = 100;

struct if_data  
{     
    u_long ifi_opackets;    
    u_long ifi_ipackets;    
    u_long ifi_obytes;        
    u_long ifi_ibytes;        
};  


extern int im_http_sendmsg(char *url, char *postdata, char *httpresp, int timeout);
extern int im_udp_sendmsg(char *msg);

const char *brocast_notify[]=
{
	"�����˴������!������������",
	"��������ѱ��Ͽ����������ֶ��Ͽ�������������",
	"�Ѱ��㽫������٣�������������",
	"������ٻ���5���Ӽ����ر�!�����˻���ԭ��",
	"��������ѽ�������ʹ��"
};


/*********************************************
function:  �Զ����������ķ�ֵ
params:
return: 
***********************************************/
void im_count_flow_peakflow(int im_cur_bandwidth)
{
	s_im_peakflow_resp.im_peak_flow = (im_cur_bandwidth/8)*0.8;
}

/*********************************************
function:   ��ȡָ���ӿ�RX���ֽ���
params:
[IN]ifi_obytes:    RX���ݵ�λbyte
return: -1��0
***********************************************/
int im_get_if_dbytes(char *ifname, unsigned long *ifi_obytes)  
{     
    struct ifaddrs *ifa_list = NULL;  
    struct ifaddrs *ifa = NULL;  
    struct if_data *ifdata = NULL;  
    int     ret = 0;  

	if (!ifi_obytes)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
    ret = getifaddrs(&ifa_list); 

    if(ret < 0) 
	{   
        IM_DEBUG("Get Interface Address Fail:");  
        goto end;  
    }     

    for(ifa=ifa_list; ifa; ifa=ifa->ifa_next)
	{  
        if(!(ifa->ifa_flags & IFF_UP) && !(ifa->ifa_flags & IFF_RUNNING))  
            continue;  
        if(ifa->ifa_data == 0)  
            continue;  
        ret = strcmp(ifa->ifa_name, ifname);  
        if(ret == 0)
		{  
           ifdata= (struct if_data *)ifa->ifa_data;  
           *ifi_obytes = ifdata->ifi_obytes;  
           break;  
        }  
    }  
  
    freeifaddrs(ifa_list);  

	
end:  
    return (ret ? MSG_CODE_UNKNOWN : MSG_CODE_SUCESS);  
} 

int im_sendbrocast_to_ios(char *url, char *noty_msg)
{
	int ret = MSG_CODE_SUCESS;
	char httpresp[1024] = {""};
	char msg[1024] = { 0 };
	char sign[33] = {0};
	char url_sign[128] = {0};
	
	ret = im_json_make_cmd_brocast_IOS(msg, noty_msg,sign);

	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_brocast_IOS fail");
		return ret;
	}
	
	sprintf(url_sign,"%s?sign=%s\n",url,sign);
	
	ret = im_http_sendmsg(url_sign, msg, NULL, HTTP_TIMEOUT);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
		return ret;
	}

}



/*********************************************
function:  ���͹㲥��Ϣ���ն�
params:
return: 
***********************************************/
#if 1
void im_brocast_to_terminal(int noty_type, char *noty_data)
{
	char udp_info[1024] = {0};
	int ret = MSG_CODE_SUCESS;
	//���첢���͹㲥��Ϣ֪ͨ�����ն�
	sprintf(udp_info, "{\"%s\":%d, \"%s\":\"%s\"}", 
			jason_key[JSON_SEND_INFORM_ID], noty_type,
			jason_key[JSON_SEND_INFORM_MSG], noty_data);

	//printf("udp_info:%s\n", udp_info);
	im_udpbrocast_timer_start(im_udp_sendmsg,udp_info);

	im_sendbrocast_to_ios(IOSSERVER_URL, noty_data);
}

#else
void im_brocast_to_terminal(char *p_msg, int errcode, char *json_data)
{
	char udp_msg[2048] = {0};
	char udp_info[1024] = {0};
	int ret = MSG_CODE_SUCESS;

	
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
#endif
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

	if (!url)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
	
	im_json_make_cmd_login(&s_im_login_cmd, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}

	printf("send_cloud_login = %s\n",msg);
	ret = im_http_sendmsg(url, msg, httpresp, HTTP_TIMEOUT);
	
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
		return ret;
	}

	ret = im_json_parse_cloud(httpresp, &s_im_cmd_header);
	if ((ret != MSG_CODE_SUCESS)||(s_im_cmd_header.im_code != MSG_CODE_SUCESS))
	{
		IM_DEBUG("im_json_parse_cloud fail ret:%d, code:%d", ret, s_im_cmd_header.im_code);
		ret = s_im_cmd_header.im_code;
	}

	return ret;
}


int im_cloud_logout(char *url)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	S_IM_MSG_HEADER s_im_cmd_header;
	int ret = MSG_CODE_SUCESS;
	
	if (!url)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
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

	if (!url)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
	
	im_json_make_cmd_heartbeat(&s_im_login_resp, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}
	printf("send_cloud_heartbeat = %s\n",msg);
	ret = im_http_sendmsg(url, msg, httpresp, HEARTBEAT_TIMEOUT);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
	}
	printf("cloud_response_heartbeat = %s\n",httpresp);
	ret = im_json_parse_cloud(httpresp, &s_im_cmd_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_parse_cloud fail");
	}

	if (s_im_cmd_header.im_code == MSG_CODE_SUCESS)
	{
		strcpy(s_im_login_resp.im_ip, s_im_heartbeat_resp.im_ip);
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
	
	printf("22222222222222222\n");
	if (!url || !s_im_cmd_header)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
	printf(" 2222222222222id =%s\n",s_im_acc.im_session_id);
	im_json_make_cmd_isacc(&s_im_acc, msg);
	printf("isacc hhhhmsg=%s\n",msg);
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

int im_cloud_startaac(char *url, S_IM_MSG_HEADER *s_im_cmd_header)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	int ret = MSG_CODE_SUCESS;

	if (!url || !s_im_cmd_header)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
	
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

	printf("msg_cloud_starthhh= %s\n",msg);
	ret = im_json_parse_cloud(httpresp, s_im_cmd_header);
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

int im_cloud_stopaac(char *url, S_IM_MSG_HEADER *s_im_cmd_header)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	int ret = MSG_CODE_SUCESS;

	if (!url || !s_im_cmd_header)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
	
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
	ret = im_json_parse_cloud(httpresp, s_im_cmd_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_parse_cloud fail");
	}
	return ret;
}


/*********************************************
function:  
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/

int im_cloud_getflow(char *url, S_IM_MSG_HEADER *s_im_cmd_header)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	int ret = MSG_CODE_SUCESS;

	if (!url || !s_im_cmd_header)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
	
	im_json_make_cmd_getflow(&s_im_login_resp, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_getflow fail");
		return ret;
	}

	ret = im_http_sendmsg(url, msg, httpresp, HTTP_TIMEOUT);
	
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
	}

	printf("httpresp -getflow =%s\n",httpresp);
	ret = im_json_parse_cloud(httpresp, s_im_cmd_header);
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

	if (!url)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
	
	im_json_make_cmd_countflow(&s_im_countflow_cmd, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}

	printf("send countflow = %s\n",msg);
	ret = im_http_sendmsg(url, msg, httpresp, HTTP_TIMEOUT);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_http_sendmsg fail");
		ret = MSG_CODE_UNKNOWN;
	}

	printf("response cloudcountflow =%s\n",httpresp);
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

int im_cloud_getaacflow(char *url, S_IM_MSG_HEADER *s_im_cmd_header)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	int ret = MSG_CODE_SUCESS;

	if (!url || !s_im_cmd_header)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
	
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

	ret = im_json_parse_cloud(httpresp, s_im_cmd_header);
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

int im_cloud_getflowinfo(char *url, S_IM_MSG_HEADER *s_im_cmd_header)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	int ret = MSG_CODE_SUCESS;

	if (!url || !s_im_cmd_header)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
	
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

	ret = im_json_parse_cloud(httpresp, s_im_cmd_header);
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


int im_cloud_getacclist(char *url, S_IM_MSG_HEADER *s_im_cmd_header)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	int ret = MSG_CODE_SUCESS;

	if (!url || !s_im_cmd_header)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
	
	ret = im_json_make_cmd_acclist(&s_im_login_resp, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_login fail");
		return ret;
	}

	printf("getacclist_msg = %s\n",msg);
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
function: ��ѯ������Ϣ
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/


int im_cloud_getflow_info(char *url, S_IM_MSG_HEADER *s_im_cmd_header)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	int ret = MSG_CODE_SUCESS;

	if (!url || !s_im_cmd_header)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
	
	ret = im_json_make_cmd_getflow_info(&s_im_login_resp, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_getflow_info fail");
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
function: ��ѯδ��ȡ��������Ϣ
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/


int im_cloud_ungetflow(char *url, S_IM_MSG_HEADER *s_im_cmd_header)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	int ret = MSG_CODE_SUCESS;

	if (!url || !s_im_cmd_header)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
	
	ret = im_json_make_cmd_ungetflow(&s_im_login_resp, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_make_cmd_ungetflow fail");
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
function:  ��ȡ��������
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/

int im_cloud_getaccinfo(char *url, S_IM_MSG_HEADER *s_im_cmd_header)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	int ret = MSG_CODE_SUCESS;

	if (!url || !s_im_cmd_header)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
	
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

	ret = im_json_parse_cloud(httpresp, s_im_cmd_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_json_parse_cloud fail");
	}

	
	return ret;
}

/*********************************************
function:  ��ѯ��Ϣ
params:
return: MSG_CODE_SUCESS, MSG_CODE_FORMAT_ERR
***********************************************/

int im_cloud_gettaskresult(char *url, S_IM_MSG_HEADER *s_im_cmd_header)
{
	char msg[1024] = { 0 };
	char httpresp[1024] = {""};
	int ret = MSG_CODE_SUCESS;

	if (!url || !s_im_cmd_header)
	{
		IM_DEBUG("input param error");
		return MSG_CODE_UNKNOWN;
	}
	
	ret = im_json_make_cmd_gettaskresult(&s_im_taskresult_cmd, msg);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_cloud_gettaskresult fail");
		return ret;
	}

	IM_DEBUG("task %s\n",msg);

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


int im_start_flow_count(void *arg)
{
	int ret = MSG_CODE_SUCESS;
	
	if (flow_count == 0)
	{
		ret = im_get_if_dbytes(WAN_IFNAME, &s_im_countflow_cmd.im_use_flow);
		
		if (ret != MSG_CODE_SUCESS)
		{
			IM_DEBUG("im_get_if_dbytes fail");
			return MSG_CODE_UNKNOWN;
		}

		flow_count = s_im_countflow_cmd.im_use_flow;
	}
	else
	{
		ret = im_get_if_dbytes(WAN_IFNAME, &s_im_countflow_cmd.im_use_flow);
		if (ret != MSG_CODE_SUCESS)
		{
			IM_DEBUG("im_get_if_dbytes fail");
			return MSG_CODE_UNKNOWN;
		}
		
		s_im_countflow_cmd.im_use_flow =(s_im_countflow_cmd.im_use_flow - flow_count)/1024;
		im_cloud_countflow(CLOUD_URL);
	}

	return ret;
}

void im_stop_flow_count(void)
{
	snmp_alarm_unregister(timer_flowcount);
	flow_count = 0;
}

void im_stop_heartbeat_cloud(void)
{
	
	snmp_alarm_unregister(timer_hearbeat);
	cnt_time = 0;
}

int im_start_heartbeat_cloud(void *arg)
{

	int ret = 0;
	
	im_cloud_heartbeat(CLOUD_URL);
	if(!s_im_keepacc.im_is_keepacc)	
	{
		cnt_time++;
		if (cnt_time == 24)
		{
			char notify_msg[512] = {0};
			sprintf(notify_msg, "%s%dkbps",
					EncodeToUTF8(brocast_notify[NOTIFY_ACC_WILLSTOP]),
					s_im_startacc_resp.im_cur_bandwidth);
				im_brocast_to_terminal(NOTIFY_ACC_WILLSTOP, notify_msg);
		}
		else if (cnt_time > 24) // 120 ����
		{
			cnt_time = 0;
			im_stop_heartbeat_cloud();
			im_stop_flow_count();
			return 0;
		}
	}
}
 
 
 
 void im_stop_rollquery_result_cloud(void)
 {
	 int err = 0;
	 
	 err = pthread_cancel(rollquery_pid);
	 if (err != 0)
	 {
		 IM_DEBUG("pthread_cancel FAIL");
	 }
 }
 
 void *im_rollquery_thread (void *arg)
 {
	int ret = MSG_CODE_SUCESS;
	S_IM_MSG_HEADER s_im_cmd_header;
	char notify_msg[512] = {0};
	char notify_msg1[512] = {0};
	char notify_msg2[512] = {0};
	int roll_time = 0;
	 
	 IM_DEBUG("START ROLL");
	 while(1)
	 {
		 ret = im_cloud_gettaskresult(CLOUD_URL, &s_im_cmd_header);
		 IM_DEBUG("ret =%d,im_code=%d\n",ret,s_im_cmd_header.im_code);
		 if((ret == MSG_CODE_SUCESS)&&(MSG_CODE_SUCESS == s_im_cmd_header.im_code))
		 {
			 if(s_im_speedupinfo_resp.im_acc_state == STATE_ACC_OPENING)
			 {	
				sprintf(notify_msg, "%s%s%dkbps",
					s_im_speedup.im_dev_name,
					EncodeToUTF8(brocast_notify[NOTIFY_ACC_SUCCESS]),
					s_im_startacc_resp.im_acc_bandwidth);
				 im_brocast_to_terminal(NOTIFY_ACC_SUCCESS, notify_msg);
				 
				 timer_hearbeat = snmp_alarm_register(TIMER_5MIN,SA_REPEAT, im_start_heartbeat_cloud, NULL);
				 sleep(1);
				 timer_flowcount = snmp_alarm_register(TIMER_5MIN,SA_REPEAT,im_start_flow_count, NULL);
				 
				 im_count_flow_peakflow(s_im_startacc_resp.im_cur_bandwidth);
				 s_im_speedupinfo_resp.im_acc_state = STATE_ACC_OPENED;
			 }	 
			 else if(s_im_speedupinfo_resp.im_acc_state == STATE_ACC_QUERYING)
			 {
			 	config_write_im_acc();
				 s_im_speedupinfo_resp.im_acc_state = STATE_ACC_CLOSED;
			 }
			 else
			 {
				 IM_DEBUG("error state:%d\n", s_im_speedupinfo_resp.im_acc_state);
			 }
			 return;
		 }
		 else if(MSG_CODE_IVALIDEDEVID == s_im_cmd_header.im_code)
		 {
			ret = im_cloud_login(CLOUD_URL);
		 }
		 else //20s ��ѯһ��
		 {
		 	if(roll_time >= 9){				//���180s��û��data����ʾ�������ٻ��߲�ѯ��Ϣʧ��
				s_im_cmd_header.im_code = MSG_CODE_UNKNOWN;
				s_im_speedupinfo_resp.im_acc_state = STATE_ACC_CLOSED;
				
				return;
		 	}
			else{	
			 sleep(20);
			 roll_time ++;
			}
		 }
	 }
 }
 
 void im_start_rollquery_result_cloud(void)
 {
 
	 int ret = 0;
	 
	 ret = pthread_create(&rollquery_pid, NULL, im_rollquery_thread, NULL);
	 if(ret != 0)
	 {
		 IM_DEBUG("can't create thread: %s\n", strerror(ret));
		 return;
	 }
	 
	 pthread_detach(rollquery_pid);
 }


int im_auto_query_isacc()
{
	int ret = MSG_CODE_SUCESS;
	S_IM_MSG_HEADER s_im_cloud_header;	//�ƶ˷��ص�ͷ�ṹ
	
	if (s_im_speedupinfo_resp.im_acc_state != STATE_ACC_CLOSED)
	{
		return ret;
	}
	s_im_speedupinfo_resp.im_acc_state = STATE_ACC_QUERYING;
	ret = im_cloud_isaac(CLOUD_URL, &s_im_cloud_header);
	if (ret != MSG_CODE_SUCESS)
	{
		IM_DEBUG("im_cloud_isaac fail");
		s_im_speedupinfo_resp.im_acc_state = STATE_ACC_CLOSED;
		return ret;
	}
	//���췵�ص�DATA
	if (s_im_cloud_header.im_code == MSG_CODE_MSGDELAY)
	{
		im_start_rollquery_result_cloud();		
	}
	else if (s_im_cloud_header.im_code == MSG_CODE_SUCESS)
	{
		im_count_flow_peakflow(s_im_speedupinfo_resp.im_cur_bandwidth);
		//im_query_server_timer_stop();	//�رղ�ѯ��ʱ��
		config_write_im_acc();
	}
	else if(MSG_CODE_INVALIDSES == s_im_cloud_header.im_code) 
	{
		//��sessionʧЧ�����µ�¼
		ret = im_cloud_login(CLOUD_URL);
		
		if (ret == MSG_CODE_SUCESS)
		{
			im_auto_query_isacc();
		}
	}
	
	return ret;
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
	json_object *json_data = NULL;
	int errcode = MSG_CODE_SUCESS; //������Ϣͷ��CODE��ֵ
	char notify_msg[512] = {0};
	json_object *DataObj;
	
	json_data = json_object_new_array();
	DataObj =  json_object_new_object();
	
	switch(s_im_cmd_header.im_cmd)
	{

		case TERM_CMD_GETFLOW:
_GETFLOW_RETRY:
			ret = im_cloud_getflow(CLOUD_URL, &s_im_cloud_header);
			if (ret != MSG_CODE_SUCESS)
			{
				IM_DEBUG("im_cloud_getflow fail");
				errcode = MSG_CODE_UNKNOWN;
				break;
			}
			
			if (s_im_cloud_header.im_code == MSG_CODE_SUCESS)
			{
				im_json_make_termres_getflow(json_data);
			}
			else if(MSG_CODE_INVALIDSES == s_im_cloud_header.im_code) 
			{
				ret = im_cloud_login(CLOUD_URL);
				if (ret == MSG_CODE_SUCESS)
				{
					sleep(1);
					goto _GETFLOW_RETRY;
				}
				else
				{
					errcode = MSG_CODE_UNKNOWN;
				}
			}
			else
			{
				errcode = s_im_cloud_header.im_code;
			}
			break;
		case TERM_CMD_KEEPACC:
			break;
		case TERM_CMD_START_ACC:
_START_ACC_RETRY:
			IM_DEBUG("coming ----im_acc_state=%d\n",s_im_speedupinfo_resp.im_acc_state);
			if (s_im_speedupinfo_resp.im_acc_state != STATE_ACC_CLOSED)
			{
				errcode = MSG_CODE_UNKNOWN;
				break;
			}

			s_im_speedupinfo_resp.im_acc_state = STATE_ACC_OPENING;
			ret = im_cloud_startaac(CLOUD_URL, &s_im_cloud_header);
			if (ret != MSG_CODE_SUCESS)
			{
				IM_DEBUG("im_cloud_startaac fail");
				errcode = MSG_CODE_UNKNOWN;
				s_im_speedupinfo_resp.im_acc_state = STATE_ACC_CLOSED;
				break;
			}
			
			errcode = s_im_cloud_header.im_code;
			//���췵�ص�DATA
			if (s_im_cloud_header.im_code == MSG_CODE_MSGDELAY)
			{
				errcode = MSG_CODE_MSGDELAY_TERM;
				im_start_rollquery_result_cloud();				
			}
			else if (s_im_cloud_header.im_code == MSG_CODE_SUCESS)
			{
				
				//��ʼ��������Ϣ���
				s_im_speedupinfo_resp.im_acc_state = STATE_ACC_OPENED;
				s_im_speedupinfo_resp.im_isp = s_im_startacc_resp.im_isp;
				s_im_speedupinfo_resp.im_cur_bandwidth = s_im_startacc_resp.im_cur_bandwidth;
				s_im_speedupinfo_resp.im_max_bandwidth = s_im_startacc_resp.im_max_bandwidth;
				s_im_speedupinfo_resp.im_acc_remaintime = s_im_startacc_resp.im_remaintime;
				s_im_speedupinfo_resp.im_acc_bandwidth = s_im_startacc_resp.im_acc_bandwidth;
				
				im_json_make_termres_startacc(json_data,DataObj);

				sprintf(notify_msg, "%s%s%dkbps",
					s_im_speedup.im_dev_name,
					EncodeToUTF8(brocast_notify[NOTIFY_ACC_SUCCESS]),
					s_im_startacc_resp.im_acc_bandwidth);
				
				im_brocast_to_terminal(NOTIFY_ACC_SUCCESS, notify_msg);

				timer_hearbeat = snmp_alarm_register(TIMER_5MIN,SA_REPEAT, im_start_heartbeat_cloud, NULL);
				timer_flowcount = snmp_alarm_register(TIMER_5MIN, SA_REPEAT,im_start_flow_count, NULL);

				im_count_flow_peakflow(s_im_startacc_resp.im_cur_bandwidth);
			}
			else if(MSG_CODE_INVALIDSES == s_im_cloud_header.im_code) 
			{
				ret = im_cloud_login(CLOUD_URL);
				if (ret == MSG_CODE_SUCESS)
				{
					sleep(1);
					s_im_speedupinfo_resp.im_acc_state = STATE_ACC_CLOSED;
					goto _START_ACC_RETRY;
				}
				else
				{
					errcode = MSG_CODE_UNKNOWN;
				}

				s_im_speedupinfo_resp.im_acc_state = STATE_ACC_CLOSED;
			}
			else
			{
				errcode = s_im_cloud_header.im_code;
			}
			break;
		case TERM_CMD_STOP_ACC:
			ret = im_cloud_stopaac(CLOUD_URL, &s_im_cloud_header);
			if (ret != MSG_CODE_SUCESS)
			{
				IM_DEBUG("im_cloud_startaac fail");
				errcode = MSG_CODE_UNKNOWN;
				break;
			}
			
			s_im_speedupinfo_resp.im_acc_state = STATE_ACC_CLOSED;
			im_stop_heartbeat_cloud();
			im_stop_flow_count();
			
			sprintf(notify_msg, "%s",
					EncodeToUTF8(brocast_notify[NOTIFY_ACC_STOP]));
				im_brocast_to_terminal(NOTIFY_ACC_STOP, notify_msg);
			break;
		case TERM_CMD_ACC_STATE_INFO:
			if((s_im_speedupinfo_resp.im_acc_state == STATE_ACC_QUERYING) || (s_im_speedupinfo_resp.im_acc_state == STATE_ACC_OPENING))
			{
				errcode = MSG_CODE_MSGDELAY_TERM;
				break;
			}

			ret = im_check_isnet_change();
			if(ret != MSG_CODE_SUCESS) //����δ�ı䲻��Ҫ��ѯ
			{
				im_json_make_termres_speedupinfo(json_data,DataObj);
			}
			else
			{
				IM_DEBUG("state=%d\n",s_im_speedupinfo_resp.im_acc_state);
				if(s_im_speedupinfo_resp.im_acc_state != STATE_ACC_CLOSED)
				{
					errcode = MSG_CODE_UNKNOWN;
					break;
				}

			_IS_ACC_RETRY:
				s_im_speedupinfo_resp.im_acc_state = STATE_ACC_QUERYING;
				ret = im_cloud_isaac(CLOUD_URL, &s_im_cloud_header);
				if (ret != MSG_CODE_SUCESS)
				{
					IM_DEBUG("im_cloud_startaac fail");
					errcode = MSG_CODE_UNKNOWN;
					s_im_speedupinfo_resp.im_acc_state = STATE_ACC_CLOSED;
					break;
				}
				
				//���췵�ص�DATA
				if (s_im_cloud_header.im_code == MSG_CODE_MSGDELAY)
				{
					errcode = MSG_CODE_MSGDELAY_TERM;
					im_start_rollquery_result_cloud();
					
				}
				else if (s_im_cloud_header.im_code == MSG_CODE_SUCESS)
				{
					im_json_make_termres_speedupinfo(json_data,DataObj);
					im_count_flow_peakflow(s_im_speedupinfo_resp.im_cur_bandwidth);
				}
				else if(MSG_CODE_INVALIDSES == s_im_cloud_header.im_code) 
				{
					ret = im_cloud_login(CLOUD_URL);
					if (ret == MSG_CODE_SUCESS)
					{
						sleep(1);
						s_im_speedupinfo_resp.im_acc_state = STATE_ACC_CLOSED;
						goto _IS_ACC_RETRY;
					}
					else
					{
						errcode = MSG_CODE_UNKNOWN;
					}

					s_im_speedupinfo_resp.im_acc_state = STATE_ACC_CLOSED;
				}
				else
				{
					errcode = s_im_cloud_header.im_code;
				}
			}
			break;
		case TERM_CMD_GETFLOW_INFO:
_GETFLOW_INFO_RETRY:
			ret = im_cloud_getflow_info(CLOUD_URL, &s_im_cloud_header);
			if (ret != MSG_CODE_SUCESS)
			{
				IM_DEBUG("im_cloud_getflow_info fail");
				errcode = MSG_CODE_UNKNOWN;
				break;
			}
			
			if (s_im_cloud_header.im_code == MSG_CODE_SUCESS)
			{
				im_json_make_termres_flowinfo(json_data,DataObj);
			}
			else if(MSG_CODE_INVALIDSES == s_im_cloud_header.im_code) 
			{
				ret = im_cloud_login(CLOUD_URL);
				if (ret == MSG_CODE_SUCESS)
				{
					sleep(1);
					goto _GETFLOW_INFO_RETRY;
				}
				else
				{
					errcode = MSG_CODE_UNKNOWN;
				}
			}
			else
			{
				errcode = s_im_cloud_header.im_code;
			}
			break;
		case TERM_CMD_UNGETTEDFLOW:
_UNGETTEDFLOW_RETRY:
			ret = im_cloud_ungetflow(CLOUD_URL, &s_im_cloud_header);
			if (ret != MSG_CODE_SUCESS)
			{
				IM_DEBUG("im_cloud_ungetflow fail");
				errcode = MSG_CODE_UNKNOWN;
				break;
			}
			
			if (s_im_cloud_header.im_code == MSG_CODE_SUCESS)
			{
				im_json_make_termres_ungetflow(json_data,DataObj);
			}
			else if(MSG_CODE_INVALIDSES == s_im_cloud_header.im_code) 
			{
				ret = im_cloud_login(CLOUD_URL);
				if (ret == MSG_CODE_SUCESS)
				{
					sleep(1);
					goto _UNGETTEDFLOW_RETRY;
				}
				else
				{
					errcode = MSG_CODE_UNKNOWN;
				}
			}
			else
			{
				errcode = s_im_cloud_header.im_code;
			}
			break;
		case TERM_CMD_PEAKFLOW:
			
			im_json_make_termres_peakflow(json_data,DataObj);
			break;
		case TERM_CMD_GET_ACCLIST:
_GET_ACCLIST_RETRY:
			ret = im_cloud_getacclist(CLOUD_URL, &s_im_cloud_header);
			if (ret != MSG_CODE_SUCESS)
			{
				IM_DEBUG("im_cloud_startaac fail");
				errcode = MSG_CODE_UNKNOWN;
				break;
			}
			
			if (s_im_cloud_header.im_code == MSG_CODE_SUCESS)
			{
				im_json_make_termres_acclist(json_data,DataObj);
			}
			else if(MSG_CODE_INVALIDSES == s_im_cloud_header.im_code) 
			{
				ret = im_cloud_login(CLOUD_URL);
				if (ret == MSG_CODE_SUCESS)
				{
					sleep(1);
					goto _GET_ACCLIST_RETRY;
				}
				else
				{
					errcode = MSG_CODE_UNKNOWN;
				}
			}
			else
			{
				errcode = s_im_cloud_header.im_code;
			}
			break;
		case TERM_CMD_IOSINFO:
			IM_DEBUG("TERM_CMD_IOSINFO");
			break;
		default:
			IM_DEBUG("unkonow cmd");
			errcode = MSG_CODE_FORMAT_ERR_TERM;
			ret = MSG_CODE_FORMAT_ERR;
			break;
	}
	
	ret = im_json_make_respone_send(p_msg, errcode, json_data, acc_socket);  
	if (ret != MSG_CODE_SUCESS) //����ʧ��
	{
		IM_DEBUG("im_json_make_respone_send fail");
		
	}

	json_object_put(DataObj);
	json_object_put(json_data);
	
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
		ret = im_json_make_respone_send(p_msg, ret, NULL, acc_socket);  
		if (ret != MSG_CODE_SUCESS) //����ʧ��
		{
			IM_DEBUG("im_json_make_respone_send fail");
			
		}
	}
	
	return MSG_CODE_SUCESS;
	
}


