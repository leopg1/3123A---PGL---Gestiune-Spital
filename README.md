# Aplicatie de Gestiune a unui Spital

> Proiect de final de semestru la disciplina **Programare Orientata pe Obiecte (POO)** — C++17.
> Tema 3123A: gestioneaza pacienti, medici, programari, internari si facturi.

Aplicatia este o consola interactiva care modeleaza activitatea unui spital:
inregistrarea pacientilor si a personalului, planificarea programarilor (cu
detectarea conflictelor de orar), internarea pacientilor, emiterea facturilor
si persistenta datelor in format JSON.

> 📄 **Documentatie completa (stil lucrare):** [docs/Documentatie_Proiect_Spital.pdf](docs/Documentatie_Proiect_Spital.pdf)
> — 19 pagini cu diagrame UML, explicatii ale conceptelor POO, concluzii si bibliografie.

---

## Cuprins

1. [Concepte POO folosite](#concepte-poo-folosite)
2. [Structura proiectului](#structura-proiectului)
3. [Arhitectura claselor](#arhitectura-claselor)
4. [Compilare si rulare](#compilare-si-rulare)
5. [Utilizare (meniu)](#utilizare-meniu)
6. [Testare](#testare)
7. [Persistenta JSON](#persistenta-json)
8. [Cerinte acoperite](#cerinte-acoperite)
9. [Posibile imbunatatiri](#posibile-imbunatatiri)

---

## Concepte POO folosite

| Concept | Unde se regaseste |
|---|---|
| **Incapsulare** | Toate clasele au campuri `private`/`protected` + getteri; validare in constructori (`Persoana::valideazaCnp`, `DataOra::valideaza`). |
| **Mostenire** | `Persoana -> Pacient`, `Persoana -> Angajat`, iar `Angajat -> Medic`, `Angajat -> Asistent` (cerinta obligatorie). |
| **Polimorfism** | `ServiciuMedical::calculeazaCost()` suprascris de `Consultatie`, `Analiza`, `Interventie`; `Angajat::calculeazaSalariu()` pentru `Medic`/`Asistent`. |
| **Clase abstracte** | `Persoana`, `Angajat`, `ServiciuMedical` (au metode virtuale pure). |
| **Excep=tii personalizate** | Ierarhia din `Exceptii.h`: `SpitalException` -> `DateInvalideException`, `ProgramareInvalidaException`, `EntitateInexistentaException`, `PersistentaException`. |
| **STL** | `std::vector`, `std::shared_ptr`, `std::string`, algoritmi (`std::copy_if`), `std::dynamic_pointer_cast`. |
| **Sablon Factory** | `FacturaFactory` creeaza facturi din programari, internari sau liste de servicii. |
| **Singleton** | `Logger` — un singur obiect de logare in toata aplicatia. |
| **Logging** | `Logger` inregistreaza operatiunile critice (internare, emitere factura). |
| **Persistenta** | Modul `json` scris de la zero (parser + serializer) folosit de `Spital::salveaza`/`incarca`. |

---

## Structura proiectului

```
Proiect POO/
├── src/                      # codul sursa
│   ├── Persoana.{h,cpp}      # clasa de baza abstracta (radacina ierarhiei)
│   ├── Pacient.{h,cpp}       # pacientul
│   ├── Angajat.{h,cpp}       # clasa de baza abstracta pentru personal
│   ├── Medic.{h,cpp}         # Angajat -> Medic
│   ├── Asistent.{h,cpp}      # Angajat -> Asistent
│   ├── ServiciuMedical.{h,cpp} # abstract + Consultatie/Analiza/Interventie
│   ├── Programare.{h,cpp}    # leaga pacient + medic + data + serviciu
│   ├── Internare.{h,cpp}     # spitalizarea unui pacient
│   ├── Factura.{h,cpp}       # documentul de plata (insumare polimorfica)
│   ├── FacturaFactory.{h,cpp}# sablonul Factory
│   ├── DataOra.{h,cpp}       # data/ora cu validare si comparatii
│   ├── Exceptii.h            # ierarhia de exceptii personalizate
│   ├── Logger.{h,cpp}        # logare (Singleton)
│   ├── Json.{h,cpp}          # modul JSON propriu (parser + serializer)
│   ├── Spital.{h,cpp}        # managerul/fatada aplicatiei
│   └── main.cpp              # meniu interactiv + date demonstrative
├── tests/
│   └── test_spital.cpp       # teste unitare (fara biblioteci externe)
├── docs/
│   ├── Documentatie_Proiect_Spital.pdf  # documentatie completa stil lucrare
│   ├── Documentatie_Proiect_Spital.html # sursa documentatiei PDF
│   ├── documentatie.md       # descrierea claselor + concepte POO
│   └── uml.md                # diagrame UML (clase + relatii)
├── data/
│   └── spital.json           # exemplu de date pentru persistenta
├── Makefile                  # build cu g++ (Linux / WSL / MinGW)
├── CMakeLists.txt            # build alternativ cu CMake
├── .gitignore
└── README.md
```

---

## Arhitectura claselor

```
              Persoana (abstracta)
              /                  \
         Pacient              Angajat (abstracta)
                              /            \
                          Medic          Asistent

        ServiciuMedical (abstracta)
        /         |          \
 Consultatie   Analiza    Interventie
```

- **Programare** = Pacient + Medic + DataOra + ServiciuMedical
- **Internare** = Pacient + salon + DataOra + cost
- **Factura** = Pacient + lista de `ServiciuMedical` (+ eventual cost internare)
- **Spital** detine toate entitatile si orchestreaza operatiunile.

Detalii complete in [docs/documentatie.md](docs/documentatie.md) si diagrame in
[docs/uml.md](docs/uml.md).

---

## Compilare si rulare

Proiectul este standard C++17 si nu foloseste biblioteci externe.

### Varianta 1 — Makefile (Linux / WSL / MinGW + make)

```bash
make            # compileaza aplicatia in build/spital
make run        # compileaza si ruleaza aplicatia
make test       # compileaza si ruleaza testele unitare
make clean      # sterge directorul build/
```

### Varianta 2 — CMake (portabil, inclusiv Visual Studio)

```bash
cmake -S . -B build
cmake --build build
./build/spital            # aplicatia
ctest --test-dir build    # testele (sau ./build/test_spital)
```

### Varianta 3 — compilare directa cu g++ (Windows / PowerShell)

```powershell
g++ -std=c++17 -Wall -Wextra -Isrc src/*.cpp -o build/spital.exe
g++ -std=c++17 -Wall -Wextra -Isrc (Get-ChildItem src/*.cpp -Exclude main.cpp) tests/test_spital.cpp -o build/test_spital.exe
./build/spital.exe
./build/test_spital.exe
```

> Pe Windows, `main.cpp` si `tests/test_spital.cpp` contin fiecare cate o
> functie `main`, deci se compileaza in executabile separate (ca mai sus).

---

## Utilizare (meniu)

La pornire, aplicatia incarca date demonstrative (pacienti, medici, asistenti,
programari). Meniul ofera:

```
   1. Afiseaza pacienti
   2. Afiseaza medici si asistenti (cu salarii)
   3. Afiseaza programari
   4. Adauga pacient
   5. Adauga programare
   6. Interneaza pacient        (operatiune critica)
   7. Emite factura             (operatiune critica)
   8. Filtreaza pacienti dupa diagnostic
   9. Salveaza datele in JSON
  10. Incarca datele din JSON
  11. Afiseaza raportul JSON complet
   0. Iesire
```

Operatiunile critice (internare, emitere factura) sunt afisate in consola si
scrise in fisierul de log `spital.log`.

---

## Testare

Testele unitare (`tests/test_spital.cpp`) acopera:

- validarea si comparatiile pentru `DataOra` (luni/zile invalide, ani bisecti);
- **calculul polimorfic al costurilor** serviciilor medicale;
- **logica programarilor** si detectarea conflictelor de orar;
- **calculul facturii** (servicii + internare + TVA);
- sablonul `FacturaFactory`;
- filtrarea pacientilor dupa diagnostic;
- validarea CNP (exceptii);
- modulul `json` (parsare, serializare, round-trip);
- persistenta entitatilor (`toJson`/`fromJson`).

Rulare:

```bash
make test     # sau ./build/test_spital
```

Programul intoarce codul de iesire `0` daca toate verificarile trec si `1` in
caz contrar (potrivit pentru integrare continua).

---

## Persistenta JSON

Starea spitalului poate fi salvata si reincarcata in format JSON folosind un
modul scris integral in cadrul proiectului (`src/Json.{h,cpp}`), fara biblioteci
externe. Vezi exemplul din [data/spital.json](data/spital.json).

---

## Cerinte acoperite

**Obligatorii:**

- [x] Clase: `Pacient`, `Medic`, `Programare`, `Factura`
- [x] Mostenire: `Angajat -> Medic`, `Angajat -> Asistent`
- [x] Polimorfism: calculul costului serviciilor medicale
- [x] Exceptii personalizate (ex: programare invalida)
- [x] Logging pentru operatiuni critice (internare, emitere factura)
- [x] Teste unitare pentru logica programarilor si calcul factura
- [x] Format cod curat + Git

**Facultative (bonus):**

- [x] Persistenta JSON
- [x] Sablonul Factory pentru crearea facturilor
- [x] Filtrarea pacientilor dupa diagnostic

---

## Posibile imbunatatiri

- Persistenta completa (inclusiv programari si internari, cu refacerea
  legaturilor dintre entitati prin id-uri).
- Interfata grafica sau API REST peste logica de domeniu existenta.
- Rapoarte statistice (grad de ocupare, venituri pe sectie).
- Inlocuirea modulului JSON propriu cu o biblioteca matura (ex: nlohmann/json).

---

*Autor: student ICE USV — proiect POO, grupa 3123A.*
