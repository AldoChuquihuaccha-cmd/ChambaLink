#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <windows.h>
#include <conio.h>
#include "RedProfesional.h"
#include "GestorArchivos.h"

using std::cout;
using std::string;

// Funciones de consola y pantallas de inicio de ChambaLink.

#define TECLA_IZQUIERDA 1001
#define TECLA_DERECHA   1002
#define TECLA_ARRIBA    1003
#define TECLA_ABAJO     1004
#define TECLA_ENTER     13
#define TECLA_ESC       27
#define TECLA_RETROCESO 8

// ======================= Consola =======================

void ubicar(int x, int y) {
    COORD posicion;
    posicion.X = (SHORT)x;
    posicion.Y = (SHORT)y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), posicion);
}

void colorBlanco() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void colorRojo() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
}

void limpiarPantalla() {
    system("cls");
}

// Se llama una sola vez al inicio del programa.
void configurarConsola() {
    system("mode con cols=120 lines=30");
    SetConsoleTitleA("ChambaLink");
    colorBlanco();
    limpiarPantalla();
}

// Espera una tecla y devuelve su codigo.
// Las flechas llegan como dos codigos seguidos (224 y luego la direccion).
int leerTecla() {
    int tecla = _getch();
    if (tecla == 0 || tecla == 224) {
        int direccion = _getch();
        if (direccion == 75) return TECLA_IZQUIERDA;
        if (direccion == 77) return TECLA_DERECHA;
        if (direccion == 72) return TECLA_ARRIBA;
        if (direccion == 80) return TECLA_ABAJO;
        return 0;   // otra tecla especial (F1, Supr...): se ignora
    }
    return tecla;
}

// ======================= Arte =======================

void dibujarChambalink() {
    ubicar(15, 2); cout << " ######  ##     ##    ###    ##     ## ########     ###    ##       #### ##    ## ##    ##";
    ubicar(15, 3); cout << "##    ## ##     ##   ## ##   ###   ### ##     ##   ## ##   ##        ##  ###   ## ##   ## ";
    ubicar(15, 4); cout << "##       ##     ##  ##   ##  #### #### ##     ##  ##   ##  ##        ##  ####  ## ##  ##  ";
    ubicar(15, 5); cout << "##       ######### ##     ## ## ### ## ########  ##     ## ##        ##  ## ## ## #####   ";
    ubicar(15, 6); cout << "##       ##     ## ######### ##     ## ##     ## ######### ##        ##  ##  #### ##  ##  ";
    ubicar(15, 7); cout << "##    ## ##     ## ##     ## ##     ## ##     ## ##     ## ##        ##  ##   ### ##   ## ";
    ubicar(15, 8); cout << " ######  ##     ## ##     ## ##     ## ########  ##     ## ######## #### ##    ## ##    ##";
}

// 5 filas x 50 columnas. Se usa en (7, 14) en la pantalla inicial y en (35, 1) como titulo.
void dibujarIndividuo(int x, int y) {
    ubicar(x, y);     cout << " ___ _   _ ____ _____     _____ ____  _   _  ___  ";
    ubicar(x, y + 1); cout << "|_ _| \\ | |  _ \\_ _\\ \\   / /_ _|  _ \\| | | |/ _ \\ ";
    ubicar(x, y + 2); cout << " | ||  \\| | | | | | \\ \\ / / | || | | | | | | | | |";
    ubicar(x, y + 3); cout << " | || |\\  | |_| | |  \\ V /  | || |_| | |_| | |_| |";
    ubicar(x, y + 4); cout << "|___|_| \\_|____/___|  \\_/  |___|____/ \\___/ \\___/ ";
}

// 5 filas x 45 columnas. Se usa en (70, 14) en la pantalla inicial y en (37, 1) como titulo.
void dibujarEmpresa(int x, int y) {
    ubicar(x, y);     cout << " _____ __  __ ____  ____  _____ ____    _    ";
    ubicar(x, y + 1); cout << "| ____|  \\/  |  _ \\|  _ \\| ____/ ___|  / \\   ";
    ubicar(x, y + 2); cout << "|  _| | |\\/| | |_) | |_) |  _| \\___ \\ / _ \\  ";
    ubicar(x, y + 3); cout << "| |___| |  | |  __/|  _ <| |___ ___) / ___ \\ ";
    ubicar(x, y + 4); cout << "|_____|_|  |_|_|   |_| \\_\\_____|____/_/   \\_\\";
}

// ======================= Pantallas de eleccion =======================

// Pantalla inicial: INDIVIDUO o EMPRESA con flechas izquierda/derecha.
// Devuelve false si se presiono ESC (salir del programa).
bool elegirTipo(bool& esEmpresa) {
    limpiarPantalla();
    dibujarChambalink();
    ubicar(50, 11); cout << "Como deseas ingresar";
    dibujarIndividuo(7, 14);
    dibujarEmpresa(70, 14);
    ubicar(44, 25); cout << "Presione ENTER para seleccionar";
    ubicar(48, 27); cout << "Presione ESC para salir";

    while (true) {
        // Se borra la flecha de un lado y se dibuja en el otro.
        ubicar(4, 16);  cout << (esEmpresa ? "  " : "->");
        ubicar(67, 16); cout << (esEmpresa ? "->" : "  ");

        int tecla = leerTecla();
        if (tecla == TECLA_IZQUIERDA) esEmpresa = false;
        else if (tecla == TECLA_DERECHA) esEmpresa = true;
        else if (tecla == TECLA_ENTER) return true;
        else if (tecla == TECLA_ESC) return false;
    }
}

// Menu Iniciar sesion / Registrarse con flechas arriba/abajo.
// opcion: 0 = Iniciar sesion, 1 = Registrarse. Devuelve false si se presiono ESC.
bool elegirAccion(bool esEmpresa, int& opcion) {
    limpiarPantalla();
    if (esEmpresa) dibujarEmpresa(37, 1);
    else dibujarIndividuo(35, 1);
    ubicar(54, 13); cout << "Iniciar sesion";
    ubicar(54, 15); cout << "Registrarse";
    ubicar(44, 25); cout << "Presione ENTER para seleccionar";
    ubicar(46, 27); cout << "Presione ESC para retroceder";

    while (true) {
        ubicar(51, 13); cout << (opcion == 0 ? "->" : "  ");
        ubicar(51, 15); cout << (opcion == 1 ? "->" : "  ");

        int tecla = leerTecla();
        if (tecla == TECLA_ARRIBA) opcion = 0;
        else if (tecla == TECLA_ABAJO) opcion = 1;
        else if (tecla == TECLA_ENTER) return true;
        else if (tecla == TECLA_ESC) return false;
    }
}

// ======================= Auxiliares de formulario =======================

// Parte comun de los formularios: titulo arriba y ayudas abajo.
// El subtitulo lo escribe cada formulario en su posicion.
void dibujarFormulario(bool esEmpresa) {
    limpiarPantalla();
    if (esEmpresa) dibujarEmpresa(37, 1);
    else dibujarIndividuo(35, 1);
    ubicar(38, 25); cout << "Presione ENTER para pasar al siguiente campo";
    ubicar(46, 27); cout << "Presione ESC para retroceder";
}

// Lee un campo tecla por tecla en la columna 53 de la fila y.
// No deja escribir mas de maximo caracteres.
// Devuelve true con ENTER y false con ESC.
bool leerCampo(int y, int maximo, string& valor) {
    valor = "";
    while (true) {
        ubicar(53 + (int)valor.length(), y);
        int tecla = leerTecla();

        if (tecla == TECLA_ENTER) return true;
        if (tecla == TECLA_ESC) return false;

        if (tecla == TECLA_RETROCESO && valor != "") {
            valor.pop_back();
            ubicar(53 + (int)valor.length(), y);
            cout << ' ';
        }
        // Solo letras, numeros y signos comunes (sin tildes ni enie).
        else if (tecla >= 32 && tecla <= 126 && (int)valor.length() < maximo) {
            cout << (char)tecla;
            valor += (char)tecla;
        }
    }
}

// Muestra el error en rojo debajo del formulario y vuelve a blanco.
// Devuelve true con ENTER (intentar de nuevo) y false con ESC (retroceder).
bool mostrarError(string mensaje) {
    colorRojo();
    ubicar(22, 23); cout << mensaje;
    colorBlanco();
    ubicar(22, 24); cout << "Presione ENTER para intentar de nuevo";
    while (true) {
        int tecla = leerTecla();
        if (tecla == TECLA_ENTER) return true;
        if (tecla == TECLA_ESC) return false;
    }
}

// Aviso de cuenta creada. Espera ENTER o ESC.
void mostrarExito(int id) {
    ubicar(22, 23); cout << "Cuenta creada con exito. Tu codigo es " << id;
    ubicar(22, 24); cout << "Presione ENTER para continuar";
    while (true) {
        int tecla = leerTecla();
        if (tecla == TECLA_ENTER || tecla == TECLA_ESC) return;
    }
}

// ======================= Formularios (usan red) =======================

// Devuelve true si la sesion se inicio (y deja el id en idSesion)
// o false si se presiono ESC.
bool formularioInicioSesion(RedProfesional& red, bool esEmpresa, int& idSesion) {
    string correo, contrasena;
    while (true) {
        dibujarFormulario(esEmpresa);
        ubicar(53, 8);  cout << "Iniciar sesion";
        ubicar(22, 11); cout << "Correo:";
        ubicar(22, 13); cout << "Contrasena:";

        if (!leerCampo(11, RedProfesional::MAX_CORREO, correo)) return false;
        if (!leerCampo(13, RedProfesional::MAX_CONTRASENA, contrasena)) return false;

        int id;
        if (esEmpresa) id = red.iniciarSesionEmpresa(correo, contrasena);
        else id = red.iniciarSesionUsuario(correo, contrasena);

        if (id != -1) {
            idSesion = id;
            return true;
        }
        if (!mostrarError(red.getUltimoError())) return false;
    }
}

// Al terminar (cuenta creada o ESC) se vuelve al menu Iniciar sesion / Registrarse.
void formularioRegistroIndividuo(RedProfesional& red) {
    string nombre, apellido, titular, distrito, correo, contrasena;
    while (true) {
        dibujarFormulario(false);
        ubicar(56, 8);  cout << "Registro";
        ubicar(40, 9);  cout << "El titular y el distrito son opcionales";
        ubicar(22, 11); cout << "Nombre:";
        ubicar(22, 13); cout << "Apellido:";
        ubicar(22, 15); cout << "Titular:";
        ubicar(22, 17); cout << "Distrito de residencia:";
        ubicar(22, 19); cout << "Correo:";
        ubicar(22, 21); cout << "Contrasena:";

        if (!leerCampo(11, RedProfesional::MAX_NOMBRE, nombre)) return;
        if (!leerCampo(13, RedProfesional::MAX_APELLIDO, apellido)) return;
        if (!leerCampo(15, RedProfesional::MAX_TITULAR, titular)) return;
        if (!leerCampo(17, RedProfesional::MAX_UBICACION, distrito)) return;
        if (!leerCampo(19, RedProfesional::MAX_CORREO, correo)) return;
        if (!leerCampo(21, RedProfesional::MAX_CONTRASENA, contrasena)) return;

        int id = red.registrarUsuario(nombre, apellido, titular, distrito, correo, contrasena);
        if (id != -1) {
            GestorArchivos::guardarTodo(red);   // se guarda enseguida
            mostrarExito(id);
            return;
        }
        if (!mostrarError(red.getUltimoError())) return;
    }
}

void formularioRegistroEmpresa(RedProfesional& red) {
    string nombre, sector, distrito, correo, contrasena;
    while (true) {
        dibujarFormulario(true);
        ubicar(56, 8);  cout << "Registro";
        ubicar(41, 9);  cout << "El sector y el distrito son opcionales";
        ubicar(22, 11); cout << "Nombre de la empresa:";
        ubicar(22, 13); cout << "Sector:";
        ubicar(22, 15); cout << "Distrito del establecimiento:";
        ubicar(22, 17); cout << "Correo:";
        ubicar(22, 19); cout << "Contrasena:";

        if (!leerCampo(11, RedProfesional::MAX_NOMBRE_EMPRESA, nombre)) return;
        if (!leerCampo(13, RedProfesional::MAX_SECTOR, sector)) return;
        if (!leerCampo(15, RedProfesional::MAX_UBICACION, distrito)) return;
        if (!leerCampo(17, RedProfesional::MAX_CORREO, correo)) return;
        if (!leerCampo(19, RedProfesional::MAX_CONTRASENA, contrasena)) return;

        int id = red.registrarEmpresa(nombre, sector, distrito, correo, contrasena);
        if (id != -1) {
            GestorArchivos::guardarTodo(red);
            mostrarExito(id);
            return;
        }
        if (!mostrarError(red.getUltimoError())) return;
    }
}

// ======================= Flujo del login =======================

// Repite Iniciar sesion / Registrarse hasta que alguien entre (true)
// o se presione ESC para volver a la pantalla inicial (false).
bool menuCuenta(RedProfesional& red, bool esEmpresa, int& idSesion) {
    int opcion = 0;
    while (true) {
        if (!elegirAccion(esEmpresa, opcion)) return false;

        if (opcion == 0) {
            if (formularioInicioSesion(red, esEmpresa, idSesion)) return true;
        }
        else if (esEmpresa) formularioRegistroEmpresa(red);
        else formularioRegistroIndividuo(red);
    }
}

// Punto de entrada del login.
// Devuelve true si alguien inicio sesion: deja en idSesion su id y en
// esEmpresa si es una empresa. Devuelve false si se presiono ESC al inicio.
bool mostrarLogin(RedProfesional& red, int& idSesion, bool& esEmpresa) {
    esEmpresa = false;
    while (true) {
        if (!elegirTipo(esEmpresa)) return false;
        if (menuCuenta(red, esEmpresa, idSesion)) return true;
    }
}