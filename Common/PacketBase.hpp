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

// ��Ŷ ����ü �̸��� �����ϰ� ������ ���ϴ�
enum class PKT_ID : uint8
{
    c2s_TEST = 0,



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
        // TODO: ��Ŷ�ڵ鷯 �����
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

    bool SerializeToArray(void* const __restrict buffer)const noexcept {
        return ::memcpy(buffer, this, sizeof(T));
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


// ��Ŷ�����
// 1. ��Ŷ ����ü �̸��� ������ enum �����ٰ� �߰����ֱ�
// 2. �ؿ� #define�ϱ�
// 3. �Լ����Ǹ���� (Handle)
// 4. �ʿ��� ��������� �߰��ϱ�
// 5. Nice..

// ��Ŷ �ߺ�
struct c2s_TEST
    :public PacketBase<c2s_TEST>
{
    // �̰Ŵ� ���� ���� ����
    // �Լ����� �� �����ڷ� ��Ŷ���̵�� ������
    DECLARE_PACKET(c2s_TEST) { }

    int a;

};

#pragma pack (pop)