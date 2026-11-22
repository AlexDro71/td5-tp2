#ifndef SOLUCION_H
#define SOLUCION_H

#include <bits/stdc++.h>

using namespace std;

class Entrada;

class Solucion {
private:
    vector<vector<int>> _camiones;
    int _k;
    double _capacidad_camion;
    const Entrada* _entrada;
    double _objetivo;
    vector<double> _espacio_camiones;
    vector<pair<double, double>> _rango_temperaturas;

public:
    explicit Solucion(const Entrada& entrada);

    void agregar_camion();
    // La heuristica verifica Tmax antes de llamar a este metodo.
    // El metodo agrega el producto y actualiza el rango termico del camion.
    void agregar_a_camion(int i, int indice_producto, double peso);
    void funcion_objetivo();

    int get_K() const;
    const vector<vector<int>>& get_camiones() const;
    const vector<double>& get_espacio_camiones() const;
    const vector<pair<double, double>>& get_rango_temperaturas() const;
    double get_objetivo() const;
};

#endif // SOLUCION_H
