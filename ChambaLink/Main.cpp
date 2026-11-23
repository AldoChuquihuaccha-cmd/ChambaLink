// ChambaLink - punto de entrada.
#include "RedProfesional.h"
#include "GestorArchivos.h"
#include "FuncionesInicio.h"
#include "FuncionesMenu.h"
#include "GeneradorDataset.h"
#include <iostream>
#include <limits>
#include <string>

static bool leerNumero(const char* texto, long long& valor) {
    try {
        std::string entrada(texto);
        std::size_t posicion = 0;
        valor = std::stoll(entrada, &posicion);
        return posicion == entrada.length();
    }
    catch (...) {
        return false;
    }
}

int main(int argc, char* argv[]) {
    // Modo de generacion: crea una red nueva y reemplaza los archivos de datos.
    // Uso: ChambaLink.exe --generar-dataset <usuarios> <empresas> <vacantes> <semilla>
    if (argc > 1 && std::string(argv[1]) == "--generar-dataset") {
        if (argc != 6) {
            std::cout << "Uso: ChambaLink.exe --generar-dataset <usuarios> <empresas> <vacantes> <semilla>\n";
            std::cout << "Ejemplo: ChambaLink.exe --generar-dataset 1000 50 300 20261122\n";
            return 1;
        }

        long long usuarios = 0, empresas = 0, vacantes = 0, semilla = 0;
        if (!leerNumero(argv[2], usuarios) || !leerNumero(argv[3], empresas) ||
            !leerNumero(argv[4], vacantes) || !leerNumero(argv[5], semilla) ||
            usuarios < 1 || empresas < 1 || vacantes < 0 ||
            usuarios > GeneradorDataset::MAX_REGISTROS ||
            empresas > GeneradorDataset::MAX_REGISTROS ||
            vacantes > GeneradorDataset::MAX_REGISTROS ||
            semilla < 0 || semilla >(std::numeric_limits<unsigned int>::max)()) {
            std::cout << "Parametros invalidos. Revise las cantidades y la semilla.\n";
            return 1;
        }

        RedProfesional red; // Red vacia para que la semilla genere un dataset reproducible.
        GeneradorDataset generador((unsigned int)semilla);

        std::cout << "Generando dataset...\n";
        if (!generador.generar(red, (int)usuarios, (int)empresas, (int)vacantes)) {
            std::cout << "No se pudo generar el dataset.\n";
            if (red.getUltimoError() != "")
                std::cout << "Detalle: " << red.getUltimoError() << "\n";
            return 1;
        }

        GestorArchivos::guardarTodo(red);
        std::cout << "Dataset generado y guardado correctamente.\n";
        std::cout << "Usuarios: " << usuarios << " | Empresas: " << empresas
            << " | Vacantes: " << vacantes << "\n";
        std::cout << "Semilla: " << semilla << "\n";
        std::cout << "Archivos TXT y chambalink.bin actualizados en la carpeta de trabajo.\n";
        std::cout << "Contrasena de todas las cuentas generadas: Demo1234\n";
        return 0;
    }

    configurarConsola();

    RedProfesional red;
    GestorArchivos::cargarTodo(red);   // siempre antes de registrar a nadie

    int idSesion = -1;
    bool esEmpresa = false;
    // mostrarLogin devuelve false cuando se presiona ESC en la pantalla inicial.
    while (mostrarLogin(red, idSesion, esEmpresa)) {
        mostrarMenu(red, idSesion, esEmpresa);   // Cerrar sesion vuelve aqui
        GestorArchivos::guardarTodo(red);
    }

    GestorArchivos::guardarTodo(red);
    return 0;
}