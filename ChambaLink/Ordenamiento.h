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

// ---------- MergeSort (pendiente: Aldo) ----------
// Idea: dividir el arreglo en dos mitades, ordenar cada una de forma
// recursiva y mezclarlas comparando con antesQue. Siempre O(n log n).
//
// template <class T>
// void mezclar(T* arreglo, int inicio, int medio, int final,
//     std::function<bool(const T&, const T&)> antesQue);
//
// template <class T>
// void mergeSortArreglo(T* arreglo, int inicio, int final,
//     std::function<bool(const T&, const T&)> antesQue);
//
// template <class T>
// Lista<T> mergeSort(const Lista<T>& lista, std::function<bool(const T&, const T&)> antesQue);

// ---------- HeapSort (pendiente: Piero) ----------
// Idea: construir un monticulo con el arreglo, sacar la raiz una por una
// y volver a acomodar. Siempre O(n log n) y sin memoria extra sobre el arreglo.
//
// template <class T>
// void hundir(T* arreglo, int tamanio, int posicion,
//     std::function<bool(const T&, const T&)> antesQue);
//
// template <class T>
// Lista<T> heapSort(const Lista<T>& lista, std::function<bool(const T&, const T&)> antesQue);