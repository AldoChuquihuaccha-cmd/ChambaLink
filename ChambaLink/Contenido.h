#pragma once
#include <string>

// Clase base abstracta de todo lo que un usuario escribe en la red.
// Publicacion, Comentario, Recomendacion y Mensaje heredan de aqui porque
// todos tienen autor, texto y fecha.
// tipo() es virtual pura: cada clase hija dice que es.
// resumen() es virtual: las hijas la sobrescriben para agregar sus datos.
// Las fechas van en formato "aaaa/mm/dd" para que el orden alfabetico
// coincida con el orden cronologico.
class Contenido {
private:
    int id;
    int idAutor;
    std::string texto;
    std::string fecha;

protected:
    // Protegido porque solo las clases hijas construyen un Contenido.
    Contenido(int pId, int pIdAutor, std::string pTexto, std::string pFecha) {
        id = pId;
        idAutor = pIdAutor;
        texto = pTexto;
        fecha = pFecha;
    }

public:
    // Virtual para que al destruir por un puntero a Contenido se llame
    // al destructor de la clase hija.
    virtual ~Contenido() {}

    int getId() const { return id; }
    int getIdAutor() const { return idAutor; }
    std::string getTexto() const { return texto; }
    std::string getFecha() const { return fecha; }

    void setTexto(std::string pTexto) { texto = pTexto; }

    bool esDeAutor(int pIdAutor) const { return idAutor == pIdAutor; }

    virtual std::string tipo() const = 0;

    virtual std::string resumen() const {
        return "[" + tipo() + "] " + fecha + " - " + texto;
    }
};
