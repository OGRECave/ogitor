/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
///
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////*/

#include "mainwindow.hxx"

#include <QtWidgets/QMessageBox>

//-------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDir::setCurrent(app.applicationDirPath());

    // See if we are loading something from the command line.
    QString fileArg("");
    QString suffix("");
    QString argument = app.arguments().last();
    QFileInfo info(app.arguments().last());
    if(info.exists() && info.isFile())
    {
        if(info.suffix() == "ofs")
        {
            // Yes we've got an ogscene file to load.
            fileArg = info.absoluteFilePath();
            suffix = info.suffix();
        }
        else if(info.baseName() != "qtOfs" && info.baseName() != "qtOfs_d")
        {
            // We are trying to load something we can't load
            // Exit the application.
            QMessageBox msg(QMessageBox::Critical, "Ofs Error", "We can only load Ofs files.\nExiting...");
            msg.exec();
            return 0;
        }
    }

    app.setOrganizationName(QString("Ofs"));
    app.setApplicationName("qtOfs");

    mOfsMainWindow = new MainWindow(fileArg);
    mOfsMainWindow->show();
    
    mOfsMainWindow->setApplicationObject(&app);

    mOfsMainWindow->repaint();

    int retval = app.exec();

    delete mOfsMainWindow;

    return retval;
}
//-------------------------------------------------------------------------------------
