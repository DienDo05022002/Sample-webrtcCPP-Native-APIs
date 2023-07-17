
#ifndef EXAMPLES_PEERCONNECTION_SERVER_UTILS_H_
#define EXAMPLES_PEERCONNECTION_SERVER_UTILS_H_

#include <stddef.h>
#include <string>

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

std::string int2str(int i);
std::string size_t2str(size_t i);

#endif  // EXAMPLES_PEERCONNECTION_SERVER_UTILS_H_
