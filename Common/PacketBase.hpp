#pragma once
#include <cstring>

using uint64 = unsigned long long;
using uint8 = unsigned char;
using uint16 = unsigned short;

extern void AddProtocol(const uint16 pktID_, void (*fpPacketHandler_)(const uint64, const char* const))noexcept;


#pragma pack (push, 1)

struct PacketHeader
{
	const uint8 pkt_size;
	const uint8 pkt_id;
};

// 패킷 구조체 이름과 동일하게 만들어야 편하다
enum class PKT_ID : uint8
{
    c2s_LOGIN = 0,
    s2c_LOGIN = 1,

    c2s_ENTER = 2,
    s2c_ENTER = 3,

    c2s_DESTROY_BLOCK = 4,
    s2c_DESTROY_BLOCK = 5,

    c2s_CREATE_BLOCK = 6,
    s2c_CREATE_BLOCK = 7,

    c2s_MOVE_OBJECT = 8,
    s2c_MOVE_OBJECT = 9,

    END,
};

template <typename T>
static constexpr inline unsigned char etoi(const T eType)noexcept { return static_cast<uint8>(eType); }


template <typename T>
struct PacketBase
	:public PacketHeader
{
private:
    static bool InitPacketBase()noexcept
    {
        T temp;
        AddProtocol(static_cast<unsigned short>(temp.pkt_id), &T::HandlePacket);
        return true;
    }
    static const inline bool g_bInitPacket = InitPacketBase();
public:
    PacketBase(const uint8 pkt_size,const uint8 pkt_id)noexcept
        :PacketHeader{ pkt_size,pkt_id }
    {
    }

    char* const SerializeToArray(void* const __restrict buffer)const noexcept {
        return (char*)::memcpy(buffer, this, sizeof(T));
    }

    static void HandlePacket(const uint64 id, const char* const pBuff_) {
        // if (sizeof(T) != len_) [[unlikely]]
        //     return false;
        return T::Handle(id, *reinterpret_cast<const T* const>(pBuff_));
    }

};
#pragma pack (pop)

#pragma pack (push, 1)

#define DECLARE_PACKET(pkt_name)                                         \
    pkt_name() : PacketBase{sizeof(pkt_name), etoi(PKT_ID::pkt_name)} {} \
    static void Handle(const uint64 id, const pkt_name& pkt_) noexcept


// 패킷만들기
// 1. 패킷 구조체 이름과 동일한 enum 위에다가 추가해주기
// 2. 밑에 #define하기
// 3. 함수정의만들기 (Handle)
// 4. 필요한 멤버변수들 추가하기
// 5. Nice..

// 패킷 견본
// struct c2s_TEST
//     :public PacketBase<c2s_TEST>
// {
//     // 이거는 피할 수가 없음
//     // 함수정의 및 생성자로 패킷아이디랑 사이즈
//     DECLARE_PACKET(c2s_TEST) { }
// 
//     int a;
// 
// };

#pragma pack (pop)


static inline int OnRecv(const uint64 id, char* const buffer, const int len, void (*const* const packet_func)(const uint64_t, const char* const))noexcept
{
	int processLen = 0;

	for (;;)
	{
		const int dataSize = len - processLen;
	
		if (dataSize < static_cast<int>(sizeof(PacketHeader)))
			break;

		const PacketHeader* const __restrict header = reinterpret_cast<const PacketHeader* const>(buffer + processLen);
		const uint8 packetSize = header->pkt_size;
		const uint8 packetId = header->pkt_id;
		
		if (dataSize < packetSize)
			break;

        packet_func[packetId](id, (char*)header);

		processLen += packetSize;
	}

    return processLen;
}

//---------------------------------------------------------------------------------------------------------
#pragma pack (push, 1)

// #define DECLARE_PACKET(pkt_name)                                         \
//     pkt_name() : PacketBase{sizeof(pkt_name), etoi(PKT_ID::pkt_name)} {} \
//     static void Handle(const uint64 id, const pkt_name& pkt_) noexcept

// c2s의 핸들러 함수는 서버에,
// s2c의 핸들러 함수는 클라이언트 쪽에서 만들어야 함

/// <summary>
///  로그인
/// </summary>
struct c2s_LOGIN
    :public PacketBase<c2s_LOGIN>
{
    DECLARE_PACKET(c2s_LOGIN);
};

struct s2c_LOGIN
    :public PacketBase<s2c_LOGIN>
{
    DECLARE_PACKET(s2c_LOGIN);
};
//

/// <summary>
/// 입장
/// </summary>

struct c2s_ENTER
    :public PacketBase<c2s_ENTER>
{
    DECLARE_PACKET(c2s_ENTER);
};

struct s2c_ENTER
    :public PacketBase<s2c_ENTER>
{
    DECLARE_PACKET(s2c_ENTER);
};
//

/// <summary>
/// 블록 파괴
/// </summary>
struct c2s_DESTROY_BLOCK
    :public PacketBase<c2s_DESTROY_BLOCK>
{
    int x, y, z;
    DECLARE_PACKET(c2s_DESTROY_BLOCK);
};

struct s2c_DESTROY_BLOCK
    :public PacketBase<s2c_DESTROY_BLOCK>
{
    int x, y, z;
    DECLARE_PACKET(s2c_DESTROY_BLOCK);
};
//

/// <summary>
/// 블록 설치
/// </summary>
struct c2s_CREATE_BLOCK
    :public PacketBase<c2s_CREATE_BLOCK>
{
    int x, y, z;
    int tile_id;
    DECLARE_PACKET(c2s_CREATE_BLOCK);
};

struct s2c_CREATE_BLOCK
    :public PacketBase<s2c_CREATE_BLOCK>
{
    int x, y, z;
    int tile_id;
    DECLARE_PACKET(s2c_CREATE_BLOCK);
};

/// <summary>
/// 오브젝트 이동
/// </summary>

struct c2s_MOVE_OBJECT
	:public PacketBase<c2s_MOVE_OBJECT>
{
    float position_x, position_y, position_z;
    float velocity_x, velocity_y, velocity_z;
    float acceleration_x, acceleration_y, acceleration_z;
    float rotation_y;
	DECLARE_PACKET(c2s_MOVE_OBJECT);
};

struct s2c_MOVE_OBJECT
	:public PacketBase<s2c_MOVE_OBJECT>
{
    int object_id;
    float position_x, position_y, position_z;
    float velocity_x, velocity_y, velocity_z;
    float acceleration_x, acceleration_y, acceleration_z;
    float rotation_y;
	DECLARE_PACKET(s2c_MOVE_OBJECT);
};

#pragma pack (pop)