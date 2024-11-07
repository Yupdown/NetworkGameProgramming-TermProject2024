#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "OpenGL32.Lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma warning(disable: 4711 4710 4100)	
//#pragma comment(lib, "freeglut.lib")
#pragma comment(lib,"fmod_vc.lib")

#ifdef _DEBUG
#pragma comment(lib,"assimp-vc143-mtd.lib")
#else
#pragma comment(lib,"assimp-vc143-mt.lib")
#endif 

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp> 
#include <glm/gtc/type_ptr.hpp> 

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <FMOD/fmod.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h" 
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"

#include <nlohmann/json.hpp>

#include <thread>
#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <execution>
#include <atomic>
#include <span>
using std::jthread;
using std::thread;
using std::vector;
using std::unique_ptr;
using std::make_unique;
#include <string>
using std::string;
using std::wstring;
#include <string_view>
using std::string_view;
using std::wstring_view;
#include <cmath>
#include <cassert>
#include <map>
#include <algorithm>
#include <bitset>
#include <unordered_map>
#include <optional>
#include <utility>
#include <variant>
#include <array>
#include <functional>
#include <any>
using std::bind;
using std::any;
using std::function;
using std::array;
using std::variant;
using std::get;
using std::optional;
using std::unordered_map;
using std::bitset;
using std::map;
using std::make_pair;
using std::shared_ptr;
using std::weak_ptr;
using std::unordered_multimap;
using std::multimap;
#include <sstream>
using std::stringstream;
#include <fstream>
using std::ifstream;
using std::ofstream;
#include <coroutine>
#include <list>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <cmath>
#include <ranges>
#include <format>
namespace fs = std::filesystem;
#include <concepts>
#include <type_traits>
#include <typeinfo>
#include <typeindex>
#include <bitset>
#include <future>
#include <cfloat>
#include <span>
#include <random>
#include <semaphore>
#include <compare>
#include <set>
#include <variant>
#include <unordered_set>
#include <shared_mutex>
using std::span;
using std::make_shared;
using std::enable_shared_from_this;
using std::wofstream;
using std::wifstream;
using std::list;
#include <initializer_list>
using std::initializer_list;
using std::pair;

#include "Singleton.hpp"
#include "define.h"
#include "Vertex.hpp"
#include "func.h"
#include "MyStb.h"
#include "CustomMath.hpp"
#include "SpinLock.hpp"
#include "CoRoutine.hpp"
#include "Delegate.hpp"
#include "CircleQueue.hpp"
#include "Animation.h"
#include "BoneInfo.h"

#define Mgr(type)	(type::GetInst())