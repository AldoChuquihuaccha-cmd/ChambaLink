#pragma once
#include <string>
using namespace std;

// Casilla de la tabla hash: guarda la clave (texto) y su valor (tipo generico T).
template <class T>
class HashEntidad
{
private:
	string key;
	T value;
public:
	HashEntidad(string key, T value)
	{
		this->key = key;
		this->value = value;
	}
	string getKey() { return key; }
	T getValue() { return value; }
	void setValue(T value) { this->value = value; }
};
