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

template<class W>
class BoundPropertyControl
{
   // W : class of widget
public:
   BoundPropertyControl(ControlBinder* binder, QString name, W* widget, QObject* obj, const char* prop);
};

template<>
class BoundPropertyControl<QCheckBox>
{
public:
   BoundPropertyControl(ControlBinder* binder, QString name, QCheckBox* widget, QObject* obj, const char* prop);
};

template<>
class BoundPropertyControl<QGroupBox>
{
public:
   BoundPropertyControl(ControlBinder* binder, QString name, QGroupBox* widget, QObject* obj, const char* prop);
};

template<>
class BoundPropertyControl<QLineEdit>
{
public:
   BoundPropertyControl(ControlBinder* binder, QString name, QLineEdit* widget, QObject* obj, const char* prop);
};


template<>
class BoundPropertyControl<QComboBox>
{
public:
   BoundPropertyControl(ControlBinder* binder, QString name, QComboBox* widget, QObject* obj, const char* prop);
};


