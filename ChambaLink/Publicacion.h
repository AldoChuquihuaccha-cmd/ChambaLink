#pragma once
#include <string>
#include <functional>
#include "Contenido.h"
#include "Lista.h"

// Publicacion que un usuario comparte en el feed.
// Guarda los ids de quienes dieron me gusta, no solo cuantos, para que
// un mismo usuario no pueda darlo dos veces.
class Publicacion : public Contenido {
private:
    Lista<int> idsMeGusta;

public:
    Publicacion() : Contenido(0, 0, "", "") {}

    Publicacion(int pId, int pIdAutor, std::string pTexto, std::string pFecha)
        : Contenido(pId, pIdAutor, pTexto, pFecha) {
    }

    uint getMeGusta() const { return idsMeGusta.longitud(); }

    bool leGustaA(int idUsuario) const {
        return idsMeGusta.existe([idUsuario](const int& id) { return id == idUsuario; });
    }

    // Devuelve false si el usuario ya habia dado me gusta.
    bool darMeGusta(int idUsuario) {
        if (leGustaA(idUsuario)) return false;
        idsMeGusta.agregaFinal(idUsuario);
        return true;
    }

    // Recorre los ids de quienes dieron me gusta.
    void paraCadaMeGusta(std::function<void(const int&)> accion) const {
        idsMeGusta.paraCada(accion);
    }

    bool quitarMeGusta(int idUsuario) {
        return idsMeGusta.eliminaSi([idUsuario](const int& id) { return id == idUsuario; });
    }

    std::string tipo() const { return "Publicacion"; }

    std::string resumen() const {
        return Contenido::resumen() + " (" + std::to_string(getMeGusta()) + " me gusta)";
    }
};