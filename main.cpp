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
    const auto& productos = entrada.get_productos();

    vector<int> orden(entrada.get_n()); //creamos un vector con los indices de los productos
    iota(orden.begin(), orden.end(), 0);

    sort(orden.begin(), orden.end(), [&](int a, int b) { //para optimizar los ordenamos de mayor a menor peso
        return productos[a].first > productos[b].first;
    });

    const double eps = 1e-9; //para no equivocarnos en comparacion de decimales

    //Procesamos los productos
    for(int i : orden){
        double peso = productos[i].first;
        double temperatura = productos[i].second;

        if(peso > entrada.get_capacidad() + eps){
            throw runtime_error("Producto excede capacidad del camion");
        }

        int mejor_camion = -1; //todavia no encontramos ningun camion valido
        double menor_espacio = numeric_limits<double>::infinity();
        double mayor_similitud = -1.0;

        //Buscamos el mejor camion existente
        for(int k = 0; k < solucion.get_K(); ++k){
            double libre = solucion.get_espacio_camiones()[k];

            //Verificamos capacidad
            if(libre + eps < peso){
                continue; //descartamos camiones invalidos
            }

            const auto& camion = solucion.get_camiones()[k];

            //Verificamos temperatura 
            if(!camion.empty()){
                auto rango = solucion.get_rango_temperaturas()[k];

                double nueva_min = min(rango.first, temperatura);
                double nueva_max = max(rango.second, temperatura);

                if(nueva_max - nueva_min > entrada.get_Tmax() + eps){
                    continue;
                }
            }

            //Calculamos el espacio que queda y la similitud agregada
            double espacio_despues = libre - peso;
            double similitud = 0.0;

            for(int j : camion){
                similitud += entrada.get_similitud(i, j);
            }

            //Elegimos al camion que menos espacio le queda, si hay epate elegimos por similitud
            if (espacio_despues < menor_espacio - eps || (abs(espacio_despues - menor_espacio) <= eps && similitud > mayor_similitud)) {
                mejor_camion = k;
                menor_espacio = espacio_despues;
                mayor_similitud = similitud;
            }
        }

        //Si no entra en ningun camion, abrimos uno nuevo
        if (mejor_camion == -1) {
            if (!solucion.get_camiones()[0].empty()) {
                solucion.agregar_camion();
            }

            mejor_camion = solucion.get_K() - 1;
        }

        //Agregar el producto al camion elegido
        solucion.agregar_a_camion(mejor_camion, i, peso);
    }
}

// -----------------------------------------------------------------------------
// Operadores de busqueda local (reemplazar el cuerpo de cada funcion)
// -----------------------------------------------------------------------------
void relocate(const Entrada& entrada, Solucion& solucion) { //relocate reubica un producto de un camion a otro
    while (true) {
        const double objetivo_actual = solucion.get_objetivo();
        double mejor_objetivo = objetivo_actual;

        int mejor_origen = -1;
        int mejor_posicion = -1;
        int mejor_destino = -1;

        const vector<vector<int>>& camiones = solucion.get_camiones();
        const vector<double>& espacio = solucion.get_espacio_camiones();
        const vector<pair<double, double>>& rangos = solucion.get_rango_temperaturas();
        const vector<pair<double, double>>& productos = entrada.get_productos();
        const int cantidad_camiones = solucion.get_K();

        for (int origen = 0; origen < cantidad_camiones; ++origen) { //Para cada camion quiero ver
            const vector<int>& camion_origen = camiones[origen];

            for (int posicion = 0; posicion < camion_origen.size(); ++posicion) { //como afecta mover cada producto
                const int indice_producto = camion_origen[posicion];
                const double peso = productos[indice_producto].first;
                const double temperatura = productos[indice_producto].second;

                for (int destino = 0; destino < cantidad_camiones; ++destino) { //a cada camion disponible
                    if (destino == origen) { // No tiene sentido realocar al mismo camion.
                        continue;
                    }
                    if (espacio[destino] < peso) { //No se puede realocar a un camion si no entra
                        continue;
                    }
                    const pair<double, double>& rango = rangos[destino];
                    const double nueva_min = min(rango.first, temperatura);
                    const double nueva_max = max(rango.second, temperatura);
                    if (nueva_max - nueva_min > entrada.get_Tmax()) { //No se puede realocar si hay mucha diferencia de temperatura
                        continue;
                    }

                    const double objetivo_candidato = solucion.evaluar_objetivo_reubicacion(origen, posicion, destino); //Evaluar relocacion sin modificar solucion

                    if (objetivo_candidato < mejor_objetivo) { //Si el objetivo mejora, guardo los indices del mejor movimiento
                        mejor_objetivo = objetivo_candidato;
                        mejor_origen = origen;
                        mejor_posicion = posicion;
                        mejor_destino = destino;
                    }
                }
            }
        }

        if (mejor_origen == -1) { //Si no hay una mejora no hago cambios
            break;
        }

        solucion.reubicar_producto(mejor_origen, mejor_posicion, mejor_destino);
        solucion.funcion_objetivo();
    }
}

void swapop(const Entrada& entrada, Solucion& solucion) { //SWAP intercambia un producto de un camion por otro producto de otro camion
    while (true) {
        const double objetivo_actual = solucion.get_objetivo();
        double mejor_objetivo = objetivo_actual;

        int mejor_camion_a = -1;
        int mejor_posicion_a = -1;
        int mejor_camion_b = -1;
        int mejor_posicion_b = -1;

        const vector<vector<int>>& camiones = solucion.get_camiones();
        const vector<double>& espacio = solucion.get_espacio_camiones();
        const vector<pair<double, double>>& productos = entrada.get_productos();

        const int cantidad_camiones = solucion.get_K();

        for (int camion_a = 0; camion_a < cantidad_camiones; ++camion_a) {
            const vector<int>& productos_a = camiones[camion_a];

            for (int camion_b = camion_a + 1; camion_b < cantidad_camiones; ++camion_b) {
                const vector<int>& productos_b = camiones[camion_b];

                for (int posicion_a = 0; posicion_a < static_cast<int>(productos_a.size()); ++posicion_a) {
                    const int indice_producto_a = productos_a[posicion_a];
                    const double peso_a = productos[indice_producto_a].first;
                    const double temperatura_a = productos[indice_producto_a].second;

                    for (int posicion_b = 0; posicion_b < static_cast<int>(productos_b.size()); ++posicion_b) {
                        const int indice_producto_b = productos_b[posicion_b];
                        const double peso_b = productos[indice_producto_b].first;
                        const double temperatura_b = productos[indice_producto_b].second;

                        // Verifico que ambos camiones respeten la capacidad despues del intercambio.
                        if (espacio[camion_a] + peso_a - peso_b < 0.0 ||
                            espacio[camion_b] + peso_b - peso_a < 0.0) {
                            continue;
                        }

                        // Calculo el rango de temperaturas del camion A despues del intercambio.
                        double temperatura_min_a = temperatura_b;
                        double temperatura_max_a = temperatura_b;

                        for (int posicion = 0; posicion < static_cast<int>(productos_a.size()); ++posicion) {
                            if (posicion != posicion_a) {
                                const double temperatura = productos[productos_a[posicion]].second;

                                temperatura_min_a = min(temperatura_min_a, temperatura);
                                temperatura_max_a = max(temperatura_max_a, temperatura);
                            }
                        }

                        if (temperatura_max_a - temperatura_min_a > entrada.get_Tmax()) {
                            continue;
                        }

                        // Calculo el rango de temperaturas del camion B despues del intercambio.
                        double temperatura_min_b = temperatura_a;
                        double temperatura_max_b = temperatura_a;

                        for (int posicion = 0; posicion < static_cast<int>(productos_b.size()); ++posicion) {
                            if (posicion != posicion_b) {
                                const double temperatura = productos[productos_b[posicion]].second;

                                temperatura_min_b = min(temperatura_min_b, temperatura);
                                temperatura_max_b = max(temperatura_max_b, temperatura);
                            }
                        }

                        if (temperatura_max_b - temperatura_min_b > entrada.get_Tmax()) {
                            continue;
                        }

                        const double objetivo_candidato =
                            solucion.evaluar_objetivo_intercambio(camion_a, posicion_a,
                                                                  camion_b, posicion_b);

                        if (objetivo_candidato < mejor_objetivo) {
                            mejor_objetivo = objetivo_candidato;

                            mejor_camion_a = camion_a;
                            mejor_posicion_a = posicion_a;
                            mejor_camion_b = camion_b;
                            mejor_posicion_b = posicion_b;
                        }
                    }
                }
            }
        }

        if (mejor_camion_a == -1) {
            break;
        }

        solucion.intercambiar_productos(mejor_camion_a, mejor_posicion_a,
                                        mejor_camion_b, mejor_posicion_b);

        solucion.funcion_objetivo();
    }
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
    solucion.funcion_objetivo();

    // El operador se aplica despues de generar la solucion, si fue solicitado.
    if (!nombre_operador.empty()) {
        if (nombre_operador == "relocate") {
            relocate(entrada, solucion);
        } else if (nombre_operador == "SWAP") {
            swapop(entrada, solucion);
        } else {
            throw runtime_error("Operador desconocido: " + nombre_operador);
        }
    }
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
