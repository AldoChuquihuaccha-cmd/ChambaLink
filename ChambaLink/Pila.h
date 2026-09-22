#pragma once
#include "Nodo.h"
#include <functional>
#include <stdexcept>

// Pila generica (LIFO): el ultimo en entrar es el primero en salir.
// Apilar, desapilar y consultar el tope son O(1).
// En ChambaLink guarda las acciones recientes del usuario para deshacerlas.
template <class T>
class Pila {
private:
    Nodo<T>* cima;
    uint lon;

    // Copia respetando el orden: se recorre desde la cima y se va enlazando
    // al final de la nueva cadena, para que la copia tenga el mismo tope.
    void copiarDe(const Pila<T>& otra) {
        Nodo<T>* ultimo = nullptr;
        for (Nodo<T>* aux = otra.cima; aux != nullptr; aux = aux->get_Sgte()) {
            Nodo<T>* nuevo = new Nodo<T>(aux->get_Elem());
            if (ultimo == nullptr) cima = nuevo;
            else ultimo->set_Sgte(nuevo);
            ultimo = nuevo;
        }
        lon = otra.lon;
    }

public:
    Pila() {
        cima = nullptr;
        lon = 0;
    }

    Pila(const Pila<T>& otra) {
        cima = nullptr;
        lon = 0;
        copiarDe(otra);
    }

    Pila<T>& operator=(const Pila<T>& otra) {
        if (this != &otra) {
            vaciar();
            copiarDe(otra);
        }
        return *this;
    }

    ~Pila() { vaciar(); }

    uint longitud() const { return lon; }
    bool esVacia() const { return lon == 0; }

    void apilar(const T& elem) {
        Nodo<T>* nuevo = new Nodo<T>(elem);
        nuevo->set_Sgte(cima);
        cima = nuevo;
        lon++;
    }

    // Retira el elemento de arriba y lo devuelve.
    T desapilar() {
        if (esVacia()) throw std::out_of_range("Pila::desapilar - pila vacia");
        Nodo<T>* aux = cima;
        T elem = aux->get_Elem();
        cima = cima->get_Sgte();
        delete aux;
        lon--;
        return elem;
    }

    // Consulta el elemento de arriba sin retirarlo.
    const T& tope() const {
        if (esVacia()) throw std::out_of_range("Pila::tope - pila vacia");
        return cima->get_Elem();
    }

    void vaciar() {
        while (cima != nullptr) {
            Nodo<T>* siguiente = cima->get_Sgte();
            delete cima;
            cima = siguiente;
        }
        lon = 0;
    }

    // Recorre del tope hacia el fondo, es decir del mas reciente al mas antiguo.
    void paraCada(std::function<void(const T&)> accion) const {
        for (Nodo<T>* aux = cima; aux != nullptr; aux = aux->get_Sgte())
            accion(aux->get_Elem());
    }
};
