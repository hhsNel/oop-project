// manual-tests/player-hud-2d.cpp
// Manual test: Player HUD 2D renderer
//
// Features tested:
//   HP bar, Armor bar, Status effects, Weapon loadout, Ammo bar,
//   Shoot cooldown bar, Reload bar, Add/remove weapons,
//   Damage / Heal / Armor commands, Burning effect, Game-over screen
//
// Controls:
//   LMB        – shoot current weapon
//   R          – reload (costs 1 reserve mag)
//   1-7        – switch weapon (by loadout slot)
//   =          – add next weapon to loadout
//   -          – remove current weapon from loadout
//   D/H/A      – open input: Damage / Heal / Armor amount, then Enter
//   B          – apply Burning status effect
//   V          – revive (only on Game Over screen)
//   ESC        – cancel command / exit

#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>

#include "util/resource-loader.h"
#include "entities/player.h"
#include "combat/weapons/pistol.h"
#include "combat/weapons/smg.h"
#include "combat/weapons/rifle.h"
#include "combat/weapons/shotgun.h"
#include "combat/weapons/sniper-rifle.h"
#include "combat/weapons/plasma-gun.h"
#include "combat/weapons/katana.h"
#include "combat/burning.h"

#include "input/input-backend.h"
#include "input/evdev-backend.h"
#include "rendering/drm-kms/backend.h"
#include "rendering/renderer-2d.h"
#include "assets/asset-manager.h"

// ── Helpers ──────────────────────────────────────────────────────────────────

// Exposes health fields via componentized operator() — bez lamania enkapsulacji.
template<typename M>
struct inspect : public M {
    using M::M;
    float hp()           const { return this->health("current_hp"_f); }
    float max_hp()       const { return this->health("max_hp"_f); }
    float armor()        const { return this->health("armor"_f); }
    float max_armor()    const { return this->health("max_armor"_f); }
    bool  dead()         const { return this->health.is_dead(); }
    int   effect_count() const { return static_cast<int>(this->health("active_effects"_f).size()); }
};

struct weapon_slot {
    combat::weapons::weapon* ref;   // non-owning, for identification
    std::string name;
    float reload_timer;
    float reload_duration;
    bool  in_loadout;
    bool  is_melee;
    bool  is_auto;
};

enum class cmd_mode { none, damage, heal, armor };

static weapon_slot* find_slot(std::vector<weapon_slot>& pool,
                               combat::weapons::weapon* cur)
{
    if (!cur) return nullptr;
    for (auto& s : pool)
        if (s.ref == cur) return &s;
    return nullptr;
}

static std::string fmt1(float v) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1) << v;
    return ss.str();
}

// ── main ─────────────────────────────────────────────────────────────────────

int main() {

    // ── Rendering backend ────────────────────────────────────────────────────
    auto r_back = std::make_unique<rendering::drm_kms::backend>();
    if (r_back->bad()) { std::cerr << "error: DRM/KMS init failed\n"; return 1; }

    auto modes = (*r_back)("modes"_f);
    if (modes.empty()) { std::cerr << "error: no display modes\n"; return 1; }
    r_back->push_mode(std::move(modes[0]));
    if (r_back->bad()) { std::cerr << "error: push_mode failed\n"; return 1; }

    const int SW = static_cast<int>((*r_back)("width"_f));
    const int SH = static_cast<int>((*r_back)("height"_f));

    // ── Resources ────────────────────────────────────────────────────────────
    util::resource_loader rl;
    auto tex_mgr = assets::asset_manager::load(rl);

    rendering::renderer_2d r2d(*r_back, tex_mgr, tex_mgr.flat_tx_by_id(0));

    // ── Layout ───────────────────────────────────────────────────────────────
    const int HUD_H = 210;
    const int HUD_Y = SH - HUD_H;
    const int COL_W = SW / 3;

    const int PAD  = 14;   // horizontal padding inside column
    const int VPAD = 12;   // vertical padding from HUD top

    const int CW  = 14;    // normal char width
    const int CH  = 22;    // normal char height
    const int LS  = CH + 4;

    const int SCW = 11;    // small char width
    const int SCH = 16;    // small char height
    const int SLS = SCH + 3;

    const int BH   = CH - 4;   // bar height
    const int BARW = 200;       // standard bar width

    // Colors
    const std::uint32_t C_BBKG    = 0xFF333333;
    const std::uint32_t C_HP_HI   = 0xFF00CC44;
    const std::uint32_t C_HP_MID  = 0xFFFF8800;
    const std::uint32_t C_HP_LO   = 0xFFCC2222;
    const std::uint32_t C_ARMOR   = 0xFF3399FF;
    const std::uint32_t C_AMMO    = 0xFFFFDD00;
    const std::uint32_t C_CD      = 0xFFBBBBBB;
    const std::uint32_t C_RELOAD  = 0xFF441111;
    const std::uint32_t C_RED     = 0xFFCC0000;
    const std::uint32_t C_OVERLAY = 0xFF1A1A2A;
    const std::uint32_t TW        = 0xFFFF00;  // white text (pixel * 1 = original white)

    // Filled progress bar: background + colored fill proportional to cur/max.
    auto draw_bar = [&](int x, int y, int w, int h, float cur, float max,
                        std::uint32_t color) {
        r2d.draw_rect(x, y, w, h, C_BBKG);
        if (max > 0.0f) {
            int fill = static_cast<int>(std::clamp(cur / max, 0.0f, 1.0f) * w);
            if (fill > 0) r2d.draw_rect(x, y, fill, h, color);
        }
    };

    // ── Player ───────────────────────────────────────────────────────────────
    inspect<entities::player> p({0.0f, 0.0f}, 0.0f, 0, 1.0f, 100.0f, 50.0f, 2.0f, 1.0f);

    // ── Weapon pool (metadata only — weapons live in p.weapons) ─────────────
    std::vector<weapon_slot> pool;

    auto add_weapon = [&](auto wptr, const std::string& name,
                          float reload_dur,
                          bool melee = false, bool is_auto = false) {
        p.weapons.push_back(std::move(wptr));
        pool.push_back({ &*p.weapons.back(), name, 0.0f, reload_dur,
                         false, melee, is_auto });
    };

    add_weapon(std::make_unique<combat::weapons::pistol>(),       "Pistol",  1.5f);
    add_weapon(std::make_unique<combat::weapons::smg>(),         "SMG",     2.5f, false, true);
    add_weapon(std::make_unique<combat::weapons::rifle>(),       "Rifle",   2.0f, false, true);
    add_weapon(std::make_unique<combat::weapons::shotgun>(),     "Shotgun", 3.0f);
    add_weapon(std::make_unique<combat::weapons::sniper_rifle>(),"Sniper",  3.5f);
    add_weapon(std::make_unique<combat::weapons::plasma_gun>(),  "Plasma",  2.0f);
    add_weapon(std::make_unique<combat::weapons::katana>(),      "Katana",  0.0f, true);

    pool[0].in_loadout = true;  // start with Pistol equipped

    // Helper: get raw pointer to current weapon
    auto cur_wp = [&]() -> combat::weapons::weapon* {
        if (p.current_weapon_index < 0 || p.current_weapon_index >= (int)p.weapons.size())
            return nullptr;
        return &*p.weapons[p.current_weapon_index];
    };

    // Helper: get indices (into p.weapons) of in-loadout weapons, in pool order
    auto loadout_indices = [&]() -> std::vector<int> {
        std::vector<int> result;
        for (auto& s : pool) {
            if (!s.in_loadout) continue;
            for (int i = 0; i < (int)p.weapons.size(); ++i)
                if (&*p.weapons[i] == s.ref) { result.push_back(i); break; }
        }
        return result;
    };

    // Select the right weapon after loadout change
    auto rebuild_loadout = [&]() {
        auto li = loadout_indices();
        if (li.empty()) { p.current_weapon_index = -1; return; }
        for (int idx : li)
            if (idx == p.current_weapon_index) return;
        p.switch_weapons(li[0]);
    };
    rebuild_loadout();

    // ── Input backend ─────────────────────────────────────────────────────────
    auto i_back = std::make_unique<input::evdev::backend>();
    if (i_back->is_bad()) { std::cerr << "error: input init failed\n"; return 1; }

    // ── Input state ───────────────────────────────────────────────────────────
    cmd_mode    mode = cmd_mode::none;
    std::string num_buf;

    bool prev_lmb   = false;
    bool prev_r     = false;
    bool prev_plus  = false;
    bool prev_minus = false;
    bool prev_d     = false;
    bool prev_h     = false;
    bool prev_a     = false;
    bool prev_b     = false;
    bool prev_v     = false;
    bool prev_enter = false;
    bool prev_bs    = false;
    bool prev_num[10] = {};

    const input::key numkeys[10] = {
        input::key::n0, input::key::n1, input::key::n2, input::key::n3,
        input::key::n4, input::key::n5, input::key::n6, input::key::n7,
        input::key::n8, input::key::n9
    };

    // Returns true on the rising edge of a key (pressed this frame, not last).
    auto edge = [&](input::key k, bool& prev) -> bool {
        bool cur   = i_back->is_key_down(k);
        bool fired = cur && !prev;
        prev = cur;
        return fired;
    };

    auto last_time = std::chrono::steady_clock::now();

    float damage_flash_timer = 0.0f;
    const float FLASH_DURATION = 0.45f;

    // ── Game loop ─────────────────────────────────────────────────────────────
    while (true) {
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - last_time).count();
        last_time = now;

        i_back->update();
        if (i_back->is_bad()) break;

        bool game_over = p.dead();
        weapon_slot* cur_slot = find_slot(pool, cur_wp());

        // ESC: cancel active command, or exit
        if (i_back->is_key_down(input::key::esc)) {
            if (mode != cmd_mode::none) { mode = cmd_mode::none; num_buf.clear(); }
            else break;
        }

        if (game_over) {
            // V: revive player to full HP (armor stays as-is)
            if (edge(input::key::v, prev_v))
                p.heal(p.max_hp());

        } else {
            // 1–7: switch weapon slot (disabled while typing a command number)
            if (mode == cmd_mode::none) {
                for (int i = 1; i <= 7; ++i) {
                    bool down = i_back->is_key_down(numkeys[i]);
                    if (down && !prev_num[i]) {
                        auto li = loadout_indices();
                        if (i - 1 < (int)li.size()) p.switch_weapons(li[i - 1]);
                    }
                    prev_num[i] = down;
                }
                cur_slot = find_slot(pool, cur_wp());
            }

            // LMB: shoot (auto = held, semi = edge)
            {
                auto mouse    = i_back->get_mouse_state();
                bool reloading = cur_slot && cur_slot->reload_timer > 0.0f;
                bool triggered = cur_slot && cur_slot->is_auto
                               ? mouse.left : (mouse.left && !prev_lmb);
                if (!reloading && triggered) p.shoot();
                prev_lmb = mouse.left;
            }

            // R: start reload (costs one reserve magazine)
            {
                bool r_down = i_back->is_key_down(input::key::r);
                if (r_down && !prev_r && cur_slot
                    && !cur_slot->is_melee
                    && cur_slot->ref->reserve_mags > 0
                    && cur_slot->reload_timer <= 0.0f)
                {
                    cur_slot->reload_timer = cur_slot->reload_duration;
                }
                prev_r = r_down;
            }

            // =: add next weapon from pool to loadout
            {
                bool plus_down = i_back->is_key_down(input::key::equals);
                if (plus_down && !prev_plus)
                    for (auto& s : pool)
                        if (!s.in_loadout) { s.in_loadout = true; rebuild_loadout(); break; }
                prev_plus = plus_down;
            }

            // -: remove current weapon from loadout
            {
                bool minus_down = i_back->is_key_down(input::key::hyphen);
                if (minus_down && !prev_minus && cur_wp())
                    for (auto& s : pool)
                        if (s.in_loadout && s.ref == cur_wp()) {
                            s.in_loadout = false; rebuild_loadout(); break;
                        }
                prev_minus = minus_down;
            }

            // D / H / A: open number-entry command; B: apply Burning instantly
            if (mode == cmd_mode::none) {
                if (edge(input::key::d, prev_d)) { mode = cmd_mode::damage; num_buf.clear(); }
                if (edge(input::key::h, prev_h)) { mode = cmd_mode::heal;   num_buf.clear(); }
                if (edge(input::key::a, prev_a)) { mode = cmd_mode::armor;  num_buf.clear(); }
                if (edge(input::key::b, prev_b))
                    p.add_effect(std::make_unique<combat::burning>(5.0f, 5));
            }

            // Number / Backspace / Enter while a command is active
            if (mode != cmd_mode::none) {
                for (int i = 0; i < 10; ++i) {
                    bool down = i_back->is_key_down(numkeys[i]);
                    if (down && !prev_num[i]) num_buf += static_cast<char>('0' + i);
                    prev_num[i] = down;
                }
                if (edge(input::key::backspace, prev_bs) && !num_buf.empty())
                    num_buf.pop_back();
                if (edge(input::key::enter, prev_enter)) {
                    if (!num_buf.empty()) {
                        float val = static_cast<float>(std::stoi(num_buf));
                        if      (mode == cmd_mode::damage) { p.take_damage(val); damage_flash_timer = FLASH_DURATION; }
                        else if (mode == cmd_mode::heal)   p.heal(val);
                        else if (mode == cmd_mode::armor)  p.add_shield(val);
                    }
                    mode = cmd_mode::none;
                    num_buf.clear();
                }
            }

            // Tick reload timers; apply reload() when timer expires
            for (auto& s : pool) {
                if (s.reload_timer > 0.0f) {
                    s.reload_timer -= dt;
                    if (s.reload_timer <= 0.0f) {
                        s.reload_timer = 0.0f;
                        s.ref->reload();
                    }
                }
            }
        }

        float hp_before = p.hp();
        p.update(dt);
        if (p.hp() < hp_before)           // status effects (np. burning) zadaly obrazenia
            damage_flash_timer = FLASH_DURATION;

        auto* cw = cur_wp();
        cur_slot = find_slot(pool, cw);
        if (damage_flash_timer > 0.0f)
            damage_flash_timer -= dt;

        // ── RENDER FRAME ──────────────────────────────────────────────────────

        // Clear to black
        std::memset((*r_back)("mmio"_f), 0,
                    static_cast<std::size_t>((*r_back)("height"_f)) * (*r_back)("pitch"_f));

        // Damage flash: red overlay fading out over FLASH_DURATION seconds
        if (damage_flash_timer > 0.0f) {
            float t     = std::clamp(damage_flash_timer / FLASH_DURATION, 0.0f, 1.0f);
            auto  alpha = static_cast<std::uint32_t>(t * 160);  // max ~63% opacity
            r2d.draw_rect(0, 0, SW, SH, (alpha << 24) | 0x00CC0000);
        }

        // HUD background: HUD.jpg (flat texture id 1) stretched across full width
        r2d.draw_texture(tex_mgr.flat_tx_by_id(1), 0, HUD_Y, SW, HUD_H);

        // Darkened column overlays for text legibility
        r2d.draw_rect(0,           HUD_Y, COL_W,            HUD_H, 0xBB080810);
        r2d.draw_rect(COL_W,       HUD_Y, COL_W,            HUD_H, 0xBB081008);
        r2d.draw_rect(COL_W * 2,   HUD_Y, SW - COL_W * 2,  HUD_H, 0xBB100808);

        // ── COL 1: Player status — HP / Armor / Effects ───────────────────
        {
            int cx = PAD;
            int cy = HUD_Y + VPAD;

            // HP bar (color shifts green → orange → red by ratio)
            float hp_ratio  = p.max_hp() > 0.0f ? p.hp() / p.max_hp() : 0.0f;
            auto  hp_color  = hp_ratio > 0.6f ? C_HP_HI
                            : hp_ratio > 0.3f ? C_HP_MID : C_HP_LO;
            r2d.draw_text("HP:", cx, cy + 2, CW, CH - 4, TW);
            draw_bar(cx + 40, cy, BARW, BH, p.hp(), p.max_hp(), hp_color);
            r2d.draw_text(fmt1(p.hp()) + "/" + fmt1(p.max_hp()),
                          cx + 44 + BARW, cy + 2, CW - 2, CH - 4, TW);
            cy += LS;

            // Armor bar
            r2d.draw_text("AR:", cx, cy + 2, CW, CH - 4, TW);
            draw_bar(cx + 40, cy, BARW, BH, p.armor(), p.max_armor(), C_ARMOR);
            r2d.draw_text(fmt1(p.armor()) + "/" + fmt1(p.max_armor()),
                          cx + 44 + BARW, cy + 2, CW - 2, CH - 4, TW);
            cy += LS;

            // Active status effects
            if (p.effect_count() > 0) {
                r2d.draw_rect(cx, cy, 180, CH, 0xFF331100);
                r2d.draw_text("FX: BURNING", cx + 4, cy + 2, CW, CH - 4, TW);
            } else {
                r2d.draw_text("FX: none", cx, cy + 2, CW, CH - 4, TW);
            }
            cy += LS + 8;

            // Controls hint
            r2d.draw_text("D:dmg  H:heal  A:armor  B:burn", cx, cy, SCW, SCH, TW);
        }

        // ── COL 2: Weapon info — name / ammo / cooldown / reload ─────────
        {
            int cx = COL_W + PAD;
            int cy = HUD_Y + VPAD;

            if (!cw || !cur_slot) {
                r2d.draw_text("Weapon: (none)", cx, cy, CW, CH, TW);
            } else {
                auto li = loadout_indices();
                int slot_pos = 0;
                for (int i = 0; i < (int)li.size(); ++i)
                    if (li[i] == p.current_weapon_index) { slot_pos = i; break; }

                // Weapon name + auto tag + slot index
                std::string wname = cur_slot->name
                    + (cur_slot->is_auto ? " [AUTO]" : "")
                    + "  " + std::to_string(slot_pos + 1)
                    + "/" + std::to_string(static_cast<int>(li.size()));
                r2d.draw_text(wname, cx, cy, CW, CH, TW);
                cy += LS;

                if (cur_slot->is_melee) {
                    // Melee weapon: no ammo, show swing cooldown bar
                    r2d.draw_text("AMMO: ---- (melee)", cx, cy, CW, CH, TW);
                    cy += LS;
                    float lst = cw->last_shot_time;
                    float mcd = cw->fire_rate > 0.0f
                              ? 1.0f / cw->fire_rate : 1.0f;
                    r2d.draw_text("SWING CD:", cx, cy + 2, CW, CH - 4, TW);
                    draw_bar(cx + 120, cy, BARW, BH, mcd - std::min(lst, mcd), mcd, C_CD);
                    r2d.draw_text(lst > 0.005f ? fmt1(lst) + "s" : "Ready",
                                  cx + 124 + BARW, cy + 2, CW - 2, CH - 4, TW);

                } else if (cur_slot->reload_timer > 0.0f) {
                    // Weapon is reloading: show ammo state and reload progress bar
                    r2d.draw_text("AMMO: "
                        + std::to_string(cw->ammo_count)
                        + " / " + std::to_string(cw->max_ammo),
                        cx, cy, CW, CH, TW);
                    cy += LS;
                    r2d.draw_text("MAGS: " + std::to_string(cur_slot->ref->reserve_mags),
                        cx, cy, CW, CH, TW);
                    cy += LS;
                    r2d.draw_rect(cx - PAD, cy - 2, COL_W, CH + 6, C_RELOAD);
                    r2d.draw_text("RELOADING  " + fmt1(cur_slot->reload_timer) + "s",
                                  cx + 4, cy, CW, CH, TW);
                    draw_bar(cx + 230, cy + 2, 150, BH - 2,
                             cur_slot->reload_duration - cur_slot->reload_timer,
                             cur_slot->reload_duration, C_CD);

                } else {
                    // Normal state: ammo bar + reserve mags + cooldown bar
                    r2d.draw_text("AMMO:", cx, cy + 2, CW, CH - 4, TW);
                    draw_bar(cx + 65, cy, BARW, BH,
                             static_cast<float>(cw->ammo_count),
                             static_cast<float>(cw->max_ammo), C_AMMO);
                    r2d.draw_text(std::to_string(cw->ammo_count)
                                  + "/" + std::to_string(cw->max_ammo),
                                  cx + 69 + BARW, cy + 2, CW - 2, CH - 4, TW);
                    cy += LS;

                    r2d.draw_text("MAGS: " + std::to_string(cur_slot->ref->reserve_mags),
                                  cx, cy, CW, CH, TW);
                    cy += LS;

                    // Shot cooldown bar (semi-auto only; auto weapons fire as fast as allowed)
                    if (!cur_slot->is_auto) {
                        float lst = cw->last_shot_time;
                        float mcd = cw->fire_rate > 0.0f
                                  ? 1.0f / cw->fire_rate : 1.0f;
                        r2d.draw_text("SHOT CD:", cx, cy + 2, CW, CH - 4, TW);
                        draw_bar(cx + 100, cy, BARW, BH,
                                 mcd - std::min(lst, mcd), mcd, C_CD);
                        r2d.draw_text(lst > 0.005f ? fmt1(lst) + "s" : "Ready",
                                      cx + 104 + BARW, cy + 2, CW - 2, CH - 4, TW);
                    }
                }
            }

            // Controls hint at bottom of column
            int hint_y = HUD_Y + HUD_H - SCH - 8;
            r2d.draw_text("LMB:shoot  R:reload  1-7:switch",
                          cx, hint_y, SCW, SCH, TW);
        }

        // ── COL 3: Weapon loadout list ────────────────────────────────────
        {
            int cx = COL_W * 2 + PAD;
            int cy = HUD_Y + VPAD;

            r2d.draw_text("LOADOUT:", cx, cy, CW, CH, TW);
            cy += LS;

            int slot_n = 0;
            for (auto& s : pool) {
                if (!s.in_loadout) continue;
                bool active = (s.ref == cw);
                std::string slot_name = s.name;
                if (slot_name.size() > 8) slot_name = slot_name.substr(0, 8);

                std::string line = std::to_string(slot_n + 1) + ": ";
                line += active ? "[" + slot_name + "]" : " " + slot_name;
                r2d.draw_text(line, cx, cy, SCW, SCH, TW);
                cy += SLS;
                ++slot_n;
            }
            if (slot_n == 0)
                r2d.draw_text("(empty)", cx, cy, SCW, SCH, TW);

            // Controls hint at bottom of column
            int hint_y = HUD_Y + HUD_H - SCH - 8;
            r2d.draw_text("=:add weapon  -:remove current",
                          cx, hint_y, SCW, SCH, TW);
        }

        // ── Game Over overlay (centered above HUD) ────────────────────────
        if (game_over) {
            const int W = 520, H = 116;
            const int X = SW / 2 - W / 2, Y = SH / 2 - H - 80;
            r2d.draw_rect(X, Y, W, H, C_RED);
            r2d.draw_text("GAME  OVER", X + 24, Y + 10, 36, 60, 0u);
            r2d.draw_text("[V] Revive              [ESC] Exit",
                          X + 24, Y + 82, CW, CH, 0u);
            r2d.draw_text("[V] Revive              [ESC] Exit",
                          X + 25, Y + 82, CW, CH, 0u);
        }

        // ── Command input overlay (centered above HUD) ────────────────────
        if (mode != cmd_mode::none) {
            const char* prompt = mode == cmd_mode::damage ? "Damage amount"
                               : mode == cmd_mode::heal   ? "Heal amount"
                               :                            "Armor amount";
            const int W = 540, H = 86;
            const int X = SW / 2 - W / 2, Y = SH / 2 - H - 80;
            r2d.draw_rect(X, Y, W, H, C_OVERLAY);
            r2d.draw_rect(X, Y, W, 2, 0xFF5555AA);
            r2d.draw_text(std::string("> ") + prompt + ": " + num_buf + "_",
                          X + 14, Y + 14, CW, CH, TW);
            r2d.draw_text("[0-9] type   [Bsp] delete   [Enter] apply   [Esc] cancel",
                          X + 14, Y + 14 + LS, CW - 2, CH - 4, TW);
        }

        r_back->flush();
        r_back->wait_for_vsync();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}
