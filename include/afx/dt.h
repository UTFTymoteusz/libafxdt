#pragma once

#include "afx/types.h"

#ifdef __cplusplus
namespace afx {
#endif
    struct fdt_header;

    // Parsed FDT structure.
    struct fdt {
        fdt_header* header;
        usz         size;
        char*       strings;
        void*       tree;
    };

    struct fdt_node {
        fdt*        owner; // Owning FDT.
        void*       start; // Pointer to the node's FDT_BEGIN_NODE.
        const char* name;  // Node name.
    };

    struct fdt_prop {
        void*       start; // Pointer to the property's start.
        const char* name;  // Property name.
        usz         size;  // Property size.
        void*       data;  // Pointer to the property's data.
    };

    /**
     * @brief Parses an FDT.
     * @param dest Struct to fill.
     * @param ptr  Pointer to the FDT's header.
     * @return Whether the parsing succeeded.
     */
    export bool fdt_parse(fdt* dest, void* ptr);

    /**
     * @brief Returns the root node of an FDT.
     * @param fdt       Parsed FDT.
     * @param dest_node Destination node struct to fill out.
     * @return True if found.
     */
    export bool fdt_rootnode(fdt* fdt, fdt_node* dest_node);

    /**
     * @brief Gets a child node.
     * @param fdt       Parsed FDT.
     * @param dest_node Destination node struct to fill out.
     * @param parent    Parent node.
     * @param name      Child node name.
     * @param strict    Whether to take characters after @ into consideration.
     * @return True if found.
     */
    export bool fdt_childnode(fdt* fdt, fdt_node* dest_node, fdt_node* parent,
                              const char* name, bool strict);

    /**
     * @brief Gets the first child node belonging to another node.
     * @param fdt       Parsed FDT.
     * @param dest_node Destination node struct to fill out.
     * @param parent    Parent node.
     * @return True if there are any child nodes at all.
     */
    export bool fdt_firstnode(fdt* fdt, fdt_node* dest_node, fdt_node* parent);

    /**
     * @brief Gets the child node that follows the current one.
     * @param fdt       Parsed FDT.
     * @param dest_node Destination node struct to fill out.
     * @param current   Current node.
     * @return True if there was a following node.
     */
    export bool fdt_nextnode(fdt* fdt, fdt_node* dest_node, fdt_node* current);

    /**
     * @brief Gets the node at the specified path.
     * @param fdt  Parsed FDT.
     * @param path Node path.
     * @return Pointer to the node if found, nullptr otherwise.
     */
    export bool fdt_getnode(fdt* fdt, const char* path);

    /**
     * @brief Gets the node at the specified path.
     * @param fdt  Parsed FDT.
     * @param prop Destination property struct to fill out.
     * @param node Node.
     * @param name Property name.
     * @return True if the property was found.
     */
    export bool fdt_getprop(fdt* fdt, fdt_prop* prop, fdt_node* node, const char* name);

    /**
     * @brief Gets an u32 from a property.
     * @param fdt    Parsed FDT.
     * @param prop   Property.
     * @param offset Offset in bytes.
     * @return Value.
     */
    export u32 fdt_getu32(fdt* fdt, fdt_prop* prop, int offset = 0);

    /**
     * @brief Gets an u64 from a property.
     * @param fdt    Parsed FDT.
     * @param prop   Property.
     * @param offset Offset in bytes.
     * @return Value.
     */
    export u64 fdt_getu64(fdt* fdt, fdt_prop* prop, int offset = 0);
#ifdef __cplusplus
}
#endif