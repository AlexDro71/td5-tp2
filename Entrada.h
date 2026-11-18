#ifndef ENTRADA_H
#define ENTRADA_H

#include <bits/stdc++.h>

using namespace std;

class Entrada {
private:
    int _n;
    double _capacidad;
    vector<pair<double, double>> _productos;
    vector<vector<double>> _similitudes;

public:
    Entrada(int cantidad_productos, double capacidad_camion);
    
    void agregar_producto(double peso, double temperatura);
    void agregar_similitud(int i, int j, double valor);

    int get_n() const;
    double get_capacidad() const;

    const vector<pair<double, double>>& get_productos() const;
    const vector<vector<double>>& get_similitudes() const;
    double get_similitud(int i, int j) const;
};

#endif // ENTRADA_H
