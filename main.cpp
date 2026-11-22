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
void umbral_similitud(const Entrada& entrada, Solucion& solucion) {

    double umbral = 0.5;

    const vector<pair<double, double>> productos = entrada.get_productos();
    solucion.agregar_a_camion(0, 0, productos[0].first); //El primer producto si o si va al primer camion

    for (int i = 1; i < entrada.get_n(); ++i) { //Entonces itero desde el segundo

        double peso = productos[i].first;
        double temperatura = productos[i].second;

        bool agregado = false;

        for (int k = 0; k < solucion.get_K(); ++k) {

            const vector<int>& camion = solucion.get_camiones()[k];

            if (solucion.get_espacio_camiones()[k] < peso) { //Si pesa mas de la capacidad restante no puedo agregar
                continue;
            }

                const pair<double, double> rango = solucion.get_rango_temperaturas()[k];

                double nueva_min = min(rango.first, temperatura);
                double nueva_max = max(rango.second, temperatura);

                if (nueva_max - nueva_min > entrada.get_Tmax()) { //Si hay mucha difernecia de temperatura no puedo agregar
                    continue;
                }
            
            bool similitud_valida = true;

            for (int j : camion) {

                if (entrada.get_similitud(i, j) < umbral) { //Si la similitud con al menos un producto es menor al umbral no puedo agregar
                    similitud_valida = false;
                    break;
                }
            }

            if (similitud_valida) {
                solucion.agregar_a_camion(k, i, peso);
                agregado = true;
                break;
            }
        }

        if (!agregado) { //Si no se pudo agregar a ningun camion, creo uno nuevo y agrego el producto
            solucion.agregar_camion();

            int nuevo_camion = solucion.get_K() - 1;

            solucion.agregar_a_camion(nuevo_camion, i, peso);
        }
    }
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
    // Operador de busqueda local 2: SWAP.
    // Intercambia un producto de un camion con un producto de otro camion
    // siempre que el intercambio sea factible (capacidad y Tmax) y aumente
    // la suma de similitudes. K no cambia: este operador solo reacomoda.
    //
    // Para que sea rapido guardo afinidad[x][t] = suma de similitudes del
    // producto x con todos los productos del camion t. Con eso la ganancia
    // de un swap se calcula en O(1), sin recorrer los camiones.
    const double EPS = 1e-9;
    const int n = entrada.get_n();
    const vector<pair<double, double>> productos = entrada.get_productos();
    const double Tmax = entrada.get_Tmax();
    const vector<vector<double>>& similitudes = entrada.get_similitudes();

    // Trabajo sobre copias locales y al final reconstruyo la Solucion,
    // porque Solucion no tiene metodos para sacar productos de un camion.
    vector<vector<int>> camiones = solucion.get_camiones();
    vector<double> espacio = solucion.get_espacio_camiones();
    const int K = static_cast<int>(camiones.size());

    vector<vector<double>> afinidad(n, vector<double>(K, 0.0));
    for (int t = 0; t < K; ++t) {
        for (int x = 0; x < n; ++x) {
            for (int k : camiones[t]) {
                afinidad[x][t] += similitudes[x][k];
            }
        }
    }

    // Por cada camion guardo las dos temperaturas mas bajas y las dos mas altas.
    // Asi puedo saber el rango que queda al sacar un producto sin recorrer el camion.
    const double INF = numeric_limits<double>::infinity();
    struct ResumenTemp {
        double min1, min2, max1, max2;
        int id_min, id_max;
    };
    vector<ResumenTemp> resumen(K);

    auto recalcular = [&](int t) {
        ResumenTemp r{INF, INF, -INF, -INF, -1, -1};
        for (int p : camiones[t]) {
            const double v = productos[p].second;
            if (v < r.min1) {
                r.min2 = r.min1;
                r.min1 = v;
                r.id_min = p;
            } else if (v < r.min2) {
                r.min2 = v;
            }
            if (v > r.max1) {
                r.max2 = r.max1;
                r.max1 = v;
                r.id_max = p;
            } else if (v > r.max2) {
                r.max2 = v;
            }
        }
        resumen[t] = r;
    };

    for (int t = 0; t < K; ++t) {
        recalcular(t);
    }

    // Dice si el camion respeta Tmax despues de sacar "quitar" y poner "poner".
    auto rango_ok = [&](int camion, int quitar, int poner) {
        const ResumenTemp& r = resumen[camion];
        double tmin = (quitar == r.id_min) ? r.min2 : r.min1;
        double tmax = (quitar == r.id_max) ? r.max2 : r.max1;
        const double v = productos[poner].second;
        tmin = min(tmin, v);
        tmax = max(tmax, v);
        return (tmax - tmin) <= Tmax + EPS;
    };

    bool hubo_cambios = false;
    bool mejoro = true;

    while (mejoro) { // repito hasta que ningun swap mejore
        mejoro = false;

        for (int a = 0; a < K; ++a) {
            for (int b = a + 1; b < K; ++b) {
                for (size_t pa = 0; pa < camiones[a].size(); ++pa) {
                    for (size_t pb = 0; pb < camiones[b].size(); ++pb) {

                        const int i = camiones[a][pa]; // producto que sale del camion a
                        const int j = camiones[b][pb]; // producto que sale del camion b

                        // 1) Ganancia de similitud: j entra a "a" (y sale i),
                        //    i entra a "b" (y sale j). Se descuenta la similitud
                        //    de cada producto consigo mismo y con el que se va.
                        const double sim_ij = similitudes[i][j];
                        const double ganancia =
                            (afinidad[j][a] - sim_ij) - (afinidad[i][a] - similitudes[i][i])
                          + (afinidad[i][b] - sim_ij) - (afinidad[j][b] - similitudes[j][j]);

                        if (ganancia <= EPS) {
                            continue;
                        }

                        // 2) Capacidad: despues del swap ningun camion se pasa.
                        const double wi = productos[i].first;
                        const double wj = productos[j].first;

                        if (espacio[a] + wi - wj < -EPS || espacio[b] + wj - wi < -EPS) {
                            continue;
                        }

                        // 3) Temperatura: ningun camion supera Tmax.
                        if (!rango_ok(a, i, j) || !rango_ok(b, j, i)) {
                            continue;
                        }

                        // Swap valido y que mejora: lo aplico y actualizo lo guardado.
                        swap(camiones[a][pa], camiones[b][pb]);
                        espacio[a] += wi - wj;
                        espacio[b] += wj - wi;

                        // La matriz es simetrica: uso las filas de i y j (memoria contigua).
                        const vector<double>& fila_i = similitudes[i];
                        const vector<double>& fila_j = similitudes[j];
                        for (int x = 0; x < n; ++x) {
                            const double delta = fila_j[x] - fila_i[x];
                            afinidad[x][a] += delta;
                            afinidad[x][b] -= delta;
                        }

                        recalcular(a);
                        recalcular(b);

                        hubo_cambios = true;
                        mejoro = true;
                        break; // el producto en la posicion pa cambio: sigo con el siguiente
                    }
                }
            }
        }
    }

    if (hubo_cambios) {
        Solucion nueva(entrada);

        for (int k = 0; k < K; ++k) {
            if (k > 0) {
                nueva.agregar_camion();
            }
            for (int p : camiones[k]) {
                nueva.agregar_a_camion(k, p, productos[p].first);
            }
        }

        solucion = nueva;
    }

    solucion.funcion_objetivo();
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
        if (nombre_algoritmo == "US") {
            umbral_similitud(entrada, solucion);
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
