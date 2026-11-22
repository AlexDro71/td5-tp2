
from pathlib import Path
import sys

def validar(ruta_instancia, ruta_solucion, tmax):
    datos = Path(ruta_instancia).read_text().split()
    n = int(datos[0])
    capacidad = float(datos[1])

    productos = []
    posicion = 2

    for _ in range(n):
        peso = float(datos[posicion])
        temperatura = float(datos[posicion + 1])
        productos.append((peso, temperatura))
        posicion += 2

    lineas = Path(ruta_solucion).read_text().splitlines()
    camiones = [
        [int(x) for x in linea.split()]
        for linea in lineas
    ]

    errores = []
    asignados = []

    for k, camion in enumerate(camiones, start=1):
        if not camion:
            errores.append(f"Camion {k} vacio")
            continue

        for i in camion:
            if i < 0 or i >= n:
                errores.append(f"Producto inexistente: {i}")
            else:
                asignados.append(i)

        validos = [i for i in camion if 0 <= i < n]

        peso_total = sum(productos[i][0] for i in validos)
        temperaturas = [productos[i][1] for i in validos]

        if peso_total > capacidad + 1e-9:
            errores.append(f"Camion {k} supera la capacidad")

        if temperaturas and max(temperaturas) - min(temperaturas) > tmax + 1e-9:
            errores.append(f"Camion {k} supera Tmax")

    for i in range(n):
        if asignados.count(i) != 1:
            errores.append(
                f"Producto {i} aparece {asignados.count(i)} veces"
            )

    if errores:
        print("SOLUCION INVALIDA")
        for error in errores:
            print("-", error)
    else:
        print(f"SOLUCION VALIDA: {len(camiones)} camiones, {n} productos")

    return not errores


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Uso: python validar_soluciones.py INSTANCIA SOLUCION TMAX")
        sys.exit(1)

    correcta = validar(sys.argv[1], sys.argv[2], float(sys.argv[3]))
    sys.exit(0 if correcta else 1)
