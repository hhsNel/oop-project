#include "bsp-node.h"

namespace geometry {
    struct bin_bsp_node {
        float pl_x, pl_y, dir_x, dir_y;
        float f_top, f_bot, f_left, f_right;
        float b_top, b_bot, b_left, b_right;
        std::uint32_t front_child, back_child;
    } __attribute__((packed));

    bool bsp_node::is_pt_front_side(math::vec2 const& point) const {
        math::vec2 delta = point - pl_coord;
        return (delta.y * pl_dir.x - delta.x * pl_dir.y) <= 0;
    }

    std::vector<bsp_node> const bsp_node::load_from_bin(util::resource const& res) {
        std::vector<bsp_node> result;
        if (!res.begin || res.size == 0) return result;

        size_t count = res.size / sizeof(bin_bsp_node);
        auto const* data = reinterpret_cast<bin_bsp_node const*>(res.begin);

        for (size_t i = 0; i < count; ++i) {
            bsp_node node;
            node.pl_coord = math::vec2(float(data[i].pl_x), float(data[i].pl_y));
            node.pl_dir = math::vec2(float(data[i].dir_x), float(data[i].dir_y));
            
            node.front_box = {
                float(data[i].f_top), float(data[i].f_bot), 
                float(data[i].f_left), float(data[i].f_right)
            };
            node.back_box = {
                float(data[i].b_top), float(data[i].b_bot), 
                float(data[i].b_left), float(data[i].b_right)
            };
            
            node.front = data[i].front_child;
            node.back = data[i].back_child;
            
            result.push_back(node);
        }
        return result;
    }
}
