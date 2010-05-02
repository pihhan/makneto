
#include <QHBoxLayout>
#include <QApplication>

#include "qlisttest.h"

Test::Test()
{
    QHBoxLayout *layout = new QHBoxLayout();
    setLayout(layout);

    view = new QListWidget();
    view->addItem("X");
    view->addItem("Y");
    
    layout->addWidget(view);

    setVisible(true);        
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    new Test();

    return app.exec();
}

