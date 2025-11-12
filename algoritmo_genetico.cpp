#include "algoritmo_genetico.hpp"

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>
#include <initializer_list>
#include <random>


/* --- CONTEXTO DE CIDADES --- */

/// @brief Cria o algoritmo genético contendo 5 cidades gratuitas
AlgoritmoGenetico::AlgoritmoGenetico() {
  this->cidades = std::map<std::string, Cidade>({});
  add_cidade("Itajai",              {});
  add_cidade("Balneario Camborio",  {{0, 30.0}});
  add_cidade("Blumenau",            {{0, 60.0}, {1, 40.0}});
  add_cidade("Itapema",             {{0, 20.0}, {1, 10.0}, {2, 50.0}});
  add_cidade("Joinville",           {{0, 90.0}, {1, 70.0}, {2, 30.0}, {3, 80.0}});
}


/// @brief Cria o algoritmo genético contendo as cidades informadas via linha de comando
AlgoritmoGenetico::AlgoritmoGenetico(char** lista_distancias) {
  
  int cont = 0;
  for(char** arg = lista_distancias; *arg != nullptr;) {
    std::map<size_t, double> distancias_para_cidades;

    for(int i = 0; i < cont; i++){
      double distancia = std::stod(*arg);
      distancias_para_cidades.emplace(i, distancia);
      arg++;
    }
    this->add_cidade("Cidade_" + std::to_string(cont), distancias_para_cidades);
    cont++;
  }
}


/// @brief Adiciona uma cidade ao conjunto de cidades, bem como suas distâncias para as outras cidades.
///
/// - as distâncias são armazenas apenas as distâncias da cidade de maior índice para as de menor índice.
/// @param nome 
/// @param distancias_para_cidades 
/// @return 
bool AlgoritmoGenetico::add_cidade(std::string nome,  std::map<size_t, double> distancias_para_cidades) {
  if (cidades.find(nome) != cidades.end()) {
    return false; // Cidade já existe
  }
  /* cria cidade */
  size_t index = cidades.size();
  cidades.emplace(nome, Cidade(nome, index, distancias_para_cidades));

  return true;
}


/// @brief  remove cidade e todas as referências a ela nas outras cidades
/// @param nome 
/// @return 
bool AlgoritmoGenetico::remove_cidade(std::string nome) {
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
void AlgoritmoGenetico::mostrar_cidades() {
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


/* --- ALGORÍTMO GENÉTICO ---*/

long unsigned int get_fatorial(long unsigned int n) {
  if(n == 0 || n == 1) return 1;
  return n * get_fatorial(n - 1);
}


bool AlgoritmoGenetico::gerar_populacao_inicial() {

  /* calcula tamanho da população (10% do fatorial) */
  long unsigned int num_cidades    = cidades.size();
  long unsigned int possibilidades = get_fatorial(num_cidades);
  long unsigned int max_cidades    = std::numeric_limits<unsigned int>::max();

  unsigned int tam_populacao = std::min(possibilidades / 10 + (possibilidades % 10 != 0), max_cidades);

  std::cout << "\nNúmero de cidades:    " << num_cidades;
  std::cout << "\nTamanho da população: " << tam_populacao;


  /* cria lista com id das cidades (para usar nas populações)*/
  std::vector<std::pair<std::string, Cidade>> cidades_copy(this->cidades.begin(), this->cidades.end());
  std::vector<unsigned int> cidades_ids;
  for(std::pair<std::string, Cidade> cidade : cidades_copy) {
    cidades_ids.push_back(cidade.second.index);
  }


  /* ordena lista aleatoriamente e salva como uma indivíduo da população */

  std::random_device rd;
  std::mt19937 g(rd()); // Gerador de números pseudo-aleatórios

  for(unsigned int i = 0; i < tam_populacao; i++) {

    std::shuffle(cidades_ids.begin(), cidades_ids.end(), g); // embaralha

    std::vector<unsigned int> caminho = cidades_ids;
    caminho.push_back(caminho.at(0));   // volta à cidade inicial
    this->populacao.push_back(caminho); // salva
  }

  return true;
}


void AlgoritmoGenetico::mostrar_populacao() {
  std::cout << "\nPopulação:\n";
  for(const auto individuo : populacao) {
    for(const auto cidade_id : individuo) {
      std::cout << cidade_id << " ";
    }
    std::cout << "\n";
  }
}
