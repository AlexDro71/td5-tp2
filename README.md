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

