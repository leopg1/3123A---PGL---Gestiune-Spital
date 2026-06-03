#ifndef SPITAL_DATAORA_H
#define SPITAL_DATAORA_H

#include <string>

// ---------------------------------------------------------------------------
// DataOra: reprezinta un moment calendaristic (data + ora) cu validare.
//
// Constructorul valideaza componentele (luna 1..12, zi corecta in functie de
// luna si anii bisecti, ora 0..23, minut 0..59) si arunca DateInvalideException
// pentru valori imposibile. Ofera operatori de comparatie pentru detectarea
// conflictelor de programari si conversie din/in string "AAAA-LL-ZZ HH:MM".
// ---------------------------------------------------------------------------

class DataOra {
public:
    DataOra(); // implicit: 2025-01-01 00:00
    DataOra(int an, int luna, int zi, int ora = 0, int minut = 0);

    int getAn() const    { return an_; }
    int getLuna() const  { return luna_; }
    int getZi() const    { return zi_; }
    int getOra() const   { return ora_; }
    int getMinut() const { return minut_; }

    // Numarul total de minute de la o origine fixa, util pentru durate si sortari.
    long long caMinute() const;

    bool operator==(const DataOra& alt) const;
    bool operator!=(const DataOra& alt) const { return !(*this == alt); }
    bool operator<(const DataOra& alt) const;
    bool operator<=(const DataOra& alt) const { return *this < alt || *this == alt; }
    bool operator>(const DataOra& alt) const  { return alt < *this; }

    std::string toString() const; // "AAAA-LL-ZZ HH:MM"

    // Construieste o DataOra dintr-un string "AAAA-LL-ZZ HH:MM".
    // Arunca DateInvalideException daca formatul este gresit.
    static DataOra dinString(const std::string& text);

    // Numarul de zile dintr-o luna data (tine cont de anii bisecti).
    static int zileInLuna(int an, int luna);
    static bool esteBisect(int an);

private:
    int an_, luna_, zi_, ora_, minut_;
    void valideaza() const;
};

#endif // SPITAL_DATAORA_H
