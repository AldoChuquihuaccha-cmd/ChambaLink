#pragma once
#include <string>
#include "Contenido.h"

// Recomendacion que un usuario escribe sobre otro, como en LinkedIn.
// El autor heredado es quien recomienda y idReceptor es el recomendado.
// No confundir con la sugerencia de conexion (contactos en comun).
class Recomendacion : public Contenido {
private:
    int idReceptor;

public:
    Recomendacion() : Contenido(0, 0, "", "") {
        idReceptor = 0;
    }

    Recomendacion(int pId, int pIdEmisor, int pIdReceptor, std::string pTexto, std::string pFecha)
        : Contenido(pId, pIdEmisor, pTexto, pFecha) {
        idReceptor = pIdReceptor;
    }

    int getIdEmisor() const { return getIdAutor(); }
    int getIdReceptor() const { return idReceptor; }

    bool esPara(int pIdReceptor) const { return idReceptor == pIdReceptor; }

    std::string tipo() const { return "Recomendacion"; }

    std::string resumen() const {
        return Contenido::resumen() + " (para el usuario " + std::to_string(idReceptor) + ")";
    }
};
