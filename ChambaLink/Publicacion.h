#pragma once
#include <string>

// Publicacion que un usuario comparte en el feed de la red.
class Publicacion {
private:
    int id;
    int idAutor;            // id del Usuario que publica
    std::string contenido;
    std::string fecha;      // formato "dd/mm/aaaa"
    int meGusta;

public:
    Publicacion();
    Publicacion(int pId, int pIdAutor, std::string pContenido, std::string pFecha);

    int getId() const;
    int getIdAutor() const;
    std::string getContenido() const;
    std::string getFecha() const;
    int getMeGusta() const;

    void setContenido(std::string pContenido);

    void darMeGusta();
    void quitarMeGusta();

    // Comparadores; se usan como predicado lambda al buscar en Lista<Publicacion>.
    bool mismoId(const Publicacion& otra) const;
    bool esDeAutor(int pIdAutor) const;
};


Publicacion::Publicacion() : id(0), idAutor(0), contenido(""), fecha(""), meGusta(0) {}

Publicacion::Publicacion(int pId, int pIdAutor, std::string pContenido, std::string pFecha)
    : id(pId), idAutor(pIdAutor), contenido(pContenido), fecha(pFecha), meGusta(0) {
}

int Publicacion::getId() const { return id; }
int Publicacion::getIdAutor() const { return idAutor; }
std::string Publicacion::getContenido() const { return contenido; }
std::string Publicacion::getFecha() const { return fecha; }
int Publicacion::getMeGusta() const { return meGusta; }

void Publicacion::setContenido(std::string pContenido) { contenido = pContenido; }

void Publicacion::darMeGusta() { meGusta++; }

void Publicacion::quitarMeGusta() {
    if (meGusta > 0) meGusta--;
}

bool Publicacion::mismoId(const Publicacion& otra) const { return id == otra.id; }
bool Publicacion::esDeAutor(int pIdAutor) const { return idAutor == pIdAutor; }