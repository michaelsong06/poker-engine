#include "mainwindow.hpp"
// #include "server/server.hpp"

#include <QApplication>
#include <QFile>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // load fonts
    int robotoFontId = QFontDatabase::addApplicationFont(":/assets/RobotoSlab-VariableFont_wght.ttf");
    int montserratFontId = QFontDatabase::addApplicationFont(":/assets/Montserrat-VariableFont_wght.ttf");
    if (robotoFontId == -1 || montserratFontId == -1) {
        qWarning() << "Failed to load fonts.";
    }
    QStringList loadedFamilies = QFontDatabase::applicationFontFamilies(robotoFontId);
    if (!loadedFamilies.empty()) {
        QFont robotoFont(loadedFamilies.at(0));
        a.setFont(robotoFont);
    }

    // load stylesheet
    QFile file(":/assets/styles.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString stylesheet = QString::fromUtf8(file.readAll());
        a.setStyleSheet(stylesheet);
    } else {
        qDebug() << "Could not open styles.qss";
    }

    return a.exec();
}
