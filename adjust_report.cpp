#include "adjust_report.h"
#include "curl/curl.h"
#include "mmqueue/mmqueue.h"
#include <string>
using namespace std;

typedef void(*adjust_on_report_t)(void *ud, int id, const char * json);

struct  adjust_report_t {
    std::string key;
    int                 max_threads;
    mmqueue_t *         mmq;
    adjust_on_report_t  cb;
    void *              cb_ud;
    CURL            * * curls;
} G;

static void _adjust_report(void * ctx, int idx, char * omsg, size_t * omsg_szp, 
                            const char * inmsg, size_t inmsg_sz){
    CURL *curl = G.curls[idx];
    //get url
    //curl_easy_setopt(curl, CURLOPT_URL, url);
    // curl_easy_perform(curl); /* ignores error */
    //http://s2s.adjust.com/revenue  (revenue events)
    //const char * report_url;

}

int     adjust_init(adjust_config_t & conf){
    if (G.mmq){
        return -1;
    }
    curl_global_init(CURL_GLOBAL_ALL);
    G.key = conf.sharekey;
    mmqueue_config_t mmconf;
    mmconf.key = (char*)G.key.data();
    mmconf.max_input_msg_size = 512;
    mmconf.max_output_msg_size = 128;
    mmconf.max_queue_size = conf.max_queue_buff_size / mmconf.max_input_msg_size;
    mmconf.max_worker = conf.max_thread;
    G.max_threads = conf.max_thread;
    G.curls = new CURL *[conf.max_thread];
    for (int i = 0; i < conf.max_thread; ++i){
        G.curls[i] = curl_easy_init();
    }
    G.mmq = mmqueue_create(&mmconf, _adjust_report, nullptr);
    if (G.mmq){
        return 0;
    }
    return -1;
}
void    adjust_set_report_cb(adjust_on_report_t cb, void * ud){
    G.cb = cb;
    G.cb_ud = ud;
}
int     adjust_poll(int maxproc = 200){
    static char _msg_buffer[4096];
    size_t _msg_sz = sizeof(_msg_buffer);
    while (maxproc--){
        if (mmqueue_take(G.mmq, _msg_buffer, &_msg_sz)){
            return maxproc;
        }
        if (G.cb){
#warning "todo get id"
            G.cb(G.cb_ud, 0, _msg_buffer);
        }
    }
    return 0;
}
int     adjust_report(const char * url, const char * json){
#warning  "todo"
    return 0;
}
int     adjust_destroy(){
    if (G.mmq){
        mmqueue_destroy(G.mmq);
        G.mmq = nullptr;
        for (int i = 0; i < G.max_threads; ++i){
            curl_easy_cleanup(G.curls[i]);
        }
        delete G.curls;
        G.curls = nullptr;
    }
    return 0;
}