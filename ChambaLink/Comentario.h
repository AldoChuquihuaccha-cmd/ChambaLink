#pragma once
#include <string>
#include "Contenido.h"

// Comentario que un usuario deja en una publicacion.
// idPadre permite responder a otro comentario y formar hilos:
//   idPadre igual a 0 -> comentario directo a la publicacion
//   idPadre distinto de 0 -> respuesta al comentario con ese id
class Comentario : public Contenido {
private:
    int idPublicacion;
    int idPadre;

public:
    Comentario() : Contenido(0, 0, "", "") {
        idPublicacion = 0;
        idPadre = 0;
    }

    Comentario(int pId, int pIdAutor, int pIdPublicacion, std::string pTexto,
        std::string pFecha, int pIdPadre = 0)
        : Contenido(pId, pIdAutor, pTexto, pFecha) {
        idPublicacion = pIdPublicacion;
        idPadre = pIdPadre;
    }

    int getIdPublicacion() const { return idPublicacion; }
    int getIdPadre() const { return idPadre; }

    bool esDePublicacion(int pIdPublicacion) const { return idPublicacion == pIdPublicacion; }
    bool esRespuesta() const { return idPadre != 0; }
    bool esRespuestaA(int pIdComentario) const { return idPadre == pIdComentario; }

    std::string tipo() const {
        if (esRespuesta()) return "Respuesta";
        return "Comentario";
    }

    std::string resumen() const {
        return Contenido::resumen() + " (en publicacion " + std::to_string(idPublicacion) + ")";
    }
};