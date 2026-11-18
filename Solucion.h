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
    bool _objetivo_calculado;
    vector<double> _espacio_camiones;

public:
    explicit Solucion(const Entrada& entrada);

    void agregar_camion();
    void agregar_a_camion(int i, int indice_producto, double peso);
    void funcion_objetivo();

    int get_K() const;
    const vector<vector<int>>& get_camiones() const;
    const vector<double>& get_espacio_camiones() const;
    double get_objetivo() const;
};

#endif // SOLUCION_H
