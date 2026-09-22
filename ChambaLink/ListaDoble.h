#pragma once
#include "NodoDoble.h"
#include <functional>
#include <stdexcept>

// Lista doblemente enlazada generica.
// Cada nodo conoce al anterior y al siguiente, asi que se puede recorrer
// en los dos sentidos y eliminar el ultimo en O(1).
// En ChambaLink guarda el historial laboral del usuario.
template <class T>
class ListaDoble {
public:
    // Iterador bidireccional: permite recorrer la lista sin conocer los nodos.
    //   for (ListaDoble<int>::Iterador it = lista.primero(); it.valido(); ++it)
    //   for (ListaDoble<int>::Iterador it = lista.ultimo(); it.valido(); --it)
    class Iterador {
    private:
        NodoDoble<T>* actual;

    public:
        Iterador(NodoDoble<T>* pNodo) { actual = pNodo; }

        // Devuelve el elemento del nodo actual.
        T& operator*() const { return actual->ref_Elem(); }

        // Avanza hacia el final de la lista.
        Iterador& operator++() {
            actual = actual->get_Sgte();
            return *this;
        }

        // Retrocede hacia el inicio de la lista.
        Iterador& operator--() {
            actual = actual->get_Ant();
            return *this;
        }

        // Indica si el iterador todavia apunta a un nodo.
        bool valido() const { return actual != nullptr; }

        bool operator!=(const Iterador& otro) const { return actual != otro.actual; }
        bool operator==(const Iterador& otro) const { return actual == otro.actual; }
    };

private:
    NodoDoble<T>* ini;
    NodoDoble<T>* fin;
    uint lon;

public:
    ListaDoble() {
        ini = nullptr;
        fin = nullptr;
        lon = 0;
    }

    ListaDoble(const ListaDoble<T>& otra) {
        ini = nullptr;
        fin = nullptr;
        lon = 0;
        for (NodoDoble<T>* aux = otra.ini; aux != nullptr; aux = aux->get_Sgte())
            agregaFinal(aux->get_Elem());
    }

    ListaDoble<T>& operator=(const ListaDoble<T>& otra) {
        if (this != &otra) {
            vaciar();
            for (NodoDoble<T>* aux = otra.ini; aux != nullptr; aux = aux->get_Sgte())
                agregaFinal(aux->get_Elem());
        }
        return *this;
    }

    ~ListaDoble() { vaciar(); }

    uint longitud() const { return lon; }
    bool esVacia() const { return lon == 0; }

    // O(1)
    void agregaInicial(const T& elem) {
        NodoDoble<T>* nuevo = new NodoDoble<T>(elem);
        nuevo->set_Sgte(ini);
        if (ini != nullptr) ini->set_Ant(nuevo);
        else fin = nuevo;                  // estaba vacia
        ini = nuevo;
        lon++;
    }

    // O(1)
    void agregaFinal(const T& elem) {
        NodoDoble<T>* nuevo = new NodoDoble<T>(elem);
        nuevo->set_Ant(fin);
        if (fin != nullptr) fin->set_Sgte(nuevo);
        else ini = nuevo;                  // estaba vacia
        fin = nuevo;
        lon++;
    }

    // O(1)
    void eliminaInicial() {
        if (esVacia()) throw std::out_of_range("ListaDoble::eliminaInicial - lista vacia");
        NodoDoble<T>* aux = ini;
        ini = ini->get_Sgte();
        if (ini != nullptr) ini->set_Ant(nullptr);
        else fin = nullptr;
        delete aux;
        lon--;
    }

    // O(1), a diferencia de la lista simple: el ultimo nodo conoce a su anterior.
    void eliminaFinal() {
        if (esVacia()) throw std::out_of_range("ListaDoble::eliminaFinal - lista vacia");
        NodoDoble<T>* aux = fin;
        fin = fin->get_Ant();
        if (fin != nullptr) fin->set_Sgte(nullptr);
        else ini = nullptr;
        delete aux;
        lon--;
    }

    void vaciar() {
        while (ini != nullptr) {
            NodoDoble<T>* siguiente = ini->get_Sgte();
            delete ini;
            ini = siguiente;
        }
        fin = nullptr;
        lon = 0;
    }

    const T& obtenerInicial() const {
        if (esVacia()) throw std::out_of_range("ListaDoble::obtenerInicial - lista vacia");
        return ini->get_Elem();
    }

    const T& obtenerFinal() const {
        if (esVacia()) throw std::out_of_range("ListaDoble::obtenerFinal - lista vacia");
        return fin->get_Elem();
    }

    bool existe(std::function<bool(const T&)> criterio) const {
        for (NodoDoble<T>* aux = ini; aux != nullptr; aux = aux->get_Sgte())
            if (criterio(aux->get_Elem())) return true;
        return false;
    }

    // Recorre del primero al ultimo.
    void paraCada(std::function<void(const T&)> accion) const {
        for (NodoDoble<T>* aux = ini; aux != nullptr; aux = aux->get_Sgte())
            accion(aux->get_Elem());
    }

    // Recorre del ultimo al primero usando el enlace hacia atras.
    void paraCadaInverso(std::function<void(const T&)> accion) const {
        for (NodoDoble<T>* aux = fin; aux != nullptr; aux = aux->get_Ant())
            accion(aux->get_Elem());
    }

    T* buscarPtr(std::function<bool(const T&)> criterio) {
        for (NodoDoble<T>* aux = ini; aux != nullptr; aux = aux->get_Sgte())
            if (criterio(aux->get_Elem())) return &aux->ref_Elem();
        return nullptr;
    }

    // Iteradores: primero() para recorrer hacia adelante y ultimo() hacia atras.
    Iterador primero() const { return Iterador(ini); }
    Iterador ultimo() const { return Iterador(fin); }

    // Soporte para for-each (del primero al ultimo).
    Iterador begin() const { return Iterador(ini); }
    Iterador end() const { return Iterador(nullptr); }
};