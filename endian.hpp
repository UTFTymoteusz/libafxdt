#include "afx/types.h"

namespace afx {
    inline u16 flip(u16 i) {
        return ((i & 0xFF) >> 8) | (i << 8);
    }

    inline u32 flip(u32 i) {
        return ((i & 0xFF000000) >> 24) | ((i & 0x00FF0000) >> 8) |
               ((i & 0x0000FF00) << 8) | ((i & 0x000000FF) << 24);
    }

    inline u64 flip(u64 i) {
        return ((i & 0xFF00000000000000) >> 56) | ((i & 0x00FF000000000000) >> 48) |
               ((i & 0x0000FF0000000000) >> 40) | ((i & 0x000000FF00000000) >> 32) |
               ((i & 0x00000000FF000000) << 32) | ((i & 0x0000000000FF0000) << 40) |
               ((i & 0x000000000000FF00) << 48) | ((i & 0x00000000000000FF) << 56);
    }

    template <typename T>
    inline T from_le(T x) {
        return bigendian ? flip(x) : x;
    }

    template <typename T>
    inline T to_le(T x) {
        return bigendian ? flip(x) : x;
    }

    template <typename T>
    inline T from_be(T x) {
        return littleendian ? flip(x) : x;
    }

    template <typename T>
    inline T to_be(T x) {
        return littleendian ? flip(x) : x;
    }

    static_assert(sizeof(u8) == 1);
    static_assert(sizeof(u16) == 2);
    static_assert(sizeof(u32) == 4);
    static_assert(sizeof(u64) == 8);

    /**
     * @brief Little endian type.
     * Automatically converts between endianness if need-be.
     */
    template <typename T>
    struct le {
        T value;

        T get() {
            return from_le(value);
        }

        void set(T value) {
            value = to_le(value);
        }

        operator T() {
            return get();
        }

        le& operator=(const T& value) {
            set(value);
            return *this;
        }
    };

    /**
     * @brief Big endian type.
     * Automatically converts between endianness if need-be.
     */
    template <typename T>
    struct be {
        T value;

        T get() {
            return from_be(value);
        }

        void set(T value) {
            value = to_be(value);
        }

        operator T() {
            return get();
        }

        be& operator=(const T& value) {
            set(value);
            return *this;
        }
    };
}