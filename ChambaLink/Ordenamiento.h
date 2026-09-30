#pragma once
#include <functional>
#include <algorithm>
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






// ---------- HeapSort (Piero) ----------
// Idea: construir un monticulo con el arreglo, sacar la raiz una por una
// y volver a acomodar. Siempre O(n log n) y sin memoria extra sobre el arreglo.
//
// HeapSort trabaja sobre el arreglo temporal generado desde Lista<T>.
// Primero construye un monticulo maximo y luego coloca los elementos mayores
// al final del arreglo. El criterio antesQue permite reutilizar el algoritmo
// para diferentes tipos de datos.
//
// Complejidad:
// Construccion del monticulo: O(n)
// Ordenamiento completo: O(n log n)
// Memoria adicional: O(1) sobre el arreglo temporal.


/*
    Mantiene la propiedad del monticulo.

    tamanio representa la cantidad de elementos considerados dentro
    del heap y posicion es el nodo que se debe acomodar.

    Caso base:
    Cuando los hijos de la posicion ya no existen o no hay intercambio,
    termina la ejecucion.

    Costo: O(log n)
*/
template <class T>
void hundir(
    T* arreglo,
    int tamanio,
    int posicion,
    std::function<bool(const T&, const T&)> antesQue
)
{
    int mayor = posicion;

    int hijoIzquierdo = 2 * posicion + 1;
    int hijoDerecho = 2 * posicion + 2;


    // Si el hijo izquierdo debe estar antes que el padre,
    // se convierte en el mayor candidato.
    if (hijoIzquierdo < tamanio &&
        antesQue(arreglo[mayor], arreglo[hijoIzquierdo]))
    {
        mayor = hijoIzquierdo;
    }


    // Si el hijo derecho es mayor, se actualiza.
    if (hijoDerecho < tamanio &&
        antesQue(arreglo[mayor], arreglo[hijoDerecho]))
    {
        mayor = hijoDerecho;
    }


    // Si el mayor no es la posicion actual,
    // se intercambian y se continua hacia abajo.
    if (mayor != posicion)
    {
        intercambiar(
            arreglo[posicion],
            arreglo[mayor]
        );


        hundir(
            arreglo,
            tamanio,
            mayor,
            antesQue
        );
    }
}



/*
    Ordena una Lista<T> utilizando HeapSort.

    Pasos:
    1. Copia los elementos de la lista a un arreglo.
    2. Construye un monticulo maximo.
    3. Extrae el elemento mayor y lo coloca al final.
    4. Convierte nuevamente el arreglo ordenado en Lista<T>.

    Complejidad:
    Mejor caso: O(n log n)
    Caso promedio: O(n log n)
    Peor caso: O(n log n)
*/
template <class T>
Lista<T> heapSort(
    const Lista<T>& lista,
    std::function<bool(const T&, const T&)> antesQue
)
{
    uint cantidad = lista.longitud();

    if (cantidad < 2)
        return lista;


    T* arreglo = listaAArreglo(lista);



    // Construccion inicial del monticulo.
    // Los nodos hoja no necesitan ser acomodados.
    for (int i = (int)cantidad / 2 - 1;
        i >= 0;
        i--)
    {
        hundir(
            arreglo,
            cantidad,
            i,
            antesQue
        );
    }



    // Extraer elementos del monticulo.
    for (int i = cantidad - 1;
        i > 0;
        i--)
    {

        // El elemento mayor pasa al final.
        intercambiar(
            arreglo[0],
            arreglo[i]
        );


        // Se vuelve a acomodar el heap restante.
        hundir(
            arreglo,
            i,
            0,
            antesQue
        );
    }



    Lista<T> resultado =
        arregloALista(
            arreglo,
            cantidad
        );


    delete[] arreglo;

    return resultado;
}

// MergeSort 
// Idea: dividir el arreglo en dos mitades, ordenar cada una de forma
// recursiva y mezclarlas comparando con antesQue.
// Complejidad: O(n log n) en el mejor, promedio y peor caso.
// Memoria extra: O(n) por el arreglo auxiliar.
// Es ESTABLE: los elementos iguales conservan su orden original.

// Mezcla dos mitades ya ordenadas: [inicio..medio] y [medio+1..final].
// Va tomando el menor de los dos frentes y lo copia al auxiliar.
// Si hay empate se toma el de la izquierda (por eso es estable).
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
    // Se devuelve el tramo mezclado al arreglo original.
    for (k = inicio; k <= final; k++) arreglo[k] = auxiliar[k];
}

template <class T>
void mergeSortArreglo(T* arreglo, T* auxiliar, int inicio, int final,
    std::function<bool(const T&, const T&)> antesQue) {
    if (inicio >= final) return;                    // caso base: 0 o 1 elemento
    int medio = inicio + (final - inicio) / 2;
    mergeSortArreglo(arreglo, auxiliar, inicio, medio, antesQue);       // ordena la izquierda
    mergeSortArreglo(arreglo, auxiliar, medio + 1, final, antesQue);    // ordena la derecha
    mezclar(arreglo, auxiliar, inicio, medio, final, antesQue);         // las junta
}

// Devuelve una lista NUEVA ordenada; la original no cambia.
// Uso: mergeSort<Publicacion>(publicaciones, [](const Publicacion& a, const Publicacion& b) {...});
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









































































































































































































