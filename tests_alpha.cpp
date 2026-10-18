#include "Matrice.hh"
#include <QApplication>
#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <iostream>
#include <vector>
#include <QColor>
#include <QPen>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // --- PARAMÈTRES FIXES ---
    int m_fixe = 50; 
    double p_fixe = 0.005;
    double epsilon = 1e-6;
    int mois_simules = 12;
    
    // 5 valeurs clés pour un graphe lisible
    std::vector<double> alpha_tests = {0.50, 0.70, 0.85, 0.90, 0.95}; 
    
    // 5 Couleurs très distinctes
    std::vector<QColor> couleurs = {
        QColor("#3498db"), // Bleu clair (0.50)
        QColor("#2ecc71"), // Vert (0.70)
        QColor("#f1c40f"), // Jaune/Or (0.85)
        QColor("#e67e22"), // Orange (0.90)
        QColor("#e74c3c")  // Rouge vif (0.95)
    };
    
    std::string nom_fichier = "wb-cs-stanford.mtx";
    QChart *chart = new QChart();
    chart->setTitle("Impact du facteur d'amortissement (alpha) : Uniforme (pointillés) vs Historique (plein)");

    int max_iter_global = 0; 

    // --- BOUCLE SUR LES VALEURS DE ALPHA ---
    for (size_t i = 0; i < alpha_tests.size(); i++) {
        double alpha = alpha_tests[i];
        QColor couleur = couleurs[i];
        
        std::cout << "\n>>> Simulation pour alpha = " << alpha << std::endl;
        
        Matrice web(nom_fichier);
        int n_actuel = web.get_n();
        std::vector<double> pi_hist(n_actuel, 1.0 / n_actuel);
        web.iteration_pagerank(pi_hist, alpha, epsilon);

        // --- SÉRIE UNIFORME ---
        QLineSeries *seriesU = new QLineSeries();
        seriesU->setName("Uni a=" + QString::number(alpha, 'f', 2));
        QPen penU(couleur);
        penU.setStyle(Qt::DotLine); 
        penU.setWidth(1);          
        seriesU->setPen(penU);

        // --- SÉRIE HISTORIQUE ---
        QLineSeries *seriesH = new QLineSeries();
        seriesH->setName("Hist a=" + QString::number(alpha, 'f', 2));
        QPen penH(couleur);
        penH.setStyle(Qt::SolidLine); 
        penH.setWidth(3);             
        seriesH->setPen(penH);

        for (int mois = 2; mois <= mois_simules; mois++) {
            web.modifier_graphe(m_fixe, p_fixe);
            n_actuel = web.get_n();

            // Calcul Uniforme
            std::vector<double> pi_u(n_actuel, 1.0 / n_actuel);
            int it_u = web.iteration_pagerank(pi_u, alpha, epsilon);
            seriesU->append(mois, it_u);

            // Calcul Historique
            pi_hist.resize(n_actuel, 0.0);
            int it_h = web.iteration_pagerank(pi_hist, alpha, epsilon);
            seriesH->append(mois, it_h);
            
            if (it_u > max_iter_global) max_iter_global = it_u;
            
            std::cout << "Mois " << mois << " | Uni: " << it_u << " | Hist: " << it_h << std::endl;
        }
        
        chart->addSeries(seriesU);
        chart->addSeries(seriesH);
    }

    // --- CONFIGURATION AXES ---
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Mois simulés");
    axisX->setRange(2, mois_simules);
    axisX->setLabelFormat("%i");
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Nombre d'itérations");
    axisY->setRange(0, max_iter_global + 10); 
    chart->addAxis(axisY, Qt::AlignLeft);

    for (auto series : chart->series()) {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(1100, 750); 
    window.show();

    return a.exec();
}