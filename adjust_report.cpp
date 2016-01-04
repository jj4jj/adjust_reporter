#include "adjust_report.h"

#include "curl/curl.h"
#include "mmqueue/mmqueue.h"

static void *pull_one_url(void *url)
{
    CURL *curl;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_perform(curl); /* ignores error */
    curl_easy_cleanup(curl);

    return NULL;
}


curl_global_init(CURL_GLOBAL_ALL);
