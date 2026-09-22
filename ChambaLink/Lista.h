#pragma once
#include "Nodo.h"
#include <functional>
#include <stdexcept>

// Lista simple enlazada generica.
//   ini apunta al primer nodo y fin al ultimo (nullptr si esta vacia).
//   lon guarda cuantos elementos hay.
// La lista no conoce el dominio: las condiciones de busqueda y los
// recorridos entran como funciones lambda.
//
// Para no copiar objetos grandes:
//   - los elementos entran por const T& (una sola copia, la del nodo),
//   - las consultas devuelven const T&,
//   - para modificar un elemento guardado se usa buscarPtr o modificarSi.
template <class T>
class Lista {
public:
    // Iterador para recorrer con for-each sin exponer los nodos.
    class Iterador {
    private:
        Nodo<T>* actual;
    public:
        Iterador(Nodo<T>* pNodo) { actual = pNodo; }
        T& operator*() const { return actual->ref_Elem(); }
        Iterador& operator++() { actual = actual->get_Sgte(); return *this; }
        bool operator!=(const Iterador& otro) const { return actual != otro.actual; }
    };

private:
    Nodo<T>* ini;
    Nodo<T>* fin;
    uint lon;

    // Devuelve el nodo de la posicion pos. Quien lo llama ya valido que pos < lon.
    Nodo<T>* nodoEn(uint pos) const {
        Nodo<T>* aux = ini;
        for (uint i = 0; i < pos; i++) aux = aux->get_Sgte();
        return aux;
    }

public:
    Lista() {
        ini = nullptr;
        fin = nullptr;
        lon = 0;
    }

    // Constructor copia: copia profunda, las dos listas no comparten nodos.
    Lista(const Lista<T>& otra) {
        ini = nullptr;
        fin = nullptr;
        lon = 0;
        for (Nodo<T>* aux = otra.ini; aux != nullptr; aux = aux->get_Sgte())
            agregaFinal(aux->get_Elem());
    }

    Lista<T>& operator=(const Lista<T>& otra) {
        if (this != &otra) {               // por si alguien escribe l = l;
            vaciar();
            for (Nodo<T>* aux = otra.ini; aux != nullptr; aux = aux->get_Sgte())
                agregaFinal(aux->get_Elem());
        }
        return *this;
    }

    ~Lista() { vaciar(); }

    uint longitud() const { return lon; }
    bool esVacia() const { return lon == 0; }

    // O(1)
    void agregaInicial(const T& elem) {
        Nodo<T>* nuevo = new Nodo<T>(elem);
        nuevo->set_Sgte(ini);
        ini = nuevo;
        if (fin == nullptr) fin = nuevo;   // la lista estaba vacia
        lon++;
    }

    // O(1) porque guardamos el puntero fin
    void agregaFinal(const T& elem) {
        Nodo<T>* nuevo = new Nodo<T>(elem);
        if (ini == nullptr) ini = nuevo;
        else fin->set_Sgte(nuevo);
        fin = nuevo;
        lon++;
    }

    // O(n). pos igual a lon significa insertar al final.
    void agregaPos(const T& elem, uint pos) {
        if (pos > lon) throw std::out_of_range("Lista::agregaPos - posicion invalida");
        if (pos == 0) { agregaInicial(elem); return; }
        if (pos == lon) { agregaFinal(elem); return; }

        Nodo<T>* anterior = nodoEn(pos - 1);
        Nodo<T>* nuevo = new Nodo<T>(elem);
        nuevo->set_Sgte(anterior->get_Sgte());
        anterior->set_Sgte(nuevo);
        lon++;
    }

    void modificarPos(const T& elem, uint pos) {
        if (pos >= lon) throw std::out_of_range("Lista::modificarPos - posicion invalida");
        nodoEn(pos)->set_Elem(elem);
    }

    // O(1)
    void eliminaInicial() {
        if (esVacia()) throw std::out_of_range("Lista::eliminaInicial - lista vacia");
        Nodo<T>* aux = ini;
        ini = ini->get_Sgte();
        if (ini == nullptr) fin = nullptr;
        delete aux;
        lon--;
    }

    // O(n): en una lista simple hay que llegar al penultimo nodo.
    void eliminaFinal() {
        if (esVacia()) throw std::out_of_range("Lista::eliminaFinal - lista vacia");
        if (lon == 1) { eliminaInicial(); return; }

        Nodo<T>* anterior = nodoEn(lon - 2);
        delete anterior->get_Sgte();
        anterior->set_Sgte(nullptr);
        fin = anterior;
        lon--;
    }

    void eliminaPos(uint pos) {
        if (pos >= lon) throw std::out_of_range("Lista::eliminaPos - posicion invalida");
        if (pos == 0) { eliminaInicial(); return; }
        if (pos == lon - 1) { eliminaFinal(); return; }

        Nodo<T>* anterior = nodoEn(pos - 1);
        Nodo<T>* objetivo = anterior->get_Sgte();
        anterior->set_Sgte(objetivo->get_Sgte());
        delete objetivo;
        lon--;
    }

    // Elimina el primer elemento que cumpla la condicion. O(n)
    bool eliminaSi(std::function<bool(const T&)> criterio) {
        int pos = indiceDe(criterio);
        if (pos < 0) return false;
        eliminaPos((uint)pos);
        return true;
    }

    void vaciar() {
        Nodo<T>* aux = ini;
        while (aux != nullptr) {
            Nodo<T>* siguiente = aux->get_Sgte();
            delete aux;
            aux = siguiente;
        }
        ini = nullptr;
        fin = nullptr;
        lon = 0;
    }

    const T& obtenerInicial() const {
        if (esVacia()) throw std::out_of_range("Lista::obtenerInicial - lista vacia");
        return ini->get_Elem();
    }

    const T& obtenerFinal() const {
        if (esVacia()) throw std::out_of_range("Lista::obtenerFinal - lista vacia");
        return fin->get_Elem();
    }

    const T& obtenerPos(uint pos) const {
        if (pos >= lon) throw std::out_of_range("Lista::obtenerPos - posicion invalida");
        return nodoEn(pos)->get_Elem();
    }

    int indiceDe(std::function<bool(const T&)> criterio) const {
        int i = 0;
        for (Nodo<T>* aux = ini; aux != nullptr; aux = aux->get_Sgte(), i++)
            if (criterio(aux->get_Elem())) return i;
        return -1;
    }

    bool existe(std::function<bool(const T&)> criterio) const {
        return indiceDe(criterio) >= 0;
    }

    // Recorre la lista sin copiar: la lambda recibe una referencia al dato. O(n)
    void paraCada(std::function<void(const T&)> accion) const {
        for (Nodo<T>* aux = ini; aux != nullptr; aux = aux->get_Sgte())
            accion(aux->get_Elem());
    }

    // Devuelve la direccion del elemento guardado, no una copia.
    // Si no lo encuentra devuelve nullptr. O(n)
    T* buscarPtr(std::function<bool(const T&)> criterio) {
        for (Nodo<T>* aux = ini; aux != nullptr; aux = aux->get_Sgte())
            if (criterio(aux->get_Elem())) return &aux->ref_Elem();
        return nullptr;
    }

    const T* buscarPtr(std::function<bool(const T&)> criterio) const {
        for (Nodo<T>* aux = ini; aux != nullptr; aux = aux->get_Sgte())
            if (criterio(aux->get_Elem())) return &aux->get_Elem();
        return nullptr;
    }

    // Aplica el cambio sobre el elemento real de la lista. O(n)
    bool modificarSi(std::function<bool(const T&)> criterio, std::function<void(T&)> cambio) {
        T* objetivo = buscarPtr(criterio);
        if (objetivo == nullptr) return false;
        cambio(*objetivo);
        return true;
    }

    Iterador begin() { return Iterador(ini); }
    Iterador end() { return Iterador(nullptr); }
};