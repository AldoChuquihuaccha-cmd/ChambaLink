#pragma once
#include "Nodo.h"
#include <functional>
#include <stdexcept>

typedef unsigned int uint;

// ============================================================
//  Lista<T>  -  Lista simple enlazada generica
//  Invariantes:
//    - ini apunta al primer nodo (nullptr si esta vacia)
//    - fin apunta al ultimo nodo (nullptr si esta vacia)
//    - lon siempre refleja la cantidad real de nodos
//  No sabe nada del dominio (Usuario, Contacto, etc.): toda
//  decision propia del dominio entra como lambda (Strategy).
// ============================================================
template <class T>
class Lista {
public:
    // Alias de tipos: hacen legible la inyeccion de lambdas.
    typedef std::function<bool(const T&)> Predicado;
    typedef std::function<int(const T&, const T&)> Comparador;
    typedef std::function<void(const T&)> Consumidor;

    // ---------- Iterador (patron Iterator) ----------
    // Permite recorrer la lista con for-each sin exponer los Nodo.
    class Iterador {
    private:
        Nodo<T>* actual;
    public:
        Iterador(Nodo<T>* pNodo) : actual(pNodo) {}
        T& operator*() { return actual->ref_Elem(); }
        Iterador& operator++() { actual = actual->get_Sgte(); return *this; }
        bool operator!=(const Iterador& otro) const { return actual != otro.actual; }
        bool operator==(const Iterador& otro) const { return actual == otro.actual; }
    };

private:
    Nodo<T>* ini;
    Nodo<T>* fin;
    uint lon;

    // Helper privado: devuelve el nodo en la posicion pos (0-based).
    // Precondicion: pos < lon. Se asume validado por quien lo llama.
    Nodo<T>* nodoEn(uint pos) const;

public:
    // ---------- Ciclo de vida (regla de tres) ----------
    Lista();
    Lista(const Lista<T>& otra);
    Lista<T>& operator=(const Lista<T>& otra);
    ~Lista();

    // ---------- Consulta ----------
    uint longitud() const;
    bool esVacia() const;

    // ---------- Insercion ----------
    void agregaInicial(T elem);
    void agregaFinal(T elem);
    void agregaPos(T elem, uint pos);

    // ---------- Modificacion ----------
    void modificarInicial(T elem);
    void modificarFinal(T elem);
    void modificarPos(T elem, uint pos);

    // ---------- Eliminacion ----------
    void eliminaInicial();
    void eliminaFinal();
    void eliminaPos(uint pos);
    bool eliminaSi(Predicado criterio);
    void vaciar();

    // ---------- Obtencion ----------
    T obtenerInicial() const;
    T obtenerFinal() const;
    T obtenerPos(uint pos) const;

    // ---------- Busqueda y recorrido (con lambdas) ----------
    bool buscar(Predicado criterio, T& resultado) const;
    bool existe(Predicado criterio) const;
    int indiceDe(Predicado criterio) const;
    void paraCada(Consumidor accion) const;

    // ---------- Soporte para for-each ----------
    Iterador begin() const { return Iterador(ini); }
    Iterador end() const { return Iterador(nullptr); }
};

// ============================================================
//  Implementacion
// ============================================================

template <class T>
Lista<T>::Lista() : ini(nullptr), fin(nullptr), lon(0) {}

// Constructor copia: copia profunda, nunca comparte nodos.
template <class T>
Lista<T>::Lista(const Lista<T>& otra) : ini(nullptr), fin(nullptr), lon(0) {
    for (Nodo<T>* aux = otra.ini; aux != nullptr; aux = aux->get_Sgte()) {
        agregaFinal(aux->get_Elem());
    }
}

template <class T>
Lista<T>& Lista<T>::operator=(const Lista<T>& otra) {
    if (this != &otra) {            // proteccion ante l = l;
        vaciar();
        for (Nodo<T>* aux = otra.ini; aux != nullptr; aux = aux->get_Sgte()) {
            agregaFinal(aux->get_Elem());
        }
    }
    return *this;
}

template <class T>
Lista<T>::~Lista() {
    vaciar();
}

template <class T>
void Lista<T>::vaciar() {
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

template <class T>
Nodo<T>* Lista<T>::nodoEn(uint pos) const {
    Nodo<T>* aux = ini;
    for (uint i = 0; i < pos; i++) {
        aux = aux->get_Sgte();
    }
    return aux;
}

template <class T>
uint Lista<T>::longitud() const {
    return lon;
}

template <class T>
bool Lista<T>::esVacia() const {
    return lon == 0;
}

// O(1)
template <class T>
void Lista<T>::agregaInicial(T elem) {
    Nodo<T>* nuevo = new Nodo<T>(elem);
    nuevo->set_Sgte(ini);
    ini = nuevo;
    if (fin == nullptr) fin = nuevo;   // la lista estaba vacia
    lon++;
}

// O(1) gracias al puntero fin
template <class T>
void Lista<T>::agregaFinal(T elem) {
    Nodo<T>* nuevo = new Nodo<T>(elem);
    if (ini == nullptr) {
        ini = nuevo;
    }
    else {
        fin->set_Sgte(nuevo);
    }
    fin = nuevo;
    lon++;
}

// O(n). pos == lon significa "insertar al final".
template <class T>
void Lista<T>::agregaPos(T elem, uint pos) {
    if (pos > lon) throw std::out_of_range("Lista::agregaPos - posicion invalida");
    if (pos == 0) { agregaInicial(elem); return; }
    if (pos == lon) { agregaFinal(elem); return; }

    Nodo<T>* anterior = nodoEn(pos - 1);
    Nodo<T>* nuevo = new Nodo<T>(elem);
    nuevo->set_Sgte(anterior->get_Sgte());
    anterior->set_Sgte(nuevo);
    lon++;
}

template <class T>
void Lista<T>::modificarInicial(T elem) {
    if (esVacia()) throw std::out_of_range("Lista::modificarInicial - lista vacia");
    ini->set_Elem(elem);
}

template <class T>
void Lista<T>::modificarFinal(T elem) {
    if (esVacia()) throw std::out_of_range("Lista::modificarFinal - lista vacia");
    fin->set_Elem(elem);
}

template <class T>
void Lista<T>::modificarPos(T elem, uint pos) {
    if (pos >= lon) throw std::out_of_range("Lista::modificarPos - posicion invalida");
    nodoEn(pos)->set_Elem(elem);
}

// O(1)
template <class T>
void Lista<T>::eliminaInicial() {
    if (esVacia()) throw std::out_of_range("Lista::eliminaInicial - lista vacia");
    Nodo<T>* aux = ini;
    ini = ini->get_Sgte();
    if (ini == nullptr) fin = nullptr;  // quedo vacia
    delete aux;
    lon--;
}

// O(n): en una lista SIMPLE hay que llegar al penultimo nodo.
template <class T>
void Lista<T>::eliminaFinal() {
    if (esVacia()) throw std::out_of_range("Lista::eliminaFinal - lista vacia");
    if (lon == 1) { eliminaInicial(); return; }

    Nodo<T>* anterior = nodoEn(lon - 2);
    delete anterior->get_Sgte();
    anterior->set_Sgte(nullptr);
    fin = anterior;
    lon--;
}

template <class T>
void Lista<T>::eliminaPos(uint pos) {
    if (pos >= lon) throw std::out_of_range("Lista::eliminaPos - posicion invalida");
    if (pos == 0) { eliminaInicial(); return; }
    if (pos == lon - 1) { eliminaFinal(); return; }

    Nodo<T>* anterior = nodoEn(pos - 1);
    Nodo<T>* objetivo = anterior->get_Sgte();
    anterior->set_Sgte(objetivo->get_Sgte());
    delete objetivo;
    lon--;
}

// Elimina el PRIMER elemento que cumpla el criterio.
// Reutiliza eliminaPos para no duplicar el manejo de punteros.
template <class T>
bool Lista<T>::eliminaSi(Predicado criterio) {
    int pos = indiceDe(criterio);
    if (pos < 0) return false;
    eliminaPos(static_cast<uint>(pos));
    return true;
}

template <class T>
T Lista<T>::obtenerInicial() const {
    if (esVacia()) throw std::out_of_range("Lista::obtenerInicial - lista vacia");
    return ini->get_Elem();
}

template <class T>
T Lista<T>::obtenerFinal() const {
    if (esVacia()) throw std::out_of_range("Lista::obtenerFinal - lista vacia");
    return fin->get_Elem();
}

template <class T>
T Lista<T>::obtenerPos(uint pos) const {
    if (pos >= lon) throw std::out_of_range("Lista::obtenerPos - posicion invalida");
    return nodoEn(pos)->get_Elem();
}

// Devuelve true y copia el hallazgo en 'resultado'.
// Evita el problema de "que retorno si no lo encuentro" con tipos objeto.
template <class T>
bool Lista<T>::buscar(Predicado criterio, T& resultado) const {
    for (Nodo<T>* aux = ini; aux != nullptr; aux = aux->get_Sgte()) {
        if (criterio(aux->get_Elem())) {
            resultado = aux->get_Elem();
            return true;
        }
    }
    return false;
}

template <class T>
bool Lista<T>::existe(Predicado criterio) const {
    return indiceDe(criterio) >= 0;
}

template <class T>
int Lista<T>::indiceDe(Predicado criterio) const {
    int i = 0;
    for (Nodo<T>* aux = ini; aux != nullptr; aux = aux->get_Sgte(), i++) {
        if (criterio(aux->get_Elem())) return i;
    }
    return -1;
}

template <class T>
void Lista<T>::paraCada(Consumidor accion) const {
    for (Nodo<T>* aux = ini; aux != nullptr; aux = aux->get_Sgte()) {
        accion(aux->get_Elem());
    }
}