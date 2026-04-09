# Opisy Projektów grupy nr 3

Autorzy:

**Michał Sikora Nr.Albumu 292845**

**Filip Klich Nr.Albumu 293446**

Lider grupy - Michał Sikora


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
Gracz wciela się w przeniesionego do innego swiata mieszkanca Japonii. Jego celem jest pokonanie armii krola demonow atakujacego krolestwo Aliud Regnum Mundi. Zadaniem jest przetrwanie i eliminacja wszystkich demonow oraz ich krola. Gracz może poruszać się, strzelać, zmieniać broń i rzucać granaty.

### System potworów
Wszystkie potwory dziedziczą po wspólnej klasie abstrakcyjnej. Dzięki polimorfizmowi każdy typ zachowuje się inaczej, ale jest traktowany przez silnik gry w identyczny sposób.

#### Dostępne typy potworów
- **Podstawowy słaby** - najprostszy przeciwnik, mała ilość HP, atak w zwarciu
- **Slaby ranged** - tutorialowy przeciwnik strzelajacy
- **Assault shooter** - strzela seriami, średni zasięg
- **Sniper** - strzela rzadko, ale bardzo celnie i z dużej odległości
- **Trapper** - zastawia pulapki i utrudnia graczowi zycie
- **Duży gruby** - duża ilość HP, wolny, zadaje duże obrażenia w zwarciu
- **Mały szybki** - mały, bardzo szybki, bardzo mała ilość HP
- **All-rounder** - przeciwnik dobry zarowno w CQC, jak i w dystansie
- **Spawner** - spawnuje pomniejszych przeciwnikow zeby walczyli za niego
- **Magic** - utility ale dla przeciwników
- **Elite Tank** - bardzo trudny przeciwnik do zabicia
- **Elite Szybki** - bardzo szybki przeciwnik ktory szarzuje
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

# Opis Projektu 2

## Symulacja Prymitywnej Cywilizacji

**Nazwa: Swit Plemion**

## Opis założeń

Projekt ma na celu symulację życia pierwotnych społeczeństw, w świecie o ograniczonej przestrzeni i zasobach. Kluczowe elementy symulacji(interakcje, statystyki kolonii lub wędrownych, pola z jedzeniem lub zwierzyną) są zaprojektowane z elementami polimorfizmu, dziedziczenia, kompozycji tak aby kod był czytelny i łatwy do rozszerzenia.

- Symulacja ma być pisana obiektowo, wszystkie jednostki na mapie mają być swoją własną klasą
- Polimorfizm wykorzystany będzie do mechanik rozróżnienia między różnymi typami jednostek
- Symulacja aktualizuje się na bieżąco
- Symulacja wykorzystuje systemy Machine learningu do kontrolowania wysłanników kolonii

---

## Opis funkcjonalności

### Podstawa symulacji: Cykl Wypraw
Życie w symulacji nie toczy się w chaosie, lecz w uporządkowanym cyklu wypraw i powrotów:

- **Ekspedycje** - Jednostki opuszczają bezpieczną kolonię, by eksplorować nieznane tereny. To wtedy dochodzi do kluczowych interakcji: zbierania owoców, polowań na zwierzynę lub spotkań z wędrownymi ludźmi, których mogą zwerbować do swojej kolonii

- **Powrót z Łupem** - Zdobycie pożywienia lub zwerbowanie nowego członka nie oznacza natychmiastowego sukcesu. Jednostka musi bezpiecznie przebyć drogę powrotną do bazy swojej kolonii. Dopiero gdy dotrze do współrzędnych swojej kolonii, zasoby są dodawane do wspólnej puli, a populacja i siła kolonii oficjalnie wzrasta

- **System głodu Kolonii** - Każda kolonia będzie posiadała swój zakres głodu, na który składa się suma poziomu głodu każdego wysłannika, powodując, że wysłannicy muszą szukać jedzenia dla kolonii a nie samych wędrowców do zwerbowania, jeśli głód kolonii dobije zera kolonie spotka ostateczny los, kończący żywot kolonii. Sprowadzane jedzenie przez wysłanników wznosi ten poziom, i jeśli poziom głodu jest powyżej połowy to podniesie to również siłę kolonii

### Interakcje i Dyplomacja Pierwotna
Spotkania między ludźmi na szlaku są ważną częścią intrygi na planszy:

- **Werbunek i Eskorta** - Udana próba perswazji sprawia, że wędrowiec zaczyna podążać za naszym wysłannikiem z kolonii. Cel jest jeden: doprowadzić go do własnej bazy

- **Punkt Zapalny** - Jeśli werbunek się nie powiedzie, może dojść do walki między wędrowcem a wysłannikiem z kolonii, w której zginie jeden z nich. Jeśli wysłannik wyjdzie z niej zwycięsko, może kontynuować poszukiwanie jedzenia bądź innego wędrowca, jeśli przegra, wysłannik ginie, cofając trochę postęp swojej kolonii, wędrowiec zostaje na mapie, możliwy do zwerbowania oraz z doświadczenia walki, przyprowadzony z powrotem do kolonii ulepszony wędrowiec da więcej punktów rozwoju niż podstawowo, jeden wędrowiec będzie miał limit ulepszeń po wygranych walkach

- **System poziomu Kolonii** - Z czasem, wysłannicy zbierając jedzenie i wędrowców podnoszą poziom siły kolonii, przynosząc ich kolonię bliżej dominacji nad planszą. Z coraz to wyższym poziomem siły kolonii, wysłannicy danej kolonii będą dostawali dodatkowe zdolności które będą ułatwiały im ich dalsze eskapady poza bazę główną

### Zakończenie Symulacji
Zwycięstwo odnosi kolonia, która jako pierwsza podniesie swoją siłę kolonii do pewnego poziomu, albo jeśli wszystkie kolonie na mapie wyginą z wygłodzenia. To powoduje, że symulacja będzie nie tylko wyścigiem o jak największą siłę, ale również o to by nie pozwolić swojej kolonii dokonać żywota z głodu.

