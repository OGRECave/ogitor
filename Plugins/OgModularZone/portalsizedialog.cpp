#include "portalsizedialog.hxx"
#include "ui_portalsizedialog.h"


PortalSizeDialog::PortalSizeDialog(QWidget *parent,float x,float y):
        QDialog(parent),
        //QMainWindow(parent),
        ui(new Ui::PortalSizeDialog)
{
   ui->setupUi(this);
   ui->widthLineEdit->setValidator(new QDoubleValidator(0.0, 999.0, 3, ui->widthLineEdit));
   ui->heightLineEdit->setValidator(new QDoubleValidator(0.0, 999.0, 3, ui->heightLineEdit));
   setWidth(x);
   setHeight(y);

}

PortalSizeDialog::~PortalSizeDialog()
{
    delete ui;
}

float PortalSizeDialog::getWidth(void)
{
    float x = ui->widthLineEdit->text().toFloat();
    if(x < MZP::PortalEditor::MIN_WIDTH)x = MZP::PortalEditor::MIN_WIDTH;
    return x;
}

float PortalSizeDialog::getHeight(void)
{
    float y = ui->heightLineEdit->text().toFloat();
    if(y < MZP::PortalEditor::MIN_HEIGHT)y = MZP::PortalEditor::MIN_HEIGHT;
    return y;
}

void PortalSizeDialog::setWidth(float x)
{
    ui->widthLineEdit->setText(QString::number(x));
}

void PortalSizeDialog::setHeight(float y)
{
    ui->heightLineEdit->setText(QString::number(y));;
}
