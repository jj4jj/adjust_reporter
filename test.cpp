#include <stdio.h>
#include <iostream>

#include "adjust_report.h"
using namespace std;

int main(int argc, char **argv)
{
    adjust_config_t ac;
    ac.sharekey = "/tmp";
    ac.max_thread = 2;
    ac.max_queue_buff_size = 1024 * 1024 * 16; //16MB
    ac.reprot_event_url = "http://s2s.adjust.com/event";
    ac.report_revenue_url = "http://s2s.adjust.com/revenue";
    int ret = adjust_init(ac);
    if (ret){
        return -1;
    }

    adjust_event_t evt;
    evt.type = adjust_event_t::EVENT;
    evt.base.app_token = "4w565xzmb54d";
    evt.base.event_token = "fOob4r";
    evt.devid.type = adjust_event_t::adjust_event_udid_t::IDFA;
    evt.devid.udid.idfa = "D2CADB5F-410F-4963-AC0C-2A78534BDF1E";

    int cbid = adjust_report(&evt);
    cout << "cbid1: " << cbid << endl;
    evt.type = adjust_event_t::REVENUE;
    evt.revenue.currency = "USD";
    evt.revenue.environment = "sandbox";
    evt.revenue.revenue = "120";
    cbid = adjust_report(&evt);
    cout << "cbid2: " << cbid << endl;

    adjust_destroy();
    return 0;
}

