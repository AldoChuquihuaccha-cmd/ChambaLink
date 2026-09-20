#pragma once
#include <string>

// Comentario que un usuario deja en una Publicacion.
class Comentario {
private:
    int id;
    int idPublicacion;      // id de la Publicacion comentada
    int idAutor;            // id del Usuario que comenta
    std::string texto;
    std::string fecha;      // formato "dd/mm/aaaa"

public:
    Comentario();
    Comentario(int pId, int pIdPublicacion, int pIdAutor, std::string pTexto, std::string pFecha);

    int getId() const;
    int getIdPublicacion() const;
    int getIdAutor() const;
    std::string getTexto() const;
    std::string getFecha() const;

    void setTexto(std::string pTexto);

    // Comparadores; se usan como predicado lambda al buscar en Lista<Comentario>.
    bool mismoId(const Comentario& otro) const;
    bool esDePublicacion(int pIdPublicacion) const;
};

Comentario::Comentario() : id(0), idPublicacion(0), idAutor(0), texto(""), fecha("") {}

Comentario::Comentario(int pId, int pIdPublicacion, int pIdAutor, std::string pTexto, std::string pFecha)
    : id(pId), idPublicacion(pIdPublicacion), idAutor(pIdAutor), texto(pTexto), fecha(pFecha) {
}

int Comentario::getId() const { return id; }
int Comentario::getIdPublicacion() const { return idPublicacion; }
int Comentario::getIdAutor() const { return idAutor; }
std::string Comentario::getTexto() const { return texto; }
std::string Comentario::getFecha() const { return fecha; }

void Comentario::setTexto(std::string pTexto) { texto = pTexto; }

bool Comentario::mismoId(const Comentario& otro) const { return id == otro.id; }
bool Comentario::esDePublicacion(int pIdPublicacion) const { return idPublicacion == pIdPublicacion; }