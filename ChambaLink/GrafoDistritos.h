#pragma once
#include <string>
#include "Lista.h"
#include "Cola.h"

// Grafo de distritos de Lima y Callao (TB2).
//   Vertices: los 50 distritos (43 de Lima y 7 del Callao), cada uno con un indice de 0 a 49.
//   Aristas: distritos que colindan. Peso = distancia aproximada en km entre sus centros.
//   Es un grafo ponderado y no dirigido: distancias[a][b] = distancias[b][a].
//
// Representacion: matriz de adyacencia de 50 x 50.
//   distancias[a][b] = km si a y b colindan, 0 si no.
//   Memoria O(V^2) = 2500 celdas. Ver si dos distritos colindan es O(1).
//
// Para distritos que no colindan, Dijkstra calcula el camino mas corto
// pasando por los distritos intermedios.

#define TOTAL_DISTRITOS 50

class GrafoDistritos {
private:
    std::string distritos[TOTAL_DISTRITOS];
    double distancias[TOTAL_DISTRITOS][TOTAL_DISTRITOS];

    // Grafo no dirigido: se guarda en las dos direcciones. O(1)
    void agregarArista(int a, int b, double km) {
        distancias[a][b] = km;
        distancias[b][a] = km;
    }

    std::string aMinusculas(std::string texto) const {
        for (int i = 0; i < (int)texto.length(); i++)
            if (texto[i] >= 'A' && texto[i] <= 'Z') texto[i] = texto[i] - 'A' + 'a';
        return texto;
    }

public:
    GrafoDistritos() {
        distritos[0] = "Ancon";
        distritos[1] = "Ate";
        distritos[2] = "Barranco";
        distritos[3] = "Brena";
        distritos[4] = "Carabayllo";
        distritos[5] = "Chaclacayo";
        distritos[6] = "Chorrillos";
        distritos[7] = "Cieneguilla";
        distritos[8] = "Comas";
        distritos[9] = "El Agustino";
        distritos[10] = "Independencia";
        distritos[11] = "Jesus Maria";
        distritos[12] = "La Molina";
        distritos[13] = "La Victoria";
        distritos[14] = "Lima";
        distritos[15] = "Lince";
        distritos[16] = "Los Olivos";
        distritos[17] = "Lurigancho";
        distritos[18] = "Lurin";
        distritos[19] = "Magdalena del Mar";
        distritos[20] = "Miraflores";
        distritos[21] = "Pachacamac";
        distritos[22] = "Pucusana";
        distritos[23] = "Pueblo Libre";
        distritos[24] = "Puente Piedra";
        distritos[25] = "Punta Hermosa";
        distritos[26] = "Punta Negra";
        distritos[27] = "Rimac";
        distritos[28] = "San Bartolo";
        distritos[29] = "San Borja";
        distritos[30] = "San Isidro";
        distritos[31] = "San Juan de Lurigancho";
        distritos[32] = "San Juan de Miraflores";
        distritos[33] = "San Luis";
        distritos[34] = "San Martin de Porres";
        distritos[35] = "San Miguel";
        distritos[36] = "Santa Anita";
        distritos[37] = "Santa Maria del Mar";
        distritos[38] = "Santa Rosa";
        distritos[39] = "Santiago de Surco";
        distritos[40] = "Surquillo";
        distritos[41] = "Villa El Salvador";
        distritos[42] = "Villa Maria del Triunfo";
        distritos[43] = "Callao";
        distritos[44] = "Bellavista";
        distritos[45] = "Carmen de la Legua";
        distritos[46] = "La Perla";
        distritos[47] = "La Punta";
        distritos[48] = "Mi Peru";
        distritos[49] = "Ventanilla";

        for (int i = 0; i < TOTAL_DISTRITOS; i++)
            for (int j = 0; j < TOTAL_DISTRITOS; j++)
                distancias[i][j] = 0;

        agregarArista(43, 49, 19.5);   // Callao - Ventanilla
        agregarArista(43, 44, 2.6);   // Callao - Bellavista
        agregarArista(43, 45, 4.1);   // Callao - Carmen de la Legua
        agregarArista(43, 46, 2.3);   // Callao - La Perla
        agregarArista(43, 47, 4.4);   // Callao - La Punta
        agregarArista(43, 34, 7.5);   // Callao - San Martin de Porres
        agregarArista(44, 46, 1.3);   // Bellavista - La Perla
        agregarArista(44, 45, 2.8);   // Bellavista - Carmen de la Legua
        agregarArista(44, 35, 2.7);   // Bellavista - San Miguel
        agregarArista(45, 14, 5.4);   // Carmen de la Legua - Lima
        agregarArista(45, 34, 4.2);   // Carmen de la Legua - San Martin de Porres
        agregarArista(46, 35, 3.4);   // La Perla - San Miguel
        agregarArista(49, 48, 2.4);   // Ventanilla - Mi Peru
        agregarArista(49, 24, 6.1);   // Ventanilla - Puente Piedra
        agregarArista(49, 38, 8.7);   // Ventanilla - Santa Rosa
        agregarArista(14, 27, 2.0);   // Lima - Rimac
        agregarArista(14, 34, 5.3);   // Lima - San Martin de Porres
        agregarArista(14, 3, 1.7);   // Lima - Brena
        agregarArista(14, 11, 3.5);   // Lima - Jesus Maria
        agregarArista(14, 23, 4.0);   // Lima - Pueblo Libre
        agregarArista(14, 35, 6.2);   // Lima - San Miguel
        agregarArista(14, 13, 3.7);   // Lima - La Victoria
        agregarArista(14, 9, 5.2);   // Lima - El Agustino
        agregarArista(14, 15, 4.4);   // Lima - Lince
        agregarArista(3, 11, 2.1);   // Brena - Jesus Maria
        agregarArista(3, 23, 2.4);   // Brena - Pueblo Libre
        agregarArista(11, 23, 1.6);   // Jesus Maria - Pueblo Libre
        agregarArista(11, 19, 2.7);   // Jesus Maria - Magdalena del Mar
        agregarArista(11, 30, 2.8);   // Jesus Maria - San Isidro
        agregarArista(11, 15, 1.9);   // Jesus Maria - Lince
        agregarArista(15, 30, 1.4);   // Lince - San Isidro
        agregarArista(15, 13, 2.3);   // Lince - La Victoria
        agregarArista(13, 30, 3.5);   // La Victoria - San Isidro
        agregarArista(13, 29, 4.3);   // La Victoria - San Borja
        agregarArista(13, 33, 2.8);   // La Victoria - San Luis
        agregarArista(13, 9, 3.9);   // La Victoria - El Agustino
        agregarArista(27, 34, 4.9);   // Rimac - San Martin de Porres
        agregarArista(27, 10, 4.7);   // Rimac - Independencia
        agregarArista(27, 31, 6.3);   // Rimac - San Juan de Lurigancho
        agregarArista(27, 9, 4.7);   // Rimac - El Agustino
        agregarArista(9, 33, 3.4);   // El Agustino - San Luis
        agregarArista(9, 1, 8.5);   // El Agustino - Ate
        agregarArista(9, 36, 2.7);   // El Agustino - Santa Anita
        agregarArista(9, 31, 6.7);   // El Agustino - San Juan de Lurigancho
        agregarArista(33, 1, 9.9);   // San Luis - Ate
        agregarArista(33, 29, 2.7);   // San Luis - San Borja
        agregarArista(23, 19, 1.9);   // Pueblo Libre - Magdalena del Mar
        agregarArista(23, 35, 2.7);   // Pueblo Libre - San Miguel
        agregarArista(19, 30, 3.9);   // Magdalena del Mar - San Isidro
        agregarArista(19, 35, 2.7);   // Magdalena del Mar - San Miguel
        agregarArista(30, 29, 4.4);   // San Isidro - San Borja
        agregarArista(30, 40, 3.2);   // San Isidro - Surquillo
        agregarArista(30, 20, 2.5);   // San Isidro - Miraflores
        agregarArista(20, 40, 2.3);   // Miraflores - Surquillo
        agregarArista(20, 39, 4.9);   // Miraflores - Santiago de Surco
        agregarArista(20, 2, 3.0);   // Miraflores - Barranco
        agregarArista(40, 29, 2.2);   // Surquillo - San Borja
        agregarArista(40, 39, 3.7);   // Surquillo - Santiago de Surco
        agregarArista(29, 39, 4.5);   // San Borja - Santiago de Surco
        agregarArista(2, 39, 3.3);   // Barranco - Santiago de Surco
        agregarArista(2, 6, 2.8);   // Barranco - Chorrillos
        agregarArista(39, 6, 4.3);   // Santiago de Surco - Chorrillos
        agregarArista(39, 32, 3.1);   // Santiago de Surco - San Juan de Miraflores
        agregarArista(39, 12, 8.6);   // Santiago de Surco - La Molina
        agregarArista(39, 1, 14.9);   // Santiago de Surco - Ate
        agregarArista(6, 32, 5.0);   // Chorrillos - San Juan de Miraflores
        agregarArista(6, 41, 9.3);   // Chorrillos - Villa El Salvador
        agregarArista(12, 1, 6.9);   // La Molina - Ate
        agregarArista(12, 7, 14.8);   // La Molina - Cieneguilla
        agregarArista(12, 21, 18.1);   // La Molina - Pachacamac
        agregarArista(12, 42, 8.3);   // La Molina - Villa Maria del Triunfo
        agregarArista(1, 36, 5.9);   // Ate - Santa Anita
        agregarArista(1, 5, 17.8);   // Ate - Chaclacayo
        agregarArista(1, 17, 25.9);   // Ate - Lurigancho
        agregarArista(1, 7, 15.5);   // Ate - Cieneguilla
        agregarArista(31, 17, 33.1);   // San Juan de Lurigancho - Lurigancho
        agregarArista(31, 10, 5.5);   // San Juan de Lurigancho - Independencia
        agregarArista(31, 8, 7.8);   // San Juan de Lurigancho - Comas
        agregarArista(17, 5, 8.4);   // Lurigancho - Chaclacayo
        agregarArista(17, 4, 36.9);   // Lurigancho - Carabayllo
        agregarArista(5, 7, 14.4);   // Chaclacayo - Cieneguilla
        agregarArista(7, 21, 15.9);   // Cieneguilla - Pachacamac
        agregarArista(10, 34, 3.5);   // Independencia - San Martin de Porres
        agregarArista(10, 16, 3.1);   // Independencia - Los Olivos
        agregarArista(10, 8, 6.1);   // Independencia - Comas
        agregarArista(34, 16, 4.5);   // San Martin de Porres - Los Olivos
        agregarArista(16, 8, 4.5);   // Los Olivos - Comas
        agregarArista(16, 24, 11.7);   // Los Olivos - Puente Piedra
        agregarArista(8, 24, 8.2);   // Comas - Puente Piedra
        agregarArista(8, 4, 8.6);   // Comas - Carabayllo
        agregarArista(4, 24, 4.9);   // Carabayllo - Puente Piedra
        agregarArista(24, 38, 11.9);   // Puente Piedra - Santa Rosa
        agregarArista(24, 0, 14.8);   // Puente Piedra - Ancon
        agregarArista(38, 0, 3.9);   // Santa Rosa - Ancon
        agregarArista(32, 41, 6.4);   // San Juan de Miraflores - Villa El Salvador
        agregarArista(32, 42, 3.8);   // San Juan de Miraflores - Villa Maria del Triunfo
        agregarArista(42, 41, 5.6);   // Villa Maria del Triunfo - Villa El Salvador
        agregarArista(42, 21, 11.3);   // Villa Maria del Triunfo - Pachacamac
        agregarArista(41, 18, 10.5);   // Villa El Salvador - Lurin
        agregarArista(41, 21, 9.0);   // Villa El Salvador - Pachacamac
        agregarArista(21, 18, 5.1);   // Pachacamac - Lurin
        agregarArista(21, 25, 12.3);   // Pachacamac - Punta Hermosa
        agregarArista(18, 25, 8.3);   // Lurin - Punta Hermosa
        agregarArista(25, 26, 4.7);   // Punta Hermosa - Punta Negra
        agregarArista(26, 28, 3.2);   // Punta Negra - San Bartolo
        agregarArista(28, 37, 2.3);   // San Bartolo - Santa Maria del Mar
        agregarArista(37, 22, 8.1);   // Santa Maria del Mar - Pucusana
    }

    int cantidad() const { return TOTAL_DISTRITOS; }

    std::string nombre(int indice) const {
        if (indice < 0 || indice >= TOTAL_DISTRITOS) return "";
        return distritos[indice];
    }

    // Devuelve el indice del distrito escrito por el usuario, o -1 si no existe.
    // No distingue mayusculas y acepta algunos nombres cortos (Surco, SJL, SMP...). O(V)
    int indiceDistrito(std::string texto) const {
        texto = aMinusculas(texto);
        if (texto == "surco") texto = "santiago de surco";
        else if (texto == "sjl") texto = "san juan de lurigancho";
        else if (texto == "sjm") texto = "san juan de miraflores";
        else if (texto == "smp") texto = "san martin de porres";
        else if (texto == "ves") texto = "villa el salvador";
        else if (texto == "vmt") texto = "villa maria del triunfo";
        else if (texto == "magdalena") texto = "magdalena del mar";
        else if (texto == "chosica") texto = "lurigancho";
        else if (texto == "cercado" || texto == "cercado de lima" || texto == "lima cercado") texto = "lima";
        else if (texto == "carmen de la legua reynoso") texto = "carmen de la legua";

        for (int i = 0; i < TOTAL_DISTRITOS; i++)
            if (aMinusculas(distritos[i]) == texto) return i;
        return -1;
    }

    // O(1): basta con mirar la celda de la matriz.
    bool colindan(int a, int b) const {
        return distancias[a][b] != 0;
    }

    // Distritos que colindan con el indicado. O(V): recorre su fila.
    Lista<int> vecinos(int indice) const {
        Lista<int> resultado;
        for (int j = 0; j < TOTAL_DISTRITOS; j++)
            if (distancias[indice][j] != 0) resultado.agregaFinal(j);
        return resultado;
    }

    // BFS (recorrido en anchura con una Cola): distritos a lo mas maxSaltos aristas
    // del origen, empezando por el mismo origen. O(V^2) con matriz de adyacencia.
    Lista<int> distritosASaltos(int origen, int maxSaltos) const {
        Lista<int> resultado;
        int saltos[TOTAL_DISTRITOS];
        for (int i = 0; i < TOTAL_DISTRITOS; i++) saltos[i] = -1;   // -1 = no visitado

        Cola<int> cola;
        saltos[origen] = 0;
        cola.encolar(origen);
        while (!cola.esVacia()) {
            int actual = cola.desencolar();
            resultado.agregaFinal(actual);
            if (saltos[actual] == maxSaltos) continue;
            for (int j = 0; j < TOTAL_DISTRITOS; j++) {
                if (distancias[actual][j] != 0 && saltos[j] == -1) {
                    saltos[j] = saltos[actual] + 1;
                    cola.encolar(j);
                }
            }
        }
        return resultado;
    }

    // Dijkstra: distancia minima en km desde el origen a cada distrito.
    // Deja el resultado en distanciaMinima[0..49].
    //   1. Todas las distancias empiezan en "infinito" y la del origen en 0.
    //   2. Se elige el distrito no visitado con menor distancia y se marca como visitado.
    //   3. Se revisan sus vecinos: si pasando por el se llega mas corto, se actualiza.
    //   4. Se repite hasta visitar todos.
    // O(V^2) con matriz de adyacencia (V = 50).
    void dijkstra(int origen, double distanciaMinima[]) const {
        bool visitado[TOTAL_DISTRITOS];
        for (int i = 0; i < TOTAL_DISTRITOS; i++) {
            distanciaMinima[i] = 1000000;   // "infinito"
            visitado[i] = false;
        }
        distanciaMinima[origen] = 0;

        for (int paso = 0; paso < TOTAL_DISTRITOS; paso++) {
            int actual = -1;
            for (int i = 0; i < TOTAL_DISTRITOS; i++)
                if (!visitado[i] && (actual == -1 || distanciaMinima[i] < distanciaMinima[actual]))
                    actual = i;
            if (actual == -1) return;
            visitado[actual] = true;

            for (int j = 0; j < TOTAL_DISTRITOS; j++) {
                if (distancias[actual][j] != 0 && !visitado[j]) {
                    double nueva = distanciaMinima[actual] + distancias[actual][j];
                    if (nueva < distanciaMinima[j]) distanciaMinima[j] = nueva;
                }
            }
        }
    }
};