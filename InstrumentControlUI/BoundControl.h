#pragma once

#include <QObject>
#include <QString>
#include <QSettings>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QFileDialog>

#include <functional>

class ControlBinder;

template<class W, class V, class U, class T>
class BoundControl
{
   // W : class of widget
   // V : class of object
   // U : class of signal and getter -> may be different to U since may have been subclassed
   // T : class of data used by widget
public:
   BoundControl(ControlBinder* binder, QString name, W* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T) = nullptr, Qt::ConnectionType connection_type = Qt::AutoConnection, bool transient = false);
};

template<class V, class U, class T>
class BoundControl<QCheckBox, V, U, T>
{
public:
   BoundControl(ControlBinder* binder, QString name, QCheckBox* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T) = nullptr, Qt::ConnectionType connection_type = Qt::AutoConnection, bool transient = false);
};

template<class V, class U, class T>
class BoundControl<QGroupBox, V, U, T>
{
public:
   BoundControl(ControlBinder* binder, QString name, QGroupBox* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T) = nullptr, Qt::ConnectionType connection_type = Qt::AutoConnection, bool transient = false);
};

template<class V, class U>
class BoundControl<QLineEdit, V, U, const QString&>
{
public:
   BoundControl(ControlBinder* binder, QString name, QLineEdit* widget, V* obj, void(U::*setter)(const QString&), const QString&(U::*getter)(void), void (U::*signal)(const QString&) = nullptr, Qt::ConnectionType connection_type = Qt::AutoConnection, bool transient = false);
};


template<class V, class U, class T>
class BoundControl<QComboBox, V, U, T>
{
public:
   BoundControl(ControlBinder* binder, QString name, QComboBox* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T) = nullptr, Qt::ConnectionType connection_type = Qt::AutoConnection, bool transient = false);
};


template<class V, class U>
class BoundFilenameControl
{
public:
   BoundFilenameControl(ControlBinder* binder, QString name, QLineEdit* widget, QPushButton* button, QString filter, V* obj, void(U::*setter)(const QString&), const QString&(U::*getter)(void), void (U::*signal)(const QString&) = nullptr, Qt::ConnectionType connection_type = Qt::AutoConnection, bool transient = false);
};

