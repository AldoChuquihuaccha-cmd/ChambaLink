#pragma once

#include <functional>
#include <cstddef>
#include <stdexcept>

// Arbol AVL generico para ChambaLink.
// El orden de los elementos se define mediante un comparador que devuelve:
//   < 0 si a va antes que b,
//   = 0 si a y b tienen la misma clave,
//   > 0 si a va despues que b.
//
// Complejidades (n = cantidad de nodos):
//   insertar / buscar / eliminar: O(log n)
//   recorrido en orden: O(n)
//   rotaciones: O(1)
//   altura: O(1), porque se mantiene actualizada en cada nodo.
//
// La clase no depende de las entidades del dominio. Puede almacenar, por
// ejemplo, claves de habilidades, vacantes u otros objetos comparables.
template <class T>
class ArbolAVL {
private:
    class Nodo {
    public:
        T dato;
        Nodo* izquierdo;
        Nodo* derecho;
        int altura;

        Nodo(const T& valor)
            : dato(valor), izquierdo(nullptr), derecho(nullptr), altura(1) {
        }
    };

    Nodo* raiz;
    std::size_t cantidad;
    std::function<int(const T&, const T&)> comparar;

    static int alturaNodo(const Nodo* nodo) {
        return nodo == nullptr ? 0 : nodo->altura;
    }

    static int mayor(int a, int b) {
        return a > b ? a : b;
    }

    static void actualizarAltura(Nodo* nodo) {
        if (nodo != nullptr)
            nodo->altura = 1 + mayor(alturaNodo(nodo->izquierdo), alturaNodo(nodo->derecho));
    }

    static int factorBalance(const Nodo* nodo) {
        if (nodo == nullptr) return 0;
        return alturaNodo(nodo->izquierdo) - alturaNodo(nodo->derecho);
    }

    // Rotacion simple a la derecha (caso LL).
    static Nodo* rotarDerecha(Nodo* y) {
        Nodo* x = y->izquierdo;
        Nodo* subArbol = x->derecho;

        x->derecho = y;
        y->izquierdo = subArbol;

        actualizarAltura(y);
        actualizarAltura(x);
        return x;
    }

    // Rotacion simple a la izquierda (caso RR).
    static Nodo* rotarIzquierda(Nodo* x) {
        Nodo* y = x->derecho;
        Nodo* subArbol = y->izquierdo;

        y->izquierdo = x;
        x->derecho = subArbol;

        actualizarAltura(x);
        actualizarAltura(y);
        return y;
    }

    // Recalcula altura y corrige los casos LL, RR, LR y RL.
    static Nodo* balancear(Nodo* nodo) {
        if (nodo == nullptr) return nullptr;

        actualizarAltura(nodo);
        int balance = factorBalance(nodo);

        // Desbalance hacia la izquierda.
        if (balance > 1) {
            if (factorBalance(nodo->izquierdo) < 0)
                nodo->izquierdo = rotarIzquierda(nodo->izquierdo); // Caso LR.
            return rotarDerecha(nodo); // Caso LL o final del caso LR.
        }

        // Desbalance hacia la derecha.
        if (balance < -1) {
            if (factorBalance(nodo->derecho) > 0)
                nodo->derecho = rotarDerecha(nodo->derecho); // Caso RL.
            return rotarIzquierda(nodo); // Caso RR o final del caso RL.
        }

        return nodo;
    }

    Nodo* insertarRec(Nodo* actual, const T& valor, bool& insertado) {
        if (actual == nullptr) {
            insertado = true;
            return new Nodo(valor);
        }

        int resultado = comparar(valor, actual->dato);
        if (resultado < 0)
            actual->izquierdo = insertarRec(actual->izquierdo, valor, insertado);
        else if (resultado > 0)
            actual->derecho = insertarRec(actual->derecho, valor, insertado);
        else
            return actual; // No se permiten claves duplicadas.

        return balancear(actual);
    }

    Nodo* buscarNodo(Nodo* actual, const T& clave) const {
        while (actual != nullptr) {
            int resultado = comparar(clave, actual->dato);
            if (resultado == 0) return actual;
            actual = resultado < 0 ? actual->izquierdo : actual->derecho;
        }
        return nullptr;
    }

    static Nodo* minimo(Nodo* actual) {
        while (actual != nullptr && actual->izquierdo != nullptr)
            actual = actual->izquierdo;
        return actual;
    }

    Nodo* eliminarRec(Nodo* actual, const T& clave, bool& eliminado) {
        if (actual == nullptr) return nullptr;

        int resultado = comparar(clave, actual->dato);
        if (resultado < 0) {
            actual->izquierdo = eliminarRec(actual->izquierdo, clave, eliminado);
        }
        else if (resultado > 0) {
            actual->derecho = eliminarRec(actual->derecho, clave, eliminado);
        }
        else {
            eliminado = true;

            // Caso de cero o un hijo.
            if (actual->izquierdo == nullptr || actual->derecho == nullptr) {
                Nodo* hijo = actual->izquierdo != nullptr ? actual->izquierdo : actual->derecho;
                delete actual;
                return hijo;
            }

            // Caso de dos hijos: reemplaza por el sucesor inorden.
            Nodo* sucesor = minimo(actual->derecho);
            actual->dato = sucesor->dato;
            bool eliminadoSucesor = false;
            actual->derecho = eliminarRec(actual->derecho, sucesor->dato, eliminadoSucesor);
        }

        return balancear(actual);
    }

    static void destruir(Nodo* nodo) {
        if (nodo == nullptr) return;
        destruir(nodo->izquierdo);
        destruir(nodo->derecho);
        delete nodo;
    }

    static Nodo* copiar(const Nodo* origen) {
        if (origen == nullptr) return nullptr;

        Nodo* nuevo = new Nodo(origen->dato);
        nuevo->izquierdo = copiar(origen->izquierdo);
        nuevo->derecho = copiar(origen->derecho);
        nuevo->altura = origen->altura;
        return nuevo;
    }

    static void enOrdenRec(const Nodo* actual,
        const std::function<void(const T&)>& accion) {
        if (actual == nullptr) return;
        enOrdenRec(actual->izquierdo, accion);
        accion(actual->dato);
        enOrdenRec(actual->derecho, accion);
    }

    static void preOrdenRec(const Nodo* actual,
        const std::function<void(const T&)>& accion) {
        if (actual == nullptr) return;
        accion(actual->dato);
        preOrdenRec(actual->izquierdo, accion);
        preOrdenRec(actual->derecho, accion);
    }

    static void postOrdenRec(const Nodo* actual,
        const std::function<void(const T&)>& accion) {
        if (actual == nullptr) return;
        postOrdenRec(actual->izquierdo, accion);
        postOrdenRec(actual->derecho, accion);
        accion(actual->dato);
    }


public:
    ArbolAVL(std::function<int(const T&, const T&)> criterio)
        : raiz(nullptr), cantidad(0), comparar(criterio) {
        if (!comparar)
            throw std::invalid_argument("ArbolAVL requiere un comparador valido");
    }

    ArbolAVL(const ArbolAVL& otro)
        : raiz(copiar(otro.raiz)), cantidad(otro.cantidad), comparar(otro.comparar) {
    }

    ArbolAVL& operator=(const ArbolAVL& otro) {
        if (this != &otro) {
            Nodo* nuevaRaiz = copiar(otro.raiz);
            destruir(raiz);
            raiz = nuevaRaiz;
            cantidad = otro.cantidad;
            comparar = otro.comparar;
        }
        return *this;
    }

    ~ArbolAVL() {
        destruir(raiz);
    }

    // Inserta una clave si no existe. Devuelve true si se inserto.
    bool insertar(const T& valor) {
        bool insertado = false;
        raiz = insertarRec(raiz, valor, insertado);
        if (insertado) cantidad++;
        return insertado;
    }

    // Devuelve un puntero al dato almacenado o nullptr si no existe.
    T* buscar(const T& clave) {
        Nodo* encontrado = buscarNodo(raiz, clave);
        return encontrado == nullptr ? nullptr : &encontrado->dato;
    }

    const T* buscar(const T& clave) const {
        Nodo* encontrado = buscarNodo(raiz, clave);
        return encontrado == nullptr ? nullptr : &encontrado->dato;
    }

    bool contiene(const T& clave) const {
        return buscar(clave) != nullptr;
    }

    // Elimina una clave y rebalancea los nodos del camino de regreso.
    bool eliminar(const T& clave) {
        bool eliminado = false;
        raiz = eliminarRec(raiz, clave, eliminado);
        if (eliminado) cantidad--;
        return eliminado;
    }

    void enOrden(std::function<void(const T&)> accion) const {
        if (accion) enOrdenRec(raiz, accion);
    }

    void preOrden(std::function<void(const T&)> accion) const {
        if (accion) preOrdenRec(raiz, accion);
    }

    void postOrden(std::function<void(const T&)> accion) const {
        if (accion) postOrdenRec(raiz, accion);
    }

    int altura() const {
        return alturaNodo(raiz);
    }

    std::size_t longitud() const {
        return cantidad;
    }

    bool esVacio() const {
        return raiz == nullptr;
    }

    void vaciar() {
        destruir(raiz);
        raiz = nullptr;
        cantidad = 0;
    }
};
