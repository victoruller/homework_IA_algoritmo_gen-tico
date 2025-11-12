#include "algoritmo_genetico.hpp"

#include <vector>
#include <map>
#include <algorithm>

/// @brief Cria o algoritmo genético contendo 5 cidades gratuitas
AlgoritmoGenetico::AlgoritmoGenetico() {
  this->cidades = std::map<std::string, Cidade>({});
  addCidade("Itajai",              {});
  addCidade("Balneario Camborio",  {{0, 30.0}});
  addCidade("Blumenau",            {{0, 60.0}, {1, 40.0}});
  addCidade("Itapema",             {{0, 20.0}, {1, 10.0}, {2, 50.0}});
  addCidade("Joinville",           {{0, 90.0}, {1, 70.0}, {2, 30.0}, {3, 80.0}});
}

/// @brief Cria o algoritmo genético contendo as cidades de entrada
AlgoritmoGenetico::AlgoritmoGenetico(char** lista_distancias) {
  
  int cont = 0;
  for(char** arg = lista_distancias; *arg != nullptr;) {
    std::map<size_t, double> distancias_para_cidades;

    for(int i = 0; i < cont; i++){
      double distancia = std::stod(*arg);
      distancias_para_cidades.emplace(i, distancia);
      arg++;
    }
    this->addCidade("Cidade_" + std::to_string(cont), distancias_para_cidades);
    cont++;
  }
}

/// @brief Adiciona uma cidade ao conjunto de cidades, bem como suas distâncias para as outras cidades.
///
/// - as distâncias são armazenas apenas as distâncias da cidade de maior índice para as de menor índice.
/// @param nome 
/// @param distancias_para_cidades 
/// @return 
bool AlgoritmoGenetico::addCidade(std::string nome,  std::map<size_t, double> distancias_para_cidades) {
  if (cidades.find(nome) != cidades.end()) {
    return false; // Cidade já existe
  }
  /* cria cidade */
  size_t index = cidades.size();
  cidades.emplace(nome, Cidade(nome, index, distancias_para_cidades));

  return true;
}

bool AlgoritmoGenetico::removeCidade(std::string nome) {
  auto cidade_iterator = cidades.find(nome);
  if (cidade_iterator == cidades.end()) {
    return false; // Cidade não existe
  }
  int index = cidade_iterator->second.index;
  cidades.erase(nome);

  for(auto& [f_index, f_cidade] : cidades) {
    if(f_cidade.index > index) f_cidade.distancias_para_cidades.erase(index);
  }


  return true;
}

/// @brief Mostra as cidades armazenadas e suas distâncias.
void AlgoritmoGenetico::mostrarCidades() {
  std::cout << "\nIndex\tCidade\tDistancias\n";

  std::vector<std::pair<std::string, Cidade>> cidades_ordenadas(this->cidades.begin(), this->cidades.end());
  std::sort(cidades_ordenadas.begin(), cidades_ordenadas.end(),
            [](const std::pair<std::string, Cidade>& a, const std::pair<std::string, Cidade>& b) {
              return a.second.index < b.second.index;
            });
  for(const auto cidade : cidades_ordenadas) {
    std::cout << "\n" << cidade.second.index << "\t" << cidade.first << " --> ";
    
    std::vector<std::pair<size_t, double>> distancias_ordenadas(cidade.second.distancias_para_cidades.begin(), cidade.second.distancias_para_cidades.end());
    std::sort(distancias_ordenadas.begin(), distancias_ordenadas.end(),
      [](const std::pair<size_t, double>& a, const std::pair<size_t, double>& b) {
        return a.first < b.first;
      });

    for(const auto distancia : distancias_ordenadas) {
      std::cout << distancia.second << " ";
    }
  }
}
