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
    this->populacao.push_back(Individuo(caminho, 0.0)); // salvaproxima_geracao
  }

  return true;
}


void AlgoritmoGenetico::mostrar_populacao() {
  std::cout << "\nPopulação:\n";
  for(const Individuo individuo : populacao) {
    for(const auto cidade_id : individuo.caminho) {
      std::cout << cidade_id << " ";
    }
    std::cout << "| Fitness: " << individuo.fitness;
    std::cout << "\n";
  }
}


void AlgoritmoGenetico::mostrar_top_n(int n) {
  std::cout << "\nTop 5 indivíduos:\n";
  for(Individuo& individuo : populacao) {
    individuo.fitness =  get_distancia_individuo(individuo);
  }


  std::sort(populacao.begin(), populacao.end(),
            [this](const Individuo& a, const Individuo& b) {
              return a.fitness < b.fitness;
            });

  int cont = 0;
  for(const Individuo individuo : populacao) {
    for(const auto cidade_id : individuo.caminho) {
      std::cout << cidade_id << " ";
    }
    std::cout << "| Fitness: " << individuo.fitness;
    std::cout << "\n";
    cont++;
    if(cont >= n) break;
  }

}


double AlgoritmoGenetico::get_distancia_individuo(Individuo& individuo) {
  double distancia_total = 0.0;

  for(size_t i = 0; i < individuo.caminho.size() - 1; i++) {
    unsigned int cidade_atual_id = individuo.caminho[i];
    unsigned int proxima_cidade_id = individuo.caminho[i + 1];

    const Cidade& cidade_atual = std::next(cidades.begin(), cidade_atual_id)->second;

    if(cidade_atual_id > proxima_cidade_id) {
      distancia_total += cidade_atual.distancias_para_cidades.at(proxima_cidade_id);
    } else if(cidade_atual_id < proxima_cidade_id) {
      const Cidade& proxima_cidade = std::next(cidades.begin(), proxima_cidade_id)->second;
      distancia_total += proxima_cidade.distancias_para_cidades.at(cidade_atual_id);
    }
  }

  return distancia_total;
}


Individuo AlgoritmoGenetico::get_cruzamento(Individuo& pai, Individuo& mae) {
  std::vector<unsigned int> caminho_filho;


  /* biblioteca para gerar números aleatórios */
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, pai.caminho.size()-2);
  int cidade_a_modificar = dis(gen);
  int cidade_antecessora = dis(gen);

  for(int i = 1; i <= pai.caminho.size()-1; i++) {
    if(pai.caminho[i] == cidade_a_modificar) {
      cidade_antecessora = pai.caminho[i - 1];
    }
  }


  /* coloca a cidade x da mãe afrente da cidade da qual ela sucede no pai */
  for(int i = 0; i <= mae.caminho.size()-2; i++) {
    if(mae.caminho[i] == cidade_a_modificar) continue;
    caminho_filho.push_back(mae.caminho[i]);
    if(mae.caminho[i] == cidade_antecessora) caminho_filho.push_back(cidade_a_modificar);
  }
  caminho_filho.push_back(caminho_filho[0]); // volta à cidade inicial


  return Individuo(caminho_filho, 0.0);
}


Individuo AlgoritmoGenetico::get_mutacao(Individuo& individuo) {
  
  /* biblioteca para números aleatórios*/
  std::vector<unsigned int> caminho_mutado = individuo.caminho;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, individuo.caminho.size()-2); // não troca a última cidade (igual à primeira)


  /* escolhe aleatoriamente as posições a mudar*/
  int idx1 = dis(gen);
  int idx2 = dis(gen);
  while(idx1 == idx2) {
    idx2 = dis(gen);
  }
  std::swap(caminho_mutado[idx1], caminho_mutado[idx2]);
  if(idx1 == 0) {
    caminho_mutado[caminho_mutado.size() - 1] = caminho_mutado[0];
  } else if(idx2 == 0) {
    caminho_mutado[caminho_mutado.size() - 1] = caminho_mutado[0];
  }

  return Individuo(caminho_mutado, 0.0);
}


void AlgoritmoGenetico::proxima_geracao() {

  std::vector<Individuo> nova_geracao;
  
  size_t num_de_elitizados = 2;
  std::vector<Individuo> individuos_elitizados;
  for(size_t i = 0; i < num_de_elitizados; i++) {
    individuos_elitizados.push_back(populacao[i]);
  };


  /* avalia população */
  double fitness_total = 0.0;
  for(Individuo& individuo : populacao) {
    individuo.fitness =  1 / get_distancia_individuo(individuo);
    fitness_total += individuo.fitness;

    /* salva os melhores indivíduos */
    Individuo copy = individuo;
    for(Individuo& melhor_individuo : individuos_elitizados) {
      if(copy.fitness > melhor_individuo.fitness) {
        Individuo c = copy;
        copy = melhor_individuo;
        melhor_individuo = c;
        break;
      }
    }
  }
  
  // mostrar_populacao();


  
  /* insere a elite */
  for(size_t i = 0; i < num_de_elitizados; i++) {
    nova_geracao.push_back(individuos_elitizados[i]);
  }

  
  /* cria roleta */
  std::vector<double> roleta;
  double acumulado = 0.0;
  for(Individuo& individuo : populacao) {
    double chances = (individuo.fitness / fitness_total);
    acumulado += chances;
    // std::cout << "\n acumulado: " << acumulado;
    roleta.push_back(acumulado);
  }

  /* coloca novos indivíduos de cruzamentos */
  for(int i = num_de_elitizados; i < populacao.size(); i++) {
    /* biblioteca para gerar números aleatórios */
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);


    double sorteio1 = dis(gen);
    double sorteio2 = dis(gen);
    Individuo sorteio1_individuo = populacao[0];
    Individuo sorteio2_individuo = populacao[0];
    int cont = 0;
    for(const double valor_roleta : roleta) {
      if(sorteio1 >= valor_roleta) sorteio1_individuo = populacao[cont];
      if(sorteio2 >= valor_roleta) sorteio2_individuo = populacao[cont];
      cont++;
    }
    // std:: cout << "\n sorteio1: " << sorteio1 << " | sorteio2: " << sorteio2 << "  |  sorteados: " << sorteio1_individuo.caminho[0] << " e " << sorteio2_individuo.caminho[0];
    Individuo filho = get_cruzamento(sorteio1_individuo, sorteio2_individuo);


    /* mutação */
    while(true) {
      if(dis(gen) < 0.05) filho = get_mutacao(filho);
      else break;
    }

    nova_geracao.push_back(filho);
    
  }
  this->populacao = nova_geracao;
    
}


bool AlgoritmoGenetico::run() {
  gerar_populacao_inicial();


  for(int i = 0; i < 20000; i++) {
    if(i % 1000 == 0) std::cout << "\nGeração " << i;
    proxima_geracao();
  }
  mostrar_top_n(20);
  return true;
}