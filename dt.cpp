#include "dt.hpp"

#include "host.h"

#include <string.h>

namespace afx {
    /**
     * @brief Gets a pointer to the inner section of a node.
     * @param node The node in question.
     * @return Pointer to the inner section of a node.
     */
    static be<u32>* node_getinner(fdt_node* node) {
        be<u32>* ptr = (be<u32>*) node->start + 1;

        ptr += afxhost_strlen(node->name) / 4 + 1;

        while (ptr->get() == FDT_NOP)
            ptr++;

        return ptr;
    }

    /**
     * @brief Gets the size of a property counting the FDT_PROP itself.
     * @param ptr Pointer to a FDT_PROP.
     * @return Size in cells.
     */
    static int prop_len(be<u32>* ptr) {
        if (ptr->get() != FDT_PROP)
            return 0;

        return 1 + ((ptr + 1)->get() + 3) / 4 + 2;
    }

    static void prop_fill(be<u32>* ptr, fdt* fdt, fdt_prop* prop) {
        prop->start = ptr;
        prop->name  = (const char*) (fdt->strings + (ptr + 2)->get());
        prop->size  = (ptr + 1)->get();
    }

    static void node_skipprops(be<u32>*& ptr) {
        while (ptr->get() == FDT_PROP) {
            int size = prop_len(ptr);
            ptr += size;

            while (ptr->get() == FDT_NOP)
                ptr++;
        }
    }

    static void node_fill(be<u32>* ptr, fdt* fdt, fdt_node* node) {
        node->owner = fdt;
        node->start = ptr;
        node->name  = (const char*) (ptr + 1);
    }

    /**
     * @brief Calculates the size of a node.
     * @return Size in cells.
     */
    static int node_len(be<u32>* start) {
        be<u32>* ptr = start;

        if (ptr->get() != FDT_BEGIN_NODE)
            return 0;

        ptr++;
        ptr += afxhost_strlen((const char*) ptr) / 4 + 1;

        node_skipprops(ptr);

        while (ptr->get() != FDT_END_NODE) {
            if (ptr->get() == FDT_NOP) {
                ptr++;
                continue;
            }
            else if (ptr->get() == FDT_BEGIN_NODE) {
                ptr += node_len(ptr);
            }
        }

        ptr++;

        return (int) (ptr - start);
    }

    int strcmp_lenient(const char* a, const char* b) {
        do {
            if (*a == '@' || *b == '@')
                break;

            if (*a < *b)
                return -1;
            else if (*a > *b)
                return 1;
        } while (*a++ != '\0' && *b++ != '\0');

        return 0;
    }

    bool fdt_parse(fdt* dest, void* ptr) {
        fdt_header* hdr = (fdt_header*) ptr;

        if (hdr->magic != 0xD00DFEED)
            return false;

        dest->header  = hdr;
        dest->size    = hdr->totalsize;
        dest->strings = (char*) ptr + hdr->off_dt_strings;
        dest->tree    = (void*) ((char*) ptr + hdr->off_dt_struct);

        return true;
    }

    bool fdt_rootnode(fdt* fdt, fdt_node* node) {
        for (usz i = 0; i < fdt->size - sizeof(fdt_header); i++) {
            be<u32>* ptr = (be<u32>*) fdt->tree + i;

            if (ptr->get() == FDT_BEGIN_NODE) {
                node->owner = fdt;
                node->start = ptr;
                node->name  = "/";

                return true;
            }
        }

        return false;
    }

    bool fdt_childnode(fdt* fdt, fdt_node* node, fdt_node* parent, const char* name,
                       bool strict) {
        fdt_node check;

        if (!fdt_firstnode(fdt, &check, parent))
            return false;

        do {
            if (strict) {
                if (afxhost_strcmp(name, check.name) == 0) {
                    memcpy(node, &check, sizeof(fdt_node));
                    return true;
                }
            }
            else {
                if (strcmp_lenient(name, check.name) == 0) {
                    memcpy(node, &check, sizeof(fdt_node));
                    return true;
                }
            }
        } while (fdt_nextnode(fdt, &check, &check));

        return false;
    }

    bool fdt_firstnode(fdt* fdt, fdt_node* dest_node, fdt_node* parent) {
        be<u32>* ptr = node_getinner(parent);

        node_skipprops(ptr);
        node_fill(ptr, fdt, dest_node);

        return true;
    }

    bool fdt_nextnode(fdt* fdt, fdt_node* dest_node, fdt_node* current) {
        be<u32>* ptr = (be<u32>*) current->start;

        ptr += node_len(ptr);

        if (ptr->get() != FDT_BEGIN_NODE)
            return false;

        node_fill(ptr, fdt, dest_node);

        return true;
    }

    bool fdt_getprop(fdt* fdt, fdt_prop* prop, fdt_node* node, const char* name) {
        be<u32>* ptr = node_getinner(node);

        while (ptr->get() == FDT_PROP) {
            if (afxhost_strcmp(name, (const char*) (fdt->strings + (ptr + 2)->get())) ==
                0) {
                prop_fill(ptr, fdt, prop);
                return true;
            }

            ptr += prop_len(ptr);

            while (ptr->get() == FDT_NOP)
                ptr++;
        }

        return false;
    }

    u32 fdt_getu32(fdt*, fdt_prop* prop, int offset) {
        return ((be<u32>*) ((char*) (prop->start + 12) + offset))->get();
    }

    u64 fdt_getu64(fdt* fdt, fdt_prop* prop, int offset) {
        return ((u64) fdt_getu32(fdt, prop, offset + 0) << 32) |
               ((u64) fdt_getu32(fdt, prop, offset + 4));
    }
}