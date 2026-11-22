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
// La consola es fija de 30 filas x 120 columnas: todas las posiciones son fijas.
// La letra se agranda en configurarConsola para que la ventana se vea mas grande.
//   ubicar(x, y): x = columna (0 a 119), y = fila (0 a 29)
//
// Cada funcion esta escrita antes de la que la usa.

// ======================= Teclas =======================
// Las flechas usan numeros mayores a 1000 para no confundirse con letras.
#define TECLA_IZQUIERDA 1001
#define TECLA_DERECHA   1002
#define TECLA_ARRIBA    1003
#define TECLA_ABAJO     1004
#define TECLA_ENTER     13
#define TECLA_ESC       27
#define TECLA_RETROCESO 8

// ======================= Colores =======================
// Codigos ANSI de color de fondo: \x1b[48;2;R;G;Bm (cada valor de 0 a 255).
// COLOR_NORMAL vuelve a los colores de siempre (sirve para borrar una celda pintada).
#define COLOR_TERRACOTA "\x1b[48;2;196;98;62m"
#define COLOR_ARENA     "\x1b[48;2;232;220;196m"
#define COLOR_MARRON    "\x1b[48;2;84;76;68m"
#define COLOR_NORMAL    "\x1b[0m"

// Codigos ANSI de color de LETRA: \x1b[38;2;R;G;Bm (el fondo no cambia).
#define TEXTO_TERRACOTA "\x1b[38;2;196;98;62m"
#define TEXTO_ARENA     "\x1b[38;2;232;220;196m"
#define TEXTO_GRIS      "\x1b[38;2;140;140;140m"
#define TEXTO_OSCURO    "\x1b[38;2;30;26;22m"

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

// Alto de la letra en pixeles (el normal es 16). La consola sigue teniendo
// 120 x 30 caracteres, pero con letra mas grande la ventana se ve mas amplia.
// Si la ventana no entra en la pantalla, bajar este valor a 18.
const int TAMANIO_LETRA = 20;

// Se llama una sola vez al inicio del programa.
// Primero se cambia la letra y despues el tamanio, asi la ventana se ajusta a la letra nueva.
void configurarConsola() {
    CONSOLE_FONT_INFOEX fuente;
    fuente.cbSize = sizeof(fuente);
    GetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &fuente);
    fuente.dwFontSize.X = 0;
    fuente.dwFontSize.Y = TAMANIO_LETRA;
    fuente.FontWeight = FW_NORMAL;
    wcscpy_s(fuente.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &fuente);

    system("mode con cols=120 lines=30");
    SetConsoleTitleA("ChambaLink");

    // Activa los codigos ANSI (\x1b[...) para pintar con colores RGB.
    DWORD modo;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &modo);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
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

// ======================= Pantalla inicial pintada =======================
// Cada numero de la matriz es una zona de la pantalla:
//   0 = nada, 1 = terracota ("Link" y un eslabon), 2 = arena ("Chamba" y el otro eslabon),
//   3 = PERSONA, 4 = EMPRESA,
//   5 = flecha de PERSONA, 6 = flecha de EMPRESA
int pantallaInicio[30][120] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,2,2,0,0,0,0,0,0,2,2,0,0,0,2,2,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,2,0,0,0,0,2,2,2,0,0,0,0,0,2,2,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,2,2,2,1,1,0,0,2,2,2,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,2,2,2,2,2,2,0,0,0,2,2,2,2,2,0,0,2,2,2,2,2,2,2,2,0,0,0,2,2,2,2,2,2,0,0,0,2,2,2,2,2,0,0,1,1,0,0,0,0,0,0,1,1,0,1,1,1,1,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,2,2,2,0,1,1,1,2,2,2,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,2,2,0,0,0,2,2,0,0,0,0,0,0,2,2,0,2,2,0,0,2,2,0,0,2,2,0,2,2,0,0,0,2,2,0,0,0,0,0,0,2,2,0,1,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,1,1,0,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,2,2,1,1,1,2,2,2,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,2,2,0,0,0,2,2,0,0,2,2,2,2,2,2,0,2,2,0,0,2,2,0,0,2,2,0,2,2,0,0,0,2,2,0,0,2,2,2,2,2,2,0,1,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,1,1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,2,2,0,0,0,2,2,0,2,2,0,0,0,2,2,0,2,2,0,0,2,2,0,0,2,2,0,2,2,0,0,0,2,2,0,2,2,0,0,0,2,2,0,1,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,1,1,0,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,2,2,0,2,2,0,0,0,2,2,0,2,2,0,0,0,2,2,0,2,2,0,0,2,2,0,0,2,2,0,2,2,0,0,0,2,2,0,2,2,0,0,0,2,2,0,1,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,1,1,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,2,2,0,0,0,2,2,0,0,2,2,2,2,2,2,0,2,2,0,0,2,2,0,0,2,2,0,2,2,2,2,2,2,0,0,0,2,2,2,2,2,2,0,1,1,1,1,1,1,1,0,1,1,0,1,1,0,0,0,1,1,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,5,5,0,0,0,0,0,0,0,0,3,3,3,3,0,0,3,3,3,3,3,0,3,3,3,3,0,0,0,3,3,3,3,0,0,3,3,3,0,0,3,0,0,0,3,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,4,4,4,4,4,0,4,0,0,0,4,0,4,4,4,4,0,0,4,4,4,4,0,0,4,4,4,4,4,0,0,4,4,4,4,0,0,4,4,4,0,0,0,0,0},
    {0,0,0,0,0,5,5,5,5,0,0,0,0,0,0,3,0,0,0,3,0,3,0,0,0,0,0,3,0,0,0,3,0,3,0,0,0,0,0,3,0,0,0,3,0,3,3,0,0,3,0,3,0,0,0,3,0,0,0,0,0,0,0,0,0,6,6,6,6,0,0,0,0,0,0,4,0,0,0,0,0,4,4,0,4,4,0,4,0,0,0,4,0,4,0,0,0,4,0,4,0,0,0,0,0,4,0,0,0,0,0,4,0,0,0,4,0,0,0,0},
    {0,0,0,0,0,5,5,5,5,5,5,0,0,0,0,3,0,0,0,3,0,3,0,0,0,0,0,3,0,0,0,3,0,3,0,0,0,0,0,3,0,0,0,3,0,3,0,3,0,3,0,3,0,0,0,3,0,0,0,0,0,0,0,0,0,6,6,6,6,6,6,0,0,0,0,4,0,0,0,0,0,4,0,4,0,4,0,4,0,0,0,4,0,4,0,0,0,4,0,4,0,0,0,0,0,4,0,0,0,0,0,4,0,0,0,4,0,0,0,0},
    {0,0,0,0,0,5,5,5,5,5,5,5,5,0,0,3,3,3,3,0,0,3,3,3,3,0,0,3,3,3,3,0,0,0,3,3,3,0,0,3,0,0,0,3,0,3,0,3,0,3,0,3,3,3,3,3,0,0,0,0,0,0,0,0,0,6,6,6,6,6,6,6,6,0,0,4,4,4,4,0,0,4,0,0,0,4,0,4,4,4,4,0,0,4,4,4,4,0,0,4,4,4,4,0,0,0,4,4,4,0,0,4,4,4,4,4,0,0,0,0},
    {0,0,0,0,0,5,5,5,5,5,5,0,0,0,0,3,0,0,0,0,0,3,0,0,0,0,0,3,0,0,3,0,0,0,0,0,0,3,0,3,0,0,0,3,0,3,0,0,3,3,0,3,0,0,0,3,0,0,0,0,0,0,0,0,0,6,6,6,6,6,6,0,0,0,0,4,0,0,0,0,0,4,0,0,0,4,0,4,0,0,0,0,0,4,0,0,4,0,0,4,0,0,0,0,0,0,0,0,0,4,0,4,0,0,0,4,0,0,0,0},
    {0,0,0,0,0,5,5,5,5,0,0,0,0,0,0,3,0,0,0,0,0,3,0,0,0,0,0,3,0,0,0,3,0,0,0,0,0,3,0,3,0,0,0,3,0,3,0,0,0,3,0,3,0,0,0,3,0,0,0,0,0,0,0,0,0,6,6,6,6,0,0,0,0,0,0,4,0,0,0,0,0,4,0,0,0,4,0,4,0,0,0,0,0,4,0,0,0,4,0,4,0,0,0,0,0,0,0,0,0,4,0,4,0,0,0,4,0,0,0,0},
    {0,0,0,0,0,5,5,0,0,0,0,0,0,0,0,3,0,0,0,0,0,3,3,3,3,3,0,3,0,0,0,3,0,3,3,3,3,0,0,0,3,3,3,0,0,3,0,0,0,3,0,3,0,0,0,3,0,0,0,0,0,0,0,0,0,6,6,0,0,0,0,0,0,0,0,4,4,4,4,4,0,4,0,0,0,4,0,4,0,0,0,0,0,4,0,0,0,4,0,4,4,4,4,4,0,4,4,4,4,0,0,4,0,0,0,4,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

// Pone el color de fondo segun el numero de la matriz.
// Las zonas 3 a 6 cambian de color segun lo que este seleccionado.
void colorDeZona(int zona, bool esEmpresa) {
    switch (zona) {
    case 1:
        cout << COLOR_TERRACOTA;
        break;
    case 2:
        cout << COLOR_ARENA;
        break;
    case 3:     // PERSONA: arena si esta elegida, marron si no
        if (esEmpresa) cout << COLOR_MARRON;
        else cout << COLOR_ARENA;
        break;
    case 4:     // EMPRESA: arena si esta elegida, marron si no
        if (esEmpresa) cout << COLOR_ARENA;
        else cout << COLOR_MARRON;
        break;
    case 5:     // flecha de PERSONA: terracota si esta elegida, se borra si no
        if (esEmpresa) cout << COLOR_NORMAL;
        else cout << COLOR_TERRACOTA;
        break;
    case 6:     // flecha de EMPRESA: terracota si esta elegida, se borra si no
        if (esEmpresa) cout << COLOR_TERRACOTA;
        else cout << COLOR_NORMAL;
        break;
    }
}

// Recorre la matriz con un doble for y pinta las celdas que no son 0.
// Al final se resetea el color ANSI y se vuelve al blanco de siempre,
// asi las demas pantallas no quedan con fondo de color.
void pintarPantallaInicio(bool esEmpresa) {
    for (int fila = 0; fila < 30; fila++) {
        for (int columna = 0; columna < 120; columna++) {
            if (pantallaInicio[fila][columna] != 0) {
                ubicar(columna, fila);
                colorDeZona(pantallaInicio[fila][columna], esEmpresa);
                cout << ' ';
            }
        }
    }
    cout << COLOR_NORMAL;
    colorBlanco();
}

// ======================= Palabras pintadas =======================
// Palabras pintadas: 1 = celda pintada, 0 = nada.
// Todas se guardan en 7 filas x 63 columnas (lo que sobra va en 0) para usar una sola funcion.
//   palabraPersona / palabraEmpresa             5 filas  (titulo de los formularios)
//   palabraPersonaGrande / palabraEmpresaGrande 7 filas  (titulo del menu de cuenta)
//   palabraIniciarSesion / palabraRegistro      4 filas  (subtitulo de los formularios)
//   palabraIniciarSesionMenu / palabraRegistrarse 5 filas (opciones del menu de cuenta)
int palabraPersona[7][63] = {
    {1,1,1,0,0,1,1,1,1,0,1,1,1,0,0,0,1,1,1,0,0,1,1,0,0,1,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,0,0,1,0,0,1,0,1,1,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,0,0,1,1,1,0,0,1,1,1,0,0,0,1,1,0,0,1,0,0,1,0,1,0,1,1,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,1,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,1,1,1,1,0,1,0,0,1,0,1,1,1,0,0,0,1,1,0,0,1,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int palabraEmpresa[7][63] = {
    {1,1,1,1,0,1,0,0,0,1,0,1,1,1,0,0,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,1,1,0,1,1,0,1,0,0,1,0,1,0,0,1,0,1,0,0,0,0,1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,1,1,0,0,1,1,1,0,0,0,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,0,1,0,0,0,1,0,1,0,0,0,0,1,0,0,1,0,1,1,1,1,0,1,1,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int palabraPersonaGrande[7][63] = {
    {1,1,1,1,0,0,1,1,1,1,1,0,1,1,1,1,0,0,0,1,1,1,1,0,0,1,1,1,0,0,1,0,0,0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,1,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,1,1,1,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,0,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,1,1,1,1,1,0,1,0,0,0,1,0,1,1,1,1,0,0,0,1,1,1,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int palabraEmpresaGrande[7][63] = {
    {1,1,1,1,1,0,1,0,0,0,1,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,1,1,0,1,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,0,0,1,0,0,0,1,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,1,1,1,1,0,1,1,1,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int palabraIniciarSesion[7][63] = {
    {1,1,1,0,1,0,0,1,0,1,1,1,0,0,1,1,1,0,1,1,1,0,0,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,1,1,1,1,0,0,1,1,1,0,1,1,1,0,0,1,1,0,0,1,0,0,1},
    {0,1,0,0,1,1,0,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1,1,0,0,0,1,1,1,0,0,1,1,0,0,0,0,1,0,0,1,0,0,1,0,1,1,0,1},
    {0,1,0,0,1,0,1,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,1,1,1,0,1,1,1,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,1,1,0,0,1,0,0,1,0,0,1,0,1,0,1,1},
    {1,1,1,0,1,0,0,1,0,1,1,1,0,0,1,1,1,0,1,1,1,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1,1,1,0,0,1,1,1,1,0,1,1,1,0,0,1,1,1,0,0,1,1,0,0,1,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int palabraRegistro[7][63] = {
    {1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,1,1,1,0,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,1,0,1,1,1,0,0,1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,0,0,1,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,0,0,1,0,0,0,0,1,0,1,1,0,0,1,0,0,0,0,1,1,0,0,0,1,0,0,0,1,1,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,1,0,1,1,1,1,0,0,1,1,1,0,1,1,1,0,1,1,1,0,0,0,0,1,0,0,0,1,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int palabraIniciarSesionMenu[7][63] = {
    {1,1,1,0,1,0,0,1,0,1,1,1,0,0,1,1,1,0,1,1,1,0,0,1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,1,1,1,1,0,0,1,1,1,0,1,1,1,0,0,1,1,0,0,1,0,0,1},
    {0,1,0,0,1,1,0,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,1,0,1,1,0,1},
    {0,1,0,0,1,0,1,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,1,1,1,0,1,1,1,0,0,0,0,0,0,1,1,0,0,1,1,1,0,0,0,1,1,0,0,0,1,0,0,1,0,0,1,0,1,0,1,1},
    {0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,1,0,0,1},
    {1,1,1,0,1,0,0,1,0,1,1,1,0,0,1,1,1,0,1,1,1,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1,1,1,0,0,1,1,1,1,0,1,1,1,0,0,1,1,1,0,0,1,1,0,0,1,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int palabraRegistrarse[7][63] = {
    {1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,1,1,1,0,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,0,0,1,1,0,0,1,1,1,0,0,0,1,1,1,0,1,1,1,1,0,0,0,0,0,0,0,0,0},
    {1,0,0,1,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,0,0,1,1,1,0,0,1,0,1,1,0,0,1,0,0,0,1,1,0,0,0,0,1,0,0,0,1,1,1,0,0,1,1,1,1,0,1,1,1,0,0,0,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0},
    {1,0,1,0,0,1,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,1,0,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,1,0,1,1,1,1,0,0,1,1,1,0,1,1,1,0,1,1,1,0,0,0,0,1,0,0,0,1,0,0,1,0,1,0,0,1,0,1,0,0,1,0,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int flecha[7][8] = {
    {1,1,0,0,0,0,0,0},
    {1,1,1,1,0,0,0,0},
    {1,1,1,1,1,1,0,0},
    {1,1,1,1,0,0,0,0},
    {1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
};

// Pinta una palabra con el color indicado, desde la columna x y la fila y.
void pintarPalabra(int palabra[7][63], int x, int y, string color) {
    cout << color;
    for (int fila = 0; fila < 7; fila++) {
        for (int columna = 0; columna < 63; columna++) {
            if (palabra[fila][columna] == 1) {
                ubicar(x + columna, y + fila);
                cout << ' ';
            }
        }
    }
    cout << COLOR_NORMAL;
    colorBlanco();
}

// Pinta la flecha de seleccion (5 filas x 6 columnas). Con COLOR_NORMAL se borra.
void pintarFlecha(int flecha[7][8], int x, int y, string color) {
    cout << color;
    for (int fila = 0; fila < 7; fila++) {
        for (int columna = 0; columna < 8; columna++) {
            if (flecha[fila][columna] == 1) {
                ubicar(x + columna, y + fila);
                cout << ' ';
            }
        }
    }
    cout << COLOR_NORMAL;
    colorBlanco();
}

// Titulo PERSONA o EMPRESA de los formularios en terracota, centrado en las filas 1 a 5.
void pintarTitulo(bool esEmpresa) {
    if (esEmpresa) pintarPalabra(palabraEmpresa, 42, 1, COLOR_TERRACOTA);
    else pintarPalabra(palabraPersona, 43, 1, COLOR_TERRACOTA);
}

// ======================= Pantallas de eleccion =======================

// Pantalla inicial: PERSONA o EMPRESA con flechas izquierda/derecha.
// Devuelve false si se presiono ESC (salir del programa).
bool elegirTipo(bool& esEmpresa) {
    limpiarPantalla();
    ubicar(50, 12); cout << "Como deseas ingresar";
    ubicar(44, 25); cout << "Presione ENTER para seleccionar";
    ubicar(48, 27); cout << "Presione ESC para salir";

    while (true) {
        // Se repinta todo: cambian los colores de las palabras y la flecha.
        pintarPantallaInicio(esEmpresa);

        int tecla = leerTecla();
        if (tecla == TECLA_IZQUIERDA) esEmpresa = false;
        else if (tecla == TECLA_DERECHA) esEmpresa = true;
        else if (tecla == TECLA_ENTER) return true;
        else if (tecla == TECLA_ESC) return false;
    }
}

// Menu INICIAR SESION / REGISTRARSE con flechas arriba/abajo.
// opcion: 0 = Iniciar sesion, 1 = Registrarse. Devuelve false si se presiono ESC.
bool elegirAccion(bool esEmpresa, int& opcion) {
    limpiarPantalla();
    if (esEmpresa) pintarPalabra(palabraEmpresaGrande, 39, 1, COLOR_TERRACOTA);
    else pintarPalabra(palabraPersonaGrande, 39, 1, COLOR_TERRACOTA);
    ubicar(44, 25); cout << "Presione ENTER para seleccionar";
    ubicar(46, 27); cout << "Presione ESC para retroceder";

    while (true) {
        // La opcion elegida va en arena con su flecha; la otra en marron y sin flecha.
        if (opcion == 0) {
            pintarFlecha(flecha, 24, 10, COLOR_TERRACOTA);
            pintarPalabra(palabraIniciarSesionMenu, 32, 10, COLOR_ARENA);
            pintarFlecha(flecha, 24, 17, COLOR_NORMAL);
            pintarPalabra(palabraRegistrarse, 32, 17, COLOR_MARRON);
        }
        else {
            pintarFlecha(flecha, 24, 10, COLOR_NORMAL);
            pintarPalabra(palabraIniciarSesionMenu, 32, 10, COLOR_MARRON);
            pintarFlecha(flecha, 24, 17, COLOR_TERRACOTA);
            pintarPalabra(palabraRegistrarse, 32, 17, COLOR_ARENA);
        }

        int tecla = leerTecla();
        if (tecla == TECLA_ARRIBA) opcion = 0;
        else if (tecla == TECLA_ABAJO) opcion = 1;
        else if (tecla == TECLA_ENTER) return true;
        else if (tecla == TECLA_ESC) return false;
    }
}

// ======================= Auxiliares de formulario =======================

// Parte comun de los formularios: titulo arriba (filas 1 a 5) y ayudas abajo.
// El subtitulo pintado (filas 7 a 10) lo pone cada formulario.
// Los mensajes de error y de exito van en las filas 25 y 26.
void dibujarFormulario(bool esEmpresa) {
    limpiarPantalla();
    pintarTitulo(esEmpresa);
    ubicar(38, 27); cout << "Presione ENTER para pasar al siguiente campo";
    ubicar(46, 28); cout << "Presione ESC para retroceder";
}

// Lee un campo tecla por tecla en la columna x de la fila y.
// No deja escribir mas de maximo caracteres.
// Devuelve true con ENTER y false con ESC.
bool leerCampoEn(int x, int y, int maximo, string& valor) {
    valor = "";
    while (true) {
        ubicar(x + (int)valor.length(), y);
        int tecla = leerTecla();

        if (tecla == TECLA_ENTER) return true;
        if (tecla == TECLA_ESC) return false;

        if (tecla == TECLA_RETROCESO && valor != "") {
            valor.pop_back();
            ubicar(x + (int)valor.length(), y);
            cout << ' ';
        }
        // Solo letras, numeros y signos comunes (sin tildes ni enie).
        else if (tecla >= 32 && tecla <= 126 && (int)valor.length() < maximo) {
            cout << (char)tecla;
            valor += (char)tecla;
        }
    }
}

// Version del login: los campos del login siempre van en la columna 53.
bool leerCampo(int y, int maximo, string& valor) {
    return leerCampoEn(53, y, maximo, valor);
}

// Muestra el error en rojo debajo del formulario y vuelve a blanco.
// Devuelve true con ENTER (intentar de nuevo) y false con ESC (retroceder).
bool mostrarError(string mensaje) {
    colorRojo();
    ubicar(22, 25); cout << mensaje;
    colorBlanco();
    ubicar(22, 26); cout << "Presione ENTER para intentar de nuevo";
    while (true) {
        int tecla = leerTecla();
        if (tecla == TECLA_ENTER) return true;
        if (tecla == TECLA_ESC) return false;
    }
}

// Aviso de cuenta creada. Espera ENTER o ESC.
void mostrarExito(int id) {
    ubicar(22, 25); cout << "Cuenta creada con exito. Tu codigo es " << id;
    ubicar(22, 26); cout << "Presione ENTER para continuar";
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
        pintarPalabra(palabraIniciarSesion, 28, 7, COLOR_ARENA);
        ubicar(22, 14); cout << "Correo:";
        ubicar(22, 16); cout << "Contrasena:";

        if (!leerCampo(14, RedProfesional::MAX_CORREO, correo)) return false;
        if (!leerCampo(16, RedProfesional::MAX_CONTRASENA, contrasena)) return false;

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
void formularioRegistroPersona(RedProfesional& red) {
    string nombre, apellido, titular, distrito, correo, contrasena;
    while (true) {
        dibujarFormulario(false);
        pintarPalabra(palabraRegistro, 40, 7, COLOR_ARENA);
        ubicar(40, 12); cout << "El titular y el distrito son opcionales";
        ubicar(22, 14); cout << "Nombre:";
        ubicar(22, 16); cout << "Apellido:";
        ubicar(22, 18); cout << "Titular:";
        ubicar(22, 20); cout << "Distrito de residencia:";
        ubicar(22, 22); cout << "Correo:";
        ubicar(22, 24); cout << "Contrasena:";

        if (!leerCampo(14, RedProfesional::MAX_NOMBRE, nombre)) return;
        if (!leerCampo(16, RedProfesional::MAX_APELLIDO, apellido)) return;
        if (!leerCampo(18, RedProfesional::MAX_TITULAR, titular)) return;
        if (!leerCampo(20, RedProfesional::MAX_UBICACION, distrito)) return;
        if (!leerCampo(22, RedProfesional::MAX_CORREO, correo)) return;
        if (!leerCampo(24, RedProfesional::MAX_CONTRASENA, contrasena)) return;

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
        pintarPalabra(palabraRegistro, 40, 7, COLOR_ARENA);
        ubicar(41, 12); cout << "El sector y el distrito son opcionales";
        ubicar(22, 14); cout << "Nombre de la empresa:";
        ubicar(22, 16); cout << "Sector:";
        ubicar(22, 18); cout << "Distrito del establecimiento:";
        ubicar(22, 20); cout << "Correo:";
        ubicar(22, 22); cout << "Contrasena:";

        if (!leerCampo(14, RedProfesional::MAX_NOMBRE_EMPRESA, nombre)) return;
        if (!leerCampo(16, RedProfesional::MAX_SECTOR, sector)) return;
        if (!leerCampo(18, RedProfesional::MAX_UBICACION, distrito)) return;
        if (!leerCampo(20, RedProfesional::MAX_CORREO, correo)) return;
        if (!leerCampo(22, RedProfesional::MAX_CONTRASENA, contrasena)) return;

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
        else formularioRegistroPersona(red);
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