#pragma once
#include "dcutils.hpp"
struct logger_t;

enum log_msg_level_type {
	LOG_LVL_PROF = 0,
	LOG_LVL_TRACE = 1,
	LOG_LVL_DEBUG = 2,
	LOG_LVL_INFO = 3,
	LOG_LVL_WARNING = 4,
	LOG_LVL_ERROR = 5,
	LOG_LVL_FATAL = 6,
	LOG_LVL_INVALID = 255
};
static inline const char *	STR_LOG_LEVEL(int lv){
	static const char * s_strlv[] = {
		"PROF", "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
	};
	if (lv < 0 || lv >= (int)(sizeof(s_strlv) / sizeof(s_strlv[0]))){
		return "";
	}
	return s_strlv[lv];
}

struct logger_config_t{
	string	dir;
	string	pattern;
	int		max_roll;
	int		max_msg_size;
	int		max_file_size;
	log_msg_level_type	lv;
	logger_config_t() :max_roll(20),
		max_msg_size(1024 * 1024), max_file_size(1024*1024*10){
		lv = LOG_LVL_TRACE;
	}
};

int				global_logger_init(const logger_config_t & conf);
void			global_logger_destroy();
logger_t *		global_logger();
//======================================================================
void            logger_lock(logger_t * logger = nullptr);
void            logger_unlock(logger_t * logger = nullptr);

logger_t *		logger_create(const logger_config_t & conf);
void			logger_destroy(logger_t *);
void			logger_set_level(logger_t *, log_msg_level_type level);
int				logger_level(logger_t * logger = nullptr);
//last msg
const char*		logger_msg(logger_t * logger = nullptr);
//last err
int				logger_errno(logger_t * logger = nullptr);
//set last
int				logger_write(logger_t *, int loglv, const char* fmt, ...);

/////////////////////////////////////////////////////////////////////////////////////////////////////////


//raw log
#ifndef LOGR
#define RAW_LOG_MSG_FORMAT_PREFIX	"%19s|%s|"
#define RAW_LOG_MSG_FORMAT_VALUES(tag)	dcsutil::strftime(_str_alloc_,err_tv_.tv_sec),(tag)

#define LOGR(log_lv_, format,...)	do{\
    if ((log_lv_) >= logger_level((nullptr))){\
        timeval err_tv_; gettimeofday(&err_tv_, NULL); std::string _str_alloc_; \
        logger_write((nullptr), (log_lv_), RAW_LOG_MSG_FORMAT_PREFIX format "\n", RAW_LOG_MSG_FORMAT_VALUES(STR_LOG_LEVEL((log_lv_))), ##__VA_ARGS__); \
    }\
} while (0)
#endif


//log to str
#ifndef LOGRSTR
#define LOGRSTR(str, tag, format,...)	do{\
    timeval err_tv_; gettimeofday(&err_tv_, NULL); std::string _str_alloc_; \
    dcsutil::strprintf((str), RAW_LOG_MSG_FORMAT_PREFIX format, RAW_LOG_MSG_FORMAT_VALUES(tag), ##__VA_ARGS__); \
} while (0)
#endif


//general log prefix and values
//2010-10-12T12:08:08.123456|TID@PID|DEBUG|FUNC:LINE|
#define LOG_MSG_FORMAT_PREFIX	"%s.%lu|%ld@%d|%s|%s:%d|%s|"
#define LOG_MSG_FORMAT_VALUES(tag)	dcsutil::strftime(_str_alloc_,err_tv_.tv_sec),err_tv_.tv_usec,gettid(),getpid(),(tag),basename(__FILE__),__LINE__,__FUNCTION__

//log to str
#ifndef LOGSTR
#define LOGSTR(str, tag, format,...)    do{\
	timeval err_tv_; gettimeofday(&err_tv_, NULL); std::string _str_alloc_; \
    dcsutil::strprintf((str), LOG_MSG_FORMAT_PREFIX format, LOG_MSG_FORMAT_VALUES(tag), ##__VA_ARGS__); \
} while (0)
#endif


//global logge
#ifndef LOG
#define LOG(logger_, log_lv_, format_, ...)	do{\
    if ((log_lv_) >= logger_level((logger_))){\
        timeval err_tv_; gettimeofday(&err_tv_, NULL); std::string _str_alloc_; \
		logger_write((logger_), (log_lv_), LOG_MSG_FORMAT_PREFIX format_ "\n", LOG_MSG_FORMAT_VALUES(STR_LOG_LEVEL(log_lv_)), ##__VA_ARGS__); \
    }\
}while (0)

////////////////////////////////////////////////////////////////////////////////////////////////

#define LOG_TRA(logger_, format_, ...)		LOG(logger_, LOG_LVL_TRACE, format_, ##__VA_ARGS__)
#define LOG_DBG(logger_, format_, ...)		LOG(logger_, LOG_LVL_DEBUG, format_, ##__VA_ARGS__)
#define LOG_IFO(logger_, format_, ...)		LOG(logger_, LOG_LVL_INFO, format_, ##__VA_ARGS__)
#define LOG_WAR(logger_, format_, ...)		LOG(logger_, LOG_LVL_WARNING, format_, ##__VA_ARGS__)
#define LOG_ERR(logger_, format_, ...)		LOG(logger_, LOG_LVL_ERROR, format_, ##__VA_ARGS__)
#define LOG_FTL(logger_, format_, ...)		LOG(logger_, LOG_LVL_FATAL, format_, ##__VA_ARGS__)



#endif



//global logger
#ifndef GLOG
#define GLOG(log_lv_, format_, ...)	LOG(nullptr, log_lv_, format_, ##__VA_ARGS__)
//////////////////////////////////////////////////////////////////////////////////

#define GLOG_TRA(format_, ...)		GLOG(LOG_LVL_TRACE, format_, ##__VA_ARGS__)
#define GLOG_DBG(format_, ...)		GLOG(LOG_LVL_DEBUG, format_, ##__VA_ARGS__)
#define GLOG_IFO(format_, ...)		GLOG(LOG_LVL_INFO, format_, ##__VA_ARGS__)
#define GLOG_WAR(format_, ...)		GLOG(LOG_LVL_WARNING, format_, ##__VA_ARGS__)
#define GLOG_ERR(format_, ...)		GLOG(LOG_LVL_ERROR, format_, ##__VA_ARGS__)
#define GLOG_FTL(format_, ...)		GLOG(LOG_LVL_FATAL, format_, ##__VA_ARGS__)

#endif



