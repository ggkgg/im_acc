/** �� �� ��: im_update.h
** Copyright (c), imove(SHENTHEN) Co., Ltd.
** ��    ��: 2014-12-27
** ��    ��:
** ��    ��:
** �޸���ʷ:
** 2014-12-27   �ű󴴽����ļ���
##############################################################################*/
#ifndef __IM_WINDBAND_SPEEDUP_H__
#define  __IM_WINDBAND_SPEEDUP_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define MAX_SESSIONID   37		//im_sessionid����ĳ���
#define MAX_SIGN		33		//im_sign ����ĳ���
#define MAX_STRING		37
#define MAX_URL_LEN		1024
#define MAX_NAME_LEN	MAX_STRING+8  //�ļ�������չ��
#define MAX_ACC_HIS		20		//��������ʷ�������
#define MAX_UDP_MSG		512		//�㲥��Ϣ������ֽ���
#define MAX_IP_LEN		16		//IP��ַ������ַ�����
#define MAX_DEVICE_TOKEN	65
#define MAX_IOS_DEV		20
#define CMD_VERSION		100

#define IOSSERVER_URL	"http://172.20.10.236:9091/interface/push_msg/"
#define CMD_SESSIONID	"imove_home"
#define CLOUD_URL		"http://172.16.2.99:8000"
#define WAN_IFNAME		"eth1"
#define LAN_IFNAME		"br-lan"
#define SIGN_KEY		"D1FA1059ECDD12F6ECF5016317C89BA3"
#define MODULE			"bandwidthAcc"

#define IM_DEBUG_FLAG 1

#if IM_DEBUG_FLAG
#define IM_DEBUG(format, ...) \
    do { \
        fprintf(stderr, "%s [%s@%s,%d] " format "\n",\
        get_format_time(), __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while (0)  
#else
#define IM_DEBUG(format, ...)
#endif


//���ٿ���״̬
enum ACC_OPEN_STATE				
{
	STATE_ACC_CLOSED = 1,		//�ѹر�
	STATE_ACC_OPENED,		//�ѿ���
	STATE_ACC_OPENING,		//���ڿ���
	STATE_ACC_QUERYING,	//��ѯ��
};


enum BROCAST_NOTIFY_TYPE
{
	NOTIFY_ACC_SUCCESS,		//xxx�����˴������
	NOTIFY_ACC_STOP,		//��������ѶϿ�
	NOTIFY_ACC_ISUP,		//THE ONE�Ѱ������
	NOTIFY_ACC_WILLSTOP,	//������ټ����ر�
	NOTIFY_ACC_END			//��������ѽ���
};

//JSON����KEYֵ����
enum JSON_IM_CMD
{
	JSON_HEADER,
	JSON_CMD,
	JSON_VER,
	JSON_SEQ,
	JSON_DEVICE,
	JSON_CODE,
	JSON_SESSIONID,
	JSON_SIGN,
	JSON_DATA,
	JSON_DEV_NAME,
	JSON_ISP,
	JSON_CUR_BANDWIDTH,
	JSON_ACC_BANDWIDTH,
	JSON_ACC_REMAINTIME,
	JSON_IS_OPEN,
	JSON_MAX_BANDWIDTH,
	JSON_IS_ACC,
	JSON_TOTAL_FLOW,
	JSON_REMAIN_FLOW,
	JSON_UNGET_FLOW,
	JSON_GET_FLOW_ID,
	JSON_GET_FLOW_INFO,
	JSON_PEAK_FLOW,
	JSON_IS_KEEPACC,
	JSON_ACC_LIST,
	JSON_USE_TIME,
	JSON_USE_FLOW,
	JSON_OPEN_ACC_TIME,
	JSON_SEND_INFORM,
	JSON_SEND_INFORM_ID,
	JSON_SEND_INFORM_MSG,
	JSON_ACC_TIME,
	JSON_ACC_URL,
	JSON_ACC_STATE,
	JSON_IP,
	JSON_DEVICETOKEN,
	JSON_DEVID,
	JSON_TASKID,
	JSON_BADGE,
	JSON_MESSAGE,
	JSON_RAND,
	JSON_MODELE
};


//������Ϣ������
enum MSG_CODE_CLOUD
{
	MSG_CODE_SUCESS,
	MSG_CODE_UNKNOWN,					//δ֪����
	MSG_CODE_TIMEOUT,
	MSG_CODE_FORMAT_ERR,				//��ʽ����
	MSG_CODE_NO_ATTR,					//ȱ������
	MSG_CODE_IVALIDEDEVID,				//�Ƿ����豸ID
	MSG_CODE_INVALIDSES,				//session ��ʧЧ
	MSG_CODE_MSGDELAY,					//��Ϣ�ӳٳ�������Ҫ��ѯ
	MSG_CODE_NO_TRAFFIC,				//û�п���ȡ������
	MSG_CODE_TRAFFIC_ACCEPTED,			//�������ѱ�����
	MSG_CODE_ACCELERATED,			//���豸�ѱ����ٹ�
	MSG_CODE_ACCELERATING,				//���豸�ڼ�����
	MSG_CODE_SPEEDUP_FAIL,				//����ʧ�ܣ��õ�����֧��
	MSG_CODE_SPEEDUP_MAX				//����ʧ�ܣ��Ѵ�����ٶ�//��ʱ
};

enum MSG_CODE_TERM
{
	MSG_CODE_FORMAT_ERR_TERM = 110,				//��ʽ����
	MSG_CODE_NO_ATTR_TERM,					//ȱ������
	MSG_CODE_IVALIDEDEVID_TERM,				//�Ƿ����豸ID
	MSG_CODE_INVALIDSES_TERM,				//session ��ʧЧ
	MSG_CODE_MSGDELAY_TERM,					//��Ϣ�ӳٳ�������Ҫ��ѯ
	MSG_CODE_NO_TRAFFIC_TERM,				//û�п���ȡ������
	MSG_CODE_TRAFFIC_ACCEPTED_TERM,			//�������ѱ�����
	MSG_CODE_ACCELERATED_TERM,			//���豸�ѱ����ٹ�
	MSG_CODE_ACCELERATING_TERM,				//���豸�ڼ�����
	MSG_CODE_SPEEDUP_FAIL_TERM,				//����ʧ�ܣ��õ�����֧��
	MSG_CODE_SPEEDUP_MAX_TERM				//����ʧ�ܣ��Ѵ�����ٶ�
};




	
enum CLOUD_MSG_CMD  
{
	CMD_DEV_LOGIN = 500,				//�豸��¼����1280
	CMD_DEV_LOGOUT,						//�豸�ǳ�
	CMD_HEART_BEAT,						//�豸����
	CMD_IS_ACC,							//����Ƿ��ܼ���
	CMD_START_ACC,						//��������
	CMD_STOP_ACC,						//ֹͣ����
	CMD_COUNT_FLOW,						//��������ͳ��
	CMD_GETACC_FLOW,					//��ȡ���������ӿ�
	CMD_GETFLOW_INFO,					//��ȡ�������
	CMD_GET_ACCLIST,					//��ȡ������ʷ
	CMD_GET_ACCINFO,					//��ȡ��������510
	CMD_GET_TASKRESULT,					//��ȡ�����ѯ���
	CMD_UNGETFLOW,						//��ѯδ��ȡ����
	CMD_GETFLOW							//��ȡ����
};

enum TERM_MSG_CMD  
{
	TERM_CMD_START_ACC = 0x600,				//��������1536
	TERM_CMD_STOP_ACC,						//ֹͣ����
	//TERM_CMD_SPEEDUP_INFO,				//��ѯ���ٷ��������
	//TERM_CMD_IS_ACC,							//����Ƿ��ܼ���
	TERM_CMD_ACC_STATE_INFO,						//��ѯ���ٷ���״̬�ӿ�
	TERM_CMD_GETFLOW_INFO,				//��ѯ������Ϣ
	TERM_CMD_UNGETTEDFLOW,				//��ѯδ��ȡ����
	TERM_CMD_GETFLOW,					//��ȡ����
	TERM_CMD_PEAKFLOW,					//��ѯ������ֵ
	TERM_CMD_KEEPACC,					//���ֳ�ʱ�����
	TERM_CMD_GET_ACCLIST,					//��ȡ������ʷ
	TERM_CMD_IOSINFO					//IOSר�÷�����Ϣ�ӿ�
};

//������ʷ�ṹ����
typedef struct
{
	int 		im_use_time;		//ʹ��ʱ���
unsigned long 	im_use_flow;		//ʹ�õ�����
unsigned long	im_open_acc_time;	//�����������ʱ��

}S_IM_DATA_ACC_HISTROY;  

//��ѯδ��ȡ������
typedef struct
{
	char 	im_getflow_id[MAX_STRING];
	char	im_getflow_info[MAX_STRING];
}S_IM_TERMSG_DATA_GETFLOW;	

//����֪ͨ����
typedef struct
{
	char 	im_sendinform_id[MAX_STRING];
	char	im_sendinform_msg[MAX_UDP_MSG];
}S_IM_TERMSG_DATA_SENDINFORM;	


//��������ṹ��
typedef struct 
{
	int 	im_cmd;
	int 	im_ver;
	int 	im_seq;
	int 	im_device;
	int 	im_code;
	char 	im_sessionid[MAX_SESSIONID];
	char 	im_sign[MAX_SIGN];
}S_IM_MSG_HEADER;

/************************************
���ն˽��յ�����Ϣ
*************************************/
//�������ٻ�ֹͣ����
typedef struct
{
	char 	im_dev_name[MAX_NAME_LEN];
}S_IM_TERMSG_DATA_SPEEDUP_CMD;  


//�������ٷ���ֵ�ṹ
typedef struct
{
	int     im_isp;
	int 	im_cur_bandwidth;				//��ǰ����
	int		im_max_bandwidth;					//���ٺ�������
	int 	im_remaintime;					//����ʣ��ʱ��
	int		im_acc_bandwidth;
}S_IM_TERMSG_DATA_STARTACC_RESP;  		

//��ѯ���ٷ���״̬�ӿڷ��ؽṹ
typedef struct
{
	int		im_acc_state;
	int		im_is_acc;							//�Ƿ�֧�ּ���
	int     im_isp;								//ISP�ṩ��1���ţ�2��ͨ
	int 	im_cur_bandwidth;					//��ǰ����
	int		im_max_bandwidth;					//���ٺ��ܴﵽ��������
	int		im_acc_bandwidth;					//���ٺ����
	int		im_acc_remaintime;					//����ʣ��ʱ��
}S_IM_TERMSG_DATA_SPEEDUP_INFO_RESP;	


//��ѯ������Ϣ���ؽṹ
typedef struct
{
unsigned long     	im_total_flow;			//������
unsigned long		im_remain_flow;			//ʣ������
	int				im_acc_remaintime;		//����ʣ��ʱ��
}S_IM_TERMSG_DATA_FLOW_INFO_RESP;  		

//��ѯδ��ȡ�������ؽṹ
typedef struct
{
	S_IM_TERMSG_DATA_GETFLOW     im_unget_flow;			//δ��ȡ����
}S_IM_TERMSG_DATA_UNGETFLOW_RESP;  		


//��ȡ��������ṹ
typedef struct
{
	char 	im_getflow_id[MAX_STRING];			//��ȡ������ID
	char 	im_dev_name[MAX_NAME_LEN];			//�ͻ����豸��ʶ
}S_IM_TERMSG_DATA_GETFLOW_CMD;  		

//IOSר�÷�����Ϣ�ӿ�
typedef struct
{
	char 	devicetoken[MAX_DEVICE_TOKEN];		//IOS�豸tokenֵ
	char 	devid[MAX_SESSIONID];
}S_IM_TERMSG_DATA_IOSINFO_CMD;  		


//��ȡ�������ؽṹ
typedef struct
{
	char 	im_dev_name[MAX_NAME_LEN];			//�ͻ����豸��ʶ
}S_IM_TERMSG_DATA_GETFLOW_RESP;  		


//��ѯ������ֵ���ؽṹ
typedef struct
{
	unsigned long		im_peak_flow;			//������ֵ
}S_IM_TERMSG_DATA_PEAKFLOW_RESP;  		


//���ֳ�ʱ���������ṹ
typedef struct
{
	int		im_is_keepacc;			//�Ƿ񱣳ֳ�ʱ�����
}S_IM_TERMSG_DATA_KEEPACC_CMD;  		



//��ȡ������ʷ�ӿڷ��ؽṹ
typedef struct
{
	S_IM_DATA_ACC_HISTROY		acc_list[MAX_ACC_HIS];			//������ֵ
}S_IM_TERMSG_DATA_ACCLIST_RESP;  		




/************************************
���͵��ƶ˵���Ϣ
*************************************/
//�豸��¼����ṹ
typedef struct
{
	char im_dev_id[MAX_STRING];			//�豸ID
	char im_ip[MAX_IP_LEN];				//�豸�ı���IP
}S_IM_CLOUD_DATA_LOGIN_CMD; 		

//�豸��¼���ؽṹ
typedef struct
{
	char im_session_id[MAX_STRING];		//�ỰID
	int im_acc_time;					//�Ѽ��ٵ�ʱ��
	char im_ip[MAX_IP_LEN];				//����IP
	
}S_IM_CLOUD_DATA_LOGIN_RESP; 		

//�豸�ǳ�����ṹ
typedef struct
{
	//�˽ṹ��ʹ�ã�ʹ�õ�¼�����sessionid 
	char im_session_id[MAX_STRING];			//�ỰID
}S_IM_CLOUD_DATA_LOGOUT_CMD; 		

//��������ṹ
typedef struct
{
	//�˽ṹ��ʹ�ã�ʹ�õ�¼�����sessionid
	char im_session_id[MAX_STRING];			//�ỰID
}S_IM_CLOUD_DATA_HEARTBEAT_CMD; 	

//�������ؽṹ
typedef struct
{
	char im_ip[MAX_IP_LEN];				//����IP
}S_IM_CLOUD_DATA_HEARTBEAT_RESP; 		


//��ѯ�Ƿ��ܼ���/������������ṹ
typedef struct
{
	char im_session_id[MAX_STRING];			//�ỰID
	char im_dev_id[MAX_STRING];			//�豸ID
	char im_ip[MAX_STRING];				//�豸��IP
}S_IM_CLOUD_DATA_ACC_CMD; 		

//��������ͳ��
typedef struct
{
	char im_session_id[MAX_STRING];			//�ỰID
	unsigned long im_use_flow;					//�˴μ�����ʹ�õ����KKB	
}S_IM_CLOUD_DATA_COUNTFLOW_CMD; 		


//��ȡ���������ӿ�
typedef struct
{
	char im_session_id[MAX_STRING];			//�ỰID
	char im_task_id[MAX_STRING];			//�˴μ�����ʹ�õ�����	
}S_IM_CLOUD_DATA_GETAACFLOW_CMD; 		

//��ȡ�������鷵�ؽṹ
typedef struct
{
	char im_acc_url[MAX_STRING];			//����˵��
}S_IM_CLOUD_DATA_ACCSPEC_RESP; 		


typedef struct
{
	char	im_taskid[MAX_STRING];			//�첽��������ID
	char 	im_session_id[MAX_STRING];			//�ỰID
}S_IM_CLOUD_DATA_TASKRESULT_CMD;


//��ѯ�Ƿ��ܼ��ٷ��ؽṹ
typedef struct
{
	int		im_is_acc;							//�Ƿ�֧�ּ���
	int     im_isp;								//ISP�ṩ��1���ţ�2��ͨ
	int 	im_cur_bandwidth;					//��ǰ����
	int		im_max_bandwidth;					//���ٺ��ܴﵽ��������
	int		im_acc_remaintime;					//����ʣ��ʱ��
}S_IM_CLOUD_DATA_ISACC_RESP;  		



extern const char *jason_key[];
extern const char *brocast_notify[];

extern S_IM_TERMSG_DATA_SPEEDUP_CMD s_im_speedup;
extern S_IM_MSG_HEADER s_im_cmd_header;
extern S_IM_TERMSG_DATA_GETFLOW_CMD s_im_getflow;
extern S_IM_TERMSG_DATA_KEEPACC_CMD s_im_keepacc;
extern S_IM_TERMSG_DATA_IOSINFO_CMD	s_im_iosinfo[MAX_IOS_DEV];


extern S_IM_CLOUD_DATA_LOGIN_CMD	    s_im_login_cmd;
extern S_IM_CLOUD_DATA_LOGIN_RESP	    s_im_login_resp;
extern S_IM_CLOUD_DATA_ACC_CMD		    s_im_acc;
extern S_IM_CLOUD_DATA_ISACC_RESP       s_im_isacc_resp;
extern S_IM_TERMSG_DATA_STARTACC_RESP   s_im_startacc_resp;
extern S_IM_TERMSG_DATA_FLOW_INFO_RESP  s_im_flowinfo_resp;
extern S_IM_CLOUD_DATA_ACCSPEC_RESP     s_im_accspec;
extern S_IM_CLOUD_DATA_COUNTFLOW_CMD    s_im_countflow_cmd;
extern S_IM_CLOUD_DATA_GETAACFLOW_CMD   s_im_getaacflow_cmd;
extern S_IM_TERMSG_DATA_SPEEDUP_INFO_RESP s_im_speedupinfo_resp;
extern S_IM_TERMSG_DATA_UNGETFLOW_RESP  s_im_ungetflow_resp;
extern S_IM_TERMSG_DATA_PEAKFLOW_RESP	s_im_peakflow_resp;
extern S_IM_TERMSG_DATA_ACCLIST_RESP	s_im_acclist_resp;
extern S_IM_CLOUD_DATA_HEARTBEAT_RESP	s_im_heartbeat_resp;
extern S_IM_CLOUD_DATA_TASKRESULT_CMD   s_im_taskresult_cmd;

extern char uci_save_ip[MAX_IP_LEN];
char * get_format_time();


extern 	int im_check_isnet_change(void);
#endif
