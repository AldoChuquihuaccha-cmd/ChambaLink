#pragma once

typedef unsigned int uint;

// Nodo de una lista enlazada simple: guarda un dato y sabe quien va despues.
// Lo usan Lista, Pila y Cola.
template <class T>
class Nodo {
private:
    T elem;
    Nodo<T>* sgte;

public:
    Nodo(const T& pElem) {
        elem = pElem;
        sgte = nullptr;
    }

    // Devuelve una referencia constante para leer sin copiar el elemento.
    const T& get_Elem() const { return elem; }

    // Acceso modificable: lo usan el iterador y buscarPtr para cambiar
    // el dato que esta guardado en el nodo.
    T& ref_Elem() { return elem; }

    void set_Elem(const T& pElem) { elem = pElem; }

    Nodo<T>* get_Sgte() const { return sgte; }
    void set_Sgte(Nodo<T>* pSgte) { sgte = pSgte; }

    bool es_ultimo() const { return sgte == nullptr; }
};
