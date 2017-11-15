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

template<class W, class T>
class BoundPropertyControl
{
   // W : class of widget
public:
   BoundPropertyControl(ControlBinder* binder, QString name, W* widget, QObject* obj, const char* prop, T value);
};

template<class T>
class BoundPropertyControl<QCheckBox,T>
{
public:
   BoundPropertyControl(ControlBinder* binder, QString name, QCheckBox* widget, QObject* obj, const char* prop, T value);
};

template<class T>
class BoundPropertyControl<QGroupBox,T>
{
public:
   BoundPropertyControl(ControlBinder* binder, QString name, QGroupBox* widget, QObject* obj, const char* prop, T value);
};

template<class T>
class BoundPropertyControl<QLineEdit,T>
{
public:
   BoundPropertyControl(ControlBinder* binder, QString name, QLineEdit* widget, QObject* obj, const char* prop, T value);
};


template<class T>
class BoundPropertyControl<QComboBox,T>
{
public:
   BoundPropertyControl(ControlBinder* binder, QString name, QComboBox* widget, QObject* obj, const char* prop, T value);
};


