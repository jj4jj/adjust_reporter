#pragma once

//this is just a part of adjust server sdk for event tracking
//write by jj4jj@github -> resc@vip.qq.com
//more detail info , please refer to as follow url:
//https://docs.adjust.com/en/event-tracking/#deduplicating-purchase-events


struct adjust_config_t {
    const char * sharekey;
    int          max_thread;
    int          max_queue_buff_size;
    const char * report_revenue_url;
    const char * reprot_event_url;
};

typedef void(*adjust_on_report_t)(void *ud, int id, const char * json);

struct adjust_event_t {
    enum adjust_event_type {
        EVENT, REVENUE
    } type;
    struct {
        const char *app_token;//adjust app token from dashboard;     4w565xzmb54d	���Կ������� adjust Ӧ��ʶ����
        const char *event_token;//adjust event token from dashboard;	f0ob4r	���Կ������� adjust �¼�ʶ����
    }  base; //requried
    struct adjust_event_udid_t{
        enum divid_type {
            IDFA,IDFV,MAC,MAC_MD5,MAC_SHA1,ANDROID_ID,GPS_ADID
        } type;
        union {
            const char * idfa;//iOS ID for Advertisers	D2CADB5F - 410F - 4963 - AC0C - 2A78534BDF1E	����̵� iOS ID	�� �� - ��
            const char * idfv;//iOS ID for Vendors	EF76726C - D952 - 451C - 8E1A - 4E86938BDC20	���ҵ� iOS ID	�� �� - ��
            const char * mac;//	15118fdce61d	�豸�� MAC ��ַ���� Android��	�̣����� �� : ��
            const char * mac_md5;//MAC address of device (Android only)	e3f5536a141811db40efd6400f1d0a4e	MAC �� MD5���� Android��	��д��ĸ������ �� : ��
            const char * mac_sha1;//MD5 of MAC (Android only)	c1976429369bfe063ed8b3409db7c7e7d87196d9	MAC �� SHA1���� Android��	��д��ĸ���� �� : ��
            const char * android_id;//SHA1 of MAC (Android only)	e1cbfb61613b4f50	Android ID
            const char * gps_adid;//Android ID	660e1d86 - 6796 - 463a - be86 - 897993136018	Google Play ����� ID	�� �� - ��
        } udid;
    } devid;
    struct {
        const char *revenue;//Value of revenue events, in full currency units 	�����¼���ֵ�������������ҵ�λ(150 = $ 150)
        const char *currency;//Currency code of revenue event, see note below; ISO 4217, e.g. USD or EUR	�����¼����Ҵ��룬�μ�����ע�ͣ�ISO 4217������USD��EUR
        const char *environment;//Environment to post the data to, ��sandbox�� or ��production��.	�������ݵĻ����� ��sandbox�� �� ��production��
    } revenue; //optional when type is revenue
};


int     adjust_init(const adjust_config_t & conf);
void    adjust_set_report_cb(adjust_on_report_t cb, void * ud);
int     adjust_poll(int maxproc = 200);
int     adjust_report(const adjust_event_t * evt);
int     adjust_destroy();


