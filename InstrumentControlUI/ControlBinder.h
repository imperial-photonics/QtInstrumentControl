#pragma once

#include <QObject>
#include <QString>
#include <QSettings>
#include <QCheckbox>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>

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
   template<class W, class V, class U, class T>
   BoundControl(ControlBinder* binder, QString name, W* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T) = nullptr, Qt::ConnectionType connection_type = Qt::AutoConnection)
   {
      auto widget_signal = static_cast<void (W::*)(T)>(&W::valueChanged);
      auto widget_setter = static_cast<void (W::*)(T)>(&W::setValue);

      binder->BindWidget(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, connection_type);
      binder->SetByValue(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal);
   }
};

template<class V, class U, class T>
struct BoundControl<QCheckBox, V, U, T>
{
public:
   template<class V, class U, class T>
   BoundControl(ControlBinder* binder, QString name, QCheckBox* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T) = nullptr, Qt::ConnectionType connection_type = Qt::AutoConnection)
   {
      auto widget_signal = static_cast<void (QCheckBox::*)(T)>(&QCheckBox::toggled);
      auto widget_setter = static_cast<void (QCheckBox::*)(T)>(&QCheckBox::setChecked);

      binder->BindWidget(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, connection_type);
      binder->SetByValue(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal);
   }
};

template<class V, class U, class T>
struct BoundControl<QGroupBox, V, U, T>
{
public:
   template<class V, class U, class T>
   BoundControl(ControlBinder* binder, QString name, QGroupBox* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T) = nullptr, Qt::ConnectionType connection_type = Qt::AutoConnection)
   {
      auto widget_signal = static_cast<void (QGroupBox::*)(T)>(&QGroupBox::toggled);
      auto widget_setter = static_cast<void (QGroupBox::*)(T)>(&QGroupBox::setChecked);

      binder->BindWidget(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, connection_type);
      binder->SetByValue(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal);
   }
};

template<class V, class U>
struct BoundControl<QLineEdit, V, U, const QString&>
{
public:
   template<class V, class U>
   BoundControl(ControlBinder* binder, QString name, QLineEdit* widget, V* obj, void(U::*setter)(const QString&), const QString&(U::*getter)(void), void (U::*signal)(const QString&) = nullptr, Qt::ConnectionType connection_type = Qt::AutoConnection)
   {
      auto widget_signal = static_cast<void (QLineEdit::*)(const QString&)>(&QLineEdit::textEdited);
      auto widget_setter = static_cast<void (QLineEdit::*)(const QString&)>(&QLineEdit::setText);

      binder->BindWidget(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, connection_type);
      binder->SetByReference(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal);
   }
};


template<class V, class U, class T>
struct BoundControl<QComboBox, V, U, T>
{
public:
   template<class V, class U, class T>
   BoundControl(ControlBinder* binder, QString name, QComboBox* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T) = nullptr, Qt::ConnectionType connection_type = Qt::AutoConnection)
   {
      auto widget_signal = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
      auto widget_setter = static_cast<void (QComboBox::*)(int)>(&QComboBox::setCurrentIndex);

      binder->BindWidget(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal);
      binder->SetByValue(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal);
   }
};



class ControlBinder 
{

public:

#define Bind(widget, ...) BindImpl(#widget, widget, ##__VA_ARGS__)
#define DirectBind(widget, ...) DirectBindImpl(#widget, widget, ##__VA_ARGS__)
#define QueuedBind(widget, ...) QueuedBindImpl(#widget, widget, ##__VA_ARGS__)

   ControlBinder(QObject* parent, QString object_name)
   {
      settings = new QSettings(parent);
   }

protected:

   template<class W, class V, class U, class T>
   void BindImpl(QString name, W* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T) = nullptr)
   {
      BoundControl<W, V, U, T>* control = new BoundControl<W, V, U, T>(this, name, widget, obj, setter, getter, signal);
   }

   template<class W, class V, class U, class T>
   void DirectBindImpl(QString name, W* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T) = nullptr)
   {
      BoundControl<W, V, U, T>* control = new BoundControl<W, V, U, T>(this, name, widget, obj, setter, getter, signal, Qt::DirectConnection);
   }

   template<class W, class V, class U, class T>
   void QueuedBindImpl(QString name, W* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T) = nullptr)
   {
      BoundControl<W, V, U, T>* control = new BoundControl<W, V, U, T>(this, name, widget, obj, setter, getter, signal, Qt::QueuedConnection);
   }

private:

   template<class W, class U, class T1, class T2>
   void BindWidget(QString name, W* widget, void(W::*widget_setter)(T1), void (W::*widget_signal)(T1), U* obj, void(U::*setter)(T2), T2(U::*getter)(void), void (U::*signal)(T2) = nullptr, Qt::ConnectionType connection_type = Qt::AutoConnection)
   {
      // Connect the widget to the object
      parent->connect(widget, widget_signal, obj, setter, connection_type);

      // Connect the object signal to the widget setter function, if there is one
      if (signal != nullptr)
         parent->connect(obj, signal, widget, widget_setter, connection_type);
   }

   template<class W, class U, class T1, class T2>
   void SetByValue(QString name, W* widget, void(W::*widget_setter)(T1), void (W::*widget_signal)(T1), U* obj, void(U::*setter)(T2), T2(U::*getter)(void), void (U::*signal)(T2) = nullptr)
   {
      // Save widget value on change
      parent->connect(widget, widget_signal,
         [this, name](T1 value) {
         this->settings->setValue(name, QVariant::fromValue(value));
      });


      // Check if settings file contains an entry for this value
      // if so update the object accordingly. Otherwise use the 
      // value already in the object
      T2 v = std::bind(getter, obj)();
      QVariant default_v = QVariant::fromValue(v);
      
      T2 v1 = (settings->value(name, default_v)).value<T2>();

      //std::bind(setter, obj, std::placeholders::_1)(v1);
      std::bind(widget_setter, widget, std::placeholders::_1)(v1);
      
      // Force widget to emit signal
      std::bind(widget_signal, widget, std::placeholders::_1)(v1);

   }

   template<class W, class U, class T>
   void SetByReference(QString name, W* widget, void(W::*widget_setter)(const T&), void (W::*widget_signal)(const T&), U* obj, void(U::*setter)(const T&), const T&(U::*getter)(void), void (U::*signal)(const T&) = nullptr)
   {
      // Save widget value on change
      parent->connect(widget, widget_signal,
         [this, name](T value) {
         this->settings->setValue(name, QVariant::fromValue(value));
      });


      // Check if settings file contains an entry for this value
      // if so update the object accordingly. Otherwise use the 
      // value already in the object
      T v = std::bind(getter, obj)();
      QVariant default_v = QVariant::fromValue(v);

      T v1 = (settings->value(name, default_v)).value<T>();


      std::bind(setter, obj, std::placeholders::_1)(v1);
      std::bind(widget_setter, widget, std::placeholders::_1)(v1);
   }

   QObject* parent;
   QSettings* settings;

   template<class W, class V, class U, class T>
   friend class BoundControl;

};