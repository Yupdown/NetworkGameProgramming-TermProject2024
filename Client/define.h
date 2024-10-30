#pragma once

#define Mgr(type)	(type::GetInst())
#define DT	(Mgr(TimeMgr)->GetDT())	

#define SPIN_LOCK	std::lock_guard<SpinLock> spin_lock{m_spinLock}

#define COMP_CLONE(type) shared_ptr<Component> Comp_Clone()const override{return std::make_shared<type>(*this);} 

#define KEY_CHECK(key,state)	((Mgr(KeyMgr)->GetKeyState(key))==state)
#define KEY_HOLD(key)			(KEY_CHECK(key,KEY_STATE::HOLD))		
#define KEY_TAP(key)			(KEY_CHECK(key,KEY_STATE::TAP))
#define KEY_AWAY(key)			(KEY_CHECK(key,KEY_STATE::AWAY))
#define KEY_NONE(key)			(KEY_CHECK(key,KEY_STATE::NONE))

using uint = unsigned int;
using ull = unsigned long long;
using ushort = unsigned short;

static constexpr glm::vec3 X_AXIS {1.f, 0.f, 0.f};
static constexpr glm::vec3 Y_AXIS {0.f, 1.f, 0.f};
static constexpr glm::vec3 Z_AXIS {0.f, 0.f, 1.f};
static constexpr glm::vec3 ZERO_VEC3 {0.f, 0.f, 0.f};

static constexpr glm::vec4 X_AXIS4 {1.f, 0.f, 0.f,1.f};
static constexpr glm::vec4 Y_AXIS4 {0.f, 1.f, 0.f,1.f};
static constexpr glm::vec4 Z_AXIS4 {0.f, 0.f, 1.f,1.f};

static constexpr glm::vec4 RGBA_RED {1.f, 0.f, 0.f,1.f};
static constexpr glm::vec4 RGBA_GREEN {0.f, 1.f, 0.f,1.f};
static constexpr glm::vec4 RGBA_BLUE {0.f, 0.f, 1.f,1.f};
static constexpr glm::vec4 RGBA_WHITE{ 1.f, 1.f, 1.f,1.f };
static constexpr glm::vec4 RGBA_BLACK{ 0.f, 0.f, 0.f,1.f };
static constexpr glm::vec4 RGBA_GRAY {0.5f, 0.5f, 0.5f, 1.0f};

static constexpr glm::vec3 RGB_RED {1.f, 0.f, 0.f};
static constexpr glm::vec3 RGB_GREEN {0.f, 1.f, 0.f};
static constexpr glm::vec3 RGB_BLUE {0.f, 0.f, 1.f};
static constexpr glm::vec3 RGB_WHITE{ 1.f, 1.f, 1.f};
static constexpr glm::vec3 RGB_BLACK{ 0.f, 0.f, 0.f };
static constexpr glm::vec3 RGB_GRAY {0.5f, 0.5f, 0.5f};

static std::mt19937 g_rng;

enum class KEY_STATE : unsigned short
{
	NONE,
	TAP,
	HOLD,
	AWAY,
};

enum class COMPONENT_TYPE
{
	TRANSFORM,
	MESH_RENDERER,
	CAMERA,
	LIGHT,
	INPUT_HANDLER,
	COROUTINE_HANDLER,
	ANIMATOR,
	COLLIDER,
	RIGIDBODY,

	MONO_BEHAVIOR,
	END,
};

enum {COMPONENT_COUNT = static_cast<int>(COMPONENT_TYPE::END)-1 };

enum class GROUP_TYPE
{
	DEFAULT,
	TILE,
	GROUND,
	MONSTER,
	PLAYER,
	PROJ_PLAYER,
	PROJ_MONSTER,

	PLAYER_WEAPON,
	DROP_ITEM,

	PARTICLE,

	UI = 31,

	END = 32,
};

enum class SCENE_TYPE
{
	INTRO,
	STAGE,

	END,
};

enum class PROJECTION_TYPE
{
	PERSPECTIVE, // 원근 투영
	ORTHOGRAPHIC, // 직교 투영
};

enum class LIGHT_TYPE
{
	POINT,
	SPOT,
	DIRECTIONAL,

	END
};

enum class SKYBOX_TYPE
{
	SPHERE,
	CUBE,

};

static constexpr float PI = 3.14159265358979323846f;

#define CAPTURE_COPY(Arg) \
    std::conditional_t< \
        std::is_lvalue_reference_v<decltype(Arg)> || !std::is_rvalue_reference_v<decltype(Arg)>, \
        std::decay_t<decltype(Arg)>, \
        decltype(Arg)> \
    (std::forward<decltype(Arg)>(Arg))


#define USE_NAGOX_ASSERT

#ifdef USE_NAGOX_ASSERT

#define NAGOX_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "Activate NagOx Assertion: " << (message) << '\n'; \
			LogStackTrace(); \
            *(int*)nullptr = 0; \
        } \
    } while (0)

#else

#define NAGOX_ASSERT(condition, message)

#endif

class GameObj;
class Component;
const bool IsAliveObj(const shared_ptr<GameObj>& obj)noexcept;
const glm::mat4& GetMatrix(const shared_ptr<GameObj>& obj)noexcept;

static constexpr const auto NotComponentNullptr = std::views::filter([](const shared_ptr<Component>& pComp)noexcept {return nullptr != pComp; });
static constexpr const auto OnlyAliveObject = std::views::filter(IsAliveObj);
static constexpr const auto ApplyObjWorldMatrix = std::views::transform(GetMatrix);