#pragma once

typedef unsigned int uint;

// Nodo de la lista doblemente enlazada: conoce al anterior y al siguiente,
// asi se puede recorrer en los dos sentidos.
template <class T>
class NodoDoble {
private:
    T elem;
    NodoDoble<T>* ant;
    NodoDoble<T>* sgte;

public:
    NodoDoble(const T& pElem) {
        elem = pElem;
        ant = nullptr;
        sgte = nullptr;
    }

    const T& get_Elem() const { return elem; }
    T& ref_Elem() { return elem; }
    void set_Elem(const T& pElem) { elem = pElem; }

    NodoDoble<T>* get_Ant() const { return ant; }
    void set_Ant(NodoDoble<T>* pAnt) { ant = pAnt; }

    NodoDoble<T>* get_Sgte() const { return sgte; }
    void set_Sgte(NodoDoble<T>* pSgte) { sgte = pSgte; }
};
