// compile: g++ -o main main.cpp Algoritmo_genetico.cpp -lm
// run: ./main 281	338	61	299	43	42	398	357	300	339	351	330	273	312	102	448	263	206	246	154	236


#include <iostream>
#include "algoritmo_genetico.hpp"


bool validar_entrada(int argc, char** argv) {
  // Verifica se o número de argumentos é válido
  int num_distancias = argc - 1;
  int num_cidades = 0;
  int distancias_percorridas = 0;
  
  for(int i = 0; i <= num_distancias; i++) {
    distancias_percorridas += i;
    if(distancias_percorridas < num_distancias) {
      num_cidades++;
      continue;
    } else if(distancias_percorridas == num_distancias) {
      num_cidades++;
      break;
    } else {
      std::cout << "Número inválido de distâncias para as cidades." << std::endl;
      return false;
    }
  }

  

  return true;
}

int main(int argc, char** argv) {
  

  if(validar_entrada(argc, argv)) {
    std::cout << "Entrada válida!" << std::endl;
  } else {
    std::cout << "Entrada inválida!" << std::endl;
    return 1;
  }

  AlgoritmoGenetico ag;
  if(argc == 1) {
    ag = AlgoritmoGenetico();
  } else {
    ag = AlgoritmoGenetico(argv + 1);
  }


  
  ag.mostrarCidades();
  // ag.removeCidade("Balneario Camborio");
  // ag.mostrarCidades();
  // ag.removeCidade("Blumenau");
  // ag.mostrarCidades();
  // ag.removeCidade("Itajai");
  // ag.mostrarCidades();
  
  
}