# Optimisation du PageRank pour un Web dynamique

## Présentation et but du projet

Lorsqu'un utilisateur fait une recherche sur Internet, le moteur de recherche doit trier des milliards de pages pour afficher les plus pertinentes en premier[cite: 46]. L'algorithme PageRank de Google résout ce problème en attribuant un score d'importance à chaque page selon les liens qui pointent vers elle[cite: 46, 47].

Cependant, le Web n'est pas figé : de nouvelles pages et de nouveaux liens apparaissent tous les jours[cite: 46, 47]. Avec la méthode classique, à chaque fois que le réseau s'agrandit, l'algorithme oublie tout, redonne le même score de départ à toutes les pages (initialisation uniforme) et recommence tous ses calculs depuis zéro, ce qui demande énormément de temps[cite: 46, 47].

**Le but de ce projet** est d'accélérer ce calcul en proposant une **approche historique** : plutôt que de repartir de zéro à chaque mise à jour du Web, on réutilise les scores PageRank du mois précédent comme point de départ pour les anciennes pages, et on initialise les nouvelles pages à zéro[cite: 46, 48]. L'objectif est de prouver que cette méthode permet d'atteindre le bon classement beaucoup plus rapidement[cite: 46, 48].

Ce projet a été réalisé en C++ avec Qt dans le cadre du module Méthodes de Ranking (M1 Informatique à l'UVSQ - Université Paris-Saclay)[cite: 44, 48].

## Travail réalisé

Dans ce projet, nous avons travaillé sur plusieurs étapes :

- **Modélisation par matrice creuse :** création d'une liste d'adjacence chaînée en C++ (`Cellule` et `Matrice`) pour stocker uniquement les liens réels en mémoire vive à partir du graphe de l'Université de Stanford (`wb-cs-stanford.mtx`, 9 914 pages et 36 854 liens)[cite: 49, 51, 54].

- **Moteur de calcul PageRank :** implémentation de la méthode des puissances avec gestion des pages sans lien sortant (*impasses / dangling nodes*), du facteur de téléportation ($\alpha$) et du critère d'arrêt en norme L1 ($\epsilon = 10^{-6}$)[cite: 46, 47, 52, 54].

- **Croissance dynamique du Web :** simulation de l'évolution mensuelle du réseau en ajoutant $m$ nouvelles pages reliées aléatoirement au graphe selon le modèle d'Erdős-Rényi avec une probabilité $p$[cite: 48, 53].

- **Étude expérimentale et visualisation Qt :** comparaison du nombre d'itérations entre l'initialisation uniforme et l'initialisation historique en faisant varier le nombre de nouvelles pages ($m$), la densité des liens ($p$) et le facteur d'amortissement ($\alpha$), avec affichage direct des courbes via `QtCharts`[cite: 48, 55, 56, 57].

## Principaux résultats

- **Gain sur les mises à jour régulières ($m = 10$) :** la méthode historique converge en 21 itérations en moyenne contre 58 pour la méthode uniforme, soit plus de 60 % de calculs économisés[cite: 55].

- **Influence de la densité ($p$) :** plus les nouvelles pages créent de liens avec le réseau ($p = 0.1$), plus ces liens agissent comme des raccourcis qui diffusent rapidement les scores (convergence en moins de 20 itérations)[cite: 56, 57].

- **Résistance aux contraintes fortes ($\alpha = 0.95$) :** lorsque le calcul devient mathématiquement difficile, la méthode uniforme explose à près de 150 itérations, tandis que la méthode historique converge en 78 itérations (coût divisé par deux)[cite: 58].

Le rapport complet détaillant la théorie mathématique et les graphiques est disponible dans `docs/Rapport_PageRank_Dynamique.pdf`.

## Technologies utilisées

- C++17
- Qt 6 (`QtWidgets`, `QtCharts`)[cite: 35, 48]
- CMake[cite: 35]
- Linux / GCC[cite: 39]

## Lancer le projet

Clonez le dépôt :

    git clone git@github.com:nassim-siziani/dynamic-pagerank-cpp.git
    cd dynamic-pagerank-cpp

Compilez les programmes avec CMake :

    mkdir -p build && cd build
    cmake ..
    make -j4

Lancez la simulation principale ou l'un des trois benchmarks :

    ./Ranking
    ./tests_m
    ./tests_p
    ./tests_alpha

## Ce que j'ai appris

Ce projet m'a permis d'implémenter en C++ une structure de matrice creuse optimisée en mémoire et d'éviter la copie inutile des données lors de l'agrandissement dynamique d'un graphe[cite: 48, 49, 53].

Il m'a aussi appris à relier des notions mathématiques (chaînes de Markov, trou spectral, temps de mélange et théorie des perturbations) à des gains de performance concrets, tout en intégrant des graphiques directement dans une application C++ avec Qt[cite: 48, 57, 58, 59].
