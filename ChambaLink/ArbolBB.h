#pragma once

#include <functional>
#include <cstddef>
#include <stdexcept>

// Arbol binario de busqueda generico.
//
// El tipo T es independiente del dominio. El orden se define mediante un
// comparador que debe devolver:
//   < 0 si a va antes que b,
//   = 0 si a y b representan la misma clave,
//   > 0 si a va despues que b.
//
// Complejidades (n = cantidad de nodos, h = altura):
//   insertar / buscar / eliminar: O(h)
//   recorrido en orden: O(n)
//   altura / cantidad: O(n)
// En un arbol equilibrado h = O(log n); en el peor caso h = O(n).
template <class T>
class ArbolBB {
private:
    class Nodo {
    public:
        T dato;
        Nodo* izquierdo;
        Nodo* derecho;

        explicit Nodo(const T& valor)
            : dato(valor), izquierdo(nullptr), derecho(nullptr) {
        }
    };

    Nodo* raiz;
    std::size_t cantidad;
    std::function<int(const T&, const T&)> comparar;

    static void destruir(Nodo* nodo) {
        if (nodo == nullptr) return;
        destruir(nodo->izquierdo);
        destruir(nodo->derecho);
        delete nodo;
    }

    static Nodo* copiar(const Nodo* origen) {
        if (origen == nullptr) return nullptr;
        Nodo* nuevo = new Nodo(origen->dato);
        try {
            nuevo->izquierdo = copiar(origen->izquierdo);
            nuevo->derecho = copiar(origen->derecho);
        }
        catch (...) {
            destruir(nuevo);
            throw;
        }
        return nuevo;
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
        // Si resultado == 0, no se permiten claves duplicadas.
        return actual;
    }

    Nodo* buscarNodo(Nodo* actual, const T& clave) const {
        while (actual != nullptr) {
            int resultado = comparar(clave, actual->dato);
            if (resultado == 0) return actual;
            actual = (resultado < 0) ? actual->izquierdo : actual->derecho;
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
            if (actual->izquierdo == nullptr) {
                Nodo* reemplazo = actual->derecho;
                delete actual;
                return reemplazo;
            }
            if (actual->derecho == nullptr) {
                Nodo* reemplazo = actual->izquierdo;
                delete actual;
                return reemplazo;
            }

            // Dos hijos: se reemplaza el dato por el sucesor inorden.
            Nodo* sucesor = minimo(actual->derecho);
            actual->dato = sucesor->dato;
            bool eliminadoSucesor = false;
            actual->derecho = eliminarRec(actual->derecho, sucesor->dato, eliminadoSucesor);
        }
        return actual;
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

    static int alturaRec(const Nodo* actual) {
        if (actual == nullptr) return 0;
        int izquierda = alturaRec(actual->izquierdo);
        int derecha = alturaRec(actual->derecho);
        return 1 + (izquierda > derecha ? izquierda : derecha);
    }

    static std::size_t contarRec(const Nodo* actual) {
        if (actual == nullptr) return 0;
        return 1 + contarRec(actual->izquierdo) + contarRec(actual->derecho);
    }

    bool validarRec(const Nodo* actual, const T* minimoPermitido,
        const T* maximoPermitido) const {
        if (actual == nullptr) return true;
        if (minimoPermitido != nullptr && comparar(actual->dato, *minimoPermitido) <= 0)
            return false;
        if (maximoPermitido != nullptr && comparar(actual->dato, *maximoPermitido) >= 0)
            return false;
        return validarRec(actual->izquierdo, minimoPermitido, &actual->dato) &&
            validarRec(actual->derecho, &actual->dato, maximoPermitido);
    }

public:
    explicit ArbolBB(std::function<int(const T&, const T&)> criterio)
        : raiz(nullptr), cantidad(0), comparar(criterio) {
        if (!comparar) throw std::invalid_argument("ArbolBB requiere un comparador valido");
    }

    ArbolBB(const ArbolBB& otro)
        : raiz(copiar(otro.raiz)), cantidad(otro.cantidad), comparar(otro.comparar) {
    }

    ArbolBB& operator=(const ArbolBB& otro) {
        if (this != &otro) {
            Nodo* nuevaRaiz = copiar(otro.raiz);
            destruir(raiz);
            raiz = nuevaRaiz;
            cantidad = otro.cantidad;
            comparar = otro.comparar;
        }
        return *this;
    }

    ~ArbolBB() { destruir(raiz); }

    bool insertar(const T& valor) {
        bool insertado = false;
        raiz = insertarRec(raiz, valor, insertado);
        if (insertado) cantidad++;
        return insertado;
    }

    T* buscar(const T& clave) {
        Nodo* encontrado = buscarNodo(raiz, clave);
        return encontrado == nullptr ? nullptr : &encontrado->dato;
    }

    const T* buscar(const T& clave) const {
        Nodo* encontrado = buscarNodo(raiz, clave);
        return encontrado == nullptr ? nullptr : &encontrado->dato;
    }

    bool contiene(const T& clave) const { return buscar(clave) != nullptr; }

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

    int altura() const { return alturaRec(raiz); }
    std::size_t longitud() const { return cantidad; }
    bool esVacio() const { return raiz == nullptr; }

    bool validarPropiedadBST() const {
        return validarRec(raiz, nullptr, nullptr);
    }

    void vaciar() {
        destruir(raiz);
        raiz = nullptr;
        cantidad = 0;
    }
};
