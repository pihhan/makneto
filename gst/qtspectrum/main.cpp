/**
Maly programek na testovani GStreameru a propojeni s QT knihovnou.
Jako ukol ma zobrazovat spektrum audio signalu.
*/


#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QString>
#include <QDebug>

#include "qpipeline.h"
#include "gstinput.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    GError *gst_error;
    if (!gst_init_check(&argc, &argv, &gst_error)) {
        qWarning() << "Gst init error: " << QString(gst_error->message);

    }

    QPipeline *pipeline = new QPipeline();
    QGstInput *input = new QGstInput();

    QSpectrum *spectrum = new QSpectrum();


    pipeline->add(input->element());
    spectrum->addToBin( pipeline->element() );  
    spectrum->linkFrom( input->element() );

    pipeline->setState(GST_STATE_PAUSED);
    input->iterateSourcePads();
    
    MainWindow mainwin(spectrum, pipeline);
    mainwin.setVisible(true);

    app.exec();
    return 0;
}



