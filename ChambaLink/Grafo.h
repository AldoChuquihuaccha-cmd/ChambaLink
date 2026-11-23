#pragma once
#include <functional>
#include "Lista.h"
#include "Cola.h"

// Grafo generico no dirigido con lista de adyacencia.
//   Cada vertice guarda su dato (T) y la lista de aristas que salen de el.
//   Cada arista guarda el indice del vertice de llegada y su peso.
//   Como es no dirigido, cada arista se guarda en los dos vertices.
//
// Se usa con dos tipos:
//   Grafo<std::string>: distritos de Lima y Callao, peso = km entre ellos.
//   Grafo<int>: red de contactos, el dato es el id del usuario y el peso es 1.
//
// Memoria O(V + E). Con una matriz serian V x V casillas: 100 millones con 10,000 usuarios.
template <class T>
class Grafo {
private:
    class Arista {
    public:
        int destino;
        double peso;

        Arista() {
            destino = -1;
            peso = 0;
        }
        Arista(int pDestino, double pPeso) {
            destino = pDestino;
            peso = pPeso;
        }
    };

    class Vertice {
    public:
        T dato;
        Lista<Arista> aristas;
    };

    Vertice* vertices;   // arreglo dinamico que crece al doble cuando se llena
    int cantidad;
    int capacidad;

    // Duplica el arreglo de vertices. O(V + E), pero pasa pocas veces.
    void crecer() {
        capacidad = capacidad * 2;
        Vertice* nuevo = new Vertice[capacidad];
        for (int i = 0; i < cantidad; i++) nuevo[i] = vertices[i];
        delete[] vertices;
        vertices = nuevo;
    }

public:
    Grafo(int pCapacidad = 16) {
        capacidad = pCapacidad < 1 ? 1 : pCapacidad;
        cantidad = 0;
        vertices = new Vertice[capacidad];
    }

    ~Grafo() {
        delete[] vertices;
    }

    // No se copia: con miles de usuarios la copia seria muy costosa.
    Grafo(const Grafo<T>& otro) = delete;
    Grafo<T>& operator=(const Grafo<T>& otro) = delete;

    // Agrega un vertice y devuelve su indice. O(1) en promedio.
    int agregarVertice(const T& dato) {
        if (cantidad == capacidad) crecer();
        vertices[cantidad].dato = dato;
        cantidad++;
        return cantidad - 1;
    }

    int cantidadVertices() const { return cantidad; }

    const T& obtenerVertice(int v) const { return vertices[v].dato; }

    // Indice del vertice que guarda ese dato, o -1 si no esta. O(V)
    int indiceDe(const T& dato) const {
        for (int i = 0; i < cantidad; i++)
            if (vertices[i].dato == dato) return i;
        return -1;
    }

    // Une a y b en los dos sentidos. No repite aristas ni une un vertice consigo mismo.
    // O(grado de a) por la revision de repetidos.
    bool agregarArista(int a, int b, double peso = 1) {
        if (a < 0 || b < 0 || a >= cantidad || b >= cantidad || a == b) return false;
        if (existeArista(a, b)) return false;
        vertices[a].aristas.agregaFinal(Arista(b, peso));
        vertices[b].aristas.agregaFinal(Arista(a, peso));
        return true;
    }

    // Quita la arista en los dos sentidos. O(grado de a + grado de b)
    bool eliminarArista(int a, int b) {
        if (a < 0 || b < 0 || a >= cantidad || b >= cantidad) return false;
        bool quitada = vertices[a].aristas.eliminaSi([b](const Arista& x) { return x.destino == b; });
        vertices[b].aristas.eliminaSi([a](const Arista& x) { return x.destino == a; });
        return quitada;
    }

    // O(grado de a)
    bool existeArista(int a, int b) const {
        return vertices[a].aristas.existe([b](const Arista& x) { return x.destino == b; });
    }

    // Cantidad de vecinos de v. O(1)
    int grado(int v) const { return (int)vertices[v].aristas.longitud(); }

    // Recorre los vecinos de v. La lambda recibe el indice del vecino y el peso. O(grado de v)
    void paraCadaVecino(int v, std::function<void(int, double)> accion) const {
        vertices[v].aristas.paraCada([&accion](const Arista& x) { accion(x.destino, x.peso); });
    }

    // BFS (recorrido en anchura con una Cola).
    // Deja en saltos[i] cuantas aristas hay del origen a i; -1 si no se puede llegar.
    // El arreglo saltos debe tener cantidadVertices() casillas. O(V + E)
    void bfs(int origen, int saltos[]) const {
        for (int i = 0; i < cantidad; i++) saltos[i] = -1;
        if (origen < 0 || origen >= cantidad) return;

        Cola<int> cola;
        saltos[origen] = 0;
        cola.encolar(origen);
        while (!cola.esVacia()) {
            int actual = cola.desencolar();
            vertices[actual].aristas.paraCada([&](const Arista& x) {
                if (saltos[x.destino] == -1) {
                    saltos[x.destino] = saltos[actual] + 1;
                    cola.encolar(x.destino);
                }
                });
        }
    }

    // Vertices a lo mas maxSaltos aristas del origen, incluido el origen,
    // en el orden en que el BFS los encuentra. O(V + E)
    Lista<int> verticesASaltos(int origen, int maxSaltos) const {
        Lista<int> resultado;
        if (origen < 0 || origen >= cantidad) return resultado;

        int* saltos = new int[cantidad];
        bfs(origen, saltos);
        for (int s = 0; s <= maxSaltos; s++)
            for (int i = 0; i < cantidad; i++)
                if (saltos[i] == s) resultado.agregaFinal(i);
        delete[] saltos;
        return resultado;
    }

    // Dijkstra: menor suma de pesos desde el origen a cada vertice.
    //   1. Todas las distancias empiezan en "infinito" y la del origen en 0.
    //   2. Se elige el vertice no visitado con menor distancia y se marca como visitado.
    //   3. Se revisan sus vecinos: si pasando por el se llega mas corto, se actualiza.
    //   4. Se repite hasta visitar todos.
    // El arreglo distancia debe tener cantidadVertices() casillas.
    // O(V^2 + E): elegir el menor recorre los V vertices en cada uno de los V pasos.
    void dijkstra(int origen, double distancia[]) const {
        bool* visitado = new bool[cantidad];
        for (int i = 0; i < cantidad; i++) {
            distancia[i] = 1000000;   // "infinito"
            visitado[i] = false;
        }
        if (origen >= 0 && origen < cantidad) distancia[origen] = 0;

        for (int paso = 0; paso < cantidad; paso++) {
            int actual = -1;
            for (int i = 0; i < cantidad; i++)
                if (!visitado[i] && (actual == -1 || distancia[i] < distancia[actual]))
                    actual = i;
            if (actual == -1 || distancia[actual] >= 1000000) break;
            visitado[actual] = true;

            vertices[actual].aristas.paraCada([&](const Arista& x) {
                double nueva = distancia[actual] + x.peso;
                if (!visitado[x.destino] && nueva < distancia[x.destino])
                    distancia[x.destino] = nueva;
                });
        }
        delete[] visitado;
    }
};
