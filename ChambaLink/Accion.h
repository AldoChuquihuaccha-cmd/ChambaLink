#pragma once
#include <string>
#include <functional>

// Una accion que el usuario realizo y que se puede deshacer.
// Guarda dos funciones lambda: una que hace el cambio y otra que lo revierte.
// La clase no sabe que cambio representa, solo sabe invocarlas, por eso se
// pueden apilar acciones de cualquier tipo en la misma pila.
//
// Importante para quien crea una Accion: las lambdas deben capturar ids y el
// puntero a la red, nunca una referencia a un Usuario. Si ese usuario se copia
// o se elimina, la referencia quedaria apuntando a memoria invalida; un id
// siempre se puede volver a buscar.
class Accion {
private:
    std::string descripcion;
    std::function<void()> hacer;
    std::function<void()> deshacer;
    bool ejecutada;

public:
    Accion() {
        descripcion = "";
        hacer = nullptr;
        deshacer = nullptr;
        ejecutada = false;
    }

    Accion(std::string pDescripcion, std::function<void()> pHacer, std::function<void()> pDeshacer) {
        descripcion = pDescripcion;
        hacer = pHacer;
        deshacer = pDeshacer;
        ejecutada = false;
    }

    // No ejecuta dos veces la misma accion.
    void ejecutar() {
        if (ejecutada) return;
        if (hacer) hacer();
        ejecutada = true;
    }

    // Solo deshace lo que realmente se ejecuto.
    void deshacerAccion() {
        if (!ejecutada) return;
        if (deshacer) deshacer();
        ejecutada = false;
    }

    std::string getDescripcion() const { return descripcion; }
    bool fueEjecutada() const { return ejecutada; }
};
