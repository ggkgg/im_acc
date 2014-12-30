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


#define MAX_SESSIONID   33		//im_sessionid����ĳ���
#define MAX_SIGN		33		//im_sign ����ĳ���
#define MAX_STRING		33
#define MAX_URL_LEN		1024
#define MAX_NAME_LEN	MAX_STRING+8  //�ļ�������չ��
#define MAX_ACC_HIS		20		//��������ʷ�������
#define MAX_UDP_MSG		512		//�㲥��Ϣ������ֽ���
#define MAX_IP_LEN		16		//IP��ַ������ַ�����

#define CMD_SESSIONID	"imove_home"
#define CLOUD_URL		"http://172.16.2.79:80"

#define IM_DEBUG_FLAG 1

#if IM_DEBUG_FLAG
#define IM_DEBUG(format, ...) \
    do { \
        fprintf(stderr, "[%s@%s,%d] " format "\n", \
             __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while (0)  
#else
#define IM_DEBUG(format, ...)
#endif

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
	JSON_GET_FLOW,
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
	JSON_ACC_URL
};


//������Ϣ������
enum MSG_CODE  
{
	MSG_CODE_SUCESS,
	MSG_CODE_UNKNOWN,					//δ֪����
	MSG_CODE_TIMEOUT,					//��ʱ
	MSG_CODE_FORMAT_ERR,				//��ʽ����
	MSG_CODE_NO_ATTR,					//ȱ������
	MSG_CODE_NO_TRAFFIC,				//û�п���ȡ������
	MSG_CODE_TRAFFIC_ACCEPTED,			//�������ѱ�����
	MSG_CODE_ACCELERATED=10,			//���豸�ѱ����ٹ�
	MSG_CODE_ACCELERATING,				//���豸�ڼ�����
	MSG_CODE_SPEEDUP_FAIL,				//����ʧ�ܣ��õ�����֧��
	MSG_CODE_SPEEDUP_MAX				//����ʧ�ܣ��Ѵ�����ٶ�
};

enum CLOUD_MSG_CMD  
{
	CMD_DEV_LOGIN = 500,				//�豸��¼����
	CMD_DEV_LOGOUT,						//�豸�ǳ�
	CMD_HEART_BEAT,						//�豸����
	CMD_IS_ACC,							//����Ƿ��ܼ���
	CMD_START_ACC,						//��������
	CMD_STOP_ACC,						//ֹͣ����
	CMD_COUNT_FLOW,						//��������ͳ��
	CMD_GETACC_FLOW,					//��ȡ���������ӿ�
	CMD_GETFLOW_INFO,					//��ȡ�������
	CMD_GET_ACCLIST,					//��ȡ������ʷ
	CMD_GET_ACCINFO						//��ȡ��������510
};

enum TERM_MSG_CMD  
{
	TERM_CMD_START_ACC = 600,				//��������
	TERM_CMD_STOP_ACC,						//ֹͣ����
	TERM_CMD_SPEEDUP_INFO,				//��ѯ���ٷ��������
	TERM_CMD_IS_ACC,							//����Ƿ��ܼ���
	TERM_CMD_GETFLOW_INFO,				//��ѯ������Ϣ
	TERM_CMD_UNGETTEDFLOW,				//��ѯδ��ȡ����
	TERM_CMD_GETFLOW,					//��ȡ����
	TERM_CMD_PEAKFLOW,					//��ѯ������ֵ
	TERM_CMD_KEEPACC,					//���ֳ�ʱ�����
	TERM_CMD_GET_ACCLIST,					//��ȡ������ʷ
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
	char 	im_cur_bandwidth[MAX_STRING];	//��ǰ����
	char	im_bandwidth[MAX_STRING];		//���ٺ����
	int 	im_remaintime;					//����ʣ��ʱ��
}S_IM_TERMSG_DATA_STARTACC_RESP;  		


//��ѯ���ٷ�����������ؽṹ
typedef struct
{
	int		im_is_open;						//�Ƿ���
	int     im_isp;							//ISP�ṩ��1���ţ�2��ͨ
	char 	im_cur_bandwidth[MAX_STRING];	//��ǰ����
	char	im_bandwidth[MAX_STRING];		//���ٺ����
	int 	im_remaintime;					//����ʣ��ʱ��
}S_IM_TERMSG_DATA_SPEEDUP_INFO_RESP;  		


//��ѯ�Ƿ��ܼ��ٷ��ؽṹ
typedef struct
{
	int		im_is_acc;							//�Ƿ�֧�ּ���
	int     im_isp;								//ISP�ṩ��1���ţ�2��ͨ
	char 	im_cur_bandwidth[MAX_STRING];		//��ǰ����
	char	im_max_bandwidth[MAX_STRING];		//���ٺ����
	int		im_acc_remaintime;					//����ʣ��ʱ��
}S_IM_TERMSG_DATA_ISACC_RESP;  		

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
	S_IM_TERMSG_DATA_GETFLOW     im_get_flow;			//δ��ȡ����
}S_IM_TERMSG_DATA_UNGETTEDFLOW_RESP;  		


//��ȡ��������ṹ
typedef struct
{
	char 	im_getflow_id[MAX_STRING];			//��ȡ������ID
	char 	im_dev_name[MAX_NAME_LEN];			//�ͻ����豸��ʶ
}S_IM_TERMSG_DATA_GETFLOW_CMD;  		

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
	char im_ip[MAX_IP_LEN];				//�豸��IP
}S_IM_CLOUD_DATA_LOGIN_CMD; 		

//�豸��¼���ؽṹ
typedef struct
{
	char im_session_id[MAX_STRING];		//�ỰID
	int im_acc_time;					//�Ѽ��ٵ�ʱ��
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






extern const char *jason_key[];
extern S_IM_TERMSG_DATA_SPEEDUP_CMD s_im_speedup;
extern S_IM_MSG_HEADER s_im_cmd_header;
extern S_IM_TERMSG_DATA_GETFLOW_CMD s_im_getflow;
extern S_IM_TERMSG_DATA_KEEPACC_CMD s_im_keepacc;

extern S_IM_CLOUD_DATA_LOGIN_CMD	 s_im_login_cmd;
extern S_IM_CLOUD_DATA_LOGIN_RESP	s_im_login_resp;
extern S_IM_CLOUD_DATA_ACC_CMD		 s_im_acc;
extern S_IM_TERMSG_DATA_ISACC_RESP  s_im_isacc_resp;
extern S_IM_TERMSG_DATA_STARTACC_RESP s_im_startacc_resp;
extern S_IM_TERMSG_DATA_FLOW_INFO_RESP s_im_flowinfo_resp;
extern S_IM_CLOUD_DATA_ACCSPEC_RESP s_im_accspec;
extern S_IM_CLOUD_DATA_COUNTFLOW_CMD s_im_countflow_cmd;
extern S_IM_CLOUD_DATA_GETAACFLOW_CMD s_im_getaacflow_cmd;
extern S_IM_TERMSG_DATA_SPEEDUP_INFO_RESP s_im_speedupinfo_resp;
#endif
