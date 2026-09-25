// ChambaLink - punto de entrada.
// main solo arranca: configura la consola, carga los datos y muestra el inicio.
#include "Consola.h"
#include "RedProfesional.h"
#include "GestorArchivos.h"
#include "Login.h"

int main() {
    Consola::configurar();

    RedProfesional red;
    GestorArchivos::cargarTodo(red);   // siempre antes de registrar a nadie (ids desde el mayor guardado)

    Login login(red);
    // mostrar() devuelve false cuando se presiona ESC en la pantalla inicial.
    while (login.mostrar()) {
        login.mostrarBienvenida();     // ESC cierra la sesion y vuelve al inicio
    }

    GestorArchivos::guardarTodo(red);
    return 0;
}