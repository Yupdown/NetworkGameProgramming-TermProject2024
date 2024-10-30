#pragma once

template <typename T> requires std::is_enum<T>::value
constexpr int etoi(T _eType) { return static_cast<int>(_eType); }
class GameObj;

template<typename T>
const ushort RemoveElement(vector<T>& _delVec,const ushort _delIdx)noexcept {
	_delVec[_delIdx] = std::move(_delVec.back());
	_delVec.pop_back();
	return _delIdx;
}

template<>
const ushort RemoveElement<shared_ptr<GameObj>>(vector<shared_ptr<GameObj>>& _delVec, const ushort _delIdx)noexcept;


template<typename T,typename Fp>
void RemoveElementAll(vector<T>& _delVec,Fp _bFp)
{
	for (ushort idx = 0; idx < (const ushort)_delVec.size();)
	{
		if (_bFp(_delVec[idx]))
		{
			idx = RemoveElement(_delVec, idx);
		}
		else
		{
			++idx;
		}
	}
}

template <typename T, typename Compare>
inline void insertionSort(T& arr_, Compare&& cmp_) noexcept
{
	using ele = std::remove_cvref_t<decltype(arr_[0])>;
	std::span<ele> arr_s = arr_;
	const short n = static_cast<const short>(arr_s.size());
	const auto arr = arr_s.data();
	std::byte key[sizeof(ele)];
	for (short i = 1; i < n; ++i)
	{
		ele* const current = arr + i;
		short j = i - 1;
		while (j >= 0 && cmp_(*current, arr[j])) {
			--j;
		}
		if (j != i - 1) {
			memcpy(key, current, sizeof(ele));
			memmove(arr + j + 2, arr + j + 1, (i - j - 1) * sizeof(ele));
			memcpy(arr + j + 1, key, sizeof(ele));
		}
	}
}


glm::vec3 RandomColor();

class CoRoutine;
class WaitForSeconds;

bool WaitSecondsHelper(WaitForSeconds*& _pWait)noexcept;

void StartCoRoutine(string_view _strKey, CoRoutine&& _coTask);

void StartCoRoutine(CoRoutine&& _coTask);

bool IsCollision2D(const glm::vec2& A_LT, const glm::vec2& A_RB, const glm::vec2& B_LT, const glm::vec2& B_RB);

std::tuple<glm::vec3, glm::quat, glm::vec3> Decompose(const glm::mat4& transformMat_)noexcept;

const glm::vec3 ScreenToOpenGL2D(const glm::vec2& mpos)noexcept;

const glm::vec2 OpenGL2D2Screen(const glm::vec3& glpos)noexcept;

std::wstring string2wstring(std::string_view input)noexcept;

std::string wstring2string(std::wstring_view input)noexcept;

glm::vec3 GetMaxXYZ(const glm::vec3 v)noexcept;

void LogStackTrace()noexcept;

const glm::vec3 NDC2World(const glm::vec3 ndc)noexcept;

const short wrapAround(const short x, const short low, const short high)noexcept;