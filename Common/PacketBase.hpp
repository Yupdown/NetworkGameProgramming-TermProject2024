#pragma once
#include <cstring>
#include "packet_define.h"

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

// ��Ŷ ����ü �̸��� �����ϰ� ������ ���ϴ�
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

    c2s_ADD_OBJECT = 8,
    s2c_ADD_OBJECT = 9,

    c2s_MOVE_OBJECT = 10,
    s2c_MOVE_OBJECT = 11,

    c2s_ADD_PROJECTILE = 12,
    s2c_ADD_PROJECTILE = 13,

    END,
};

template <typename T>
static constexpr inline unsigned char etoi(const T eType)noexcept { return static_cast<uint8>(eType); }

#pragma pack (pop)

#define DECLARE_PACKET(pkt_name)                                                                    \
    pkt_name() : PacketHeader{sizeof(pkt_name), etoi(PKT_ID::pkt_name)} {}                          \
    private:                                                                                        \
       static void HandlePacket(const uint64 id, const char* const pBuff_) {                        \
        return pkt_name::Handle(id, *reinterpret_cast<const pkt_name* const>(pBuff_));              \
    }                                                                                               \
    static bool RegisterHandleFunc() noexcept                                                       \
    {                                                                                               \
        if constexpr ((#pkt_name[0] == 'c' && G_PROJECT == PROJECT_TYPE::SERVER) ||                 \
                      (#pkt_name[0] == 's' && G_PROJECT == PROJECT_TYPE::CLIENT))                   \
        {                                                                                           \
            pkt_name temp;                                                                          \
            AddProtocol(static_cast<unsigned short>(temp.pkt_id), &pkt_name::HandlePacket);         \
        }                                                                                           \
        return true;                                                                                \
    }                                                                                               \
    static const inline bool g_bInitPacket = RegisterHandleFunc();                                  \
    static void Handle(const uint64 id, const pkt_name& pkt_) noexcept;


// ��Ŷ�����
// 1. ��Ŷ ����ü �̸��� ������ enum �����ٰ� �߰����ֱ�
// 2. �ؿ� #define�ϱ�
// 3. �Լ����Ǹ���� (Handle)
// 4. �ʿ��� ��������� �߰��ϱ�
// 5. Nice..

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

//---------------------------------------------------------------------------------------------------------//
#pragma pack (push, 1)

// c2s�� �ڵ鷯 �Լ��� ������,
// s2c�� �ڵ鷯 �Լ��� Ŭ���̾�Ʈ �ʿ��� ������ ��

/// <summary>
///  �α���
/// </summary>
struct c2s_LOGIN
    :public PacketHeader
{
    DECLARE_PACKET(c2s_LOGIN);
};

struct s2c_LOGIN
    :public PacketHeader
{
    DECLARE_PACKET(s2c_LOGIN);
};
//

/// <summary>
/// ����
/// </summary>

struct c2s_ENTER
    :public PacketHeader
{
    DECLARE_PACKET(c2s_ENTER);
};

struct s2c_ENTER
    :public PacketHeader
{
    DECLARE_PACKET(s2c_ENTER);
};
//

/// <summary>
/// ��� �ı�
/// </summary>
struct c2s_DESTROY_BLOCK
    :public PacketHeader
{
    int x, y, z;
    DECLARE_PACKET(c2s_DESTROY_BLOCK);
};

struct s2c_DESTROY_BLOCK
    :public PacketHeader
{
    int x, y, z;
    DECLARE_PACKET(s2c_DESTROY_BLOCK);
};
//

/// <summary>
/// ��� ��ġ
/// </summary>
struct c2s_CREATE_BLOCK
    :public PacketHeader
{
    int x, y, z;
    uint8_t tile_id;
    DECLARE_PACKET(c2s_CREATE_BLOCK);
};

struct s2c_CREATE_BLOCK
    :public PacketHeader
{
    int x, y, z;
    uint8_t tile_id;
    DECLARE_PACKET(s2c_CREATE_BLOCK);
};

/// <summary>
/// ������Ʈ ����
/// </summary>
struct c2s_ADD_OBJECT
    :public PacketHeader
{
	int object_id;
	float position_x, position_y, position_z;
	float rotation_y;
	DECLARE_PACKET(c2s_ADD_OBJECT);
};

struct s2c_ADD_OBJECT
    :public PacketHeader
{
	int object_id;
	float position_x, position_y, position_z;
	float rotation_y;
	DECLARE_PACKET(s2c_ADD_OBJECT);
};

/// <summary>
/// ������Ʈ �̵�
/// </summary>
struct c2s_MOVE_OBJECT
    :public PacketHeader
{
    float position_x, position_y, position_z;
    float velocity_x, velocity_y, velocity_z;
    float acceleration_x, acceleration_y, acceleration_z;
    float rotation_y;
    DECLARE_PACKET(c2s_MOVE_OBJECT);
};

struct s2c_MOVE_OBJECT
    :public PacketHeader
{
    int object_id;
    float position_x, position_y, position_z;
    float velocity_x, velocity_y, velocity_z;
    float acceleration_x, acceleration_y, acceleration_z;
    float rotation_y;
    DECLARE_PACKET(s2c_MOVE_OBJECT);
};

/// <summary>
/// ����ü ����
/// </summary>
struct c2s_ADD_PROJECTILE
    :public PacketHeader
{
	float position_x, position_y, position_z;
	float velocity_x, velocity_y, velocity_z;
	DECLARE_PACKET(c2s_ADD_PROJECTILE);
};

struct s2c_ADD_PROJECTILE
    :public PacketHeader
{
    int projectile_id;
    float position_x, position_y, position_z;
    float velocity_x, velocity_y, velocity_z;
    DECLARE_PACKET(s2c_ADD_PROJECTILE);
};

#pragma pack (pop)