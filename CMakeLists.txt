cmake_minimum_required(VERSION 3.10)
project(GestiuneSpital LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# --- Biblioteca cu logica de domeniu (partajata intre aplicatie si teste) ---
set(SURSE_DOMENIU
    src/Persoana.cpp
    src/Pacient.cpp
    src/Angajat.cpp
    src/Medic.cpp
    src/Asistent.cpp
    src/DataOra.cpp
    src/ServiciuMedical.cpp
    src/Programare.cpp
    src/Internare.cpp
    src/Factura.cpp
    src/FacturaFactory.cpp
    src/Logger.cpp
    src/Json.cpp
    src/Spital.cpp
)

add_library(spital_lib STATIC ${SURSE_DOMENIU})
target_include_directories(spital_lib PUBLIC src)

# --- Aplicatia principala ---
add_executable(spital src/main.cpp)
target_link_libraries(spital PRIVATE spital_lib)

# --- Teste unitare ---
enable_testing()
add_executable(test_spital tests/test_spital.cpp)
target_link_libraries(test_spital PRIVATE spital_lib)
add_test(NAME teste_unitare COMMAND test_spital)
