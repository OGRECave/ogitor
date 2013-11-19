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
#include "shortcutsettings.hxx"
#include "ui_shortcutsettings.h"
#include "shortcuts.hxx"

#include <QtWidgets/QMessageBox>
#include <QtGui/QKeyEvent>

ShortCutSettings::ShortCutSettings(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ShortCutSettings)
{
    m_ui->setupUi(this);
    initShortcuts();
}

ShortCutSettings::~ShortCutSettings()
{
    delete m_ui;
}

void ShortCutSettings::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ShortCutSettings::initShortcuts()
{
    Part0 = "";
    Part1 = "";
    Part2 = "";
    Part3 = "";
    keyCode = 0;

    m_ui->shortcutLabel->setText ( "" );

    shortcutModel = new QStandardItemModel ( 0, 3, this );
    reloadShortcuts();
    m_ui->shortcutList->setModel ( shortcutModel );
    m_ui->shortcutList->setShowGrid ( false );
    m_ui->shortcutList->setSelectionBehavior ( QAbstractItemView::SelectRows );
    m_ui->shortcutList->setSelectionMode ( QAbstractItemView::SingleSelection );
    m_ui->shortcutList->resizeColumnsToContents();
    m_ui->shortcutList->resizeRowsToContents();
    m_ui->shortcutList->setSortingEnabled ( false );
    //m_ui->shortcutList->sortByColumn ( 0, Qt::AscendingOrder );

    connect ( m_ui->clearButton, SIGNAL ( clicked() ), this, SLOT ( slotClearShortcut() ) );
    connect ( m_ui->changeButton, SIGNAL ( clicked() ), this, SLOT ( slotChangeShortcut() ) );
    connect ( m_ui->shortcutList, SIGNAL ( clicked ( const QModelIndex& ) ), this, SLOT ( slotActionSelected ( const QModelIndex& ) ) );
    //connect ( m_ui->shortcutList, SIGNAL ( activated ( const QModelIndex& ) ), changeButton, SLOT ( toggle() ) );
}

void ShortCutSettings::slotChangeShortcut()
{
    if ( m_ui->changeButton->isChecked() )
    {
        keyCode = 0;
        Part0 = "";
        Part1 = "";
        Part2 = "";
        Part3 = "";
        Part4 = "";
        grabKeyboard();
    }
    else
        releaseKeyboard();
}

void ShortCutSettings::slotClearShortcut()
{
    QModelIndex index = m_ui->shortcutList->currentIndex();
    if ( !index.isValid() )
        return;

    int row = index.row();
    QStandardItem *item = shortcutModel->item ( row, 2 );
    QString iText = item->text();

    if ( QMessageBox::question ( this, tr ( "Reset Shortcut" ),
                                 "<qt>" + tr ( "You are going to reset ", "to it's default value." ) +
                                 QString ( "<br/><b>%1</b>.<br/>" ).arg ( iText ) +
                                 tr ( "Are you sure you want to do this?" ) + "</qt>",
                                 QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
    {
        Shortcuts::getInstance()->clearShortcut ( iText );
        m_ui->shortcutLabel->clear();
        reloadShortcuts();
        setSelected ( iText );
    }
}

void ShortCutSettings::slotActionSelected ( const QModelIndex &mi )
{
    QModelIndex index = m_ui->shortcutList->currentIndex();
    if ( !index.isValid() )
        return;

    int row = index.row();
    QStandardItem *item = shortcutModel->item ( row, 1 );
    QString iShortcut = item->text();
    m_ui->shortcutLabel->setText ( iShortcut );
}


bool ShortCutSettings::event ( QEvent* ev )
{
    bool ret = QWidget::event ( ev );
    if ( ev->type() == QEvent::KeyPress )
        keyPressEvent ( ( QKeyEvent* ) ev );
    if ( ev->type() == QEvent::KeyRelease )
        keyReleaseEvent ( ( QKeyEvent* ) ev );
    return ret;
}

void ShortCutSettings::keyPressEvent ( QKeyEvent *k )
{
    if ( m_ui->changeButton->isChecked() )
    {
        QStringList tl;
        if ( !m_ui->shortcutLabel->text().isEmpty() )
        {
            tl = m_ui->shortcutLabel->text().split ( "+", QString::SkipEmptyParts );
            Part4 = tl[tl.count()-1];
            if ( Part4 == tr ( "Alt" ) || Part4 == tr ( "Ctrl" ) || Part4 == tr ( "Shift" ) || Part4 == tr ( "Meta" ) )
                Part4 = "";
        }
        else
            Part4 = "";

        switch ( k->key() )
        {
        case Qt::Key_Meta:
            Part0 = tr ( "Meta+" );
            keyCode |= Qt::META;
            break;
        case Qt::Key_Shift:
            Part3 = tr ( "Shift+" );
            keyCode |= Qt::SHIFT;
            break;
        case Qt::Key_Alt:
            Part2 = tr ( "Alt+" );
            keyCode |= Qt::ALT;
            break;
        case Qt::Key_Control:
            Part1 = tr ( "Ctrl+" );
            keyCode |= Qt::CTRL;
            break;
        default:
            keyCode |= k->key();
            m_ui->shortcutLabel->setText ( getKeyText ( keyCode ) );
            m_ui->changeButton->setChecked ( false );
            releaseKeyboard();
            shortcutSet ( m_ui->shortcutLabel->text(), keyCode );
        }
    }

    if ( m_ui->changeButton->isChecked() )
    {
        m_ui->shortcutLabel->setText ( Part0+Part1+Part2+Part3+Part4 );
    }
}

void ShortCutSettings::keyReleaseEvent ( QKeyEvent *k )
{
    if ( m_ui->changeButton->isChecked() )
    {
        if ( !m_ui->shortcutLabel->text().isEmpty() )
        {
            QStringList tl;
            tl = m_ui->shortcutLabel->text().split ( "+", QString::SkipEmptyParts );
            Part4 = tl[tl.count()-1];
            if ( Part4 == tr ( "Alt" ) || Part4 == tr ( "Ctrl" ) || Part4 == tr ( "Shift" ) || Part4 == tr ( "Meta" ) )
            Part4 = "";
        }
        else
            Part4 = "";

        if ( k->key() == Qt::Key_Meta )
        {
            Part0 = "";
            keyCode &= ~Qt::META;
        }
        if ( k->key() == Qt::Key_Shift )
        {
            Part3 = "";
            keyCode &= ~Qt::SHIFT;
        }
        if ( k->key() == Qt::Key_Alt )
        {
            Part2 = "";
            keyCode &= ~Qt::ALT;
        }
        if ( k->key() == Qt::Key_Control )
        {
            Part1 = "";
            keyCode &= ~Qt::CTRL;
        }
        m_ui->shortcutLabel->setText ( Part0+Part1+Part2+Part3+Part4 );
    }
}

QString ShortCutSettings::getKeyText ( int KeyC )
{
    if ( ( KeyC & ~ ( Qt::META | Qt::CTRL | Qt::ALT | Qt::SHIFT ) ) == 0 )
        return "";
    // on OSX Qt translates modifiers to forsaken symbols, arrows and the like
    // we prefer plain English
    QString res;
    if ( ( KeyC & Qt::META ) != 0 )
        res += "Meta+";
    if ( ( KeyC & Qt::CTRL ) != 0 )
        res += "Ctrl+";
    if ( ( KeyC & Qt::ALT ) != 0 )
        res += "Alt+";
    if ( ( KeyC & Qt::SHIFT ) != 0 )
        res += "Shift+";
    return res + QString ( QKeySequence(KeyC & ~ (Qt::META | Qt::CTRL | Qt::ALT | Qt::SHIFT)).toString() );
}

void ShortCutSettings::shortcutSet ( const QString &shortcut, const int &keycode )
{
    QModelIndex index = m_ui->shortcutList->currentIndex();
    if ( !index.isValid() )
        return;

    int row = index.row();
    QStandardItem *item = shortcutModel->item ( row, 0 );
    QString iText = item->text();
    QStandardItem *spk = shortcutModel->item ( row, 2 );
    QString iSPK = spk->text();

    Shortcuts *tmp = Shortcuts::getInstance();
    QString reserved = tmp->isReserved ( shortcut, iSPK );
    if ( !reserved.isEmpty() ) // shortcut is already in use
    {
        if ( QMessageBox::question ( this, tr ( "Replace" ),
                                     "<qt>" + tr ( "Shortcut is already in use for", "action name will be appended to this" ) +
                                     QString ( "<br/><b>%1</b>.<br/>" ).arg ( reserved ) +
                                     tr ( "Do you still want to assign it?" ) + "</qt>",
                                     QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
        {
            tmp->clearShortcut ( iSPK );
        }
        else
        {
            return; // user choose not to replace an existing shortcut
        }
    }
    tmp->setShortcut ( shortcut, iText, iSPK, keycode );
    reloadShortcuts();
    setSelected ( iText );

    emit isDirty();
}

void ShortCutSettings::reloadShortcuts()
{
    QFont font ( m_ui->shortcutList->font() );
    font.setBold ( true );
    shortcutModel->clear();
    QList<ShortCut> alist = Shortcuts::getInstance()->getActions();
    Shortcuts *scuts = Shortcuts::getInstance();
    foreach ( ShortCut act, alist )
    {
        QStandardItem *iText = new QStandardItem ( scuts->cleanName(act.description) );
        QStandardItem *iShortcut = new QStandardItem ( act.shortcut );
        QStandardItem *iSPK = new QStandardItem ( act.SPK );
        iShortcut->setFont ( font );
        QList<QStandardItem *> iRow;
        iRow << iText << iShortcut << iSPK;
        shortcutModel->appendRow ( iRow );
    }
    shortcutModel->setHeaderData ( 0, Qt::Horizontal, tr ( "Action" ) );
    shortcutModel->setHeaderData ( 1, Qt::Horizontal, tr ( "Shortcut" ) );
    shortcutModel->setHeaderData ( 2, Qt::Horizontal, tr ( "KeyCode" ) );
    m_ui->shortcutList->resizeColumnsToContents();
    m_ui->shortcutList->resizeRowsToContents();
    //m_ui->shortcutList->setSortingEnabled ( true );
}

void ShortCutSettings::setSelected ( const QString &actionText )
{
    QList<QStandardItem*> ilist = shortcutModel->findItems ( Shortcuts::getInstance()->cleanName(actionText) );
    if ( ilist.count() > 0 )
    {
        int row = ilist.at ( 0 )->row();
        m_ui->shortcutList->selectRow ( row );
    }
}

