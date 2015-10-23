#include <iostream>

#include <QApplication>
#include <QDesktopWidget>
#include <QStyleFactory>

#include "mainform.h"
#include "mapeditorform.h"

#include "Manager.h"
#include "Params.h"

int main(int argc, char *argv[])
{   
    qRegisterMetaType<Message2>();

    GetParamsHolder().ParseParams(argc, argv);
    QApplication app(argc, argv);

    app.setStyle(QStyleFactory::create("fusion"));

    if (!GetParamsHolder().GetParamBool("-editor"))
    {
        MainForm main_form;
        main_form.show();
        return app.exec();
    }
    MapEditorForm editor_form;
    editor_form.show();
    return app.exec();
}
