
import random
from pathlib import Path

CARPETA = Path("input/experimentos")
CARPETA.mkdir(parents=True, exist_ok=True)

CAPACIDAD = 20
TAMANIO = 50

ESCENARIOS = [
    "similitud_baja",
    "similitud_alta",
    "similitud_grupos",
    "temperaturas_cercanas",
    "temperaturas_extremas",
    "productos_pesados",
    "productos_livianos",
    "ajuste_exacto",
]


def generar_productos(n, escenario, rng):
    productos = []

    for i in range(n):
        peso = rng.randint(1, 10)
        temperatura = 4

        if escenario == "temperaturas_cercanas":
            temperatura = rng.randint(2, 6)

        elif escenario == "temperaturas_extremas":
            temperatura = rng.choice([-18, 4, 20])

        elif escenario == "productos_pesados":
            peso = rng.randint(11, 20)

        elif escenario == "productos_livianos":
            peso = rng.randint(1, 3)

        elif escenario == "ajuste_exacto":
            # Los productos forman pares de 12 y 8 kg
            peso = 12 if i % 2 == 0 else 8

        productos.append((peso, temperatura))

    return productos


def generar_similitudes(n, escenario, rng):
    matriz = [[0.0] * n for _ in range(n)]

    for i in range(n):
        matriz[i][i] = 1.0

        for j in range(i + 1, n):

            if escenario == "similitud_baja":
                valor = rng.uniform(0, 0.4)

            elif escenario == "similitud_alta":
                valor = rng.uniform(0.6, 1)

            elif escenario == "similitud_grupos":
                # Tres grupos con alta similitud interna
                if i % 3 == j % 3:
                    valor = rng.uniform(0.7, 1)
                else:
                    valor = rng.uniform(0, 0.3)

            else:
                valor = rng.uniform(0, 1)

            valor = round(valor, 2)
            matriz[i][j] = valor
            matriz[j][i] = valor

    return matriz


def guardar_instancia(ruta, productos, similitudes):
    n = len(productos)

    with ruta.open("w", encoding="utf-8") as archivo:
        archivo.write(f"{n} {CAPACIDAD}\n")

        for peso, temperatura in productos:
            archivo.write(f"{peso} {temperatura}\n")

        for fila in similitudes:
            archivo.write(" ".join(map(str, fila)) + "\n")


def generar(n, escenario, semilla, nombre):
    rng = random.Random(semilla)

    productos = generar_productos(n, escenario, rng)
    similitudes = generar_similitudes(n, escenario, rng)

    ruta = CARPETA / f"{nombre}.txt"
    guardar_instancia(ruta, productos, similitudes)

    print(f"Generada: {ruta}")


# Tres repeticiones por escenario
for numero, escenario in enumerate(ESCENARIOS):
    for repeticion in range(1, 4):
        semilla = 1000 + numero * 10 + repeticion

        generar(
            TAMANIO,
            escenario,
            semilla,
            f"{escenario}_r{repeticion}"
        )


# Caso extremo: un solo producto
generar(1, "normal", 999, "un_producto")


# Caso extremo invalido: producto que no entra
ruta = CARPETA / "producto_imposible.txt"

with ruta.open("w", encoding="utf-8") as archivo:
    archivo.write("1 20\n")
    archivo.write("25 4\n")
    archivo.write("1\n")

print(f"Generada: {ruta} (caso invalido)")
