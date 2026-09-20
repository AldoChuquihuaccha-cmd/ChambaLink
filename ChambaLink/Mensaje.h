#pragma once
#include <string>


class Mensaje {

private:

    int id;
    int idEmisor;
    int idReceptor;
    std::string contenido;
    std::string fecha;
    bool leido;


public:


    Mensaje()
        : id(0), idEmisor(0), idReceptor(0),
          contenido(""), fecha(""), leido(false) {}


    Mensaje(int pId, int pEmisor, int pReceptor,
            std::string pContenido, std::string pFecha)
        : id(pId), idEmisor(pEmisor),
          idReceptor(pReceptor),
          contenido(pContenido),
          fecha(pFecha),
          leido(false) {}



    int getId() const {
        return id;
    }


    int getIdEmisor() const {
        return idEmisor;
    }


    int getIdReceptor() const {
        return idReceptor;
    }


    std::string getContenido() const {
        return contenido;
    }


    std::string getFecha() const {
        return fecha;
    }


    bool fueLeido() const {
        return leido;
    }


    void marcarLeido() {
        leido = true;
    }


    bool perteneceA(int usuario) const {
        return idEmisor == usuario || idReceptor == usuario;
    }
};
