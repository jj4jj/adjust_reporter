#include "adjust_report.h"
#include "curl/curl.h"
#include "mmqueue/mmqueue.h"
#include <string>
#include <cstring>
#include <strings.h>
#include <cstdarg>
#include <algorithm>
using namespace std;

typedef void(*adjust_on_report_t)(void *ud, int id, const char * json);

struct  adjust_report_t {
    std::string key;
    std::string event_url;
    std::string revenue_url;
    int                 max_threads;
    mmqueue_t *         mmq;
    adjust_on_report_t  cb;
    void *              cb_ud;
    int                 seqid;
} G;

struct adjust_work_input_t {
    int         seqid;
    char        url[1];
};
struct adjust_work_output_t {
    int         seqid;
    char        data[1];
};
#define ADJUST_REPORT_MAX_MSG_SZ        (512)
#define ADJUST_RESPONSE_MAX_MSG_SZ       (256)
static size_t write_to_output(char * data, size_t size, size_t nmemb, void *output){
    size_t w = std::min((int)(size*nmemb), ADJUST_RESPONSE_MAX_MSG_SZ);
    memcpy(output, data, w);
    if(w < ADJUST_RESPONSE_MAX_MSG_SZ){
        ((char*)output)[w] = 0;
    }
    else {
        ((char*)output)[ADJUST_RESPONSE_MAX_MSG_SZ-1] = 0;
    }
    return w;
}
static void _adjust_report(void * ctx, int idx, char * omsg, size_t * omsg_szp, 
                            const char * inmsg, size_t inmsg_sz){
    CURL *curl = curl_easy_init();
    adjust_work_input_t * input = (adjust_work_input_t*)inmsg;
    adjust_work_output_t * output = (adjust_work_output_t*)omsg;
    output->seqid = input->seqid;
    output->data[0] = 0;
    /* write to this file */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output->data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_output);
    curl_easy_setopt(curl, CURLOPT_URL, input->url);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK){
        //error
        return;
    }
}
int     adjust_init(const adjust_config_t & conf){
    if (G.mmq){
        return -1;
    }
    curl_global_init(CURL_GLOBAL_ALL);
    G.key = conf.sharekey;
    mmqueue_config_t mmconf;
    mmconf.key = (char*)G.key.data();
    mmconf.max_input_msg_size = ADJUST_REPORT_MAX_MSG_SZ;
    mmconf.max_output_msg_size = ADJUST_RESPONSE_MAX_MSG_SZ;
    mmconf.max_queue_size = conf.max_queue_buff_size / mmconf.max_input_msg_size;
    mmconf.max_worker = conf.max_thread;
    G.max_threads = conf.max_thread;
    G.mmq = mmqueue_create(&mmconf, _adjust_report, nullptr);
    if (G.mmq){
        return 0;
    }
    G.seqid = 1;
    return -1;
}
void    adjust_set_report_cb(adjust_on_report_t cb, void * ud){
    G.cb = cb;
    G.cb_ud = ud;
}
int     adjust_poll(int maxproc){
    static char _msg_buffer[4096];
    size_t _msg_sz = sizeof(_msg_buffer);
    while (maxproc--){
        if (mmqueue_take(G.mmq, _msg_buffer, &_msg_sz)){
            return maxproc;
        }
        if (G.cb){
            adjust_work_output_t * p = (adjust_work_output_t*)_msg_buffer;
            G.cb(G.cb_ud, p->seqid, p->data);
        }
    }
    return 0;
}
static const char * _make_url_param(char * buff, int buffsz, ...){
    CURL *curl = curl_easy_init();
    if (!curl) {
        return nullptr;
    }

    va_list ap;
    va_start(ap, buffsz);
    const char * pkey = va_arg(ap, const char *);
    const char * pval = va_arg(ap, const char *);
    int c = strlen(buff);
    while (pkey && pval){
        char *output = curl_easy_escape(curl, pkey, strlen(pkey));
        if (!output) {
            curl_easy_cleanup(curl);
            return nullptr;
        }
        strncpy(buff + c, output, buffsz - c - 1);
        c += strlen(output);
        curl_free(output);
        buff[c] = '=';
        c++;
        output = curl_easy_escape(curl, pval, strlen(pval));
        if (!output) {
            curl_easy_cleanup(curl);
            return nullptr;
        }
        strncpy(buff + c, output, buffsz - c - 1);
        c += strlen(output);
        curl_free(output);
        buff[c] = 0;
        pkey = va_arg(ap, const char *);
        if (pkey){
            pval = va_arg(ap, const char *);
        }
        else {
            pval = nullptr;
        }
    }
    va_end(ap);
    curl_easy_cleanup(curl);
    return buff;
}
int     adjust_report(const adjust_event_t * evt){
    static char input_msg_buff[4096];
    adjust_work_input_t * input = (adjust_work_input_t *)input_msg_buff;
    if (G.seqid <= 0){
        G.seqid = 1;
    }
    int seqid = G.seqid;
    input->seqid = seqid;
    input->url[0] = 0;
    if (evt->type == adjust_event_t::REVENUE){
        strcpy(input->url, G.revenue_url.c_str());
        strcat(input->url, "?");
    }
    else {
        strcpy(input_msg_buff, G.event_url.c_str());
        strcat(input->url, "?");
    }

    char bufftime[32];
    time_t tn = time(NULL);
    struct tm _tmp;
    struct tm * ntm = localtime_r(&tn, &_tmp);
    const char * udidk = "idfa";
    const char * udidv = "";
    switch (evt->devid.type){
    case adjust_event_t::adjust_event_udid_t::IDFA:
        udidk = "idfa";
        udidv = evt->devid.udid.idfa;
        break;
    case adjust_event_t::adjust_event_udid_t::IDFV:
        udidk = "idfv";
        udidv = evt->devid.udid.idfv;
        break;
    case adjust_event_t::adjust_event_udid_t::MAC:
        udidk = "mac";
        udidv = evt->devid.udid.mac;
        break;
    case adjust_event_t::adjust_event_udid_t::MAC_MD5:
        udidk = "mac_md5";
        udidv = evt->devid.udid.mac_md5;
        break;
    case adjust_event_t::adjust_event_udid_t::MAC_SHA1:
        udidk = "mac_sha1";
        udidv = evt->devid.udid.mac_sha1;
        break;
    case adjust_event_t::adjust_event_udid_t::ANDROID_ID:
        udidk = "android_id";
        udidv = evt->devid.udid.android_id;
        break;
    case adjust_event_t::adjust_event_udid_t::GPS_ADID:
        udidk = "gps_adid";
        udidv = evt->devid.udid.gps_adid;
        break;
    }
    strftime(bufftime, sizeof(bufftime), "%FT%X%z", ntm);
    const char * param = _make_url_param(input->url, sizeof(input_msg_buff) - sizeof(adjust_work_input_t),
        "app_token", evt->base.app_token, 
        "event_token", evt->base.event_token,
        "s2s", "1",
        "created_at", bufftime,
        udidk, udidv,
        nullptr, nullptr);
    if(!param){
        return -1;
    }
    if (evt->type == adjust_event_t::REVENUE){
        param = _make_url_param(input->url, sizeof(input_msg_buff)-sizeof(adjust_work_input_t),
            "revenue", evt->revenue.revenue,
            "currency", evt->revenue.currency,
            "environment", evt->revenue.environment,
            nullptr, nullptr);
        if(!param){
            return -1;
        }
    }
    int ret = mmqueue_put(G.mmq, input_msg_buff, sizeof(adjust_work_input_t) + strlen(input->url));
    if (ret){
        return -1;
    }
    ++G.seqid;
    return seqid;
}
int     adjust_destroy(){
    if (G.mmq){
        mmqueue_destroy(G.mmq);
        G.mmq = nullptr;
    }
    return 0;
}
