#pragma once
#include <string>

// Recomendacion que un usuario escribe sobre otro (estilo LinkedIn).
class Recomendacion {
private:
    int id;
    int idEmisor;           // id del Usuario que recomienda
    int idReceptor;         // id del Usuario recomendado
    std::string texto;
    std::string fecha;      // formato "dd/mm/aaaa"

public:
    Recomendacion();
    Recomendacion(int pId, int pIdEmisor, int pIdReceptor, std::string pTexto, std::string pFecha);

    int getId() const;
    int getIdEmisor() const;
    int getIdReceptor() const;
    std::string getTexto() const;
    std::string getFecha() const;

    void setTexto(std::string pTexto);

    // Comparadores; se usan como predicado lambda al buscar en Lista<Recomendacion>.
    bool mismoId(const Recomendacion& otra) const;
    bool esPara(int pIdReceptor) const;
};
Recomendacion::Recomendacion() : id(0), idEmisor(0), idReceptor(0), texto(""), fecha("") {}

Recomendacion::Recomendacion(int pId, int pIdEmisor, int pIdReceptor, std::string pTexto, std::string pFecha)
    : id(pId), idEmisor(pIdEmisor), idReceptor(pIdReceptor), texto(pTexto), fecha(pFecha) {
}

int Recomendacion::getId() const { return id; }
int Recomendacion::getIdEmisor() const { return idEmisor; }
int Recomendacion::getIdReceptor() const { return idReceptor; }
std::string Recomendacion::getTexto() const { return texto; }
std::string Recomendacion::getFecha() const { return fecha; }

void Recomendacion::setTexto(std::string pTexto) { texto = pTexto; }

bool Recomendacion::mismoId(const Recomendacion& otra) const { return id == otra.id; }
bool Recomendacion::esPara(int pIdReceptor) const { return idReceptor == pIdReceptor; }