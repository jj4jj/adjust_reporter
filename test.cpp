#include <stdio.h>
#include "adjust_report.h"




int main(int argc, char **argv)
{
    adjust_config_t ac;
    int ret = adjust_init(ac);
    if (ret){
        return -1;
    }

    //https://docs.adjust.com/en/event-tracking/#deduplicating-purchase-events
    //http://s2s.adjust.com/revenue  (revenue events)
    //http://s2s.adjust.com/event    (non-revenue events)
    adjust_report("http://s2s.adjust.com/revenue ", ...);




    adjust_destroy();
    return 0;
}

