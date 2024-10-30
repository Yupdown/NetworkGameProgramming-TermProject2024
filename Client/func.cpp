#include "pch.h"
#include "func.h"
#include "EventMgr.h"
#include "Core.h"
#include "Camera.h"

template<>
const ushort RemoveElement<shared_ptr<GameObj>>(vector<shared_ptr<GameObj>>& _delVec, const ushort _delIdx)noexcept
{
	Mgr(EventMgr)->AddDeadObj(std::move(_delVec[_delIdx]));
	_delVec[_delIdx].swap(_delVec.back());
	_delVec.pop_back();
	return _delIdx;
}

glm::vec3 RandomColor()
{
	static std::mt19937 rng{ std::random_device{}() };
	static std::uniform_int_distribution<> uid{ 0,4 };
	switch (uid(rng))
	{
	case 0:return RGBA_RED;
	case 1:return RGBA_GREEN;
	case 2:return RGBA_BLUE;
	case 3:return RGBA_WHITE;
	case 4:return RGBA_BLACK;
	default:
		break;
	}
	return RGBA_GRAY;
}

bool WaitSecondsHelper(WaitForSeconds*& _pWait)noexcept
{
	if (nullptr == _pWait)
	{
		return true;
	}
	_pWait->m_accTime += DT;
	return _pWait->m_accTime >= _pWait->m_waitTime ? !(_pWait = nullptr) : false;
}

void StartCoRoutine(string_view _strKey, CoRoutine&& _coTask)
{
	Mgr(EventMgr)->AddCoRoutine(_strKey, std::move(_coTask));
}

void StartCoRoutine(CoRoutine&& _coTask)
{
	Mgr(EventMgr)->AddCoRoutine(std::move(_coTask));
}

bool IsCollision2D(const glm::vec2& A_LT, const glm::vec2& A_RB, const glm::vec2& B_LT, const glm::vec2& B_RB)
{
	if (A_RB.x >= B_LT.x && A_LT.x <= B_RB.x && A_LT.y <= B_RB.y && A_RB.y >= B_LT.y)
	{
		return true;
	}
	return false;
}

std::tuple<glm::vec3, glm::quat, glm::vec3> Decompose(const glm::mat4& transformMat_)noexcept
{
	const glm::vec3 translation = glm::vec3(transformMat_[3]);

	const glm::mat4 rotationMatrix{
		glm::normalize(transformMat_[0]),
		glm::normalize(transformMat_[1]),
		glm::normalize(transformMat_[2]),
		glm::vec4{0.f,0.f,0.f,1.f}
	};

	const glm::vec3 scale{
	 glm::length(glm::vec3(transformMat_[0]))
	,glm::length(glm::vec3(transformMat_[1]))
	,glm::length(glm::vec3(transformMat_[2])) };
	
	return std::make_tuple(translation, glm::quat_cast(rotationMatrix), scale);
}

const glm::vec3 ScreenToOpenGL2D(const glm::vec2& mpos)noexcept
{
	const auto [width, height] = Mgr(Core)->GetWidthHeight();
	const auto [proj, view] = Camera::GetMainCamProjViewMat();
	const auto pos = glm::vec3{ (2.0f * mpos.x) / width - 1.0f ,1.0f - (2.0f * mpos.y) / height ,0.f };
	return glm::inverse(proj * view) * glm::vec4{ pos,1.f };
}

const glm::vec2 OpenGL2D2Screen(const glm::vec3& glpos) noexcept
{
	const auto [width, height] = Mgr(Core)->GetWidthHeight();
	const auto pCam = Camera::GetCurCam();
	const glm::mat4 projectionMatrix = pCam->GetCamMatProj();
	const glm::mat4 viewMatrix = pCam->GetCamMatView();
	const glm::vec4 clipSpacePoint = projectionMatrix * viewMatrix * glm::vec4(glpos, 1.0f);

	const glm::vec3 ndcSpacePoint = glm::vec3(clipSpacePoint) / clipSpacePoint.w;

	return glm::vec2{
		(ndcSpacePoint.x + 1.0f) * 0.5f * width,
		(1.0f - ndcSpacePoint.y) * 0.5f * height
	};
}

std::wstring string2wstring(std::string_view input)noexcept
{
	const int wideSize = MultiByteToWideChar(CP_UTF8, 0, input.data(), static_cast<int>(input.size()), NULL, 0);
	std::vector<wchar_t> wideString(wideSize);
	MultiByteToWideChar(CP_UTF8, 0, input.data(), static_cast<int>(input.size()), wideString.data(), wideSize);
	return std::wstring(wideString.begin(), wideString.end());
}

std::string wstring2string(std::wstring_view input)noexcept
{
	const int multiSize = WideCharToMultiByte(CP_UTF8, 0, input.data(), static_cast<int>(input.size()), NULL, 0, NULL, NULL);
	std::vector<char> multiString(multiSize);
	WideCharToMultiByte(CP_UTF8, 0, input.data(), static_cast<int>(input.size()), multiString.data(), multiSize, NULL, NULL);
	return std::string(multiString.begin(), multiString.end());
}

glm::vec3 GetMaxXYZ(const glm::vec3 v)noexcept
{
	const glm::vec3 absVec(::bitwise_absf(v.x), ::bitwise_absf(v.y), ::bitwise_absf(v.z));
	const auto maxIndex = std::distance(&absVec[0], std::max_element(&absVec[0], &absVec[0] + 3));
	glm::vec3 result(0);
	result[(const ushort)maxIndex] = 1;
	return result;
}

void LogStackTrace() noexcept
{
	const int MaxFrames = 64;
	void* stack[MaxFrames];
	USHORT frames = CaptureStackBackTrace(0, MaxFrames, stack, NULL);

	SymInitialize(GetCurrentProcess(), NULL, TRUE);

	SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	for (USHORT i = 0; i < frames; i++)
	{
		::SymFromAddr(GetCurrentProcess(), (DWORD64)(stack[i]), 0, symbol);
		std::cout << i << ": " << symbol->Name << " - 0x" << std::hex << symbol->Address << std::dec << std::endl;
	}

	free(symbol);
	SymCleanup(GetCurrentProcess());
}

const glm::vec3 NDC2World(const glm::vec3 ndc) noexcept
{
	const auto [proj, view] = Camera::GetMainCamProjViewMat();
	const glm::mat4 invVP = glm::inverse(proj * view); 
	const glm::vec4 worldCoords = invVP * glm::vec4(ndc, 1.0f); 
	return glm::vec3(worldCoords) / worldCoords.w; 
}

const short wrapAround(const short x, const short low, const short high)noexcept
{
	const short n = (x - low) % (high - low);
	return (n >= 0) ? (n + low) : (n + high);
}
