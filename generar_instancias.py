
import random
from pathlib import Path

def generar_instancia(n, capacidad=20, semilla=42):
    random.seed(semilla)

    productos = []
    for _ in range(n):
        peso = random.randint(1, 10)
        temperatura = random.choice([-18, 4, 20])
        productos.append((peso, temperatura))

    similitudes = [[0.0] * n for _ in range(n)]

    for i in range(n):
        similitudes[i][i] = 1.0

        for j in range(i + 1, n):
            valor = round(random.uniform(0, 1), 2)
            similitudes[i][j] = valor
            similitudes[j][i] = valor

    ruta = Path(f"input/instancia_{n}.txt")
    ruta.parent.mkdir(exist_ok=True)

    with ruta.open("w") as archivo:
        archivo.write(f"{n} {capacidad}\n")

        for peso, temperatura in productos:
            archivo.write(f"{peso} {temperatura}\n")

        for fila in similitudes:
            archivo.write(" ".join(map(str, fila)) + "\n")

    print(f"Generada: {ruta}")


for cantidad in [10, 50, 100, 500]:
    generar_instancia(cantidad)
