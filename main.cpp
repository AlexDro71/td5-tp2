#include <bits/stdc++.h>

#include "Entrada.h"
#include "Heuristicas.h"
#include "Metaheuristica.h"
#include "Operadores.h"
#include "Solucion.h"

using namespace std;

Entrada leer_entrada(const string& ruta) {
    ifstream archivo(ruta);

    if (!archivo.is_open()) {
        throw runtime_error("No se pudo abrir el archivo de entrada");
    }

    int n;
    double capacidad;
    archivo >> n >> capacidad;

    Entrada entrada(n, capacidad);

    for (int i = 0; i < n; ++i) {
        double peso;
        double temperatura;
        archivo >> peso >> temperatura;
        entrada.agregar_producto(peso, temperatura);
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double similitud;
            archivo >> similitud;
            entrada.agregar_similitud(i, j, similitud);
        }
    }

    return entrada;
}

void ejecutar_algoritmo(const string& tipo_algoritmo,
                        const string& nombre_algoritmo,
                        const string& nombre_operador,
                        const Entrada& entrada,
                        Solucion& solucion) {

    // if (tipo_algoritmo == "heuristica") {

    //     Heuristicas heuristicas;
    //     heuristicas.ejecutar(nombre_algoritmo, entrada, solucion);

    //     if (!nombre_operador.empty()) {
    //         Operadores operadores;
    //         operadores.aplicar(nombre_operador, entrada, solucion);
    //     }

    // } else {

    //     Metaheuristica metaheuristica;
    //     metaheuristica.ejecutar(nombre_algoritmo, entrada, solucion);

    // }

    solucion.funcion_objetivo();
}

void escribir_solucion(const string& ruta, const Solucion& solucion) {
    ofstream archivo(ruta);

    const vector<vector<int>>& camiones = solucion.get_camiones();

    for (const vector<int>& camion : camiones) {
        for (size_t i = 0; i < camion.size(); ++i) {
            if (i > 0) {
                archivo << ' ';
            }
            archivo << camion[i];
        }
        archivo << '\n';
    }
}

int main(int argc, char* argv[]) {
    string ruta_entrada;
    string ruta_salida;
    string tipo_algoritmo;
    string nombre_algoritmo;
    string nombre_operador;

    try {
        // Cada argumento se identifica por su prefijo o etiqueta, por lo que
        // el orden en el que se escriben en la consola no afecta su interpretacion.
        for (int i = 1; i < argc; ++i) {
            const string argumento = argv[i];

            if (argumento.rfind("input/", 0) == 0) {
                ruta_entrada = argumento;
            } else if (argumento.rfind("output/", 0) == 0) {
                ruta_salida = argumento;
            } else if (argumento == "--Heuristica" || argumento == "--Metaheuristica") {
                if (i + 1 >= argc) {
                    throw runtime_error("Falta indicar el nombre del algoritmo despues de " + argumento);
                }

                if (!tipo_algoritmo.empty()) {
                    throw runtime_error("Se debe indicar una sola heuristica o metaheuristica.");
                }

                tipo_algoritmo = (argumento == "--Heuristica") ? "heuristica" : "metaheuristica";
                nombre_algoritmo = argv[++i];
            } else if (argumento == "--Operador") {
                if (i + 1 >= argc) {
                    throw runtime_error("Falta indicar el nombre del operador despues de --Operador.");
                }

                nombre_operador = argv[++i];
            } else {
                throw runtime_error("Argumento no reconocido: " + argumento);
            }
        }

        if (ruta_entrada.empty() || ruta_salida.empty() || tipo_algoritmo.empty() || nombre_algoritmo.empty()) {
            throw runtime_error("Se debe indicar input/archivo, output/archivo y una heuristica o metaheuristica.");
        }

        Entrada entrada = leer_entrada(ruta_entrada);
        Solucion solucion(entrada);

        ejecutar_algoritmo(tipo_algoritmo,
                           nombre_algoritmo,
                           nombre_operador,
                           entrada,
                           solucion);

        escribir_solucion(ruta_salida, solucion);

        cout << "Solucion generada correctamente.\n"
             << "K: " << solucion.get_K() << '\n'
             << "Objetivo: " << solucion.get_objetivo() << '\n'
             << "Archivo de salida: " << ruta_salida << '\n';
    } catch (const exception& error) {
        cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
