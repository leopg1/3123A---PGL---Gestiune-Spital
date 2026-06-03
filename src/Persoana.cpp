#include "Persoana.h"
#include "Exceptii.h"

#include <cctype>
#include <utility>

Persoana::Persoana(std::string nume, std::string cnp)
    : nume_(std::move(nume)), cnp_(std::move(cnp)) {
    valideazaCnp(cnp_);
}

void Persoana::valideazaCnp(const std::string& cnp) {
    if (cnp.size() != 13) {
        throw DateInvalideException("CNP-ul trebuie sa aiba 13 cifre (are " +
                                    std::to_string(cnp.size()) + ")");
    }
    for (char c : cnp) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            throw DateInvalideException("CNP-ul contine caractere care nu sunt cifre");
        }
    }
}

std::string Persoana::descriere() const {
    return tip() + " " + nume_ + " (CNP: " + cnp_ + ")";
}
