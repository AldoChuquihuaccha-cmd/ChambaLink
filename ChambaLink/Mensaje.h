#pragma once
#include <string>
#include "Contenido.h"

// Mensaje directo entre dos usuarios conectados.
// El autor heredado es el emisor.
class Mensaje : public Contenido {
private:
    int idReceptor;
    bool leido;

public:
    Mensaje() : Contenido(0, 0, "", "") {
        idReceptor = 0;
        leido = false;
    }

    Mensaje(int pId, int pIdEmisor, int pIdReceptor, std::string pTexto, std::string pFecha)
        : Contenido(pId, pIdEmisor, pTexto, pFecha) {
        idReceptor = pIdReceptor;
        leido = false;
    }

    int getIdEmisor() const { return getIdAutor(); }
    int getIdReceptor() const { return idReceptor; }

    bool fueLeido() const { return leido; }
    void marcarLeido() { leido = true; }

    bool perteneceA(int idUsuario) const {
        return getIdAutor() == idUsuario || idReceptor == idUsuario;
    }

    // Sirve para los dos sentidos de la conversacion.
    bool esConversacionEntre(int idA, int idB) const {
        return (getIdAutor() == idA && idReceptor == idB)
            || (getIdAutor() == idB && idReceptor == idA);
    }

    std::string tipo() const { return "Mensaje"; }

    std::string resumen() const {
        if (leido) return Contenido::resumen() + " (leido)";
        return Contenido::resumen() + " (no leido)";
    }
};
