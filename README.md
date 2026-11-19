compilar: g++ -std=c++17 -Wall -Wextra -pedantic -O2 main.cpp Entrada.cpp Solucion.cpp Heuristicas.cpp Operadores.cpp Metaheuristica.cpp -o programa.exe

en programa se puede poner cualquier cosa

ejecutar alguna heuristica con input "in" con salida en output "out": .\programa.exe input/in.txt output/out.txt --Heuristica nombre_heuristica

ejecutar con heuristica y operador: .\programa.exe input/in.txt output/out.txt --Heuristica nombre_heuristica --Operador nombre_operador

ejecutar metaheuristica: .\programa.exe input/in.txt output/out.txt --Metaheuristica nombre_metaheuristica

Cualquiera de los terminos de la ejecucion pueden ser puestos en culquier orden siempre que resptene su sufijo(como input/ o --Operador)