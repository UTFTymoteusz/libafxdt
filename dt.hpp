#pragma once

#include "afx/types.h"
#include "dt.hpp"
#include "endian.hpp"

namespace afx {
    enum fdt_tokentype {
        FDT_BEGIN_NODE = 0x00000001,
        FDT_END_NODE   = 0x00000002,
        FDT_PROP       = 0x00000003,
        FDT_NOP        = 0x00000004,
        FDT_END        = 0x00000009,
    };

    struct fdt_header;

    // Parsed FDT structure.
    struct fdt {
        fdt_header* header;
        usz         size;
        char*       strings;
        void*       tree;
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

    struct fdt_node {
        fdt*        owner; // Owning FDT.
        be<u32>*    start; // Pointer to the node's FDT_BEGIN_NODE.
        const char* name;  // Node name.
    };

    struct fdt_prop {
        be<u32>*    start; // Pointer to the property's start.
        const char* name;  // Property name.
        usz         size;  // Property size.
    };

    /**
     * @brief Parses an FDT.
     * @param dest Struct to fill.
     * @param ptr  Pointer to the FDT's header.
     * @return Whether the parsing succeeded.
     */
    bool fdt_parse(fdt* dest, void* ptr);

    /**
     * @brief Returns the root node of an FDT.
     * @param fdt       Parsed FDT.
     * @param dest_node Destination node struct to fill out.
     * @return True if found.
     */
    bool fdt_rootnode(fdt* fdt, fdt_node* dest_node);

    /**
     * @brief Gets a child node.
     * @param fdt       Parsed FDT.
     * @param dest_node Destination node struct to fill out.
     * @param parent    Parent node.
     * @param name      Child node name.
     * @param strict    Whether to take characters after @ into consideration.
     * @return True if found.
     */
    bool fdt_childnode(fdt* fdt, fdt_node* dest_node, fdt_node* parent, const char* name,
                       bool strict);

    /**
     * @brief Gets the first child node belonging to another node.
     * @param fdt       Parsed FDT.
     * @param dest_node Destination node struct to fill out.
     * @param parent    Parent node.
     * @return True if there are any child nodes at all.
     */
    bool fdt_firstnode(fdt* fdt, fdt_node* dest_node, fdt_node* parent);

    /**
     * @brief Gets the child node that follows the current one.
     * @param fdt       Parsed FDT.
     * @param dest_node Destination node struct to fill out.
     * @param current   Current node.
     * @return True if there was a following node.
     */
    bool fdt_nextnode(fdt* fdt, fdt_node* dest_node, fdt_node* current);

    /**
     * @brief Gets the node at the specified path.
     * @param fdt  Parsed FDT.
     * @param path Node path.
     * @return Pointer to the node if found, nullptr otherwise.
     */
    bool fdt_getnode(fdt* fdt, const char* path);

    /**
     * @brief Gets the node at the specified path.
     * @param fdt  Parsed FDT.
     * @param prop Destination property struct to fill out.
     * @param node Node.
     * @param name Property name.
     * @return True if the property was found.
     */
    bool fdt_getprop(fdt* fdt, fdt_prop* prop, fdt_node* node, const char* name);

    /**
     * @brief Gets an u32 from a property.
     * @param fdt    Parsed FDT.
     * @param prop   Property.
     * @param offset Offset in bytes.
     * @return Value.
     */
    u32 fdt_getu32(fdt* fdt, fdt_prop* prop, int offset = 0);

    /**
     * @brief Gets an u64 from a property.
     * @param fdt    Parsed FDT.
     * @param prop   Property.
     * @param offset Offset in bytes.
     * @return Value.
     */
    u64 fdt_getu64(fdt* fdt, fdt_prop* prop, int offset = 0);
}