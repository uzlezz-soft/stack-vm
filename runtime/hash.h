#include <cstdint>
#include <string_view>

namespace svm
{

	inline uint64_t murmur64(const char* data, uint64_t length, uint64_t seed)
    {
        constexpr uint64_t m = 0xc6a4a7935bd1e995ULL;
        constexpr int r = 47;
        const uint64_t length8 = length / 8;

        uint64_t h = seed ^ (length * m);

        for (uint64_t i = 0; i < length8; i++)
        {
            uint64_t k = *data++;
            k *= m;
            k ^= k >> r;
            k *= m;
            h ^= k;
            h *= m;
        }

        const auto* tail = reinterpret_cast<const uint8_t*>(data);
        switch (length & 7)
        {
        case 7: h ^= uint64_t(tail[6]) << 48;
        case 6: h ^= uint64_t(tail[5]) << 40;
        case 5: h ^= uint64_t(tail[4]) << 32;
        case 4: h ^= uint64_t(tail[3]) << 24;
        case 3: h ^= uint64_t(tail[2]) << 16;
        case 2: h ^= uint64_t(tail[1]) << 8;
        case 1: h ^= uint64_t(tail[0]);
            h *= m;
        }

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;
    }

    inline uint64_t hashString(std::string_view str)
    {
        constexpr uint64_t seed = 0;
        return murmur64(str.data(), str.size(), seed);
    }

}