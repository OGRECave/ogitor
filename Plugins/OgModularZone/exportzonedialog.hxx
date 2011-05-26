#ifndef EXPORTZONEDIALOG_H
#define EXPORTZONEDIALOG_H

#include <QtGui/QDialog>
#include "ui_exportzonedialog.h"
#include "OgreString.h"

//namespace Ui {
//    class ExportZoneDialog;
//}

class ExportZoneDialog : public QDialog,public Ui::ExportZoneDialog{
    Q_OBJECT
public:
    ExportZoneDialog(QWidget *parent = 0);
    ~ExportZoneDialog();
	Ogre::String getShortDescription(void){return m_ui->lineEditShortDesc->text().toStdString();}
	Ogre::String getLongDescription(void){return m_ui->plainTextEditLongDesc->toPlainText().toStdString();}
	Ogre::String getFileName(void){return m_ui->lineEditFileName->text().toStdString();}


protected:
    void changeEvent(QEvent *e);

private:
    Ui::ExportZoneDialog *m_ui;
};

#endif // EXPORTZONEDIALOG_H
