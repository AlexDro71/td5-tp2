#include <bits/stdc++.h>
#include "Entrada.h"
#include "Heuristicas.h"
#include "Metaheuristica.h"
#include "Operadores.h"
#include "Solucion.h"

using namespace std;

constexpr double TOLERANCIA = 1e-9;

Entrada leer_entrada(const string& ruta) {
    ifstream archivo(ruta);
    int n;
    double capacidad;

    Entrada entrada(n, capacidad);

    for (int i = 0; i < n; ++i) {
        double peso;
        double temperatura;
        entrada.agregar_producto(peso, temperatura);
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double valor;
            entrada.agregar_similitud(i, j, valor);
        }
    }

    return entrada;
}

void mostrar_estado(const optional<Solucion>& ultima_solucion, const optional<Solucion>& mejor_solucion) {
    cout << "Solucion actual: ";
    if (ultima_solucion && ultima_solucion->get_objetivo()==-1) {
        cout << ultima_solucion->get_objetivo();
    } else {
        cout << "-";
    }
    cout << '\n';

    cout << "Mejor solucion: ";
    if (mejor_solucion && mejor_solucion->get_objetivo()==-1) {
        cout << mejor_solucion->get_objetivo();
    } else {
        cout << "-";
    }
    cout << "\n\n";
}

void presionar_enter() {
    cout << "\nPresione ENTER para volver al menu...";
    string linea;
    getline(cin, linea);
}

bool leer_opcion(int& opcion) {
    if (cin >> opcion) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return true;
    }

    if (cin.eof()) {
        return false;
    }

    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Entrada invalida. Ingrese un numero de opcion.\n";
    return true;
}

void mostrar_menu_heuristicas() {
    cout << "Heuristicas constructivas\n\n"
              << "Todavia no hay heuristicas implementadas.\n";
    presionar_enter();
}

void mostrar_menu_operadores(bool existe_ultima_solucion) {
    if (!existe_ultima_solucion) {
        cout << "Todavia no existe una solucion sobre la que ejecutar busqueda local.\n\n"
                  << "0. Volver al menu principal\n\n"
                  << "Seleccione una opcion: ";

        int opcion;
        while (true) {
            if (!leer_opcion(opcion)) {
                return;
            }
            if (opcion == 0) {
                return;
            }
            cout << "Opcion invalida. Ingrese 0 para volver al menu principal: ";
        }
    }

    cout << "Operadores de busqueda local\n\n"
              << "Todavia no hay operadores implementados.\n";
    presionar_enter();
}

void ejecutar_metaheuristica_placeholder() {
    cout << "Metaheuristica\n\n"
              << "Todavia no hay una metaheuristica implementada.\n";
    presionar_enter();
}

void escribir_solucion(const string& ruta, const Solucion& solucion) {
    ofstream archivo(ruta);
    if (!archivo.is_open()) {
        throw runtime_error("No se pudo abrir el archivo de salida: " + ruta);
    }

    const auto& camiones = solucion.get_camiones();
    for (size_t i = 0; i < camiones.size(); ++i) {
        const auto& camion = camiones[i];
        for (size_t j = 0; j < camion.size(); ++j) {
            if (j > 0) {
                archivo << ' ';
            }
            // Los indices internos son 0-based; el archivo se escribe 1-based.
            archivo << camion[j] + 1;
        }
        archivo << '\n';
    }

    if (!archivo) {
        throw runtime_error("Ocurrio un error al escribir el archivo de salida.");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <archivo_entrada.txt> <archivo_salida.txt>\n";
        return 1;
    }

    const string ruta_entrada = argv[1];
    const string ruta_salida = argv[2];

    try {
        Entrada entrada = leer_entrada(ruta_entrada);
        cout << "Archivo de entrada cargado correctamente.\n"
                  << "Cantidad de productos: " << entrada.get_n() << '\n'
                  << "Capacidad de los camiones: " << entrada.get_capacidad() << "\n\n";

        // Las clases se incluyen y quedan listas para incorporar sus metodos.
        Heuristicas heuristicas;
        Operadores operadores;
        Metaheuristica metaheuristica;
        (void)heuristicas;
        (void)operadores;
        (void)metaheuristica;

        // Permanecen vacias hasta que una heuristica/metaheuristica genere una solucion.
        optional<Solucion> ultima_solucion;
        optional<Solucion> mejor_solucion;

        bool continuar = true;
        while (continuar) {
            mostrar_estado(ultima_solucion, mejor_solucion);

            cout << "1. Heuristicas\n"
                      << "2. Operadores de busqueda local\n"
                      << "3. Metaheuristica\n"
                      << "4. Finalizar y generar archivo de salida\n\n"
                      << "Seleccione una opcion: ";

            int opcion;
            if (!leer_opcion(opcion)) {
                cout << "\nFin de la entrada por consola.\n";
                break;
            }

            switch (opcion) {
                case 1:
                    mostrar_menu_heuristicas();
                    // TODO: recibir la solucion generada, actualizar ultima_solucion
                    // y reemplazar mejor_solucion solamente si mejora su objetivo.
                    break;

                case 2:
                    mostrar_menu_operadores(ultima_solucion.has_value());
                    // TODO: aplicar el operador elegido sobre ultima_solucion y actualizar
                    // mejor_solucion si el nuevo objetivo es menor.
                    break;

                case 3:
                    ejecutar_metaheuristica_placeholder();
                    // TODO: recibir la solucion final de la metaheuristica y actualizar
                    // ultima_solucion y mejor_solucion.
                    break;

                case 4:
                    if (!mejor_solucion.has_value()) {
                        cout << "No se genero ninguna solucion. No se escribio el archivo de salida.\n";
                    } else {
                        escribir_solucion(ruta_salida, *mejor_solucion);
                        cout << "Mejor solucion exportada correctamente.\n"
                                  << "Archivo: " << ruta_salida << '\n';
                    }
                    continuar = false;
                    break;

                default:
                    cout << "Opcion invalida. Seleccione una opcion entre 1 y 4.\n\n";
                    break;
            }
        }
    } catch (const exception& error) {
        cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    cout << "Programa finalizado.\n";
    return 0;
}
