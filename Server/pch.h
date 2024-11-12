#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <mutex>
#include <functional>
#include <memory>
#include <shared_mutex>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include <format>
#include "func.h"

#pragma comment(lib, "Ws2_32.lib")

using std::shared_ptr;

template<typename T>
using S_ptr = std::shared_ptr<T>;

template <typename T>
using W_ptr = std::weak_ptr<T>;

#define USE_NAGOX_ASSERT

#ifdef USE_NAGOX_ASSERT

#define NAGOX_ASSERT(condition)                                 \
    do {                                                        \
        if (!(condition)) [[unlikely]] {                        \
            std::cerr << "Activate NagOx Assertion !" << '\n';  \
            *(int*)nullptr = 0;                                 \
        }                                                       \
    } while (0)

#else

#define NAGOX_ASSERT(condition)

#endif