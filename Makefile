CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Isrc

# Sursele cu logica de domeniu (toate, fara punctul de intrare main.cpp).
SRC = src/Persoana.cpp src/Pacient.cpp src/Angajat.cpp src/Medic.cpp \
      src/Asistent.cpp src/DataOra.cpp src/ServiciuMedical.cpp \
      src/Programare.cpp src/Internare.cpp src/Factura.cpp \
      src/FacturaFactory.cpp src/Logger.cpp src/Json.cpp src/Spital.cpp

APP = build/spital
TEST_APP = build/test_spital

.PHONY: all run test clean

all: $(APP)

# Aplicatia principala (meniu interactiv).
$(APP): $(SRC) src/main.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) src/main.cpp -o $(APP)

run: $(APP)
	./$(APP)

# Testele unitare.
test: $(TEST_APP)
	./$(TEST_APP)

$(TEST_APP): $(SRC) tests/test_spital.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) tests/test_spital.cpp -o $(TEST_APP)

clean:
	rm -rf build
