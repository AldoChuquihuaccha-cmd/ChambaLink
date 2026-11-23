#pragma once
#include <iostream>
#include "HashEntidad.h"

// Tabla hash generica: clave de texto (por ejemplo, el correo) -> valor de tipo T.
// Usa hashing doble: hash1 da la posicion inicial y hash2 el salto cuando hay colision.
// El tamanio de la tabla siempre es primo para que el salto pueda recorrer todas las casillas.
template <class T>
class HashTable
{
private:
	HashEntidad<T>** tabla;
	int numElementos;
	int TABLE_SIZE;
	int colisiones;

	bool esPrimo(int n)
	{
		if (n < 2) return false;
		for (int i = 2; i * i <= n; i++)
		{
			if (n % i == 0) return false;
		}
		return true;
	}

	int siguientePrimo(int n)
	{
		while (!esPrimo(n)) n++;
		return n;
	}

	/* Funcion hash propia. Recorre cada caracter de la clave:
	   1. polinomial base 31: el orden de las letras importa ("ab" != "ba")
	   2. multiplica la letra por su posicion: correos parecidos (persona000001, persona000002)
	      quedan mas separados
	   3. XOR con desplazamiento: mezcla los bits altos con los bajos antes del modulo
	   Se usa unsigned long long para que el numero nunca sea negativo. O(L), L = largo de la clave */
	unsigned long long codigo(string key)
	{
		unsigned long long h = 7;
		for (int i = 0; i < (int)key.length(); i++)
		{
			h = h * 31 + (unsigned char)key[i] * (i + 1);
			h = h ^ (h >> 15);
		}
		return h;
	}

	/* Funcion hash 1: posicion inicial */
	int hash1(string key)
	{
		return (int)(codigo(key) % TABLE_SIZE);
	}

	/* Funcion hash 2: tamanio del salto, entre 1 y TABLE_SIZE - 1 (nunca 0) */
	int hash2(string key)
	{
		return 1 + (int)(codigo(key) % (TABLE_SIZE - 1));
	}

	/* Pasa al siguiente primo mayor al doble y vuelve a insertar todo. O(n) */
	void Redimensionar()
	{
		HashEntidad<T>** anterior = tabla;
		int tamanioAnterior = TABLE_SIZE;

		TABLE_SIZE = siguientePrimo(TABLE_SIZE * 2);
		tabla = new HashEntidad<T> * [TABLE_SIZE];
		for (int i = 0; i < TABLE_SIZE; i++)
		{
			tabla[i] = nullptr;
		}
		numElementos = 0;
		colisiones = 0;

		for (int i = 0; i < tamanioAnterior; i++)
		{
			if (anterior[i] != nullptr)
			{
				Insertar(anterior[i]->getKey(), anterior[i]->getValue());
				delete anterior[i];
			}
		}
		delete[] anterior;
	}

public:
	HashTable(int tamanio = 131)
	{
		TABLE_SIZE = siguientePrimo(tamanio);
		tabla = new HashEntidad<T> * [TABLE_SIZE];
		for (int i = 0; i < TABLE_SIZE; i++)
		{
			tabla[i] = nullptr;
		}
		numElementos = 0;
		colisiones = 0;
	}

	~HashTable()
	{
		for (int i = 0; i < TABLE_SIZE; i++)
		{
			if (tabla[i] != nullptr)
				delete tabla[i];
		}
		delete[] tabla;
	}

	/* Si la clave ya existe, solo actualiza su valor. Si la tabla pasa del 70%, crece.
	   O(1) en promedio; O(n) cuando toca redimensionar */
	void Insertar(string key, T value)
	{
		if ((numElementos + 1) * 10 > TABLE_SIZE * 7) Redimensionar();

		int posicion = hash1(key);
		int salto = hash2(key);
		while (tabla[posicion] != nullptr)
		{
			if (tabla[posicion]->getKey() == key)
			{
				tabla[posicion]->setValue(value);
				return;
			}
			posicion = (posicion + salto) % TABLE_SIZE;
			colisiones++;
		}
		tabla[posicion] = new HashEntidad<T>(key, value);
		numElementos++;
	}

	int size()
	{
		return TABLE_SIZE;
	}

	int sizeActual()
	{
		return numElementos;
	}

	/* Devuelve la posicion de la clave o -1 si no esta. Sigue el mismo camino que Insertar.
	   O(1) en promedio */
	int buscar(string key)
	{
		int posicion = hash1(key);
		int salto = hash2(key);
		while (tabla[posicion] != nullptr)
		{
			if (tabla[posicion]->getKey() == key) return posicion;
			posicion = (posicion + salto) % TABLE_SIZE;
		}
		return -1;
	}

	T obtenerValor(int posicion)
	{
		return tabla[posicion]->getValue();
	}

	/* Porcentaje de casillas ocupadas */
	double factorCarga()
	{
		return numElementos * 100.0 / TABLE_SIZE;
	}

	/* Cuantas veces se tuvo que saltar a otra casilla al insertar */
	int getColisiones()
	{
		return colisiones;
	}

	/* Borra todas las entradas (se usa al reconstruir los indices) */
	void vaciar()
	{
		for (int i = 0; i < TABLE_SIZE; i++)
		{
			if (tabla[i] != nullptr)
			{
				delete tabla[i];
				tabla[i] = nullptr;
			}
		}
		numElementos = 0;
		colisiones = 0;
	}
};
