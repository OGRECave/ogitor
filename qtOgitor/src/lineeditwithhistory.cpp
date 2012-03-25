/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2012 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
//
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
// Author: Greg Santucci, 2009
// Email: thecodewitch@gmail.com
// Web: http://createuniverses.blogspot.com/

#include "lineeditwithhistory.hxx"

#include <math.h>

#include <QtGui/QWidget>
#include <QtCore/QEvent>
#include <QtGui/QKeyEvent>

LineEditWithHistory::LineEditWithHistory(QWidget *parent) :
    QLineEdit(parent)
{
    m_nCurrentCommand = -1;
    m_history.reserve(100);

    QObject::connect(this, SIGNAL(returnPressed()), this, SLOT(StoreLine()));
}

LineEditWithHistory::~LineEditWithHistory()
{
}

void LineEditWithHistory::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Up)
    {
        GetPrevious();
        emit UpPressed();
        return;
    }

    if(event->key() == Qt::Key_Down)
    {
        GetNext();
        emit DownPressed();
        return;
    }

    if(event->key() == Qt::Key_Escape)
    {
        setText(m_sEditBuffer.c_str());
        m_nCurrentCommand = m_history.size();
        return;
    }

    QLineEdit::keyPressEvent(event);

    m_sEditBuffer = text().toStdString();
}

void LineEditWithHistory::StoreLine()
{
    AddToHistory();
}

void LineEditWithHistory::AddToHistory()
{
    if(m_nCurrentCommand <= 0)
    {
        m_history.push_back(text().toStdString());
        m_nCurrentCommand = m_history.size();
    }
    else if(text().toStdString() != m_history[m_nCurrentCommand-1])
    {
        m_history.push_back(text().toStdString());
        m_nCurrentCommand = m_history.size();
    }
}

void LineEditWithHistory::GetPrevious()
{
    if(m_history.size() <= 0)
    {
        m_nCurrentCommand = -1;
        return;
    }

    m_nCurrentCommand--;
    if(m_nCurrentCommand < 0)
        m_nCurrentCommand = 0;
    if(m_nCurrentCommand >= (int)m_history.size())
        m_nCurrentCommand = m_history.size() - 1;

    setText(m_history[m_nCurrentCommand].c_str());
}

void LineEditWithHistory::GetNext()
{
    if(m_history.size() <= 0)
    {
        m_nCurrentCommand = -1;
        return;
    }

    m_nCurrentCommand++;
    if(m_nCurrentCommand < 0)                  
        m_nCurrentCommand = 0;
    if(m_nCurrentCommand >= (int)m_history.size())
    {
        m_nCurrentCommand = m_history.size();
        setText(m_sEditBuffer.c_str());

        return;
    }

    setText(m_history[m_nCurrentCommand].c_str());
}
