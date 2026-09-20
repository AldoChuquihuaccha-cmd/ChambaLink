#pragma once
#include <string>

// Empresa que publica vacantes y aparece en el perfil laboral de los usuarios.
class Empresa {
private:
    int id;
    std::string nombre;
    std::string sector;     // por ejemplo "Tecnologia", "Finanzas"
    std::string ubicacion;  // ciudad o pais

public:
    Empresa();
    Empresa(int pId, std::string pNombre, std::string pSector, std::string pUbicacion);

    int getId() const;
    std::string getNombre() const;
    std::string getSector() const;
    std::string getUbicacion() const;

    void setSector(std::string pSector);
    void setUbicacion(std::string pUbicacion);

    // Comparadores; se usan como predicado lambda al buscar en Lista<Empresa>.
    bool mismoId(const Empresa& otra) const;
    bool mismoNombre(const Empresa& otra) const;
};
Empresa::Empresa() : id(0), nombre(""), sector(""), ubicacion("") {}

Empresa::Empresa(int pId, std::string pNombre, std::string pSector, std::string pUbicacion)
    : id(pId), nombre(pNombre), sector(pSector), ubicacion(pUbicacion) {
}

int Empresa::getId() const { return id; }
std::string Empresa::getNombre() const { return nombre; }
std::string Empresa::getSector() const { return sector; }
std::string Empresa::getUbicacion() const { return ubicacion; }

void Empresa::setSector(std::string pSector) { sector = pSector; }
void Empresa::setUbicacion(std::string pUbicacion) { ubicacion = pUbicacion; }

bool Empresa::mismoId(const Empresa& otra) const { return id == otra.id; }
bool Empresa::mismoNombre(const Empresa& otra) const { return nombre == otra.nombre; }