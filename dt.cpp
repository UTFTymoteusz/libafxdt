#include "dt.hpp"

#include "host.h"

namespace afx {
    struct fdtnod {
        be<u32> len;
        be<u32> nameoff;
    } packed;

    /**
     * @brief Calculates the size of a node.
     * Please point it at a FDT_BEGIN_NODE.
     * @return The size of the node (including the FDT_BEGIN_NODE).
     */
    static int node_size(be<u32>* ptr) {
        be<u32>* start = ptr;
        int      depth = 0;
        int      size  = 1;

        if ((ptr + 1)->get() != FDT_BEGIN_NODE)
            return 0;

        do {
            switch (ptr->get()) {
            case FDT_BEGIN_NODE: {
            } break;
            case FDT_PROP: {
            } break;
            case FDT_END_NODE:
                break;
            case FDT_NOP:
                break;
            default:
                break;
            }

            size++;
            ptr = start + size;
        } while (depth != 0);

        return size;
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
        for (usz i = 0; i < fdt->size - sizeof(fdt_header); i += sizeof(u32)) {
            be<u32>* ptr = (be<u32>*) fdt->tree + i;

            if (ptr->get() == FDT_BEGIN_NODE) {
                node->owner = fdt;
                node->start = ptr;
                node->name  = "/";

                return true;
            }
            else if (ptr->get() == FDT_PROP) {
                i += 2;
            }
        }

        return false;
    }

    bool fdt_childnode(fdt* fdt, fdt_node* node, fdt_node* parent, const char* name,
                       bool strict) {
        be<u32>* ptr = (be<u32>*) parent->start + 1;

        ptr += ((afxhost_strlen(parent->name) + 1) + 3) / 4;

        while (ptr->get() != FDT_END_NODE) {
            u32 val = ptr->get();

            if (val == FDT_PROP) {
                fdtnod* prop      = (fdtnod*) (ptr + 1);
                usz     prop_size = (prop->len + 3) / 4;

                ptr += prop_size + sizeof(fdtnod) / sizeof(be<u32>) + 1;
            }

            if (val == FDT_BEGIN_NODE) {
                const char* node_name = ((const char*) ptr) + 4;

                if (strcmp_lenient(node_name, name) == 0) {
                    node->owner = fdt;
                    node->start = (void*) ptr;
                    node->name  = (const char*) (ptr + 1);

                    return true;
                }

                ptr += node_size(ptr);
            }

            if (val == FDT_NOP)
                ptr++;
        }

        return false;
    }

    bool fdt_firstnode(fdt* fdt, fdt_node* dest_node, fdt_node* parent) {
        be<u32>* ptr = (be<u32>*) parent->start;

        ptr += afxhost_strlen(parent->name) / 4 + 1;

        while (ptr->get() != FDT_END_NODE) {
            u32 val = ptr->get();

            if (val == FDT_PROP) {
                usz prop_size = ((++ptr)->get() + 3) / 4;
                ptr += prop_size + 1;
            }

            if (val == FDT_BEGIN_NODE) {
                dest_node->owner = fdt;
                dest_node->start = (void*) ptr;
                dest_node->name  = (const char*) (ptr + 1);

                return true;
            }

            ptr++;
        }

        return false;
    }

    bool fdt_nextnode(fdt* fdt, fdt_node* dest_node, fdt_node* current) {
        be<u32>* ptr = (be<u32>*) current->start;

        ptr += node_size(ptr) + 1;

        while (ptr->get() != FDT_END_NODE) {
            u32 val = ptr->get();

            if (val == FDT_PROP) {
                usz prop_size = ((++ptr)->get() + 3) / 4;
                ptr += prop_size + 1;
            }

            if (val == FDT_BEGIN_NODE) {
                dest_node->owner = fdt;
                dest_node->start = (void*) ptr;
                dest_node->name  = (const char*) (ptr + 1);

                return true;
            }

            ptr++;
        }

        return false;
    }

    bool fdt_getprop(fdt* fdt, fdt_prop* prop, fdt_node* node, const char* name) {
        be<u32>* ptr = (be<u32>*) node->start;

        ptr += (afxhost_strlen(node->name) + 3) / 4 + 1;

        while (ptr->get() != FDT_END_NODE) {
            u32 val = ptr->get();

            if (val == FDT_PROP) {
                usz         prop_size = ((++ptr + 0)->get() + 3) / 4;
                const char* prop_name = (const char*) fdt->strings + (ptr + 1)->get();

                if (afxhost_strcmp(prop_name, name) == 0) {
                    prop->size  = ptr->get();
                    prop->name  = prop_name;
                    prop->start = (void*) (ptr - 1);

                    return true;
                }

                ptr += prop_size + 2;
            }

            if (val == FDT_BEGIN_NODE)
                ptr += node_size(ptr);
        }

        return false;
    }

    u32 fdt_getu32(fdt*, fdt_prop* prop, int offset) {
        return ((be<u32>*) ((char*) (prop->start + 3) + offset))->get();
    }

    u64 fdt_getu64(fdt*, fdt_prop* prop, int offset) {
        return ((u64) ((be<u32>*) ((char*) (prop->start + 3 + 0) + offset))->get()) |
               ((be<u32>*) ((char*) (prop->start + 3 + 1) + offset))->get();
    }
}