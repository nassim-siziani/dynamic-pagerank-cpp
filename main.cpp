#include "Matrice.hh"
#include <QApplication>
#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <iostream>
#include <vector>
#include <fstream>
#include <exception>

int main(int argc, char *argv[]) {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::cout << "========== DEMARRAGE DU TEST COMPLET ==========" << std::endl;
    QApplication a(argc, argv);

    // --- PARAMÈTRES DE LA SIMULATION ---
    int m_nouveaux = 50;
    double p_nouveau = 0.005;
    double alpha = 0.85;
    double epsilon = 1e-6;
    int mois_simules = 12;

    std::string nom_fichier = "wb-cs-stanford.mtx";

    std::cout << "Verification du fichier sur le disque..." << std::endl;
    std::ifstream test_fichier(nom_fichier);
    if (!test_fichier.good()) {
        std::cerr << "Fichier introuvable a l'adresse : " << nom_fichier << std::endl;
        return -1;
    }
    test_fichier.close();
    std::cout << " -> Fichier trouve." << std::endl;

    // --- CHARGEMENT DU GRAPHE ---
    Matrice* web = nullptr;
    try {
        std::cout << "Lecture du fichier .mtx en memoire..." << std::endl;
        web = new Matrice(nom_fichier);
        std::cout << " -> Lecture reussie ! Nombre de pages initiales : " << web->get_n() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "CRASH LECTURE " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Erreur memoire (SegFault) lors de la construction de Matrice." << std::endl;
        return -1;
    }

    // --- PREMIER CALCUL ---
    int n_actuel = web->get_n();
    std::vector<double> pi_historique(n_actuel, 1.0 / n_actuel);

    std::cout << "Premier calcul PageRank (Initialisation)..." << std::endl;
    web->iteration_pagerank(pi_historique, alpha, epsilon);
    std::cout << " -> Calcul initial termine." << std::endl;

    // --- PRÉPARATION DES COURBES QT ---
    QLineSeries *seriesUniforme = new QLineSeries();
    seriesUniforme->setName("Initialisation Uniforme (Classique)");

    QLineSeries *seriesHistorique = new QLineSeries();
    seriesHistorique->setName("Initialisation Historique (Projet)");

    std::cout << "Lancement de la boucle temporelle (Croissance du Web)..." << std::endl;

    int max_iter = 0; // Pour dimensionner l'axe Y plus tard

    // --- LA BOUCLE DU TEMPS ---
    for(int mois = 2; mois <= mois_simules; mois++) {
        try {
            // Le Web grandit
            web->modifier_graphe(m_nouveaux, p_nouveau);
            n_actuel = web->get_n();

            // Expérience A : Uniforme
            std::vector<double> pi_uniforme(n_actuel, 1.0 / n_actuel);
            int iter_uniforme = web->iteration_pagerank(pi_uniforme, alpha, epsilon);
            seriesUniforme->append(mois, iter_uniforme);

            if (iter_uniforme > max_iter) max_iter = iter_uniforme;

            // Expérience B : Historique
            pi_historique.resize(n_actuel, 0.0);
            int iter_historique = web->iteration_pagerank(pi_historique, alpha, epsilon);
            seriesHistorique->append(mois, iter_historique);

            std::cout << " -> Mois " << mois << " : Uniforme = " << iter_uniforme
                      << " it | Historique = " << iter_historique
                      << " it | Gain = " << (iter_uniforme - iter_historique) << std::endl;

        } catch (...) {
            std::cerr << "Erreur fatale pendant le calcul du mois " << mois << std::endl;
            return -1;
        }
    }

    std::cout << "[ÉTAPE 5] Affichage du graphique final..." << std::endl;

    // --- AFFICHAGE DU GRAPHIQUE ---
    QChart *chart = new QChart();
    chart->addSeries(seriesUniforme);
    chart->addSeries(seriesHistorique);
    chart->setTitle("Vitesse de convergence du PageRank lors de l'evolution du Web");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Mois simulés");
    axisX->setRange(2, mois_simules);
    axisX->setLabelFormat("%i");
    chart->addAxis(axisX, Qt::AlignBottom);
    seriesUniforme->attachAxis(axisX);
    seriesHistorique->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Nombre d'itérations pour converger");
    axisY->setRange(0, max_iter + 5); // Axe dynamique basé sur le pire cas
    chart->addAxis(axisY, Qt::AlignLeft);
    seriesUniforme->attachAxis(axisY);
    seriesHistorique->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(900, 600);
    window.setWindowTitle("UVSQ - Projet Ranking 2026");
    window.show();

    delete web; // Nettoyage de la mémoire
    return a.exec();
}