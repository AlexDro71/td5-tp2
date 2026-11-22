#include <bits/stdc++.h>
#include <filesystem>
#include <chrono>

#include "Entrada.h"
#include "Solucion.h"

using namespace std;
namespace fs = std::filesystem;

// -----------------------------------------------------------------------------
// Lectura de la instancia
// -----------------------------------------------------------------------------
Entrada leer_entrada(const string& ruta, double Tmax) {
    ifstream archivo(ruta);

    if (!archivo.is_open()) {
        throw runtime_error("No se pudo abrir el archivo de entrada: " + ruta);
    }

    int n;
    double capacidad;
    archivo >> n >> capacidad;

    Entrada entrada(n, capacidad, Tmax);

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

// -----------------------------------------------------------------------------
// Heuristicas constructivas (reemplazar el cuerpo de cada funcion)
// -----------------------------------------------------------------------------
void heuristica_1(const Entrada& entrada, Solucion& solucion) {
    // TODO: implementar la heuristica constructiva 1.
    (void)entrada;
    (void)solucion;
}

void heuristica_2(const Entrada& entrada, Solucion& solucion) {
    // TODO: implementar la heuristica constructiva 2.
    (void)entrada;
    (void)solucion;
}

// -----------------------------------------------------------------------------
// Operadores de busqueda local (reemplazar el cuerpo de cada funcion)
// -----------------------------------------------------------------------------
void operador_1(const Entrada& entrada, Solucion& solucion) {
    // TODO: implementar el operador de busqueda local 1.
    (void)entrada;
    (void)solucion;
}

void operador_2(const Entrada& entrada, Solucion& solucion) {
    // TODO: implementar el operador de busqueda local 2.
    (void)entrada;
    (void)solucion;
}

// -----------------------------------------------------------------------------
// Metaheuristica (reemplazar el cuerpo de la funcion)
// -----------------------------------------------------------------------------
void metaheuristica_1(const Entrada& entrada, Solucion& solucion) {
    // TODO: implementar la metaheuristica.
    (void)entrada;
    (void)solucion;
}

// -----------------------------------------------------------------------------
// Seleccion y ejecucion del algoritmo solicitado
// -----------------------------------------------------------------------------
void ejecutar_algoritmo(const string& tipo_algoritmo,
                        const string& nombre_algoritmo,
                        const string& nombre_operador,
                        const Entrada& entrada,
                        Solucion& solucion) {

    if (tipo_algoritmo == "heuristica") {
        if (nombre_algoritmo == "H1") {
            heuristica_1(entrada, solucion);
        } else if (nombre_algoritmo == "H2") {
            heuristica_2(entrada, solucion);
        } else {
            throw runtime_error("Heuristica desconocida: " + nombre_algoritmo);
        }

    } else if (tipo_algoritmo == "metaheuristica") {
        if (nombre_algoritmo == "M1") {
            metaheuristica_1(entrada, solucion);
        } else {
            throw runtime_error("Metaheuristica desconocida: " + nombre_algoritmo);
        }

    } else {
        throw runtime_error("Tipo de algoritmo desconocido: " + tipo_algoritmo);
    }

    // El operador se aplica despues de generar la solucion, si fue solicitado.
    if (!nombre_operador.empty()) {
        if (nombre_operador == "O1") {
            operador_1(entrada, solucion);
        } else if (nombre_operador == "O2") {
            operador_2(entrada, solucion);
        } else {
            throw runtime_error("Operador desconocido: " + nombre_operador);
        }
    }

    // Se calcula el objetivo sobre la solucion final, luego del operador si lo hubo.
    solucion.funcion_objetivo();
}

// -----------------------------------------------------------------------------
// Escritura de la solucion
// -----------------------------------------------------------------------------
void escribir_solucion(const string& ruta, const Solucion& solucion) {
    ofstream archivo(ruta);

    const vector<vector<int>>& camiones = solucion.get_camiones();

    for (const vector<int>& camion : camiones) {
        for (size_t i = 0; i < camion.size(); ++i) {
            if (i > 0) {
                archivo << ' ';
            }
            // Los indices se escriben desde 0, igual que en la representacion interna.
            archivo << camion[i];
        }
        archivo << '\n';
    }
}

// Escapa un campo para que pueda almacenarse correctamente en un archivo CSV.
string escapar_csv(const string& valor) {
    string resultado = valor;
    size_t posicion = 0;

    while ((posicion = resultado.find('"', posicion)) != string::npos) {
        resultado.insert(posicion, 1, '"');
        posicion += 2;
    }

    if (resultado.find_first_of(",\n\r\"") != string::npos) {
        resultado = '"' + resultado + '"';
    }

    return resultado;
}

// Agrega una fila al registro general de experimentos.
void registrar_experimento(const string& ruta_entrada,
                           const string& tipo_algoritmo,
                           const string& nombre_algoritmo,
                           const string& nombre_operador,
                           const Entrada& entrada,
                           const Solucion& solucion,
                           double tiempo_ms) {
    const string ruta_csv = "output/resultado.csv";
    const bool archivo_nuevo = !fs::exists(ruta_csv) || fs::file_size(ruta_csv) == 0;

    ofstream archivo(ruta_csv, ios::app);

    if (!archivo.is_open()) {
        throw runtime_error("No se pudo abrir el registro de experimentos: " + ruta_csv);
    }

    if (archivo_nuevo) {
        archivo << "instancia,n,peso_total,capacidad,Tmax,tipo_algoritmo,algoritmo,operador,K,objetivo,tiempo_ms\n";
    }

    double peso_total = 0.0;
    for (const auto& producto : entrada.get_productos()) {
        peso_total += producto.first;
    }

    const string nombre_instancia = fs::path(ruta_entrada).filename().string();
    const string tipo = (tipo_algoritmo == "heuristica") ? "Heuristica" : "Metaheuristica";

    archivo << escapar_csv(nombre_instancia) << ','
            << entrada.get_n() << ','
            << setprecision(12) << peso_total << ','
            << entrada.get_capacidad() << ','
            << entrada.get_Tmax() << ','
            << tipo << ','
            << escapar_csv(nombre_algoritmo) << ','
            << escapar_csv(nombre_operador) << ','
            << solucion.get_K() << ','
            << solucion.get_objetivo() << ','
            << tiempo_ms << '\n';
}

int main(int argc, char* argv[]) {
    string ruta_entrada;
    string ruta_salida;
    string tipo_algoritmo;
    string nombre_algoritmo;
    string nombre_operador;
    double Tmax;
    bool tiene_Tmax = false;

    try {
        // Los argumentos se identifican por etiquetas o prefijos; su orden es indistinto.
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

                if (!nombre_operador.empty()) {
                    throw runtime_error("Se debe indicar como maximo un operador.");
                }

                nombre_operador = argv[++i];
            } else if (argumento == "--T") {
                if (i + 1 >= argc) {
                    throw runtime_error("Falta indicar Tmax despues de --T.");
                }

                if (tiene_Tmax) {
                    throw runtime_error("Tmax debe indicarse una sola vez.");
                }

                Tmax = stod(argv[++i]);
                tiene_Tmax = true;
            } else {
                throw runtime_error("Argumento no reconocido: " + argumento);
            }
        }

        if (ruta_entrada.empty() || ruta_salida.empty() || tipo_algoritmo.empty() || nombre_algoritmo.empty() || !tiene_Tmax) {
            throw runtime_error("Se debe indicar input/archivo, output/archivo, una heuristica o metaheuristica y --T Tmax.");
        }

        Entrada entrada = leer_entrada(ruta_entrada, Tmax);
        Solucion solucion(entrada);

        const auto inicio = chrono::steady_clock::now();
        ejecutar_algoritmo(tipo_algoritmo,
                           nombre_algoritmo,
                           nombre_operador,
                           entrada,
                           solucion);
        const auto fin = chrono::steady_clock::now();

        const double tiempo_ms = chrono::duration<double, milli>(fin - inicio).count();

        escribir_solucion(ruta_salida, solucion);
        registrar_experimento(ruta_entrada,
                              tipo_algoritmo,
                              nombre_algoritmo,
                              nombre_operador,
                              entrada,
                              solucion,
                              tiempo_ms);

        cout << "Solucion generada correctamente.\n"
             << "K: " << solucion.get_K() << '\n'
             << "Objetivo: " << solucion.get_objetivo() << '\n'
             << "Tmax: " << entrada.get_Tmax() << '\n'
             << "Tiempo de ejecucion: " << fixed << setprecision(3) << tiempo_ms << " ms\n"
             << "Archivo de salida: " << ruta_salida << '\n'
             << "Registro experimental: output/resultado.csv\n";

    } catch (const exception& error) {
        cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
