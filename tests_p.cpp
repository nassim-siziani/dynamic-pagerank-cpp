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

    // --- PARAMÈTRES FIXES POUR LE TEST DE DENSITÉ ---
    int m_fixe = 50; 
    std::vector<double> p_tests = {0.001, 0.01, 0.1}; // Les 3 valeurs que tu as testées
    double alpha = 0.85;
    double epsilon = 1e-6;
    int mois_simules = 12;
    std::string nom_fichier = "wb-cs-stanford.mtx";

    QChart *chart = new QChart();
    chart->setTitle("Impact de la densité des liens (p) sur la convergence");

    // 1. COURBE DE RÉFÉRENCE (UNIFORME)
    QLineSeries *seriesUniforme = new QLineSeries();
    seriesUniforme->setName("Uniforme (Référence)");
    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setStyle(Qt::DashLine);
    seriesUniforme->setPen(pen);

    // 2. BOUCLE SUR LES VALEURS DE P
    for (double p : p_tests) {
        std::cout << "\n>>> Simulation pour p = " << p << std::endl;
        
        Matrice web(nom_fichier);
        int n_actuel = web.get_n();
        std::vector<double> pi_hist(n_actuel, 1.0 / n_actuel);
        web.iteration_pagerank(pi_hist, alpha, epsilon);

        QLineSeries *seriesH = new QLineSeries();
        seriesH->setName("Historique (p = " + QString::number(p) + ")");

        for (int mois = 2; mois <= mois_simules; mois++) {
            web.modifier_graphe(m_fixe, p); // On utilise m_fixe et on varie p
            n_actuel = web.get_n();

            // Calcul Uniforme (uniquement lors du premier passage p=0.001 car il change pas)
            if (p == 0.001) {
                std::vector<double> pi_u(n_actuel, 1.0 / n_actuel);
                int it_u = web.iteration_pagerank(pi_u, alpha, epsilon);
                seriesUniforme->append(mois, it_u);
            }

            // Calcul Historique
            pi_hist.resize(n_actuel, 0.0);
            int it_h = web.iteration_pagerank(pi_hist, alpha, epsilon);
            seriesH->append(mois, it_h);
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