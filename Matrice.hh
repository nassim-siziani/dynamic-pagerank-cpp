#pragma once
#include <vector>
#include <string>

struct Cellule {
    int _i;
    double _p;
    Cellule* _next;
    Cellule(int destination, double proba) : _i(destination), _p(proba), _next(nullptr) {}
};

class Matrice {
public:
    Matrice(std::string const & nom_fichier);
    ~Matrice();

    Matrice(Matrice const &) = delete;
    Matrice& operator=(Matrice const &) = delete;

    int get_n() const { return _n; }

    void modifier_graphe(int m, double p);
    int iteration_pagerank(std::vector<double> & pi, double alpha, double epsilon) const;

private:
    std::vector<Cellule *> _p;
    int _m;
    int _n;

    std::vector<double> soustraction_matricielle(std::vector<double> const & matrice1, std::vector<double> const & matrice2) const;
    double norme1(std::vector<double> const & matrice) const;
};
