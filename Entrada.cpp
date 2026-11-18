#include <bits/stdc++.h>
#include "Entrada.h"

using namespace std;

vector<vector<double>> crear_matriz(int n) {
    vector<vector<double>> matriz(n, vector<double>(n, 0));
    return matriz;
}

Entrada::Entrada(int cantidad_productos, double capacidad_camion)
    : _n(cantidad_productos),
      _capacidad(capacidad_camion),
      _productos(),
      _similitudes(crear_matriz(cantidad_productos)) {
    _productos.reserve(static_cast<size_t>(cantidad_productos));
}

void Entrada::agregar_producto(double peso, double temperatura) {
    _productos.push_back(make_pair(peso, temperatura));
}

void Entrada::agregar_similitud(int i, int j, double valor) {
    _similitudes[i][j] = valor;
}

int Entrada::get_n() const {
    return _n;
}

double Entrada::get_capacidad() const {
    return _capacidad;
}

const vector<pair<double, double>>& Entrada::get_productos() const {
    return _productos;
}

const vector<vector<double>>& Entrada::get_similitudes() const {
    return _similitudes;
}

double Entrada::get_similitud(int i, int j) const {
    return _similitudes[i][j];
}
