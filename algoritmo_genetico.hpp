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


class AlgoritmoGenetico {
public:
  AlgoritmoGenetico();
  virtual bool addCidade(const std::string nome, std::map<size_t, double> distancias_para_cidades);
  virtual bool removeCidade(const std::string nome);
  virtual void mostrarCidades();

private:

  std::map<std::string, Cidade> cidades;
};
#endif // ALGORITMO_GENETICO_HPP
