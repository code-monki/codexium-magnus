#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("Codexium Magnus");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("CodexiumMagnus");
    app.setOrganizationDomain("codexium.com");
    
    // Create and show main window
    CodexiumMagnus::MainWindow window;
    window.setWindowTitle("Codexium Magnus");
    window.resize(1200, 800);
    window.show();
    
    return app.exec();
}