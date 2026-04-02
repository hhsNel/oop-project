**Opis założeń projektu**
# Opis Projektu 1

## Objectively Worse Doom  

**Klon klasycznej gry Doom**  

## Opis załozeń
Projekt ma na celu stworzenie w pełni funkcjonalnej, minimalistycznej gry typu retro FPS. Wszystkie kluczowe elementy mechaniki gry (potwory, bronie, pociski) są projektowane z wykorzystaniem polimorfizmu, dziedziczenia i kompozycji, tak aby kod był czytelny i rozszerzalny.

- Gra jest w pełni obiektowa - każdy byt w świecie gry (potwór, broń, pocisk, gracz) jest instancją odpowiedniej klasy lub interfejsu.
- Polimorfizm jest używany jako główny mechanizm rozróżniania zachowania (różne typy potworów i pocisków).
- Gra działa w czasie rzeczywistym (pętla gry z update’ami i renderowaniem).
- Celem gry jest zabicie bossa.
- Prosta grafika imitujaca 3d, podobna do Castle Wolfenstein 3d, ale napisana od zera

---

## Opis funkcjonalny

### Ogólna koncepcja rozgrywki
Gracz wciela się w **INSERT LORE HERE**. Jego celem jest **INSERT LORE HERE**. Zadaniem jest przetrwanie i eliminacja wszystkich **INSERT LORE HERE**. Gracz może poruszać się, strzelać, zmieniać broń i rzucać granaty.

### System potworów
Wszystkie potwory dziedziczą po wspólnej klasie abstrakcyjnej. Dzięki polimorfizmowi każdy typ zachowuje się inaczej, ale jest traktowany przez silnik gry w identyczny sposób.

#### Dostępne typy potworów
- **Podstawowy słaby** - najprostszy przeciwnik, mała ilość HP, atak w zwarciu
- **Assault shooter** - strzela seriami, średni zasięg
- **Sniper** - strzela rzadko, ale bardzo celnie i z dużej odległości
- **Duży gruby** - duża ilość HP, wolny, zadaje duże obrażenia w zwarciu
- **Mały szybki** - mały, bardzo szybki, bardzo mała ilość HP
- **Magic** - utility ale dla przeciwników
- **Boss**

### System broni
Gracz może nosić wiele broni, zaimplementowanych polimorfizmem.

#### Dostępne bronie
- **M1911** - pistolet podstawowy, szybki słaby ogień pojedyńczy
- **Pistolet maszynowy** - szybkostrzelny, niskie obrażenia od trafienia
- **Karabin** - zrównoważona broń średniego dystansu
- **Sniper rifle** - bardzo wysokie obrażenia, wolne przeładowanie
- **Plasma gun** - strzela dużymi, powolnymi pociskami energetycznymi
- **Shotgun** - strzela wiązką śrutu (kilka pocisków naraz)
- **Katana** - broń biała, nie zużywa amunicji
- **Granaty** - rzucane, eksplodują po krótkim czasie

### System pocisków
Wszystkie pociski dziedziczą po klasie abstrakcyjnej `Projectile`

#### Typy pocisków
- **Pocisk zwyczajny** - klasyczny raycast
- **Pocisk zapalający** - zadaje obrażenia rozłożone w czasie
- **Pocisk spowalniający** - nakłada efekt spowolnienia na cel
- **Pocisk specjalny** - oczarowuje przeciwnika by stanął po jego stronie i zaczął atakować innych przeciwników
- **Plasma** - nie raycast,duże obrażenia, pocisk porusza się wolno
- **Slug** - wiele małych pocisków jednocześnie, nie raycast
- **Granat** - nie raycast, obrażenia obszarowe

### 5. Dodatkowe mechaniki
- System zdrowia i pancerza
- System amunicji
- Proste pickupy
- Status effecty

