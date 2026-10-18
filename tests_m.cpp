#include "Matrice.hh"
#include <QApplication>
#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // --- PARAMÈTRES FIXES ---
    std::vector<int> m_tests = {10, 100, 1000};
    double p_nouveau = 0.005;
    double alpha = 0.85;
    double epsilon = 1e-6;
    int mois_simules = 12;
    std::string nom_fichier = "wb-cs-stanford.mtx";

    QChart *chart = new QChart();
    chart->setTitle("Impact du volume de mise à jour (m) sur le gain de convergence");

    // 1. COURBE DE RÉFÉRENCE (UNIFORME)
    // On la calcule une seule fois car elle est stable
    QLineSeries *seriesUniforme = new QLineSeries();
    seriesUniforme->setName("Initialisation Uniforme (Référence)");
    
    // On change le style pour qu'elle se distingue (pointillés par exemple)
    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setStyle(Qt::DashLine);
    seriesUniforme->setPen(pen);

    // 2. BOUCLE SUR LES VALEURS DE M
    for (int m : m_tests) {
        std::cout << "\n>>> Simulation pour m = " << m << std::endl;
        
        // On recharge la matrice à chaque fois pour repartir du même graphe de Stanford
        Matrice web(nom_fichier);
        int n_actuel = web.get_n();
        std::vector<double> pi_hist(n_actuel, 1.0 / n_actuel);
        
        // Initialisation du vecteur historique
        web.iteration_pagerank(pi_hist, alpha, epsilon);

        QLineSeries *seriesH = new QLineSeries();
        seriesH->setName("Historique (m = " + QString::number(m) + ")");

        for (int mois = 2; mois <= mois_simules; mois++) {
            web.modifier_graphe(m, p_nouveau);
            n_actuel = web.get_n();

            // Calcul Uniforme (uniquement pour la référence lors du premier test m=10)
            if (m == 10) {
                std::vector<double> pi_u(n_actuel, 1.0 / n_actuel);
                int it_u = web.iteration_pagerank(pi_u, alpha, epsilon);
                seriesUniforme->append(mois, it_u);
            }

            // Calcul Historique
            pi_hist.resize(n_actuel, 0.0);
            int it_h = web.iteration_pagerank(pi_hist, alpha, epsilon);
            seriesH->append(mois, it_h);
            
            std::cout << "Mois " << mois << " termine." << std::endl;
        }
        chart->addSeries(seriesH);
    }

    chart->addSeries(seriesUniforme);

    // --- CONFIGURATION AXES ---
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Mois simulés");
    axisX->setRange(2, mois_simules);
    axisX->setLabelFormat("%i");
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Nombre d'itérations");
    axisY->setRange(0, 70); 
    chart->addAxis(axisY, Qt::AlignLeft);

    // Attacher les axes à toutes les séries
    for (auto series : chart->series()) {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(1000, 700);
    window.show();

    return a.exec();
}