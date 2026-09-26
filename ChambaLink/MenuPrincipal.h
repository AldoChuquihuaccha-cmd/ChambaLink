#pragma once
#include <iostream>
#include "RedProfesional.h"
#include "GestorArchivos.h"

class MenuPrincipal
{
private:
    RedProfesional& red;
    int idSesion;
    bool empresa;
    void mostrarError()
    {
        std::cout << "Error: "
            << red.getUltimoError()
            << std::endl;
    }
    void menuEmpresa()
    {
        int op;
        do {
            std::cout << "\n===== EMPRESA =====\n1.Publicar vacante\n2.Revisar postulacion\n3.Cerrar vacante\n4. Ordenar vacantes\n0.Salir\nOpcion: ";
            std::cin >> op;

            if (op == 1)
            {
                std::string t, d, r, m;
                std::cin.ignore();
                std::cout << "Titulo: "; getline(std::cin, t);
                std::cout << "Descripcion: "; getline(std::cin, d);
                std::cout << "Requisitos: "; getline(std::cin, r);
                std::cout << "Modalidad: "; getline(std::cin, m);
                if (red.publicarVacante(idSesion, t, d, r, m) == -1)
                    mostrarError();
                else
                    std::cout << "Vacante publicada correctamente\n";
            }
            else if (op == 2)
            {
                int id; char resp;
                std::cout << "ID Vacante: ";
                std::cin >> id;
                std::cout << "Aceptar? s/n: ";
                std::cin >> resp;
                red.revisarSiguientePostulacion(id, resp == 's');
            }
            else if (op == 3)
            {
                int id;

                std::cout << "ID Vacante: ";
                std::cin >> id;

                if (red.cerrarVacante(id))
                    std::cout << "Vacante cerrada correctamente\n";
                else
                    mostrarError();
            }
            else if (op == 4)
            {
                red.ordenarVacantesHeap();

                red.paraCadaVacanteActiva([](const Vacante& v)
                    {
                        std::cout << "\nID: "
                            << v.getId()
                            << " - "
                            << v.getTitulo();
                    });
            }

        } while (op != 0);
    }


    void menuUsuario()
    {
        int op;
        do {
            std::cout << "\n===== USUARIO =====\n1.Ver vacantes\n2.Postular\n3.Compatibilidad\n4.Mis postulaciones\n0.Salir\nOpcion: ";
            std::cin >> op;

            if (op == 1)
            {
                bool hay = false;
                red.paraCadaVacanteActiva([&hay](const Vacante& v) {
                    hay = true;
                    std::cout << "\nID: " << v.getId() << " - " << v.getTitulo();
                    });
                if (!hay) std::cout << "\nNo hay vacantes disponibles\n";
}
            else if (op == 2)
            {
                int id;
                std::cout << "ID Vacante: ";
                std::cin >> id;
                if (red.postular(idSesion, id) == -1)
                    mostrarError();
                else
                    std::cout << "Postulacion realizada correctamente\n";
            }
            else if (op == 3)
            {
                int id;
                std::cout << "ID Vacante: ";
                std::cin >> id;
                std::cout << "Compatibilidad: "
                    << red.calcularCompatibilidad(idSesion, id) << "%\n";
            }
            else if (op == 4)
            {
                red.paraCadaPostulacionDe(idSesion, [](const Postulacion& p) {
                    std::cout << "\nPostulacion ID:" << p.getId();
                    });
            }

        } while (op != 0);
    }


public:
    MenuPrincipal(RedProfesional& r, int id, bool e) :red(r), idSesion(id), empresa(e) {}

    void mostrar()
    {
        int op;

        do
        {
            std::cout << "\n===== MENU PRINCIPAL =====\n";
            std::cout << "1. Modulo actual\n";
            std::cout << "2. Guardar datos\n";
            std::cout << "3. Cargar datos\n";
            std::cout << "0. Salir\n";
            std::cout << "Opcion: ";

            std::cin >> op;

            if (op == 1)
            {
                if (empresa)
                    menuEmpresa();
                else
                    menuUsuario();
            }
            else if (op == 2)
            {
                if (GestorArchivos::guardarTodo(red))
                    std::cout << "Datos guardados correctamente\n";
                else
                    std::cout << "Error al guardar datos\n";
            }
            else if (op == 3)
            {
                if (GestorArchivos::cargarTodo(red))
                    std::cout << "Datos cargados correctamente\n";
                else
                    std::cout << "No se encontraron datos\n";
            }

        } while (op != 0);

        GestorArchivos::guardarTodo(red);
    }
};