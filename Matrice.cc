#include "Matrice.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>
#include <stdexcept>
#include <algorithm>

Matrice::Matrice(std::string const & nom_fichier) {
    std::ifstream fichier(nom_fichier);
    if(!fichier.is_open()) {
        throw std::runtime_error("Impossible d'ouvrir le fichier : " + nom_fichier);
    }

    std::string ligne;
    while(std::getline(fichier, ligne)) {
        if(ligne.empty()) continue;
        if(ligne[0] != '%') break;
    }

    std::stringstream ss(ligne);
    int cols;
    ss >> _n >> cols >> _m;

    std::vector<std::vector<int>> liens(_n);
    int u, v;
    while(fichier >> u >> v) {
        liens[u - 1].push_back(v - 1);
    }

    _p.resize(_n, nullptr);
    for(int i = 0; i < _n; i++) {
        int degre = liens[i].size();
        double proba = (degre > 0) ? 1.0 / degre : 0.0;

        Cellule* pointeur = nullptr;
        for(int j = 0; j < degre; j++) {
            Cellule* nouvelleCellule = new Cellule(liens[i][j], proba);
            if(j == 0) {
                _p[i] = nouvelleCellule;
            } else {
                pointeur->_next = nouvelleCellule;
            }
            pointeur = nouvelleCellule;
        }
    }
}

void Matrice::modifier_graphe(int m, double p) {
    int ancien_n = _n;
    _n += m;

    _p.resize(_n, nullptr);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for(int i = ancien_n; i < _n; i++) {
        std::vector<int> destinations;
        for(int j = 0; j < _n; j++) {
            if(i != j && dis(gen) < p) {
                destinations.push_back(j);
            }
        }

        int degre = destinations.size();
        double proba = (degre > 0) ? 1.0 / degre : 0.0;

        Cellule* pointeur = nullptr;
        for(int j = 0; j < degre; j++) {
            Cellule* nouvelleCellule = new Cellule(destinations[j], proba);
            if(j == 0) {
                _p[i] = nouvelleCellule;
            } else {
                pointeur->_next = nouvelleCellule;
            }
            pointeur = nouvelleCellule;
        }
        _m += degre;
    }
}

int Matrice::iteration_pagerank(std::vector<double> & pi, double alpha, double epsilon) const {
    std::vector<double> x = pi;
    std::vector<double> y(_n, 0.0);
    int nb_iterations = 0;

    while(true) {
        nb_iterations++;

        double xf = 0.0;
        for(int i = 0; i < _n; i++) {
            if(_p[i] == nullptr) xf += x[i];
        }

        double base_proba = (1.0 - alpha) * (1.0 / _n) + alpha * (1.0 / _n) * xf;
        std::fill(y.begin(), y.end(), base_proba);

        for(int i = 0; i < _n; i++) {
            Cellule* courante = _p[i];
            while(courante != nullptr) {
                y[courante->_i] += alpha * x[i] * courante->_p;
                courante = courante->_next;
            }
        }

        double diff_norme1 = 0.0;
        for(int i = 0; i < _n; i++) {
            diff_norme1 += std::abs(y[i] - x[i]);
        }

        if(diff_norme1 <= epsilon) {
            pi = y;
            return nb_iterations;
        } else {
            x = y;
        }
    }
}

std::vector<double> Matrice::soustraction_matricielle(std::vector<double> const & m1, std::vector<double> const & m2) const {
    std::vector<double> resultat(m1.size());
    for(size_t i = 0; i < m1.size(); i++) resultat[i] = m1[i] - m2[i];
    return resultat;
}

double Matrice::norme1(std::vector<double> const & matrice) const {
    double somme = 0.0;
    for(double val : matrice) somme += std::abs(val);
    return somme;
}

Matrice::~Matrice() {
    for(size_t i = 0; i < _p.size(); i++) {
        Cellule* courante = _p[i];
        while(courante != nullptr) {
            Cellule* a_supprimer = courante;
            courante = courante->_next;
            delete a_supprimer;
        }
    }
}
