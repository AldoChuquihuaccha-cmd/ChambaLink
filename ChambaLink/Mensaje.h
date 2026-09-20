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
    Mensaje();

    Mensaje(int pId, int pEmisor, int pReceptor,
            std::string pContenido, std::string pFecha);

    int getId() const;
    int getIdEmisor() const;
    int getIdReceptor() const;

    std::string getContenido() const;
    std::string getFecha() const;

    bool fueLeido() const;

    void marcarLeido();

    bool perteneceA(int usuario) const;
};
