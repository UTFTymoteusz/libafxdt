#pragma once

#include "afx/dt.h"
#include "afx/types.h"
#include "endian.hpp"

namespace afx {
    enum fdt_tokentype {
        FDT_BEGIN_NODE = 0x00000001,
        FDT_END_NODE   = 0x00000002,
        FDT_PROP       = 0x00000003,
        FDT_NOP        = 0x00000004,
        FDT_END        = 0x00000009,
    };

    struct fdt_header {
        be<u32> magic;
        be<u32> totalsize;
        be<u32> off_dt_struct;
        be<u32> off_dt_strings;
        be<u32> off_mem_rsvmap;
        be<u32> version;
        be<u32> last_comp_version;
        be<u32> boot_cpuid_phys;
        be<u32> size_dt_strings;
        be<u32> size_dt_struct;
    };
}