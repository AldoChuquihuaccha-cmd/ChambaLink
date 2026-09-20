#pragma once
#include <string>

// Nivel de dominio que el usuario declara para una habilidad propia.
enum class NivelHabilidad { Basico, Intermedio, Avanzado, Experto };

class Habilidad {
private:
    std::string nombre;
    NivelHabilidad nivel;

public:
    Habilidad();
    Habilidad(std::string pNombre, NivelHabilidad pNivel);

    std::string getNombre() const;
    NivelHabilidad getNivel() const;
    void setNivel(NivelHabilidad pNivel);

    std::string nivelToString() const;

    // Comparador por nombre; se usa como predicado lambda al buscar
    // en Lista<Habilidad> (evita depender de operator== de fuera).
    bool mismoNombre(const Habilidad& otra) const;
};