#ifndef FUCNTION_H
#define FUCNTION_H

// IdleServer Functions
namespace idleserver
{
	std::string stats_proc(std::string fileName);
    bool stats_http(CURL *curl, FILE *curlF, const unsigned short &port, bool &status);
}

#endif
