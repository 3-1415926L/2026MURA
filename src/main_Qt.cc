#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.resize(800, 600);
    window.setWindowTitle("My Math Program");
    window.show();

    return app.exec();
}