compilar: g++ -std=c++17 -Wall -Wextra -pedantic -O2 main.cpp Entrada.cpp Solucion.cpp -o programa.exe

en programa se puede poner el nombre que uno desee.

Las siguientes son todas las opciones de ejecucion, para cada una es necesario definir el valor T_max
UMBS: .\programa.exe inputs/ejemplo.txt outputs/umbs.txt --Heuristica UMBS --T T_max

UMBS + RELOCATE: .\programa.exe inputs/ejemplo.txt outputs/umbs_relocate.txt --Heuristica UMBS --Operador RELOCATE --T T_max

UMBS + SWAP: .\programa.exe inputs/ejemplo.txt outputs/umbs_swap.txt --Heuristica UMBS --Operador SWAP --T T_max

APC: .\programa.exe inputs/ejemplo.txt outputs/apc.txt --Heuristica APC --T T_max

APC + RELOCATE: .\programa.exe inputs/ejemplo.txt outputs/apc_relocate.txt --Heuristica APC --Operador RELOCATE --T T_max

APC + SWAP: .\programa.exe inputs/ejemplo.txt outputs/apc_swap.txt --Heuristica APC --Operador SWAP --T T_max

ILS: .\programa.exe inputs/ejemplo.txt outputs/ils.txt --Metaheuristica ILS --T T_max

ILS + RELOCATE: .\programa.exe inputs/ejemplo.txt outputs/ils_relocate.txt --Metaheuristica ILS --Operador RELOCATE --T T_max

ILS + SWAP: .\programa.exe inputs/ejemplo.txt outputs/ils_swap.txt --Metaheuristica ILS --Operador SWAP --T T_max

4. Experimento 1: comparación de estrategias
1. Comprobar que los parámetros estén en sus valores iniciales:
   - mu = 0.5.
   - Tiempo ILS = 10 segundos.
   - Divisor de fuerza = 100.
   - Semilla = 12345.
2. Ejecutar:
py .\scripts_experimentos\experimento1.py


3. Esperar a que finalicen las 84 corridas.
5. Experimento 2: influencia del tipo y tamaño de instancia
1. Ejecutar después de finalizar el experimento 1:
py .\scripts_experimentos\experimento2.py


6. Experimento 3: variación de mu
Se ejecutan cuatro tandas. Para cada una, modificar el valor de umbral en main.cpp y el valor de EXPERIMENTO3_MU_ACTUAL en config_experimentos.py. Ambos valores deben coincidir.

Primera tanda:

En main.cpp:
double umbral = 0.25;


En config_experimentos.py:
EXPERIMENTO3_MU_ACTUAL = 0.25



Ejecutar:
py .\scripts_experimentos\experimento3.py


Segunda tanda:

En main.cpp:
double umbral = 0.5;


En config_experimentos.py:
EXPERIMENTO3_MU_ACTUAL = 0.5



Ejecutar:
py .\scripts_experimentos\experimento3.py


Tercera tanda:

En main.cpp:
double umbral = 0.7;


En config_experimentos.py:
EXPERIMENTO3_MU_ACTUAL = 0.7



Ejecutar:
py .\scripts_experimentos\experimento3.py


Cuarta tanda:

En main.cpp:
double umbral = 0.9;


En config_experimentos.py:
EXPERIMENTO3_MU_ACTUAL = 0.9



Ejecutar:
py .\scripts_experimentos\experimento3.py


Al finalizar, restaurar umbral = 0.5 en main.cpp y establecer nuevamente MU_ACTUAL = 0.5 y EXPERIMENTO3_MU_ACTUAL = 0.5 en config_experimentos.py.

Guardar ambos archivos.
7. Experimento 4: variación de Tmax
1. Comprobar que los parámetros estén en sus valores iniciales:
   - mu = 0.5.
   - Tiempo ILS = 10 segundos.
   - Divisor de fuerza = 100.
   - Semilla = 12345.
2. No modificar manualmente Tmax.
3. Ejecutar:
py .\scripts_experimentos\experimento4.py


4. Esperar a que finalicen las 126 corridas.
8. Experimento 5: tuning de ILS
Se ejecutan nueve tandas. En cada una se modifica un solo parámetro, manteniendo los otros dos en sus valores iniciales.

Mantener siempre Tmax = 10 y mu = 0.5.
8.1. Variación del tiempo
Mantener:
- Divisor de fuerza = 100.
- Semilla = 12345.
En config_experimentos.py, establecer:
EXPERIMENTO5_PARAMETRO_EN_PRUEBA = "TIEMPO"



Primera tanda: 5 segundos.

En main.cpp:
const double limite_segundos = 5.0;


En config_experimentos.py:
ILS_TIEMPO_SEGUNDOS_ACTUAL = 5.0ILS_FUERZA_DIVISOR_ACTUAL = 100ILS_SEMILLA_ACTUAL = 12345



Ejecutar:
py .\scripts_experimentos\experimento5.py


Segunda tanda: 10 segundos.

En main.cpp:
const double limite_segundos = 10.0;


En config_experimentos.py:
ILS_TIEMPO_SEGUNDOS_ACTUAL = 10.0



Ejecutar:
py .\scripts_experimentos\experimento5.py


Tercera tanda: 20 segundos.

En main.cpp:
const double limite_segundos = 20.0;


En config_experimentos.py:
ILS_TIEMPO_SEGUNDOS_ACTUAL = 20.0



Ejecutar:
py .\scripts_experimentos\experimento5.py


8.2. Variación de la fuerza
Restablecer:
- Tiempo = 10 segundos.
- Semilla = 12345.
En config_experimentos.py, establecer:
EXPERIMENTO5_PARAMETRO_EN_PRUEBA = "FUERZA"ILS_TIEMPO_SEGUNDOS_ACTUAL = 10.0ILS_SEMILLA_ACTUAL = 12345



Primera tanda: divisor 200.

En main.cpp:
max(2, entrada.get_n() / 200)


En config_experimentos.py:
ILS_FUERZA_DIVISOR_ACTUAL = 200



Ejecutar:
py .\scripts_experimentos\experimento5.py


Segunda tanda: divisor 100.

En main.cpp:
max(2, entrada.get_n() / 100)


En config_experimentos.py:
ILS_FUERZA_DIVISOR_ACTUAL = 100



Ejecutar:
py .\scripts_experimentos\experimento5.py


Tercera tanda: divisor 50.

En main.cpp:
max(2, entrada.get_n() / 50)


En config_experimentos.py:
ILS_FUERZA_DIVISOR_ACTUAL = 50



Ejecutar:
py .\scripts_experimentos\experimento5.py


8.3. Variación de la semilla
Restablecer:
- Tiempo = 10 segundos.
- Divisor de fuerza = 100.
En config_experimentos.py, establecer:
EXPERIMENTO5_PARAMETRO_EN_PRUEBA = "SEMILLA"ILS_TIEMPO_SEGUNDOS_ACTUAL = 10.0ILS_FUERZA_DIVISOR_ACTUAL = 100



Primera tanda: semilla 12345.

En main.cpp:
mt19937 rng(12345);


En config_experimentos.py:
ILS_SEMILLA_ACTUAL = 12345



Ejecutar:
py .\scripts_experimentos\experimento5.py


Segunda tanda: semilla 23451.

En main.cpp:
mt19937 rng(23451);


En config_experimentos.py:
ILS_SEMILLA_ACTUAL = 23451



Ejecutar:
py .\scripts_experimentos\experimento5.py


Tercera tanda: semilla 34512.

En main.cpp:
mt19937 rng(34512);


En config_experimentos.py:
ILS_SEMILLA_ACTUAL = 34512



Ejecutar:
py .\scripts_experimentos\experimento5.py


9. Restauración de los parámetros
1. Abrir main.cpp y establecer:
double umbral = 0.5;

const double limite_segundos = 10.0;

mt19937 rng(12345);

max(2, entrada.get_n() / 100)


2. Abrir scripts_experimentos/config_experimentos.py y establecer:
TMAX_BASE = 10.0MU_ACTUAL = 0.5ILS_TIEMPO_SEGUNDOS_ACTUAL = 10.0ILS_FUERZA_DIVISOR_ACTUAL = 100ILS_SEMILLA_ACTUAL = 12345EXPERIMENTO3_MU_ACTUAL = 0.5EXPERIMENTO5_PARAMETRO_EN_PRUEBA = "TIEMPO"RANDOM_TUNING = "random_1"



3. Guardar ambos archivos.
