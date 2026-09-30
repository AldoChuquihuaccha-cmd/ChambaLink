// ChambaLink - punto de entrada.
#include "RedProfesional.h"
#include "GestorArchivos.h"
#include "FuncionesConsola.h"
#include "MenuPrincipal.h"

int main() {
    configurarConsola();

    RedProfesional red;
    GestorArchivos::cargarTodo(red);   // siempre antes de registrar a nadie

    int idSesion;
    bool esEmpresa;
    // mostrarLogin devuelve false cuando se presiona ESC en la pantalla inicial.
    while (mostrarLogin(red, idSesion, esEmpresa)) {
        limpiarPantalla();
        MenuPrincipal menu(red, idSesion, esEmpresa);
        menu.mostrar();
        GestorArchivos::guardarTodo(red);
    }

    GestorArchivos::guardarTodo(red);
    return 0;
}