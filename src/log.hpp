#pragma once
enum LEVEL { FATAL, ERROR, WARN, INFO, DEBUG, TRACE };

void log(int level, const char* format, ...);

#define LOGF(...) log(FATAL, __VA_ARGS__)
#define LOGE(...) log(ERROR, __VA_ARGS__)
#define LOGW(...) log(WARN, __VA_ARGS__)
#define LOGI(...) log(INFO, __VA_ARGS__)
#define LOGD(...) log(DEBUG, __VA_ARGS__)
#define LOGT(...) log(TRACE, __VA_ARGS__)
