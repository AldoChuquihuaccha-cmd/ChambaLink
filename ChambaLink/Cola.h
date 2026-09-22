#pragma once
#include "Nodo.h"
#include <functional>
#include <stdexcept>

// Cola generica (FIFO): el primero en entrar es el primero en salir.
// Encolar y desencolar son O(1) porque guardamos los dos extremos.
// En ChambaLink la usan las solicitudes de conexion, las notificaciones
// y las postulaciones que esperan revision en cada vacante.
template <class T>
class Cola {
private:
    Nodo<T>* primero;
    Nodo<T>* ultimo;
    uint lon;

public:
    Cola() {
        primero = nullptr;
        ultimo = nullptr;
        lon = 0;
    }

    Cola(const Cola<T>& otra) {
        primero = nullptr;
        ultimo = nullptr;
        lon = 0;
        for (Nodo<T>* aux = otra.primero; aux != nullptr; aux = aux->get_Sgte())
            encolar(aux->get_Elem());
    }

    Cola<T>& operator=(const Cola<T>& otra) {
        if (this != &otra) {
            vaciar();
            for (Nodo<T>* aux = otra.primero; aux != nullptr; aux = aux->get_Sgte())
                encolar(aux->get_Elem());
        }
        return *this;
    }

    ~Cola() { vaciar(); }

    uint longitud() const { return lon; }
    bool esVacia() const { return lon == 0; }

    // Entra por el final.
    void encolar(const T& elem) {
        Nodo<T>* nuevo = new Nodo<T>(elem);
        if (ultimo == nullptr) primero = nuevo;
        else ultimo->set_Sgte(nuevo);
        ultimo = nuevo;
        lon++;
    }

    // Sale por el frente.
    T desencolar() {
        if (esVacia()) throw std::out_of_range("Cola::desencolar - cola vacia");
        Nodo<T>* aux = primero;
        T elem = aux->get_Elem();
        primero = primero->get_Sgte();
        if (primero == nullptr) ultimo = nullptr;
        delete aux;
        lon--;
        return elem;
    }

    // Consulta el primero de la fila sin retirarlo.
    const T& frente() const {
        if (esVacia()) throw std::out_of_range("Cola::frente - cola vacia");
        return primero->get_Elem();
    }

    void vaciar() {
        while (primero != nullptr) {
            Nodo<T>* siguiente = primero->get_Sgte();
            delete primero;
            primero = siguiente;
        }
        ultimo = nullptr;
        lon = 0;
    }

    bool existe(std::function<bool(const T&)> criterio) const {
        for (Nodo<T>* aux = primero; aux != nullptr; aux = aux->get_Sgte())
            if (criterio(aux->get_Elem())) return true;
        return false;
    }

    // Recorre en orden de llegada.
    void paraCada(std::function<void(const T&)> accion) const {
        for (Nodo<T>* aux = primero; aux != nullptr; aux = aux->get_Sgte())
            accion(aux->get_Elem());
    }
};
