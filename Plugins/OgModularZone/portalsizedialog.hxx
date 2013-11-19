#ifndef PORTALSIZEDIALOG_H
#define PORTALSIZEDIALOG_H

#include "PortalEditor.h"

#include <QtWidgets/QDialog>

namespace Ui
{
    class PortalSizeDialog;
}

class PortalSizeDialog : public QDialog
{
    Q_OBJECT

public:
    PortalSizeDialog(QWidget *parent = 0,float x = MZP::PortalEditor::DEFAULT_WIDTH,float y = MZP::PortalEditor::DEFAULT_HEIGHT);
    ~PortalSizeDialog();


private:
    Ui::PortalSizeDialog *ui;

public:
    float getWidth(void);//{return ui->widthLineEdit->text().toFloat();}
    float getHeight(void);//{return ui->widthLineEdit->text().toFloat();}
    void setWidth(float);
    void setHeight(float);
};

#endif // PORTALSIZEDIALOG_H
