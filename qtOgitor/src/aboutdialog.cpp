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

#include "aboutdialog.hxx"
#include "OgitorsPrerequisites.h"
#include <QtCore/QSettings>

//------------------------------------------------------------------------------
aboutdialog::aboutdialog(QWidget *parent, const QString version_string, Qt::WindowFlags flags) :
    QDialog(parent, flags)
{
    setupUi(this);
    if (!version_string.isEmpty())
        setWindowTitle("About Ogitor " + version_string);

    QSettings settings;
    QString style = settings.value("preferences/customStyleSheet").toString();

    QString psheet = "";
    if(style == ":/stylesheets/dark.qss") {
        psheet = QString::fromLatin1("a { color: white;}");
    } else {
        psheet = QString::fromLatin1("a { color: black;}");
    }
    projectBrowser->document()->setDefaultStyleSheet(psheet);
    projectBrowser->setSource(QUrl("qrc:/about/project.html"));
    QString text = projectBrowser->toHtml();
    // Replace version placeholder with actual current version number
    text.replace("%OGITOR_VERSION%", Ogitors::Globals::OGITOR_VERSION.c_str());
    projectBrowser->setHtml(text);
    
    peopleBrowser->setSource(QUrl("qrc:/about/people.html"));

    QString sheet = "";
    if(style == ":/stylesheets/dark.qss") {
        sheet = QString::fromLatin1("tt{ color: %1; font-size: 60%;}").arg(QString("rgb(220,220,220)"));
    } else {
        sheet = QString::fromLatin1("tt{ color: %1; font-size: 60%;}").arg(QString("green"));
    }
    licenseBrowser->document()->setDefaultStyleSheet(sheet);

    licenseBrowser->setSource(QUrl("qrc:/about/license.html"));
}
//------------------------------------------------------------------------------
aboutdialog::~aboutdialog()
{
}
//------------------------------------------------------------------------------
