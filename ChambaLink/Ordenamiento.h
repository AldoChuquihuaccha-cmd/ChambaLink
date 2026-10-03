#pragma once
#include <functional>
#include "Lista.h"

// Algoritmos de ordenamiento para Lista<T>.
//
// Una lista enlazada no permite acceder por posicion en O(1): llegar al
// elemento i cuesta O(i). Por eso los tres algoritmos trabajan igual:
//   1. copian los elementos a un arreglo dinamico,
//   2. ordenan el arreglo,
//   3. construyen y devuelven una lista nueva ya ordenada.
// El costo es O(n) de memoria extra; en el informe se analiza si conviene.
//
// El criterio de orden entra como lambda: antesQue(a, b) devuelve true si
// a debe quedar antes que b. Asi la misma funcion sirve para ordenar
// contactos por nombre, publicaciones por fecha o vacantes por titulo.
//
// Reparto: QuickSort (Joao), MergeSort (Aldo), HeapSort (Piero).

// ---------- Funciones de apoyo ----------

template <class T>
void intercambiar(T& a, T& b) {
    T temporal = a;
    a = b;
    b = temporal;
}

// Copia los elementos de la lista a un arreglo dinamico. O(n)
// Quien la llama debe liberar el arreglo con delete[].
template <class T>
T* listaAArreglo(const Lista<T>& lista) {
    uint cantidad = lista.longitud();
    if (cantidad == 0) return nullptr;

    T* arreglo = new T[cantidad];
    uint i = 0;
    lista.paraCada([&arreglo, &i](const T& elem) {
        arreglo[i] = elem;
        i++;
        });
    return arreglo;
}

// Construye una lista nueva a partir del arreglo. O(n)
template <class T>
Lista<T> arregloALista(T* arreglo, uint cantidad) {
    Lista<T> resultado;
    for (uint i = 0; i < cantidad; i++)
        resultado.agregaFinal(arreglo[i]);
    return resultado;
}

// ---------- QuickSort (Joao) ----------

// Coloca el pivote (el ultimo elemento) en su posicion definitiva y deja
// a la izquierda los elementos que van antes que el. Devuelve esa posicion.
template <class T>
int particionar(T* arreglo, int inicio, int final,
    std::function<bool(const T&, const T&)> antesQue) {
    T pivote = arreglo[final];
    int limite = inicio - 1;

    for (int i = inicio; i < final; i++) {
        if (antesQue(arreglo[i], pivote)) {
            limite++;
            intercambiar(arreglo[limite], arreglo[i]);
        }
    }
    intercambiar(arreglo[limite + 1], arreglo[final]);
    return limite + 1;
}

// Ordena el arreglo entre las posiciones inicio y final.
// Es recursivo: divide en dos partes y ordena cada una.
template <class T>
void quickSortArreglo(T* arreglo, int inicio, int final,
    std::function<bool(const T&, const T&)> antesQue) {
    if (inicio >= final) return;                    // caso base: 0 o 1 elemento

    int posicionPivote = particionar(arreglo, inicio, final, antesQue);
    quickSortArreglo(arreglo, inicio, posicionPivote - 1, antesQue);
    quickSortArreglo(arreglo, posicionPivote + 1, final, antesQue);
}

// Devuelve una lista nueva ordenada con QuickSort.
// Promedio O(n log n); peor caso O(n^2) cuando el pivote siempre queda
// en un extremo, por ejemplo si la lista ya venia ordenada.
template <class T>
Lista<T> quickSort(const Lista<T>& lista, std::function<bool(const T&, const T&)> antesQue) {
    uint cantidad = lista.longitud();
    if (cantidad < 2) return lista;                 // vacia o de un solo elemento

    T* arreglo = listaAArreglo(lista);
    quickSortArreglo(arreglo, 0, (int)cantidad - 1, antesQue);
    Lista<T> resultado = arregloALista(arreglo, cantidad);
    delete[] arreglo;
    return resultado;
}








// ---------- HeapSort (Piero) ----------

// Acomoda el elemento de la posicion indicada hacia abajo hasta que el
// monticulo vuelva a cumplir su propiedad (el padre va despues que sus hijos
// segun antesQue). Es recursivo.
// Caso base: no tiene hijos que deban subir. O(log n)
template <class T>
void hundir(T* arreglo, int tamanio, int posicion,
    std::function<bool(const T&, const T&)> antesQue) {
    int mayor = posicion;
    int hijoIzquierdo = 2 * posicion + 1;
    int hijoDerecho = 2 * posicion + 2;

    if (hijoIzquierdo < tamanio && antesQue(arreglo[mayor], arreglo[hijoIzquierdo]))
        mayor = hijoIzquierdo;
    if (hijoDerecho < tamanio && antesQue(arreglo[mayor], arreglo[hijoDerecho]))
        mayor = hijoDerecho;

    if (mayor != posicion) {
        intercambiar(arreglo[posicion], arreglo[mayor]);
        hundir(arreglo, tamanio, mayor, antesQue);
    }
}

// Devuelve una lista nueva ordenada con HeapSort.
//   1. arma un monticulo con el arreglo: O(n)
//   2. pasa la raiz (el que va al final) a la ultima posicion libre y acomoda: O(n log n)
// Siempre O(n log n) y no usa memoria extra sobre el arreglo. No es estable.
template <class T>
Lista<T> heapSort(const Lista<T>& lista, std::function<bool(const T&, const T&)> antesQue) {
    uint cantidad = lista.longitud();
    if (cantidad < 2) return lista;

    T* arreglo = listaAArreglo(lista);
    for (int i = (int)cantidad / 2 - 1; i >= 0; i--)    // las hojas no se acomodan
        hundir(arreglo, (int)cantidad, i, antesQue);

    for (int i = (int)cantidad - 1; i > 0; i--) {
        intercambiar(arreglo[0], arreglo[i]);
        hundir(arreglo, i, 0, antesQue);
    }

    Lista<T> resultado = arregloALista(arreglo, cantidad);
    delete[] arreglo;
    return resultado;
}

// ---------- MergeSort (Aldo) ----------

// Mezcla dos mitades ya ordenadas: [inicio..medio] y [medio+1..final].
// Va tomando el menor de los dos frentes y lo copia al auxiliar.
// Si hay empate se toma el de la izquierda: por eso es estable.
template <class T>
void mezclar(T* arreglo, T* auxiliar, int inicio, int medio, int final,
    std::function<bool(const T&, const T&)> antesQue) {
    int i = inicio;       // frente de la mitad izquierda
    int j = medio + 1;    // frente de la mitad derecha
    int k = inicio;       // posicion en el auxiliar
    while (i <= medio && j <= final) {
        if (antesQue(arreglo[j], arreglo[i])) { auxiliar[k] = arreglo[j]; j++; }
        else { auxiliar[k] = arreglo[i]; i++; }
        k++;
    }
    // Lo que sobre de una de las mitades ya esta ordenado: se copia tal cual.
    while (i <= medio) { auxiliar[k] = arreglo[i]; i++; k++; }
    while (j <= final) { auxiliar[k] = arreglo[j]; j++; k++; }
    for (k = inicio; k <= final; k++) arreglo[k] = auxiliar[k];
}

// Recursivo: divide en dos mitades, ordena cada una y las mezcla.
template <class T>
void mergeSortArreglo(T* arreglo, T* auxiliar, int inicio, int final,
    std::function<bool(const T&, const T&)> antesQue) {
    if (inicio >= final) return;                    // caso base: 0 o 1 elemento
    int medio = inicio + (final - inicio) / 2;
    mergeSortArreglo(arreglo, auxiliar, inicio, medio, antesQue);
    mergeSortArreglo(arreglo, auxiliar, medio + 1, final, antesQue);
    mezclar(arreglo, auxiliar, inicio, medio, final, antesQue);
}

// Devuelve una lista nueva ordenada con MergeSort; la original no cambia.
// Siempre O(n log n). Memoria extra O(n) por el arreglo auxiliar.
template <class T>
Lista<T> mergeSort(const Lista<T>& lista, std::function<bool(const T&, const T&)> antesQue) {
    uint cantidad = lista.longitud();
    if (cantidad < 2) return lista;

    T* arreglo = listaAArreglo(lista);
    T* auxiliar = new T[cantidad];      // se reserva una sola vez para toda la recursion
    mergeSortArreglo(arreglo, auxiliar, 0, (int)cantidad - 1, antesQue);
    Lista<T> resultado = arregloALista(arreglo, cantidad);
    delete[] auxiliar;
    delete[] arreglo;
    return resultado;
}
