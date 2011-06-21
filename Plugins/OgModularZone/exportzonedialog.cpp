#include "exportzonedialog.hxx"
#include "ui_exportzonedialog.h"

ExportZoneDialog::ExportZoneDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ExportZoneDialog)
{
    m_ui->setupUi(this);
}

ExportZoneDialog::~ExportZoneDialog()
{
    delete m_ui;
}

void ExportZoneDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
