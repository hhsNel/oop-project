#!/usr/bin/python3

from graphviz import Digraph

def create_unrolled_renderer_flowchart():
    dot = Digraph('SoftwareRendererLineByLine', format='png')
    dot.attr(size='8.27,11.69!')
    
    # Visual Tuning for clear layouts with high node density
    dot.attr(rankdir='TB', splines='true', nodesep='0.25', ranksep='0.35')
    
    # Global Node Style Specifications (by flowchart convention)
    dot.attr('node', fontname='Courier', fontsize='9', shape='box', style='filled', fillcolor='#F2F4F4', color='#34495E')
    dot.attr('edge', fontname='Helvetica', fontsize='8', color='#2C3E50', weight='1.0')

    # Styles mapping helper
    def add_node(cid, label, n_type='process'):
        if n_type == 'entry':
            dot.node(cid, label, shape='parallelogram', fillcolor='#3498DB', fontcolor='white', style='filled,bold')
        elif n_type == 'exit':
            dot.node(cid, label, shape='parallelogram', fillcolor='#E74C3C', fontcolor='white', style='filled,bold')
        elif n_type == 'condition':
            dot.node(cid, label, shape='diamond', fillcolor='#F1C40F', style='filled')
        elif n_type == 'call':
            dot.node(cid, label, shape='box', fillcolor='#AED6F1', style='filled,dashed')
        else:
            dot.node(cid, label, shape='box', fillcolor='#FAFAFA')

    # =========================================================================
    # 1. METHOD: render_bsp()
    # =========================================================================
    with dot.subgraph(name='cluster_render_bsp') as c:
        c.attr(label='software_renderer::render_bsp()', style='filled', fillcolor='#EAEDED')
        
        add_node('rb_entry', 'ENTRY: render_bsp(cam_pos, cam_height, cam_angle, fov)', 'entry')
        add_node('rb_check_root', 'current_map.root_node_id !=\nnullid?', 'condition')
        
        # Individual Variable Assignments
        add_node('rb_as1', 'frd.cam_pos = cam_pos')
        add_node('rb_as2', 'frd.cam_height = cam_height')
        add_node('rb_as3', 'frd.cam_angle = cam_angle')
        add_node('rb_as4', 'frd.sw = target("width")')
        add_node('rb_as5', 'frd.sh = target("height")')
        add_node('rb_as6', 'frd.half_sw = frd.sw / 2.0f')
        add_node('rb_as7', 'frd.pitch = target("pitch") / sizeof(uint32_t)')
        add_node('rb_as8', 'frd.mmio = target("mmio")')
        add_node('rb_as9', 'frd.fov_scale = frd.half_sw / std::tan(fov/2.0f)')
        add_node('rb_as10', 'frd.inv_fov_scale = 1.0f / frd.fov_scale')
        add_node('rb_as11', 'frd.cos_cam_angle = std::cos(frd.cam_angle)')
        add_node('rb_as12', 'frd.sin_cam_angle = std::sin(frd.cam_angle)')
        
        add_node('rb_as13', 'upper_clip.assign(frd.sw, -1)')
        add_node('rb_as14', 'lower_clip.assign(frd.sw, frd.sh)')
        
        # Euclidean Dist Loop Block (Expanded Line-by-Line)
        add_node('rb_cond_dist', 'euclidian_dist_factor.size() !=\nfrd.sw?', 'condition')
        add_node('rb_dist_resize', 'euclidian_dist_factor.resize(frd.sw)')
        add_node('rb_dist_cx', 'center_x = frd.half_sw')
        
        # Loop components explicitly listed
        add_node('rb_loop_init', 'unsigned int x = 0')
        add_node('rb_loop_cond', 'x < frd.sw?', 'condition')
        add_node('rb_loop_dx', 'float dx = (x - center_x) * frd.inv_fov_scale')
        add_node('rb_loop_calc', 'euclidian_dist_factor[x] = std::sqrt(dx * dx + 1.0f)')
        add_node('rb_loop_inc', '++x')
        
        add_node('rb_as15', 'frd.euclidian_dist_factor = &euclidian_dist_factor')
        
        # External Pipeline Transitions
        add_node('rb_call_node', 'CALL: render_bsp_node(\ncurrent_map.root_node_id, frd)', 'call')
        add_node('rb_call_vp', 'CALL: render_visplanes(frd)', 'call')
        add_node('rb_call_vs', 'CALL: render_vissprites(frd)', 'call')
        add_node('rb_exit', 'RETURN / EXIT FRAME', 'exit')

        # Connect render_bsp sequence sequentially
        dot.edge('rb_entry', 'rb_check_root')
        dot.edge('rb_check_root', 'rb_as1', label='True')
        dot.edge('rb_check_root', 'rb_exit', label='False')
        
        # Chain single assignments
        rb_chain = ['rb_as1', 'rb_as2', 'rb_as3', 'rb_as4', 'rb_as5', 'rb_as6', 'rb_as7', 'rb_as8', 'rb_as9', 'rb_as10', 'rb_as11', 'rb_as12', 'rb_as13', 'rb_as14', 'rb_cond_dist']
        for i in range(len(rb_chain)-1):
            dot.edge(rb_chain[i], rb_chain[i+1])
            
        dot.edge('rb_cond_dist', 'rb_dist_resize', label='True')
        dot.edge('rb_cond_dist', 'rb_as15', label='False')
        dot.edge('rb_dist_resize', 'rb_dist_cx')
        dot.edge('rb_dist_cx', 'rb_loop_init')
        dot.edge('rb_loop_init', 'rb_loop_cond')
        dot.edge('rb_loop_cond', 'rb_loop_dx', label='True')
        dot.edge('rb_loop_dx', 'rb_loop_calc')
        dot.edge('rb_loop_calc', 'rb_loop_inc')
        dot.edge('rb_loop_inc', 'rb_loop_cond') # Loop back
        dot.edge('rb_loop_cond', 'rb_as15', label='False')
        
        dot.edge('rb_as15', 'rb_call_node')
        dot.edge('rb_call_node', 'rb_call_vp')
        dot.edge('rb_call_vp', 'rb_call_vs')
        dot.edge('rb_call_vs', 'rb_exit')

    # =========================================================================
    # 2. METHOD: render_bsp_node()
    # =========================================================================
    with dot.subgraph(name='cluster_render_bsp_node') as c:
        c.attr(label='software_renderer::render_bsp_node()', style='filled', fillcolor='#E8F8F5')
        
        add_node('rn_entry', 'ENTRY: render_bsp_node(node_id, frd)', 'entry')
        add_node('rn_is_leaf', 'geometry::bsp_node::\nis_leaf(node_id)?', 'condition')
        
        # Subsector (Leaf) Processing Sequence
        add_node('rn_leaf_id', 'auto subsector_id = bsp_node::get_id(node_id)')
        add_node('rn_leaf_sub', 'sub = current_map.subsectors[subsector_id]')
        add_node('rn_leaf_l0', 'uint8_t sub_light = 255')
        add_node('rn_leaf_empty', '!sub.lines.empty()?', 'condition')
        add_node('rn_leaf_fline', 'first_line = current_map.linedefs[sub.lines[0]]')
        add_node('rn_leaf_lupd', 'sub_light = sectors[sidedefs[first_line.front].facing_sector].light_level')
        
        # Dynamic unrolled iteration abstractions for Sprites & Lines loops
        add_node('rn_sprite_loop', 'Loop each sprite\nin sub.sprites')
        add_node('rn_call_addsprite', 'CALL: add_vissprite(\nsprite, sub_light, frd)', 'call')
        
        add_node('rn_line_loop', 'Loop each line_id\nin sub.lines')
        add_node('rn_call_projline', 'CALL: project_and_draw_linedef(\ncurrent_map.linedefs[line_id], frd)', 'call')
        add_node('rn_leaf_ret', 'RETURN', 'exit')
        
        # Node Splitting (Inner Node Path)
        add_node('rn_node_get', 'node = current_map.nodes[node_id]')
        add_node('rn_node_front', 'bool is_front = node.is_pt_front_side(frd.cam_pos)')
        add_node('rn_node_closer', 'closer_node = is_front ? node.front : node.back')
        add_node('rn_node_farther', 'farther_node = is_front ? node.back : node.front')
        add_node('rn_node_box', 'farther_box = is_front ? node.back_box : node.front_box')
        add_node('rn_recurse_close', 'CALL (Recurse):\nrender_bsp_node(closer_node, frd)', 'call')
        add_node('rn_box_visible', 'is_box_visible(\nfarther_box, frd)?', 'condition')
        add_node('rn_recurse_far', 'CALL (Recurse):\nrender_bsp_node(farther_node, frd)', 'call')
        add_node('rn_node_ret', 'RETURN', 'exit')

        # Connect Leaf/Node structures line-by-line
        dot.edge('rn_entry', 'rn_is_leaf')
        dot.edge('rn_is_leaf', 'rn_leaf_id', label='True (Leaf)')
        dot.edge('rn_leaf_id', 'rn_leaf_sub')
        dot.edge('rn_leaf_sub', 'rn_leaf_l0')
        dot.edge('rn_leaf_l0', 'rn_leaf_empty')
        dot.edge('rn_leaf_empty', 'rn_leaf_fline', label='True')
        dot.edge('rn_leaf_fline', 'rn_leaf_lupd')
        dot.edge('rn_leaf_lupd', 'rn_sprite_loop')
        dot.edge('rn_leaf_empty', 'rn_sprite_loop', label='False')
        
        dot.edge('rn_sprite_loop', 'rn_call_addsprite')
        dot.edge('rn_call_addsprite', 'rn_line_loop')
        dot.edge('rn_line_loop', 'rn_call_projline')
        dot.edge('rn_call_projline', 'rn_leaf_ret')
        
        # Connect Inner Node sequence
        dot.edge('rn_is_leaf', 'rn_node_get', label='False (Node)')
        dot.edge('rn_node_get', 'rn_node_front')
        dot.edge('rn_node_front', 'rn_node_closer')
        dot.edge('rn_node_closer', 'rn_node_farther')
        dot.edge('rn_node_farther', 'rn_node_box')
        dot.edge('rn_node_box', 'rn_recurse_close')
        dot.edge('rn_recurse_close', 'rn_box_visible')
        dot.edge('rn_box_visible', 'rn_recurse_far', label='True')
        dot.edge('rn_box_visible', 'rn_node_ret', label='False')
        dot.edge('rn_recurse_far', 'rn_node_ret')

    # =========================================================================
    # 3. METHOD: project_and_draw_linedef()
    # =========================================================================
    with dot.subgraph(name='cluster_project_linedef') as c:
        c.attr(label='software_renderer::project_and_draw_linedef()', style='filled', fillcolor='#FEF9E7')
        
        add_node('pl_entry', 'ENTRY: project_and_draw_linedef(line, frd)', 'entry')
        add_node('pl_as1', 'tr_v1 = line("seg")("point0") - frd.cam_pos')
        add_node('pl_as2', 'tr_v2 = line("seg")("point1") - frd.cam_pos')
        add_node('pl_as3', 'tr_v1 = vec2::rotate_with_known_trig(tr_v1, frd.cos, -frd.sin)')
        add_node('pl_as4', 'tr_v2 = vec2::rotate_with_known_trig(tr_v2, frd.cos, -frd.sin)')
        add_node('pl_as5', 'float u1 = 0.0f')
        add_node('pl_as6', 'float u2 = line.len()')
        
        add_node('pl_cond_behind', 'tr_v1("y") <= near_z &&\ntr_v2("y") <= near_z?', 'condition')
        add_node('pl_ret_behind', 'RETURN', 'exit')
        
        add_node('pl_as7', 'float inv_dy = 1.0f / (tr_v2("y") - tr_v1("y"))')
        
        # Near Plane Clipping Logic branches
        add_node('pl_clip_v1', 'tr_v1("y") < near_z?', 'condition')
        add_node('pl_v1_t', 'float t = (near_z - tr_v1("y")) * inv_dy')
        add_node('pl_v1_x', 'tr_v1("x") = tr_v1("x") + t * (tr_v2("x") - tr_v1("x"))')
        add_node('pl_v1_y', 'tr_v1("y") = near_z')
        add_node('pl_v1_u', 'u1 = u1 + t * (u2 - u1)')
        
        add_node('pl_clip_v2', 'tr_v2("y") < near_z?', 'condition')
        add_node('pl_v2_t', 'float t = (tr_v2("y") - near_z) * inv_dy')
        add_node('pl_v2_x', 'tr_v2("x") = tr_v2("x") + t * (tr_v1("x") - tr_v2("x"))')
        add_node('pl_v2_y', 'tr_v2("y") = near_z')
        add_node('pl_v2_u', 'u2 = u2 + t * (u1 - u2)')
        
        add_node('pl_as8', 'float proj_x1 = (tr_v1("x") / tr_v1("y")) * frd.fov_scale + frd.half_sw')
        add_node('pl_as9', 'float proj_x2 = (tr_v2("x") / tr_v2("y")) * frd.fov_scale + frd.half_sw')
        
        # Back-face viewing/swapping
        add_node('pl_cond_swap', 'proj_x1 > proj_x2?', 'condition')
        add_node('pl_swap_cull', 'line.is_wall()?', 'condition')
        add_node('pl_ret_cull', 'RETURN', 'exit')
        add_node('pl_do_swap_x', 'std::swap(proj_x1, proj_x2)')
        add_node('pl_do_swap_v', 'std::swap(tr_v1, tr_v2)')
        add_node('pl_do_swap_u', 'std::swap(u1, u2)')
        add_node('pl_do_swap_sd', 'std::swap(line.front, line.back)')
        
        add_node('pl_check_wall', 'line.is_wall()?', 'condition')
        add_node('pl_call_solid', 'CALL: draw_solid_wall_span(...)', 'call')
        add_node('pl_call_portal', 'CALL: draw_portal_wall_span(...)', 'call')

        # Linking Statement-by-Statement
        dot.edge('pl_entry', 'pl_as1')
        dot.edge('pl_as1', 'pl_as2')
        dot.edge('pl_as2', 'pl_as3')
        dot.edge('pl_as3', 'pl_as4')
        dot.edge('pl_as4', 'pl_as5')
        dot.edge('pl_as5', 'pl_as6')
        dot.edge('pl_as6', 'pl_cond_behind')
        dot.edge('pl_cond_behind', 'pl_ret_behind', label='True')
        dot.edge('pl_cond_behind', 'pl_as7', label='False')
        dot.edge('pl_as7', 'pl_clip_v1')
        
        # V1 Clip Branch
        dot.edge('pl_clip_v1', 'pl_v1_t', label='True')
        dot.edge('pl_v1_t', 'pl_v1_x')
        dot.edge('pl_v1_x', 'pl_v1_y')
        dot.edge('pl_v1_y', 'pl_v1_u')
        dot.edge('pl_v1_u', 'pl_clip_v2')
        dot.edge('pl_clip_v1', 'pl_clip_v2', label='False')
        
        # V2 Clip Branch
        dot.edge('pl_clip_v2', 'pl_v2_t', label='True')
        dot.edge('pl_v2_t', 'pl_v2_x')
        dot.edge('pl_v2_x', 'pl_v2_y')
        dot.edge('pl_v2_y', 'pl_v2_u')
        dot.edge('pl_v2_u', 'pl_as8')
        dot.edge('pl_clip_v2', 'pl_as8', label='False')
        
        dot.edge('pl_as8', 'pl_as9')
        dot.edge('pl_as9', 'pl_cond_swap')
        
        # Backface Switch / Swap Chains
        dot.edge('pl_cond_swap', 'pl_swap_cull', label='True')
        dot.edge('pl_swap_cull', 'pl_ret_cull', label='True')
        dot.edge('pl_swap_cull', 'pl_do_swap_x', label='False')
        dot.edge('pl_do_swap_x', 'pl_do_swap_v')
        dot.edge('pl_do_swap_v', 'pl_do_swap_u')
        dot.edge('pl_do_swap_u', 'pl_do_swap_sd')
        dot.edge('pl_do_swap_sd', 'pl_check_wall')
        
        dot.edge('pl_cond_swap', 'pl_check_wall', label='False')
        dot.edge('pl_check_wall', 'pl_call_solid', label='True')
        dot.edge('pl_check_wall', 'pl_call_portal', label='False')

    # =========================================================================
    # 4. METHOD: draw_solid_wall_span()
    # =========================================================================
    with dot.subgraph(name='cluster_draw_solid') as c:
        c.attr(label='software_renderer::draw_solid_wall_span()', style='filled', fillcolor='#FBEEE6')
        
        add_node('sw_entry', 'ENTRY: draw_solid_wall_span(...)', 'entry')
        add_node('sw_as1', 'sd = current_map.sidedefs[line.front]')
        add_node('sw_as2', 's = current_map.sectors[sd.facing_sector]')
        add_node('sw_as3', 'mt = tex_manager.wall_tx_by_id(sd.middle_tex)')
        add_node('sw_as4', 'int x1 = static_cast<int>(proj_x1)')
        add_node('sw_as4_2', 'int x2 = static_cast<int>(proj_x2)')
        add_node('sw_as5', 'float inv_z1 = 1.0f / z1')
        add_node('sw_as6', 'float inv_z2 = 1.0f / z2')
        add_node('sw_as7', 'int cx1 = std::max(0, x1)')
        add_node('sw_as8', 'int cx2 = std::min((int)frd.sw, x2)')
        
        # Horizontal X Scan Loop 
        add_node('sw_x_init', 'int x = cx1')
        add_node('sw_x_cond', 'x < cx2?', 'condition')
        add_node('sw_x_clip_chk', 'lower_clip[x] <=\nupper_clip[x]?', 'condition')
        
        # Math Loop Setup Line-by-Line
        add_node('sw_x_t', 'float t = (x - proj_x1) / (proj_x2 - proj_x1)')
        add_node('sw_x_invz', 'float inv_z = (1.0f - t) * inv_z1 + t * inv_z2')
        add_node('sw_x_depth', 'float depth = 1.0f / inv_z')
        add_node('sw_x_uoverz', 'float u_over_z = (1.0f - t) * u1 * inv_z1 + t * u2 * inv_z2')
        add_node('sw_x_u', 'unsigned int u = (unsigned int)(u_over_z * depth) % mt("width")')
        add_node('sw_x_light', 'int column_light = lighting::calculate(s.light_level, depth * (*frd.euclidian_dist_factor)[x])')
        add_node('sw_x_topy', 'int top_y = frd.sh/2 - (int)((s.ceiling_height - frd.cam_height) * frd.fov_scale * inv_z)')
        add_node('sw_x_boty', 'int bot_y = frd.sh/2 - (int)((s.floor_height - frd.cam_height) * frd.fov_scale * inv_z)')
        add_node('sw_x_ctopy', 'int cropped_top_y = std::max(top_y, std::max(0, upper_clip[x]))')
        add_node('sw_x_cboty', 'int cropped_bot_y = std::min(bot_y, lower_clip[x])')
        
        # Visplane allocations
        add_node('sw_call_vp1', 'CALL: visplane::add_column(\nvisplanes, x, std::max(0, upper_clip[x]), cropped_top_y, ... [Ceiling])', 'call')
        add_node('sw_call_vp2', 'CALL: visplane::add_column(\nvisplanes, x, cropped_bot_y, lower_clip[x], ... [Floor])', 'call')
        
        add_node('sw_x_vstep', 'float v_step = (float)mt("height") / (float)(bot_y - top_y)')
        add_node('sw_x_curv', 'float current_v = (float)(cropped_top_y - top_y) * v_step')
        
        # Vertical Y Raster Loop
        add_node('sw_y_init', 'int y = cropped_top_y')
        add_node('sw_y_cond', 'y < cropped_bot_y?', 'condition')
        add_node('sw_y_v', 'unsigned int v = static_cast<unsigned int>(current_v)')
        add_node('sw_y_vinc', 'current_v += v_step')
        add_node('sw_y_color', 'uint32_t color = mt("pixels")[u * mt("height") + v]')
        add_node('sw_y_write', 'frd.mmio[x + y * frd.pitch] = lighting::apply(color, column_light)')
        add_node('sw_y_inc', '++y')
        
        # End of X column loop iteration
        add_node('sw_x_uclip', 'upper_clip[x] = frd.sh')
        add_node('sw_x_lclip', 'lower_clip[x] = 0')
        add_node('sw_x_inc', '++x')
        add_node('sw_exit', 'RETURN', 'exit')

        # Hook up Solid pipeline
        dot.edge('sw_entry', 'sw_as1')
        dot.edge('sw_as1', 'sw_as2')
        dot.edge('sw_as2', 'sw_as3')
        dot.edge('sw_as3', 'sw_as4')
        dot.edge('sw_as4', 'sw_as4_2')
        dot.edge('sw_as4_2', 'sw_as5')
        dot.edge('sw_as5', 'sw_as6')
        dot.edge('sw_as6', 'sw_as7')
        dot.edge('sw_as7', 'sw_as8')
        dot.edge('sw_as8', 'sw_x_init')
        dot.edge('sw_x_init', 'sw_x_cond')
        
        dot.edge('sw_x_cond', 'sw_x_clip_chk', label='True')
        dot.edge('sw_x_cond', 'sw_exit', label='False')
        
        dot.edge('sw_x_clip_chk', 'sw_x_inc', label='True (Culled)')
        dot.edge('sw_x_clip_chk', 'sw_x_t', label='False')
        
        # Loop Arithmetic Sequential Flow
        dot.edge('sw_x_t', 'sw_x_invz')
        dot.edge('sw_x_invz', 'sw_x_depth')
        dot.edge('sw_x_depth', 'sw_x_uoverz')
        dot.edge('sw_x_uoverz', 'sw_x_u')
        dot.edge('sw_x_u', 'sw_x_light')
        dot.edge('sw_x_light', 'sw_x_topy')
        dot.edge('sw_x_topy', 'sw_x_boty')
        dot.edge('sw_x_boty', 'sw_x_ctopy')
        dot.edge('sw_x_ctopy', 'sw_x_cboty')
        dot.edge('sw_x_cboty', 'sw_call_vp1')
        dot.edge('sw_call_vp1', 'sw_call_vp2')
        dot.edge('sw_call_vp2', 'sw_x_vstep')
        dot.edge('sw_x_vstep', 'sw_x_curv')
        dot.edge('sw_x_curv', 'sw_y_init')
        
        # Raster Inner Loop
        dot.edge('sw_y_init', 'sw_y_cond')
        dot.edge('sw_y_cond', 'sw_y_v', label='True')
        dot.edge('sw_y_v', 'sw_y_vinc')
        dot.edge('sw_y_vinc', 'sw_y_color')
        dot.edge('sw_y_color', 'sw_y_write')
        dot.edge('sw_y_write', 'sw_y_inc')
        dot.edge('sw_y_inc', 'sw_y_cond')
        
        # Post-Raster updating
        dot.edge('sw_y_cond', 'sw_x_uclip', label='False')
        dot.edge('sw_x_uclip', 'sw_x_lclip')
        dot.edge('sw_x_lclip', 'sw_x_inc')
        dot.edge('sw_x_inc', 'sw_x_cond')

    # =========================================================================
    # INTER-CLUSTER JUMPS
    # =========================================================================
    dot.edge('rb_call_node', 'rn_entry', style='dashed', constraint='false')
    dot.edge('pl_call_solid', 'sw_entry', style='dashed', constraint='false')

    # 5. METHOD: draw_portal_wall_span()
# =========================================================================
    with dot.subgraph(name='cluster_draw_portal') as c:
        c.attr(label='software_renderer::draw_portal_wall_span()', style='filled', fillcolor='#EBF5FB')
        
        add_node('pw_entry', 'ENTRY: draw_portal_wall_span(...)', 'entry')
        add_node('pw_as1', 'front_sd = current_map.sidedefs[line.front]\nback_sd = current_map.sidedefs[line.back]')
        add_node('pw_as2', 'front = current_map.sectors[front_sd.facing_sector]\nback = current_map.sectors[back_sd.facing_sector]')
        add_node('pw_as3', 'ut = tex_manager.wall_tx_by_id(front_sd.upper_tex)\nlt = tex_manager.wall_tx_by_id(front_sd.lower_tex)')
        add_node('pw_as4', 'int x1 = (int)proj_x1\nint x2 = (int)proj_x2')
        add_node('pw_as5', 'int cx1 = std::max(0, x1)\nint cx2 = std::min((int)frd.sw, x2)')
        
        # Horizontal X Scan Loop 
        add_node('pw_x_init', 'int x = cx1')
        add_node('pw_x_cond', 'x < cx2?', 'condition')
        add_node('pw_x_clip_chk', 'lower_clip[x] <=\nupper_clip[x]?', 'condition')
        
        # Math & Projections
        add_node('pw_x_math', 'float t = ((float)x - proj_x1) / (proj_x2 - proj_x1)\nfloat inv_z = (1.0f - t) * inv_z1 + t * inv_z2\nfloat depth = 1.0f / inv_z')
        add_node('pw_x_tex', 'unsigned int u = (unsigned int)(u_over_z * depth)\nu_ut = u % ut("width")\nu_lt = u % lt("width")')
        add_node('pw_x_light', 'int column_light = lighting::calculate(...)')
        add_node('pw_x_proj', 'c_fc = projection(front.ceiling)\nc_ff = projection(front.floor)\nc_bc = projection(back.ceiling)\nc_bf = projection(back.floor)')
        
        # Visplanes allocation
        add_node('pw_call_vp1', 'CALL: visplane::add_column [Ceiling]', 'call')
        add_node('pw_call_vp2', 'CALL: visplane::add_column [Floor]', 'call')
        
        # Upper Wall Portal Loop
        add_node('pw_cond_upper', 'c_fc < c_bc?', 'condition')
        add_node('pw_upper_raster', 'Rasterize upper wall texture (ut)\nfrom draw_top to draw_bot')
        
        # Lower Wall Portal Loop
        add_node('pw_cond_lower', 'c_ff > c_bf?', 'condition')
        add_node('pw_lower_raster', 'Rasterize lower wall texture (lt)\nfrom draw_top to draw_bot')
        
        # Update clipping
        add_node('pw_x_clip_upd', 'upper_clip[x] = std::max(upper_clip[x], std::max(c_fc, c_bc))\nlower_clip[x] = std::min(lower_clip[x], std::min(c_ff, c_bf))')
        add_node('pw_x_inc', '++x')
        add_node('pw_exit', 'RETURN', 'exit')
        
        # Hook up Portal pipeline
        dot.edge('pw_entry', 'pw_as1')
        dot.edge('pw_as1', 'pw_as2')
        dot.edge('pw_as2', 'pw_as3')
        dot.edge('pw_as3', 'pw_as4')
        dot.edge('pw_as4', 'pw_as5')
        dot.edge('pw_as5', 'pw_x_init')
        dot.edge('pw_x_init', 'pw_x_cond')
        dot.edge('pw_x_cond', 'pw_x_clip_chk', label='True')
        dot.edge('pw_x_cond', 'pw_exit', label='False')
        
        dot.edge('pw_x_clip_chk', 'pw_x_inc', label='True (Culled)')
        dot.edge('pw_x_clip_chk', 'pw_x_math', label='False')
        dot.edge('pw_x_math', 'pw_x_tex')
        dot.edge('pw_x_tex', 'pw_x_light')
        dot.edge('pw_x_light', 'pw_x_proj')
        dot.edge('pw_x_proj', 'pw_call_vp1')
        dot.edge('pw_call_vp1', 'pw_call_vp2')
        dot.edge('pw_call_vp2', 'pw_cond_upper')
        
        dot.edge('pw_cond_upper', 'pw_upper_raster', label='True')
        dot.edge('pw_cond_upper', 'pw_cond_lower', label='False')
        dot.edge('pw_upper_raster', 'pw_cond_lower')
        
        dot.edge('pw_cond_lower', 'pw_lower_raster', label='True')
        dot.edge('pw_cond_lower', 'pw_x_clip_upd', label='False')
        dot.edge('pw_lower_raster', 'pw_x_clip_upd')
        
        dot.edge('pw_x_clip_upd', 'pw_x_inc')
        dot.edge('pw_x_inc', 'pw_x_cond')

# =========================================================================
# 6. METHOD: is_box_visible()
# =========================================================================
    with dot.subgraph(name='cluster_is_box_visible') as c:
        c.attr(label='software_renderer::is_box_visible()', style='filled', fillcolor='#F4ECF7')
        
        add_node('bv_entry', 'ENTRY: is_box_visible(box, frd)', 'entry')
        add_node('bv_chk_inside', 'Camera inside bounding box?', 'condition')
        add_node('bv_ret_true1', 'RETURN true', 'exit')
        
        add_node('bv_setup_corners', 'Define 4 box corners\nTransform & rotate to camera space')
        add_node('bv_clip_loop', 'Clip rectangle against near plane\nProduce clipped vertices (max 5)')
        add_node('bv_chk_clipped', 'num_clipped == 0?', 'condition')
        add_node('bv_ret_false1', 'RETURN false', 'exit')
        
        add_node('bv_proj_x', 'Project clipped vertices to screen\nFind min_proj_x and max_proj_x')
        add_node('bv_frustum_cull', 'Outside screen FOV?\n(min_x >= sw || max_x < 0)', 'condition')
        add_node('bv_ret_false2', 'RETURN false', 'exit')
        
        add_node('bv_clamp', 'Clamp screen bounds to [cx1, cx2]')
        add_node('bv_occ_loop', 'Loop x from cx1 to cx2\nlower_clip[x] > upper_clip[x]?', 'condition')
        add_node('bv_ret_true2', 'RETURN true', 'exit')
        add_node('bv_ret_false3', 'RETURN false', 'exit')
        
        dot.edge('bv_entry', 'bv_chk_inside')
        dot.edge('bv_chk_inside', 'bv_ret_true1', label='True')
        dot.edge('bv_chk_inside', 'bv_setup_corners', label='False')
        dot.edge('bv_setup_corners', 'bv_clip_loop')
        dot.edge('bv_clip_loop', 'bv_chk_clipped')
        dot.edge('bv_chk_clipped', 'bv_ret_false1', label='True')
        dot.edge('bv_chk_clipped', 'bv_proj_x', label='False')
        dot.edge('bv_proj_x', 'bv_frustum_cull')
        dot.edge('bv_frustum_cull', 'bv_ret_false2', label='True')
        dot.edge('bv_frustum_cull', 'bv_clamp', label='False')
        dot.edge('bv_clamp', 'bv_occ_loop')
        dot.edge('bv_occ_loop', 'bv_ret_true2', label='True (Visible)')
        dot.edge('bv_occ_loop', 'bv_ret_false3', label='False (Fully Occluded)')

# =========================================================================
# 7. METHOD: add_vissprite()
# =========================================================================
    with dot.subgraph(name='cluster_add_vissprite') as c:
        c.attr(label='software_renderer::add_vissprite()', style='filled', fillcolor='#E9F7EF')
        
        add_node('av_entry', 'ENTRY: add_vissprite(sprite, light, frd)', 'entry')
        add_node('av_transform', 'Translate sprite position to camera space\nRotate using camera angle trig')
        add_node('av_chk_near', 'tr_pos("y") <= near_z?', 'condition')
        add_node('av_ret_near', 'RETURN', 'exit')
        
        add_node('av_project', 'Project X to screen (proj_x)\nCalculate horizontal screen bounds [x1, x2]')
        add_node('av_frustum', 'Frustum cull: x1 >= sw || x2 < 0?', 'condition')
        add_node('av_ret_frustum', 'RETURN', 'exit')
        
        add_node('av_clamp', 'Clamp screen bounds to [cx1, cx2]')
        add_node('av_occ_chk', 'Loop x from cx1 to cx2:\nAny lower_clip[x] > upper_clip[x]?', 'condition')
        add_node('av_ret_occ', 'RETURN (Hidden)', 'exit')
        
        add_node('av_slice', 'Slice upper_clip & lower_clip vectors\nEmplace back to vissprites vector')
        add_node('av_exit', 'RETURN', 'exit')
        
        dot.edge('av_entry', 'av_transform')
        dot.edge('av_transform', 'av_chk_near')
        dot.edge('av_chk_near', 'av_ret_near', label='True')
        dot.edge('av_chk_near', 'av_project', label='False')
        dot.edge('av_project', 'av_frustum')
        dot.edge('av_frustum', 'av_ret_frustum', label='True')
        dot.edge('av_frustum', 'av_clamp', label='False')
        dot.edge('av_clamp', 'av_occ_chk')
        dot.edge('av_occ_chk', 'av_slice', label='True (Visible)')
        dot.edge('av_occ_chk', 'av_ret_occ', label='False')
        dot.edge('av_slice', 'av_exit')

# =========================================================================
# 8. METHOD: visplane::render()
# =========================================================================
    with dot.subgraph(name='cluster_visplane_render') as c:
        c.attr(label='visplane::render()', style='filled', fillcolor='#F2F3F4')
        
        add_node('vpr_entry', 'ENTRY: render(tex_manager, frd)', 'entry')
        add_node('vpr_precalc', 'Precalculate world space step factors\nFind min_y and max_y bounds across columns')
        
        add_node('vpr_y_loop', 'Loop y from min_y to max_y', 'condition')
        add_node('vpr_y_setup', 'Calculate depth z based on height delta\nCalculate row light level\nCalculate fixed-point world step sizes')
        
        add_node('vpr_x_scan', 'Scan row columns to identify active spans')
        add_node('vpr_span_loop', 'Loop each identified span', 'condition')
        add_node('vpr_span_coord', 'Grab initial fixed-point world coordinates')
        add_node('vpr_pixel_loop', 'Loop sx from span_start to span_end')
        add_node('vpr_pixel_draw', 'Fetch flat texture pixel (u, v)\nApply row lighting\nWrite to frd.mmio framebuffer\nStep fixed-point coordinates (fx, fy)')
        
        add_node('vpr_exit', 'RETURN', 'exit')
        
        dot.edge('vpr_entry', 'vpr_precalc')
        dot.edge('vpr_precalc', 'vpr_y_loop')
        dot.edge('vpr_y_loop', 'vpr_y_setup', label='Next Row')
        dot.edge('vpr_y_loop', 'vpr_exit', label='Done')
        dot.edge('vpr_y_setup', 'vpr_x_scan')
        dot.edge('vpr_x_scan', 'vpr_span_loop')
        dot.edge('vpr_span_loop', 'vpr_span_coord', label='Next Span')
        dot.edge('vpr_span_loop', 'vpr_y_loop', label='Spans Done')
        dot.edge('vpr_span_coord', 'vpr_pixel_loop')
        dot.edge('vpr_pixel_loop', 'vpr_pixel_draw')
        dot.edge('vpr_pixel_draw', 'vpr_pixel_loop', label='Next Pixel')
        dot.edge('vpr_pixel_loop', 'vpr_span_loop', label='Span Done')

# =========================================================================
# 9. METHOD: vissprite::render()
# =========================================================================
    with dot.subgraph(name='cluster_vissprite_render') as c:
        c.attr(label='vissprite::render()', style='filled', fillcolor='#FBFAF0')
        
        add_node('vsr_entry', 'ENTRY: render(tex_manager, frd)', 'entry')
        add_node('vsr_setup', 'Fetch sprite texture by id\nCalculate distance factor & lighting\nProject vertical screen bounds [scr_y_top, scr_y_bot]')
        
        add_node('vsr_x_loop', 'Loop x from cx1 to cx2', 'condition')
        add_node('vsr_clip', 'Get slice upper_clip and lower_clip\nOcclusion culling: if clip_bot <= clip_top continue')
        add_node('vsr_y_bounds', 'Calculate draw_top and draw_bot\nCalculate texture coordinate u')
        
        add_node('vsr_y_loop', 'Loop y from draw_top to draw_bot', 'condition')
        add_node('vsr_pixel_chk', 'Fetch pixel from sprite column\nIs pixel transparent (0xffff00ff)?', 'condition')
        add_node('vsr_pixel_draw', 'Apply sprite lighting\nIf flash_red, apply red tinting math\nWrite to frd.mmio frame buffer')
        add_node('vsr_y_inc', 'Increment current_v by inv_scale\n++y')
        add_node('vsr_x_inc', '++x')
        add_node('vsr_exit', 'RETURN', 'exit')
        
        dot.edge('vsr_entry', 'vsr_setup')
        dot.edge('vsr_setup', 'vsr_x_loop')
        dot.edge('vsr_x_loop', 'vsr_clip', label='Next Column')
        dot.edge('vsr_x_loop', 'vsr_exit', label='Done')
        dot.edge('vsr_clip', 'vsr_x_inc', label='Occluded')
        dot.edge('vsr_clip', 'vsr_y_bounds', label='Visible Column')
        dot.edge('vsr_y_bounds', 'vsr_y_loop')
        dot.edge('vsr_y_loop', 'vsr_pixel_chk', label='Next Pixel')
        dot.edge('vsr_y_loop', 'vsr_x_inc', label='Column Done')
        dot.edge('vsr_pixel_chk', 'vsr_y_inc', label='Transparent')
        dot.edge('vsr_pixel_chk', 'vsr_pixel_draw', label='Opaque')
        dot.edge('vsr_pixel_draw', 'vsr_y_inc')
        dot.edge('vsr_y_inc', 'vsr_y_loop')
        dot.edge('vsr_x_inc', 'vsr_x_loop')

# =========================================================================
# ADDITIONAL INTER-CLUSTER PIPELINE CONNECTIONS
# =========================================================================
    dot.edge('rn_box_visible', 'bv_entry', style='dashed', constraint='false')
    dot.edge('rn_call_addsprite', 'av_entry', style='dashed', constraint='false')
    dot.edge('pl_call_portal', 'pw_entry', style='dashed', constraint='false')
    dot.edge('rb_call_vp', 'vpr_entry', style='dashed', constraint='false')
    dot.edge('rb_call_vs', 'vsr_entry', style='dashed', constraint='false')

    return dot

if __name__ == '__main__':
    chart = create_unrolled_renderer_flowchart()
    chart.render('block-diagram', format='pdf')
    print("Graph generated successfully as 'renderer_line_by_line_unrolled.png'.")
