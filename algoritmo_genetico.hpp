#include <iostream>

#include <vector>
#include <map>


#ifndef ALGORITMO_GENETICO_HPP
#define ALGORITMO_GENETICO_HPP

class Cidade {
public:
  std::string nome;
  size_t index;
  std::map<size_t, double> distancias_para_cidades;


  Cidade(std::string nome, size_t index, std::map<size_t, double> distancias_para_cidades)
      : nome(nome), index(index), distancias_para_cidades(distancias_para_cidades) {};
};

class Individuo {
public:
  Individuo(std::vector<unsigned int> caminho, double fitness)
      : caminho(caminho), fitness(fitness) {};

  std::vector<unsigned int> caminho;
  double fitness;
};


class AlgoritmoGenetico {
public:
  AlgoritmoGenetico();
  AlgoritmoGenetico(char** lista_distancias);


  virtual bool      add_cidade   (const std::string nome, std::map<size_t, double> distancias_para_cidades);
  virtual bool      remove_cidade(const std::string nome);
  virtual void      mostrar_cidades();

  virtual bool      run(); // to do
  virtual bool      gerar_populacao_inicial();
  virtual void      mostrar_populacao();
  virtual void      mostrar_top_n(int n);
  virtual void      proxima_geracao();
  virtual double    get_distancia_individuo(Individuo& individuo);
  virtual Individuo get_cruzamento(Individuo& pai, Individuo& mae);
  virtual Individuo get_mutacao(Individuo& individuo);

private:

  std::map<std::string, Cidade> cidades;
  std::vector<Individuo> populacao;
};
#endif // ALGORITMO_GENETICO_HPP
