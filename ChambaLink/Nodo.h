#pragma once

// ============================================================
//  Nodo<T>
//  Unidad minima de almacenamiento de una lista enlazada.
//  Responsabilidad unica (SRP): guardar un elemento y saber
//  quien va despues. No conoce la lista ni la recorre.
// ============================================================
template <class T>
class Nodo
{
private:
    T elem;          // dato almacenado
    Nodo<T>* sgte;   // puntero al siguiente nodo

public:
    // ---------- Constructores ----------
    Nodo() : elem(T()), sgte(nullptr) {}

    Nodo(T pElem) : elem(pElem), sgte(nullptr) {}

    // ---------- Setters / Getters ----------
    void set_Elem(const T& pElem) { elem = pElem; }

    T get_Elem() const { return elem; }

    // Acceso por referencia: lo usa el Iterador de la Lista
    // para poder modificar el elemento in situ.
    T& ref_Elem() { return elem; }

    void set_Sgte(Nodo<T>* pSgte) { sgte = pSgte; }

    Nodo<T>* get_Sgte() const { return sgte; }

    // ---------- Metodo de servicio ----------
    // Un nodo es el ultimo cuando no apunta a nadie.
    bool es_ultimo() const { return sgte == nullptr; }
};