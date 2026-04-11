**Opis założeń projektu**
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

