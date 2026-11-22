#include <bits/stdc++.h>
#include "Solucion.h"
#include "Entrada.h"

using namespace std;

Solucion::Solucion(const Entrada& e)
    : _camiones(1),
      _k(1),
      _capacidad_camion(e.get_capacidad()),
      _entrada(&e),
      _objetivo(-1),
      _espacio_camiones(1, e.get_capacidad()),
      _rango_temperaturas(1, make_pair(0.0, 0.0)) {}

void Solucion::agregar_camion() {
    vector<int> nuevo;
    _camiones.push_back(nuevo);
    _espacio_camiones.push_back(_capacidad_camion);
    _rango_temperaturas.push_back(make_pair(0.0, 0.0));
    _k++;
}

void Solucion::agregar_a_camion(int i, int indice_producto, double peso) {
    const double temperatura = _entrada->get_productos()[indice_producto].second;

    _camiones[i].push_back(indice_producto);
    _espacio_camiones[i] -= peso;

    // Si es el primer producto, la temperatura minima y maxima coinciden con su temperatura.
    if (_camiones[i].size() == 1) {
        _rango_temperaturas[i] = make_pair(temperatura, temperatura);
    } else {
        _rango_temperaturas[i].first = min(_rango_temperaturas[i].first, temperatura);
        _rango_temperaturas[i].second = max(_rango_temperaturas[i].second, temperatura);
    }
}

void Solucion::funcion_objetivo() {
    double alfa = 1.0;
    double beta = 1.0;
    double suma_similitudes = 0.0;

    for (const auto& camion : _camiones) {
        for (size_t i = 0; i < camion.size(); ++i) {
            for (size_t j = i + 1; j < camion.size(); ++j) {
                suma_similitudes += _entrada->get_similitud(camion[i], camion[j]);
            }
        }
    }

    _objetivo = alfa * static_cast<double>(_k) - beta * suma_similitudes;
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
