#include <iostream>
#include <string>
#include <memory>
 
#include "engine/actor.h"
#include "combat/burning.h"
#include "combat/slowed.h"
#include "combat/charmed.h"
 
// Minimalny konkretny aktor do testów — implementuje wymaganą metodę update()
// i udostępnia gettery potrzebne do weryfikacji stanu.
class test_actor : public engine::actor {
public:
	test_actor(float hp, float shield, float speed)
		: actor(hp, shield, speed, engine::faction::enemy) {}
 
	void update(float dt) override {
		engine::actor::update(dt);
	}
 
	float hp()     const { return health.current_hp; }
	float shield() const { return health.armor; }
};
 
// Pomocnik — wypisuje RESULT i wartość z dokładnością do 2 miejsc po przecinku
static void result(float v) {
	// Zaokrąglamy do 2 miejsc aby uniknąć błędów float w porównaniach
	std::cout << "RESULT " << static_cast<int>(v * 100 + 0.5f) / 100.0f << std::endl;
}
static void result(const std::string& s) {
	std::cout << "RESULT " << s << std::endl;
}
 
int main() {
	std::cout << "READY" << std::endl;
	std::string cmd;
 
	while (std::cin >> cmd) {
 
		// =====================================================================
		// BURNING
		// =====================================================================
 
		// burning_tick — jeden tick (0.5s) zadaje intensity obrażeń bezpośrednio w HP
		if (cmd == "burning_tick") {
			test_actor a(100.0f, 0.0f, 1.0f);
			engine::combat::burning b(10.0f, 5); // duration=10s, intensity=5
			b.affect(a);
			result(a.hp()); // 100 - 5 = 95
 
		// burning_ignores_armor — ogień omija pancerz, trafia wprost w HP
		} else if (cmd == "burning_ignores_armor") {
			test_actor a(100.0f, 50.0f, 1.0f);
			engine::combat::burning b(10.0f, 20);
			b.affect(a);
			result(a.shield()); // armor bez zmian = 50
			result(a.hp());     // hp = 80
 
		// burning_expires — efekt wygasa po duration, update() zwraca true
		} else if (cmd == "burning_expires") {
			test_actor a(100.0f, 0.0f, 1.0f);
			auto b = std::make_unique<engine::combat::burning>(1.0f, 5);
			b->update(1.1f); // przekraczamy duration
			result(b->is_expired() ? "YES" : "NO");
 
		// burning_via_actor — efekt nałożony przez add_effect tickuje przez update()
		} else if (cmd == "burning_via_actor") {
			test_actor a(100.0f, 0.0f, 1.0f);
			a.add_effect(std::make_unique<engine::combat::burning>(10.0f, 3));
			// Symulujemy 0.5s — tick powinien nastąpić
			a.update(0.5f);
			result(a.hp()); // 100 - 3 = 97
 
		// burning_multi_tick — kilka ticków w czasie
		} else if (cmd == "burning_multi_tick") {
			test_actor a(100.0f, 0.0f, 1.0f);
			a.add_effect(std::make_unique<engine::combat::burning>(10.0f, 4));
			a.update(0.5f); // tick 1: hp = 96
			a.update(0.5f); // tick 2: hp = 92
			a.update(0.5f); // tick 3: hp = 88
			result(a.hp());
 
		// =====================================================================
		// SLOWED
		// =====================================================================
 
		// slowed_apply — on_apply redukuje prędkość o intensity%
		} else if (cmd == "slowed_apply") {
			test_actor a(100.0f, 0.0f, 100.0f);
			engine::combat::slowed s(5.0f, 50); // 50% spowolnienia
			s.on_apply(a);
			result(a.movement_speed); // 100 * 0.5 = 50
 
		// slowed_expire — on_expire przywraca prędkość
		} else if (cmd == "slowed_expire") {
			test_actor a(100.0f, 0.0f, 100.0f);
			engine::combat::slowed s(5.0f, 50);
			s.on_apply(a);
			s.on_expire(a);
			result(a.movement_speed); // wraca do 100
 
		// slowed_minimum — prędkość nie spada poniżej 10% bazy
		} else if (cmd == "slowed_minimum") {
			test_actor a(100.0f, 0.0f, 100.0f);
			engine::combat::slowed s(5.0f, 99); // 99% spowolnienia -> min 10%
			s.on_apply(a);
			result(a.movement_speed); // >= 10
 
		// slowed_minimum_restore — po wygaśnięciu wraca do oryginalnej wartości
		} else if (cmd == "slowed_minimum_restore") {
			test_actor a(100.0f, 0.0f, 100.0f);
			engine::combat::slowed s(5.0f, 99);
			s.on_apply(a);
			s.on_expire(a);
			result(a.movement_speed); // wraca do 100
 
		// slowed_via_actor — efekt nałożony przez add_effect, wygasa po czasie
		} else if (cmd == "slowed_via_actor") {
			test_actor a(100.0f, 0.0f, 100.0f);
			a.add_effect(std::make_unique<engine::combat::slowed>(1.0f, 50));
			// Po nałożeniu prędkość zredukowana
			result(a.movement_speed); // 50
			a.update(1.1f); // efekt wygasa
			result(a.movement_speed); // wraca do 100
 
		// =====================================================================
		// CHARMED
		// =====================================================================
 
		// charmed_apply — on_apply zmienia frakcję na player
		} else if (cmd == "charmed_apply") {
			test_actor a(100.0f, 0.0f, 1.0f); // faction::enemy
			engine::combat::charmed c(5.0f, 1);
			c.on_apply(a);
			result(a.team == engine::faction::player ? "player" : "enemy");
 
		// charmed_expire — on_expire przywraca frakcję enemy
		} else if (cmd == "charmed_expire") {
			test_actor a(100.0f, 0.0f, 1.0f);
			engine::combat::charmed c(5.0f, 1);
			c.on_apply(a);
			c.on_expire(a);
			result(a.team == engine::faction::enemy ? "enemy" : "player");
 
		// charmed_via_actor — efekt nałożony przez add_effect, wygasa po czasie
		} else if (cmd == "charmed_via_actor") {
			test_actor a(100.0f, 0.0f, 1.0f);
			a.add_effect(std::make_unique<engine::combat::charmed>(1.0f, 1));
			result(a.team == engine::faction::player ? "player" : "enemy"); // player
			a.update(1.1f); // efekt wygasa
			result(a.team == engine::faction::enemy ? "enemy" : "player");  // enemy
 
		// charmed_no_damage — charmed nie zadaje obrażeń
		} else if (cmd == "charmed_no_damage") {
			test_actor a(100.0f, 0.0f, 1.0f);
			engine::combat::charmed c(5.0f, 1);
			c.on_apply(a);
			result(a.hp()); // hp bez zmian = 100
 
		// =====================================================================
		// PARAMETRYCZNE — do testów losowych i edge cases
		// =====================================================================

		// burning_dmg <hp> <armor> <intensity>
		// Sprawdza że burning omija pancerz: armor bez zmian, hp spada o intensity
		} else if (cmd == "burning_dmg") {
			float hp, armor;
			int raw_intensity;
			std::cin >> hp >> armor >> raw_intensity;
			unsigned int intensity = static_cast<unsigned int>(std::max(0, raw_intensity));
			test_actor a(hp, armor, 1.0f);
			engine::combat::burning b(10.0f, intensity);
			b.affect(a);
			result(a.shield()); // armor — bez zmian
			result(a.hp());     // hp = max(0, hp - intensity)

		// burning_ticks <hp> <intensity> <n>
		// Sprawdza wynik n ticków (co 0.5s)
		} else if (cmd == "burning_ticks") {
			float hp;
			int raw_intensity, n;
			std::cin >> hp >> raw_intensity >> n;
			unsigned int intensity = static_cast<unsigned int>(std::max(0, raw_intensity));
			n = std::max(0, n);
			float dur = static_cast<float>(n) * 0.5f + 1.0f;
			test_actor a(hp, 0.0f, 1.0f);
			a.add_effect(std::make_unique<engine::combat::burning>(dur, intensity));
			for (int i = 0; i < n; ++i)
				a.update(0.5f);
			result(a.hp()); // hp = max(0, hp - intensity * n)

		// slowed_calc <speed> <intensity>
		// Sprawdza prędkość po on_apply
		} else if (cmd == "slowed_calc") {
			float speed;
			int raw_intensity;
			std::cin >> speed >> raw_intensity;
			unsigned int intensity = static_cast<unsigned int>(std::max(0, raw_intensity));
			test_actor a(100.0f, 0.0f, speed);
			engine::combat::slowed s(10.0f, intensity);
			s.on_apply(a);
			result(a.movement_speed); // speed * max(0.1, 1 - intensity/100)

		// slowed_restore <speed> <intensity>
		// Sprawdza że on_apply + on_expire przywraca oryginalną prędkość
		} else if (cmd == "slowed_restore") {
			float speed;
			int raw_intensity;
			std::cin >> speed >> raw_intensity;
			unsigned int intensity = static_cast<unsigned int>(std::max(0, raw_intensity));
			test_actor a(100.0f, 0.0f, speed);
			engine::combat::slowed s(10.0f, intensity);
			s.on_apply(a);
			s.on_expire(a);
			result(a.movement_speed); // wraca do oryginału

		// expire_check <duration> <dt>
		// Sprawdza is_expired() po jednym update(dt)
		} else if (cmd == "expire_check") {
			float duration, dt;
			std::cin >> duration >> dt;
			auto b = std::make_unique<engine::combat::burning>(duration, 1);
			b->update(dt);
			result(b->is_expired() ? "YES" : "NO");

		} else if (cmd == "exit") {
			break;
		} else {
			result("UNKNOWN");
		}
	}
	return 0;
}
