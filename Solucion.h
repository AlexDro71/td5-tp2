#ifndef SOLUCION_H
#define SOLUCION_H

#include <bits/stdc++.h>

using namespace std;

class Entrada;

class Solucion {
private:
    vector<vector<int>> _camiones;
    int _k;
    const Entrada* _entrada;
    double _objetivo;
    double _suma_similitudes;
    vector<double> _espacio_camiones;
    vector<pair<double, double>> _rango_temperaturas;

    double calcular_objetivo_con_componentes(int cantidad_camiones,
                                             double suma_similitudes) const;

    void recalcular_rango_temperaturas(int indice_camion);

public:
    explicit Solucion(const Entrada& entrada);

    void agregar_camion();
    void agregar_a_camion(int i, int indice_producto, double peso);

    void reubicar_producto(int camion_origen, int posicion_producto, int camion_destino);
    void intercambiar_productos(int camion_a, int posicion_a, int camion_b, int posicion_b);
    double evaluar_objetivo_reubicacion(int camion_origen, int posicion_producto, int camion_destino) const;
    double evaluar_objetivo_intercambio(int camion_a, int posicion_a, int camion_b, int posicion_b) const;

    void funcion_objetivo();

    int get_K() const;
    const vector<vector<int>>& get_camiones() const;
    const vector<double>& get_espacio_camiones() const;
    const vector<pair<double, double>>& get_rango_temperaturas() const;
    double get_objetivo() const;
    double get_suma_similitudes() const;
};

#endif // SOLUCION_H
