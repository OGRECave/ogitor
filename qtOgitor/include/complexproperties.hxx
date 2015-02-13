/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2015 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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
#ifndef COMPLEXPROPERTIES_HXX
#define COMPLEXPROPERTIES_HXX

//-------------------------------------------------------------------------

#include <QtVariantPropertyManager>
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include <Ogre.h>

//-------------------------------------------------------------------------

class QuaternionManager : public QtVariantPropertyManager
{
    Q_OBJECT
public:
    QuaternionManager(QObject *parent = 0);
    ~QuaternionManager();

    virtual QVariant value(const QtProperty *property) const;
    virtual int valueType(int propertyType) const;
    virtual bool isPropertyTypeSupported(int propertyType) const;

    QString valueText(const QtProperty *property) const;

public Q_SLOTS:
    virtual void setValue(QtProperty *property, const QVariant &val);
    virtual void setMinimum(QtProperty *property, const QVariant &val);
    virtual void setMaximum(QtProperty *property, const QVariant &val);
    virtual void setStepSize(QtProperty *property, const QVariant &val);

protected:
    virtual void initializeProperty(QtProperty *property);
    virtual void uninitializeProperty(QtProperty *property);

private Q_SLOTS:
    void slotValueChanged(QtProperty *property, const QVariant &value);
    void slotPropertyDestroyed(QtProperty *property);

private:
    struct Data 
    {
        QVariant value;
        QtVariantProperty *w;
        QtVariantProperty *x;
        QtVariantProperty *y;
        QtVariantProperty *z;
    };

    QMap<const QtProperty *, Data> propertyToData;
    QMap<const QtProperty *, QtProperty *> wToProperty;
    QMap<const QtProperty *, QtProperty *> xToProperty;
    QMap<const QtProperty *, QtProperty *> yToProperty;
    QMap<const QtProperty *, QtProperty *> zToProperty;
};

//-------------------------------------------------------------------------

class Vector4Manager : public QtVariantPropertyManager
{
    Q_OBJECT
public:
    Vector4Manager(QObject *parent = 0);
    ~Vector4Manager();

    virtual QVariant value(const QtProperty *property) const;
    virtual int valueType(int propertyType) const;
    virtual bool isPropertyTypeSupported(int propertyType) const;

    QString valueText(const QtProperty *property) const;

    void setPropertyNames(QtProperty *property, QString fx, QString fy, QString fz, QString fw);

public Q_SLOTS:
    virtual void setValue(QtProperty *property, const QVariant &val);
    virtual void setMinimum(QtProperty *property, const QVariant &val);
    virtual void setMaximum(QtProperty *property, const QVariant &val);
    virtual void setStepSize(QtProperty *property, const QVariant &val);

protected:
    virtual void initializeProperty(QtProperty *property);
    virtual void uninitializeProperty(QtProperty *property);

private Q_SLOTS:
    void slotValueChanged(QtProperty *property, const QVariant &value);
    void slotPropertyDestroyed(QtProperty *property);

private:
    struct Data
    {
        QVariant value;
        QtVariantProperty *w;
        QtVariantProperty *x;
        QtVariantProperty *y;
        QtVariantProperty *z;
    };

    QMap<const QtProperty *, Data> propertyToData;
    QMap<const QtProperty *, QtProperty *> wToProperty;
    QMap<const QtProperty *, QtProperty *> xToProperty;
    QMap<const QtProperty *, QtProperty *> yToProperty;
    QMap<const QtProperty *, QtProperty *> zToProperty;
};

//-------------------------------------------------------------------------

class Vector3Manager : public QtVariantPropertyManager
{
    Q_OBJECT
public:
    Vector3Manager(QObject *parent = 0);
    ~Vector3Manager();

    virtual QVariant value(const QtProperty *property) const;
    virtual int valueType(int propertyType) const;
    virtual bool isPropertyTypeSupported(int propertyType) const;

    QString valueText(const QtProperty *property) const;

    void setPropertyNames(QtProperty *property, QString fx, QString fy, QString fz);

public Q_SLOTS:
    virtual void setValue(QtProperty *property, const QVariant &val);
    virtual void setMinimum(QtProperty *property, const QVariant &val);
    virtual void setMaximum(QtProperty *property, const QVariant &val);
    virtual void setStepSize(QtProperty *property, const QVariant &val);

protected:
    virtual void initializeProperty(QtProperty *property);
    virtual void uninitializeProperty(QtProperty *property);

private Q_SLOTS:
    void slotValueChanged(QtProperty *property, const QVariant &value);
    void slotPropertyDestroyed(QtProperty *property);

private:
    struct Data 
    {
        QVariant value;
        QtVariantProperty *x;
        QtVariantProperty *y;
        QtVariantProperty *z;
    };

    QMap<const QtProperty *, Data> propertyToData;
    QMap<const QtProperty *, QtProperty *> xToProperty;
    QMap<const QtProperty *, QtProperty *> yToProperty;
    QMap<const QtProperty *, QtProperty *> zToProperty;
};

//-------------------------------------------------------------------------

class Vector2Manager : public QtVariantPropertyManager
{
    Q_OBJECT
public:
    Vector2Manager(QObject *parent = 0);
    ~Vector2Manager();

    virtual QVariant value(const QtProperty *property) const;
    virtual int valueType(int propertyType) const;
    virtual bool isPropertyTypeSupported(int propertyType) const;

    QString valueText(const QtProperty *property) const;

    void setPropertyNames(QtProperty *property, QString fx, QString fy);

public Q_SLOTS:
    virtual void setValue(QtProperty *property, const QVariant &val);
    virtual void setMinimum(QtProperty *property, const QVariant &val);
    virtual void setMaximum(QtProperty *property, const QVariant &val);
    virtual void setStepSize(QtProperty *property, const QVariant &val);

protected:
    virtual void initializeProperty(QtProperty *property);
    virtual void uninitializeProperty(QtProperty *property);

private Q_SLOTS:
    void slotValueChanged(QtProperty *property, const QVariant &value);
    void slotPropertyDestroyed(QtProperty *property);

private:
    struct Data 
    {
        QVariant value;
        QtVariantProperty *x;
        QtVariantProperty *y;
    };

    QMap<const QtProperty *, Data> propertyToData;
    QMap<const QtProperty *, QtProperty *> xToProperty;
    QMap<const QtProperty *, QtProperty *> yToProperty;
};

//-------------------------------------------------------------------------

#endif // COMPLEXPROPERTIES_HXX

//-------------------------------------------------------------------------

