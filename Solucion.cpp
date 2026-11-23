#include <bits/stdc++.h>
#include "Solucion.h"
#include "Entrada.h"

using namespace std;

Solucion::Solucion(const Entrada& e)
    : _camiones(1),
      _k(1),
      _entrada(&e),
      _suma_similitudes(0.0),
      _espacio_camiones(1, e.get_capacidad()),
      _rango_temperaturas(1, make_pair(0.0, 0.0)) {}

void Solucion::agregar_camion() {
    _camiones.emplace_back();
    _espacio_camiones.push_back(_entrada->get_capacidad());
    _rango_temperaturas.emplace_back(0.0, 0.0);
    ++_k;
}

void Solucion::agregar_a_camion(int i, int indice_producto, double peso) {
    const double temperatura = _entrada->get_productos()[indice_producto].second;
    for (int otro_producto : _camiones[i]) {
        _suma_similitudes += _entrada->get_similitud(indice_producto, otro_producto);
    }
    _camiones[i].push_back(indice_producto);
    _espacio_camiones[i] -= peso;
    if (_camiones[i].size() == 1) {
        _rango_temperaturas[i] = make_pair(temperatura, temperatura);
    } else {
        _rango_temperaturas[i].first = min(_rango_temperaturas[i].first, temperatura);
        _rango_temperaturas[i].second = max(_rango_temperaturas[i].second, temperatura);
    }
}

void Solucion::recalcular_rango_temperaturas(int indice_camion) {
    const vector<int>& camion = _camiones[indice_camion];
    const vector<pair<double, double>>& productos = _entrada->get_productos();
    const double temperatura_inicial = productos[camion.front()].second;
    double minimo = temperatura_inicial;
    double maximo = temperatura_inicial;

    for (int indice_producto : camion) {
        const double temperatura = productos[indice_producto].second;
        minimo = min(minimo, temperatura);
        maximo = max(maximo, temperatura);
    }

    _rango_temperaturas[indice_camion] = make_pair(minimo, maximo);
}

void Solucion::reubicar_producto(int camion_origen, int posicion_producto, int camion_destino) { 
    const int indice_producto = _camiones[camion_origen][posicion_producto];
    const double peso = _entrada->get_productos()[indice_producto].first;
    const double temperatura = _entrada->get_productos()[indice_producto].second;

    double similitud_origen = 0.0;
    const vector<int>& productos_origen = _camiones[camion_origen];
    for (size_t posicion = 0; posicion < productos_origen.size(); ++posicion) {
        if (static_cast<int>(posicion) != posicion_producto) {
            similitud_origen += _entrada->get_similitud(
                indice_producto, productos_origen[posicion]);
        }
    }

    double similitud_destino = 0.0;
    for (int otro_producto : _camiones[camion_destino]) {
        similitud_destino += _entrada->get_similitud(indice_producto, otro_producto);
    }
    _suma_similitudes += similitud_destino - similitud_origen;

    // Agregar el producto al destino y actualizar sus datos asociados.
    _camiones[camion_destino].push_back(indice_producto);
    _espacio_camiones[camion_destino] -= peso;
    _rango_temperaturas[camion_destino].first =
        min(_rango_temperaturas[camion_destino].first, temperatura);
    _rango_temperaturas[camion_destino].second =
        max(_rango_temperaturas[camion_destino].second, temperatura);

    // Quitar el producto del origen y devolver su peso disponible.
    _camiones[camion_origen].erase(
        _camiones[camion_origen].begin() + posicion_producto);
    _espacio_camiones[camion_origen] += peso;

    // Un camion sin productos deja de formar parte de la solucion.
    if (_camiones[camion_origen].empty()) {
        _camiones.erase(_camiones.begin() + camion_origen);
        _espacio_camiones.erase(_espacio_camiones.begin() + camion_origen);
        _rango_temperaturas.erase(_rango_temperaturas.begin() + camion_origen);
        --_k;
    } else {
        recalcular_rango_temperaturas(camion_origen);
    }
}

void Solucion::intercambiar_productos(int camion_a, int posicion_a, int camion_b, int posicion_b) {

    const vector<pair<double, double>>& productos = _entrada->get_productos();
    const vector<vector<double>>& similitudes = _entrada->get_similitudes();

    const int indice_producto_a = _camiones[camion_a][posicion_a];
    const int indice_producto_b = _camiones[camion_b][posicion_b];

    const double peso_a = productos[indice_producto_a].first;
    const double peso_b = productos[indice_producto_b].first;

    // Calculo las similitudes que se pierden y se incorporan en cada camion.
    double similitud_a_origen = 0.0;
    double similitud_b_origen = 0.0;
    double similitud_b_destino = 0.0;
    double similitud_a_destino = 0.0;

    for (int producto : _camiones[camion_a]) {
        if (producto != indice_producto_a) {
            similitud_a_origen += similitudes[indice_producto_a][producto];
            similitud_b_origen += similitudes[indice_producto_b][producto];
        }
    }

    for (int producto : _camiones[camion_b]) {
        if (producto != indice_producto_b) {
            similitud_b_destino += similitudes[indice_producto_b][producto];
            similitud_a_destino += similitudes[indice_producto_a][producto];
        }
    }

    _suma_similitudes += similitud_b_origen + similitud_a_destino
                       - similitud_a_origen - similitud_b_destino;

    
    std::swap(_camiones[camion_a][posicion_a], _camiones[camion_b][posicion_b]);

    // Actualizo el espacio disponible de ambos camiones.
    _espacio_camiones[camion_a] += peso_a - peso_b;
    _espacio_camiones[camion_b] += peso_b - peso_a;
    
    recalcular_rango_temperaturas(camion_a);
    recalcular_rango_temperaturas(camion_b);
}

double Solucion::calcular_objetivo_con_componentes(int cantidad_camiones, double suma_similitudes) const {
    const double alfa = 1.0;
    const double beta = 1.0;
    return alfa * static_cast<double>(cantidad_camiones) - beta * suma_similitudes;
}

void Solucion::funcion_objetivo() {
    _objetivo = calcular_objetivo_con_componentes(_k, _suma_similitudes);
}

double Solucion::evaluar_objetivo_reubicacion(int camion_origen, int posicion_producto, int camion_destino) const {
    const vector<int>& origen = _camiones[camion_origen];
    const vector<int>& destino = _camiones[camion_destino];
    const int indice_producto = origen[posicion_producto];

    double similitud_origen = 0.0;
    for (size_t posicion = 0; posicion < origen.size(); ++posicion) {
        if (static_cast<int>(posicion) != posicion_producto) {
            similitud_origen += _entrada->get_similitud(
                indice_producto, origen[posicion]);
        }
    }

    double similitud_destino = 0.0;
    for (int otro_producto : destino) {
        similitud_destino += _entrada->get_similitud(indice_producto, otro_producto);
    }

    // Si el producto era el unico del origen, ese camion desaparece.
    const int nuevo_k = _k - (origen.size() == 1 ? 1 : 0);
    const double nueva_suma_similitudes =
        _suma_similitudes - similitud_origen + similitud_destino;

    return calcular_objetivo_con_componentes(nuevo_k, nueva_suma_similitudes);
}

double Solucion::evaluar_objetivo_intercambio(int camion_a, int posicion_a, int camion_b, int posicion_b) const {
    const vector<int>& productos_a = _camiones[camion_a];
    const vector<int>& productos_b = _camiones[camion_b];
    const int indice_producto_a = productos_a[posicion_a];
    const int indice_producto_b = productos_b[posicion_b];

    double similitud_a_antes = 0.0;
    double similitud_a_despues = 0.0;

    for (size_t posicion = 0; posicion < productos_a.size(); ++posicion) {
        if (static_cast<int>(posicion) != posicion_a) {
            const int otro_producto = productos_a[posicion];

            similitud_a_antes += _entrada->get_similitud(indice_producto_a, otro_producto);
            similitud_a_despues += _entrada->get_similitud(indice_producto_b, otro_producto);
        }
    }

    double similitud_b_antes = 0.0;
    double similitud_b_despues = 0.0;

    for (size_t posicion = 0; posicion < productos_b.size(); ++posicion) {
        if (static_cast<int>(posicion) != posicion_b) {
            const int otro_producto = productos_b[posicion];

            similitud_b_antes += _entrada->get_similitud(indice_producto_b, otro_producto);
            similitud_b_despues += _entrada->get_similitud(indice_producto_a, otro_producto);
        }
    }

    const double nueva_suma_similitudes = _suma_similitudes - similitud_a_antes - similitud_b_antes + similitud_a_despues + similitud_b_despues;

    return calcular_objetivo_con_componentes(_k, nueva_suma_similitudes);
}

int Solucion::get_K() const {
    return _k;
}

const vector<vector<int>>& Solucion::get_camiones() const {
    return _camiones;
}

const vector<double>& Solucion::get_espacio_camiones() const {
    return _espacio_camiones;
}

const vector<pair<double, double>>& Solucion::get_rango_temperaturas() const {
    return _rango_temperaturas;
}

double Solucion::get_objetivo() const {
    return _objetivo;
}

double Solucion::get_suma_similitudes() const {
    return _suma_similitudes;
}
