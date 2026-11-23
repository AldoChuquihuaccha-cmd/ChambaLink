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
#define COLOR_TERRACOTA "\x1b[48;2;196;98;62m"
#define COLOR_ARENA     "\x1b[48;2;232;220;196m"
#define COLOR_MARRON    "\x1b[48;2;84;76;68m"

// Cajas de los campos de texto: mas clara la que se esta escribiendo.
#define FONDO_CAMPO     "\x1b[48;2;78;66;56m"
#define FONDO_LLENO     "\x1b[48;2;50;42;36m"

// COLOR_NORMAL vuelve a los colores de siempre (sirve para borrar una celda pintada).
#define COLOR_NORMAL    "\x1b[0m"

// Codigos ANSI de color de LETRA: \x1b[38;2;R;G;Bm (el fondo no cambia).
#define TEXTO_TERRACOTA "\x1b[38;2;196;98;62m"
#define TEXTO_ARENA     "\x1b[38;2;232;220;196m"
#define TEXTO_GRIS      "\x1b[38;2;158;147;135m"
#define TEXTO_OSCURO    "\x1b[38;2;30;26;22m"
#define TEXTO_CLARO     "\x1b[38;2;250;244;234m"
#define TEXTO_VERDE     "\x1b[38;2;141;190;126m"
#define TEXTO_AMBAR     "\x1b[38;2;227;181;99m"
#define TEXTO_ERROR     "\x1b[38;2;232;122;100m"
#define TEXTO_LINEA     "\x1b[38;2;92;80;70m"

// Mostrar u ocultar el cursor parpadeante.
#define CURSOR_OCULTO   "\x1b[?25l"
#define CURSOR_VISIBLE  "\x1b[?25h"

// Dibujo de lineas: "\x1b(0" cambia las letras por trazos y "\x1b(B" vuelve a las letras.
//   l = esquina sup. izq.  k = sup. der.  m = inf. izq.  j = inf. der.
//   q = linea horizontal   x = vertical   t = union izq.  u = union der.
#define LINEAS_ON       "\x1b(0"
#define LINEAS_OFF      "\x1b(B"

// ======================= Consola =======================

void ubicar(int x, int y) {
    COORD posicion;
    posicion.X = (SHORT)x;
    posicion.Y = (SHORT)y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), posicion);
}

void colorBlanco() {
    cout << COLOR_NORMAL;
}

void colorRojo() {
    cout << TEXTO_ERROR;
}

// Borra toda la pantalla con un codigo ANSI: es instantaneo;
// system("cls") abria otro proceso y hacia parpadear la pantalla.
void limpiarPantalla() {
    cout << COLOR_NORMAL << "\x1b[2J\x1b[H";
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
    cout << CURSOR_OCULTO;   // el cursor solo aparece al escribir en un campo
    limpiarPantalla();
}

// Al salir del programa: colores originales y cursor visible otra vez.
void restaurarConsola() {
    cout << "\x1b[0m" << CURSOR_VISIBLE << "\x1b[2J\x1b[H";
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
// Borra un rectangulo de la pantalla escribiendo espacios.
void borrarZona(int x, int y, int ancho, int alto) {
    cout << COLOR_NORMAL;
    for (int fila = 0; fila < alto; fila++) {
        ubicar(x, y + fila);
        cout << string(ancho, ' ');
    }
}

// Tecla de ayuda: [ENTER] en arena con letra oscura y su descripcion en gris.
void teclaPie(string tecla, string accion) {
    cout << COLOR_ARENA << TEXTO_OSCURO << " " << tecla << " " << COLOR_NORMAL;
    cout << TEXTO_GRIS << " " << accion << "    " << COLOR_NORMAL;
}

void dibujarFormulario(bool esEmpresa) {
    limpiarPantalla();
    pintarTitulo(esEmpresa);
    ubicar(36, 28);
    teclaPie("ENTER", "siguiente campo");
    teclaPie("ESC", "retroceder");
}

// Escribe un texto centrado en la fila y (la pantalla tiene 120 columnas).
void escribirCentrado(int y, string texto, string color) {
    ubicar((120 - (int)texto.length()) / 2, y);
    cout << COLOR_NORMAL << color << texto << COLOR_NORMAL;
}

// Lee un campo tecla por tecla en la columna x de la fila y.
// No deja escribir mas de maximo caracteres.
// Devuelve true con ENTER y false con ESC.
// El campo se dibuja como una caja: mas clara mientras se escribe en ella.
// Si oculto es true (contrasenas) se muestra * en lugar de cada letra.
void dibujarCaja(int x, int y, int ancho, string fondo, string texto) {
    ubicar(x - 1, y);
    cout << fondo << TEXTO_CLARO << ' ' << texto << string(ancho - (int)texto.length(), ' ');
}

// Ancho de la caja: lo que se puede escribir mas 1, sin pasar la columna 119.
int anchoCampo(int x, int maximo) {
    int ancho = maximo + 1;
    if (x + ancho > 119) ancho = 119 - x;
    return ancho;
}

// Caja vacia de un campo que todavia no se llena (asi se ven todos desde el inicio).
void prepararCampo(int x, int y, int maximo) {
    dibujarCaja(x, y, anchoCampo(x, maximo), FONDO_LLENO, "");
    cout << COLOR_NORMAL;
}

bool leerCampoEn(int x, int y, int maximo, string& valor, bool oculto = false) {
    int ancho = anchoCampo(x, maximo);
    valor = "";
    dibujarCaja(x, y, ancho, FONDO_CAMPO, "");
    cout << CURSOR_VISIBLE;
    while (true) {
        ubicar(x + (int)valor.length(), y);
        int tecla = leerTecla();

        if (tecla == TECLA_ENTER || tecla == TECLA_ESC) {
            cout << CURSOR_OCULTO;
            string visible = oculto ? string(valor.length(), '*') : valor;
            dibujarCaja(x, y, ancho, FONDO_LLENO, visible);
            cout << COLOR_NORMAL;
            return tecla == TECLA_ENTER;
        }

        if (tecla == TECLA_RETROCESO && valor != "") {
            valor.pop_back();
            ubicar(x + (int)valor.length(), y);
            cout << ' ';
        }
        // Solo letras, numeros y signos comunes (sin tildes ni enie).
        else if (tecla >= 32 && tecla <= 126 && (int)valor.length() < maximo) {
            if (oculto) cout << '*';
            else cout << (char)tecla;
            valor += (char)tecla;
        }
    }
}



// Muestra el error en rojo debajo del formulario y vuelve a blanco.
// Devuelve true con ENTER (intentar de nuevo) y false con ESC (retroceder).
// El mensaje va centrado debajo de la tarjeta, en la fila indicada.
bool mostrarError(string mensaje, int fila) {
    escribirCentrado(fila, mensaje + "   (ENTER: intentar de nuevo)", TEXTO_ERROR);
    while (true) {
        int tecla = leerTecla();
        if (tecla == TECLA_ENTER) return true;
        if (tecla == TECLA_ESC) return false;
    }
}

// Aviso de cuenta creada. Espera ENTER o ESC.
void mostrarExito(int id, int fila) {
    escribirCentrado(fila, "Cuenta creada con exito. Tu codigo es " + std::to_string(id) + "   (ENTER: continuar)", TEXTO_VERDE);
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
        // Formulario centrado: etiquetas en la columna 27 y campos en la 42.
        ubicar(27, 15); cout << "Correo:"; prepararCampo(42, 15, RedProfesional::MAX_CORREO);
        ubicar(27, 17); cout << "Contrasena:"; prepararCampo(42, 17, RedProfesional::MAX_CONTRASENA);

        if (!leerCampoEn(42, 15, RedProfesional::MAX_CORREO, correo)) return false;
        if (!leerCampoEn(42, 17, RedProfesional::MAX_CONTRASENA, contrasena, true)) return false;

        int id;
        if (esEmpresa) id = red.iniciarSesionEmpresa(correo, contrasena);
        else id = red.iniciarSesionUsuario(correo, contrasena);

        if (id != -1) {
            idSesion = id;
            return true;
        }
        if (!mostrarError(red.getUltimoError(), 22)) return false;
    }
}

// Al terminar (cuenta creada o ESC) se vuelve al menu Iniciar sesion / Registrarse.
void formularioRegistroPersona(RedProfesional& red) {
    string nombre, apellido, titular, distrito, correo, contrasena;
    while (true) {
        dibujarFormulario(false);
        pintarPalabra(palabraRegistro, 40, 7, COLOR_ARENA);
        escribirCentrado(12, "El titular y el distrito son opcionales", TEXTO_GRIS);
        // Formulario centrado: etiquetas en la columna 21 y campos en la 48.
        ubicar(21, 14); cout << "Nombre:"; prepararCampo(48, 14, RedProfesional::MAX_NOMBRE);
        ubicar(21, 16); cout << "Apellido:"; prepararCampo(48, 16, RedProfesional::MAX_APELLIDO);
        ubicar(21, 18); cout << "Titular:"; prepararCampo(48, 18, RedProfesional::MAX_TITULAR);
        ubicar(21, 20); cout << "Distrito de residencia:"; prepararCampo(48, 20, RedProfesional::MAX_UBICACION);
        ubicar(21, 22); cout << "Correo:"; prepararCampo(48, 22, RedProfesional::MAX_CORREO);
        ubicar(21, 24); cout << "Contrasena:"; prepararCampo(48, 24, RedProfesional::MAX_CONTRASENA);

        if (!leerCampoEn(48, 14, RedProfesional::MAX_NOMBRE, nombre)) return;
        if (!leerCampoEn(48, 16, RedProfesional::MAX_APELLIDO, apellido)) return;
        if (!leerCampoEn(48, 18, RedProfesional::MAX_TITULAR, titular)) return;
        if (!leerCampoEn(48, 20, RedProfesional::MAX_UBICACION, distrito)) return;
        if (!leerCampoEn(48, 22, RedProfesional::MAX_CORREO, correo)) return;
        if (!leerCampoEn(48, 24, RedProfesional::MAX_CONTRASENA, contrasena, true)) return;

        int id = red.registrarUsuario(nombre, apellido, titular, distrito, correo, contrasena);
        if (id != -1) {
            GestorArchivos::guardarTodo(red);   // se guarda enseguida
            mostrarExito(id, 26);
            return;
        }
        if (!mostrarError(red.getUltimoError(), 26)) return;
    }
}

void formularioRegistroEmpresa(RedProfesional& red) {
    string nombre, sector, distrito, correo, contrasena;
    while (true) {
        dibujarFormulario(true);
        pintarPalabra(palabraRegistro, 40, 7, COLOR_ARENA);
        escribirCentrado(12, "El sector y el distrito son opcionales", TEXTO_GRIS);
        // Formulario centrado: etiquetas en la columna 18 y campos en la 51.
        ubicar(18, 14); cout << "Nombre de la empresa:"; prepararCampo(51, 14, RedProfesional::MAX_NOMBRE_EMPRESA);
        ubicar(18, 16); cout << "Sector:"; prepararCampo(51, 16, RedProfesional::MAX_SECTOR);
        ubicar(18, 18); cout << "Distrito del establecimiento:"; prepararCampo(51, 18, RedProfesional::MAX_UBICACION);
        ubicar(18, 20); cout << "Correo:"; prepararCampo(51, 20, RedProfesional::MAX_CORREO);
        ubicar(18, 22); cout << "Contrasena:"; prepararCampo(51, 22, RedProfesional::MAX_CONTRASENA);

        if (!leerCampoEn(51, 14, RedProfesional::MAX_NOMBRE_EMPRESA, nombre)) return;
        if (!leerCampoEn(51, 16, RedProfesional::MAX_SECTOR, sector)) return;
        if (!leerCampoEn(51, 18, RedProfesional::MAX_UBICACION, distrito)) return;
        if (!leerCampoEn(51, 20, RedProfesional::MAX_CORREO, correo)) return;
        if (!leerCampoEn(51, 22, RedProfesional::MAX_CONTRASENA, contrasena, true)) return;

        int id = red.registrarEmpresa(nombre, sector, distrito, correo, contrasena);
        if (id != -1) {
            GestorArchivos::guardarTodo(red);
            mostrarExito(id, 26);
            return;
        }
        if (!mostrarError(red.getUltimoError(), 26)) return;
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