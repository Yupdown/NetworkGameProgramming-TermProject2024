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
        // TODO: 패킷핸들러 만들기
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


// 패킷만들기
// 1. 패킷 구조체 이름과 동일한 enum 위에다가 추가해주기
// 2. 밑에 #define하기
// 3. 함수정의만들기 (Handle)
// 4. 필요한 멤버변수들 추가하기
// 5. Nice..

// 패킷 견본
struct c2s_TEST
    :public PacketBase<c2s_TEST>
{
    // 이거는 피할 수가 없음
    // 함수정의 및 생성자로 패킷아이디랑 사이즈
    DECLARE_PACKET(c2s_TEST) { }

    int a;

};

#pragma pack (pop)