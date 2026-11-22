#pragma once
#include <iostream>
#include "HashEntidad.h"

class HashTable
{
private:
	HashEntidad** tabla;
	int numElementos;
	int TABLE_SIZE;

	/* Funcion hash propia: procesa cada caracter del correo (polinomial base 31) */
	int hash1(string key)
	{
		int hash = 0;
		for (int i = 0; i < key.length(); i++)
		{
			hash = (hash * 31 + key[i]) % TABLE_SIZE;
		}
		if (hash < 0) hash += TABLE_SIZE;
		return hash;
	}

	/* Duplica el tamanio de la tabla y vuelve a insertar todo cuando pasa el 70% */
	void Redimensionar()
	{
		HashEntidad** anterior = tabla;
		int tamanioAnterior = TABLE_SIZE;

		TABLE_SIZE = TABLE_SIZE * 2 + 1;
		tabla = new HashEntidad * [TABLE_SIZE];
		for (int i = 0; i < TABLE_SIZE; i++)
		{
			tabla[i] = nullptr;
		}
		numElementos = 0;

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
	HashTable(int TABLE_SIZE = 128)
	{
		this->TABLE_SIZE = TABLE_SIZE;
		tabla = new HashEntidad * [TABLE_SIZE];
		for (int i = 0; i < TABLE_SIZE; i++)
		{
			tabla[i] = nullptr;
		}
		numElementos = 0;
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

	/* Hashing abierto (busca otra posicion libre en el arreglo) */
	void Insertar(string key, int value)
	{
		int base, step, hash;
		if (numElementos * 10 >= TABLE_SIZE * 7) Redimensionar();
		//validar si la tabla esta llena
		if (numElementos == TABLE_SIZE) return;

		//Funcion Hash 1
		base = hash1(key);
		hash = base;

		//Constante para Hash 2
		step = 0;
		while (tabla[hash] != nullptr)
		{
			//Función Hash 2
			hash = (base + step) % TABLE_SIZE;
			step++;
		}
		//Almacenamos en la tabla
		tabla[hash] = new HashEntidad(key, value);
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

	int buscar(string key)
	{
		int step = 0;
		int i, base;
		//hash1 = hash2 cuando step=0
		i = base = hash1(key);
		while (true)
		{
			if (tabla[i] == nullptr) return -1;
			else if (tabla[i]->getKey() == key)
				return i;
			else
				step++;
			i = (base + step) % TABLE_SIZE;
		}
	}

	int obtenerValor(int posicion)
	{
		return tabla[posicion]->getValue();
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
	}
};