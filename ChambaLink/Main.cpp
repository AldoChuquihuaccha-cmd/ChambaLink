// ChambaLink - punto de entrada.
#include "RedProfesional.h"
#include "GestorArchivos.h"
#include "FuncionesInicio.h"
#include "FuncionesMenu.h"

int main() {
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