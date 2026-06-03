#ifndef SPITAL_PERSOANA_H
#define SPITAL_PERSOANA_H

#include <string>

// ---------------------------------------------------------------------------
// Persoana: clasa de baza ABSTRACTA pentru oricine apare in sistem.
//
// Radacina ierarhiei de mostenire:
//      Persoana (abstracta)
//        |-- Pacient
//        |-- Angajat (abstracta)
//              |-- Medic
//              |-- Asistent
//
// Demonstreaza: incapsulare (campuri protected + getteri), metode virtuale
// pure (tip()) si o metoda virtuala cu implementare implicita (descriere()).
// ---------------------------------------------------------------------------

class Persoana {
protected:
    std::string nume_;
    std::string cnp_;   // 13 cifre, validat in constructor

public:
    Persoana(std::string nume, std::string cnp);
    virtual ~Persoana() = default; // destructor virtual: stergere polimorfica sigura

    const std::string& getNume() const { return nume_; }
    const std::string& getCnp() const  { return cnp_; }

    // Metoda virtuala PURA: fiecare tip de persoana se identifica diferit.
    virtual std::string tip() const = 0;

    // Metoda virtuala cu implementare implicita; poate fi suprascrisa.
    virtual std::string descriere() const;

protected:
    // Verifica formatul CNP-ului (13 cifre). Arunca DateInvalideException.
    static void valideazaCnp(const std::string& cnp);
};

#endif // SPITAL_PERSOANA_H
