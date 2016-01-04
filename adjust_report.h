#pragma once

struct adjust_config_t {
    const char * sharekey;
    int          max_thread;
};

typedef void(*adjust_on_report_t)(void *ud, int id, const char * json);
//http://s2s.adjust.com/revenue  (revenue events)
//const char * report_url;

int     adjust_init(adjust_config_t & conf);
void    adjust_set_report_cb(adjust_on_report_t cb, void * ud);
int     adjust_poll(int maxproc = 200);
int     adjust_report(const char * url, const char * json);
int     adjust_destroy();


