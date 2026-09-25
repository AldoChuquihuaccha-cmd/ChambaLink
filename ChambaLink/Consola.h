#pragma once
// Evita que windows.h defina las macros min/max y cargue partes que no se usan.
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <conio.h>
#include <string>

// Teclas que entiende ChambaLink. Todo lo demas se devuelve como Otra
// y las pantallas simplemente la ignoran.
enum class Tecla { Izquierda, Derecha, Arriba, Abajo, Enter, Esc, Retroceso, Caracter, Otra };

// Herramientas de pantalla y teclado para toda la aplicacion.
// La consola es fija de 30 filas x 120 columnas.
//
// Todo se escribe con posicion (fila, columna) usando la API de Windows,
// sin cout y sin system("cls"): asi no hay parpadeo ni desplazamiento, y
// borrar la pantalla o una fila es una sola llamada, sin recorrer celdas.
//
// Filas y columnas empiezan en 0.
class Consola {
private:
    static HANDLE salida() { return GetStdHandle(STD_OUTPUT_HANDLE); }

    // Color original de la consola (el "blanco" normal, que en Windows es gris claro).
    // Se guarda en configurar() para volver a el despues de escribir en rojo.
    static WORD& colorNormal() {
        static WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        return color;
    }

public:
    static const int FILAS = 30;
    static const int COLUMNAS = 120;

    // Se llama una sola vez al inicio del programa.
    static void configurar() {
        HANDLE h = salida();
        SetConsoleTitleA("ChambaLink");

        CONSOLE_SCREEN_BUFFER_INFO info;
        if (GetConsoleScreenBufferInfo(h, &info)) colorNormal() = info.wAttributes;

        // Primero se achica la ventana, luego se fija el buffer y por ultimo
        // se agranda la ventana al tamano final. En otro orden Windows rechaza
        // el cambio si la ventana actual es mas grande que el buffer nuevo.
        SMALL_RECT minima = { 0, 0, 1, 1 };
        SetConsoleWindowInfo(h, TRUE, &minima);
        COORD tamanio = { (SHORT)COLUMNAS, (SHORT)FILAS };
        SetConsoleScreenBufferSize(h, tamanio);   // buffer = ventana: sin barra de desplazamiento
        SMALL_RECT ventana = { 0, 0, (SHORT)(COLUMNAS - 1), (SHORT)(FILAS - 1) };
        SetConsoleWindowInfo(h, TRUE, &ventana);

        // Quita los botones de maximizar y el borde para cambiar el tamano.
        HWND ventanaWindows = GetConsoleWindow();
        LONG estilo = GetWindowLong(ventanaWindows, GWL_STYLE);
        SetWindowLong(ventanaWindows, GWL_STYLE, estilo & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

        mostrarCursor(false);
        limpiar();
    }

    static void mostrarCursor(bool visible) {
        CONSOLE_CURSOR_INFO cursor;
        cursor.dwSize = 25;
        cursor.bVisible = visible ? TRUE : FALSE;
        SetConsoleCursorInfo(salida(), &cursor);
    }

    static void moverCursor(int fila, int columna) {
        COORD posicion = { (SHORT)columna, (SHORT)fila };
        SetConsoleCursorPosition(salida(), posicion);
    }

    // Borra toda la pantalla y le devuelve el color normal.
    static void limpiar() {
        DWORD escritos;
        COORD inicio = { 0, 0 };
        FillConsoleOutputCharacterA(salida(), ' ', FILAS * COLUMNAS, inicio, &escritos);
        FillConsoleOutputAttribute(salida(), colorNormal(), FILAS * COLUMNAS, inicio, &escritos);
    }

    // Borra una fila completa y le devuelve el color normal.
    static void limpiarFila(int fila) {
        DWORD escritos;
        COORD inicio = { 0, (SHORT)fila };
        FillConsoleOutputCharacterA(salida(), ' ', COLUMNAS, inicio, &escritos);
        FillConsoleOutputAttribute(salida(), colorNormal(), COLUMNAS, inicio, &escritos);
    }

    // Escribe el texto en la posicion indicada con el color que ya tenga esa zona.
    static void escribir(int fila, int columna, const std::string& texto) {
        DWORD escritos;
        COORD posicion = { (SHORT)columna, (SHORT)fila };
        WriteConsoleOutputCharacterA(salida(), texto.c_str(), (DWORD)texto.length(), posicion, &escritos);
    }

    // Columna en la que hay que empezar para que un texto de ese ancho quede centrado.
    static int columnaCentrada(int ancho) { return (COLUMNAS - ancho) / 2; }

    static void escribirCentrado(int fila, const std::string& texto) {
        escribir(fila, columnaCentrada((int)texto.length()), texto);
    }

    // Escribe centrado en rojo. El color queda solo en esas celdas:
    // al llamar a limpiarFila la fila vuelve al color normal.
    static void escribirCentradoRojo(int fila, const std::string& texto) {
        int columna = columnaCentrada((int)texto.length());
        escribir(fila, columna, texto);
        DWORD escritos;
        COORD posicion = { (SHORT)columna, (SHORT)fila };
        FillConsoleOutputAttribute(salida(), FOREGROUND_RED | FOREGROUND_INTENSITY,
            (DWORD)texto.length(), posicion, &escritos);
    }

    // Espera una tecla y la traduce.
    // Las flechas llegan como dos codigos: primero 0 o 224 y luego el de la
    // direccion. Se leen los dos para que el segundo no cuente como otra tecla.
    static Tecla leerTecla(char& caracter) {
        int codigo = _getch();
        if (codigo == 0 || codigo == 224) {
            int direccion = _getch();
            if (direccion == 75) return Tecla::Izquierda;
            if (direccion == 77) return Tecla::Derecha;
            if (direccion == 72) return Tecla::Arriba;
            if (direccion == 80) return Tecla::Abajo;
            return Tecla::Otra;
        }
        if (codigo == 13) return Tecla::Enter;
        if (codigo == 27) return Tecla::Esc;
        if (codigo == 8) return Tecla::Retroceso;
        // Solo caracteres ASCII imprimibles: sin tildes ni enie, que la consola
        // de Windows no muestra bien sin configurar la codificacion.
        if (codigo >= 32 && codigo <= 126) {
            caracter = (char)codigo;
            return Tecla::Caracter;
        }
        return Tecla::Otra;
    }

    // Version para los menus, donde no interesa que caracter se escribio.
    static Tecla leerTecla() {
        char ignorado;
        return leerTecla(ignorado);
    }
};