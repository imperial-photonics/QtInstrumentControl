#pragma once

#include "BoundControl.h"
#include "BoundPropertyControl.h"

#include <QMetaProperty>
#include <functional>

class ControlBinder 
{

public:

#define BindProperty(widget, ...) BindPropertyImpl(#widget, widget, ##__VA_ARGS__)

#define Bind(widget, ...) BindImpl(#widget, widget, ##__VA_ARGS__)
#define DirectBind(widget, ...) DirectBindImpl(#widget, widget, ##__VA_ARGS__)
#define QueuedBind(widget, ...) QueuedBindImpl(#widget, widget, ##__VA_ARGS__)
#define FilenameBind(widget, ...) FilenameBindImpl(#widget, widget, ##__VA_ARGS__)

   ControlBinder(QObject* parent, QString object_name)
   {
      settings = new QSettings(parent);
   }

protected:

   template<class W>
   void BindPropertyImpl(QString name, W* widget, QObject* obj, const char* prop)
   {
      BoundPropertyControl<W>* control = new BoundPropertyControl<W>(this, name, widget, obj, prop);
   }


   template<class W, class V, class U, class T>
   void BindImpl(QString name, W* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T) = nullptr, bool transient = false)
   {
      BoundControl<W, V, U, T>* control = new BoundControl<W, V, U, T>(this, name, widget, obj, setter, getter, signal, Qt::AutoConnection, transient);
   }

   template<class W, class V, class U, class T>
   void BindImpl(QString name, W* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), bool transient)
   {
      void (U::*signal)(T) = nullptr;
      BoundControl<W, V, U, T>* control = new BoundControl<W, V, U, T>(this, name, widget, obj, setter, getter, signal, Qt::AutoConnection, transient);
   }


   template<class W, class V, class U, class T>
   void DirectBindImpl(QString name, W* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T) = nullptr, bool transient = false)
   {
      BoundControl<W, V, U, T>* control = new BoundControl<W, V, U, T>(this, name, widget, obj, setter, getter, signal, Qt::DirectConnection, transient);
   }

   template<class W, class V, class U, class T>
   void DirectBindImpl(QString name, W* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), bool transient)
   {
      void (U::*signal)(T) = nullptr;
      BoundControl<W, V, U, T>* control = new BoundControl<W, V, U, T>(this, name, widget, obj, setter, getter, signal, Qt::DirectConnection, transient);
   }

   template<class W, class V, class U, class T>
   void QueuedBindImpl(QString name, W* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T) = nullptr, bool transient = false)
   {
      BoundControl<W, V, U, T>* control = new BoundControl<W, V, U, T>(this, name, widget, obj, setter, getter, signal, Qt::QueuedConnection, transient);
   }

   template<class W, class V, class U, class T>
   void QueuedBindImpl(QString name, W* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), bool transient)
   {
      void (U::*signal)(T) = nullptr;
      BoundControl<W, V, U, T>* control = new BoundControl<W, V, U, T>(this, name, widget, obj, setter, getter, signal, Qt::QueuedConnection, transient);
   }

   template<class V, class U>
   void FilenameBindImpl(QString name, QLineEdit* widget, QPushButton* button, QString filter, V* obj, void(U::*setter)(const QString&), const QString&(U::*getter)(void), void (U::*signal)(const QString&) = nullptr, bool transient = false)
   {
      BoundFilenameControl<V, U>* control = new BoundFilenameControl<V, U>(this, name, widget, button, filter, obj, setter, getter, signal);
   }

   template<class V, class U>
   void FilenameBindImpl(QString name, QLineEdit* widget, QPushButton* button, QString filter, V* obj, void(U::*setter)(const QString&), const QString&(U::*getter)(void), bool transient = false)
   {
      void (U::*signal)(const QString&) = nullptr;
      BoundFilenameControl<V, U>* control = new BoundFilenameControl<V, U>(this, name, widget, button, filter, obj, setter, getter, signal);
   }

private:

   template<class W, class U, class T1, class T2>
   void BindWidget(QString name, W* widget, void(W::*widget_setter)(T1), void (W::*widget_signal)(T1), U* obj, void(U::*setter)(T2), T2(U::*getter)(void), void (U::*signal)(T2) = nullptr, Qt::ConnectionType connection_type = Qt::AutoConnection, bool transient = false)
   {
      // Connect the widget to the object
      parent->connect(widget, widget_signal, obj, setter, connection_type);

      // Connect the object signal to the widget setter function, if there is one
      if (signal != nullptr)
         parent->connect(obj, signal, widget, widget_setter, connection_type);
   }

   template<class W, class WB, class T1>
   void BindWidgetToProperty(QString name, W* widget, void(WB::*widget_setter)(T1), void (WB::*widget_signal)(T1), QObject* obj, const char* prop)
   {
      const QMetaObject* meta = obj->metaObject();
      int idx = meta->indexOfProperty(prop);
      QMetaProperty metaprop = meta->property(idx);

      if (idx == -1)
         throw std::runtime_error("Property not found");

      // Connect the widget to the object
      parent->connect(widget, widget_signal, [=](T1 v) { obj->setProperty(prop,v); });

      // Connect the object signal to the widget setter function, if there is one
      //if (metaprop.hasNotifySignal())
      //   parent->connect(obj, metaprop.notifySignal().methodSignature(), widget, widget_setter);
   }


   template<class W, class U, class T1, class T2>
   void SetByValue(QString name, W* widget, void(W::*widget_setter)(T1), void (W::*widget_signal)(T1), U* obj, void(U::*setter)(T2), T2(U::*getter)(void), void (U::*signal)(T2) = nullptr, bool transient = false)
   {
      // Save widget value on change
      if (!transient)
      {
         parent->connect(widget, widget_signal,
            [this, name](T1 value) {
            this->settings->setValue(name, QVariant::fromValue(value));
         });
      }

      // Check if settings file contains an entry for this value
      // if so update the object accordingly. Otherwise use the 
      // value already in the object
      T2 v = (obj->*getter)();
      QVariant default_v = QVariant::fromValue(v);
      
      if (!transient)
         v = (settings->value(name, default_v)).value<T2>();
      
      (widget->*widget_setter)(v);

      // Force widget to emit signal
      (widget->*widget_signal)(v);
   }

   template<class W, class WB, class T>
   void SetPropertyByValue(QString name, W* widget, void(WB::*widget_setter)(T), void (WB::*widget_signal)(T), QObject* obj, const char* prop)
   {
      parent->connect(widget, widget_signal,
         [this, name](T value) {
         this->settings->setValue(name, QVariant::fromValue(value));
      });

      // Check if settings file contains an entry for this value
      // if so update the object accordingly. Otherwise use the 
      // value already in the object
      QVariant default_v = obj->property(prop);
      T v = (settings->value(name, default_v)).value<T>();
      
      (widget->*widget_setter)(v);

      // Force widget to emit signal
      (widget->*widget_signal)(v);
   }

   template<class W, class U, class T>
   void SetByReference(QString name, W* widget, void(W::*widget_setter)(const T&), void (W::*widget_signal)(const T&), U* obj, void(U::*setter)(const T&), const T&(U::*getter)(void), void (U::*signal)(const T&) = nullptr, bool transient = false)
   {
      // Save widget value on change
      if (!transient)
      {
         // Save widget value on change
         parent->connect(widget, widget_signal,
            [this, name](T value) {
            this->settings->setValue(name, QVariant::fromValue(value));
         });
      }


      // Check if settings file contains an entry for this value
      // if so update the object accordingly. Otherwise use the 
      // value already in the object
      T v = (obj->*getter)();
      QVariant default_v = QVariant::fromValue(v);
      
      if (!transient)
         v = (settings->value(name, default_v)).value<T>();
      
      (obj->*setter)(v);
      (widget->*widget_setter)(v);
   }

   template<class W, class WB, class T>
   void SetPropertyByReference(QString name, W* widget, void(WB::*widget_setter)(const T&), void (WB::*widget_signal)(const T&), QObject* obj, const char* prop)
   {
      parent->connect(widget, widget_signal,
         [this, name](T value) {
         this->settings->setValue(name, QVariant::fromValue(value));
      });

      // Check if settings file contains an entry for this value
      // if so update the object accordingly. Otherwise use the 
      // value already in the object
      QVariant default_v = obj->property(prop);
      T v = (settings->value(name, default_v)).value<T>();

      (widget->*widget_setter)(v);

      // Force widget to emit signal
      (widget->*widget_signal)(v);
   }

   QObject* parent;
   QSettings* settings;

   template<class W, class V, class U, class T>
   friend class BoundControl;

   template<class W>
   friend class BoundPropertyControl;

   template<class V, class U>
   friend class BoundFilenameControl;
};

#include "BoundControlImpl.h"
#include "BoundPropertyControlImpl.h"