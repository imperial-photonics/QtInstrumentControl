
template<class W, class V, class U, class T>
BoundControl<W,V,U,T>::BoundControl(ControlBinder* binder, QString name, W* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T), Qt::ConnectionType connection_type, bool transient)
{
   auto widget_signal = static_cast<void (W::*)(T)>(&W::valueChanged);
   auto widget_setter = static_cast<void (W::*)(T)>(&W::setValue);

   binder->BindWidget(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, connection_type, transient);
   binder->SetByValue(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, transient);
}


template<class V, class U, class T>
BoundControl<QCheckBox,V,U,T>::BoundControl(ControlBinder* binder, QString name, QCheckBox* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T), Qt::ConnectionType connection_type, bool transient)
{
   auto widget_signal = static_cast<void (QCheckBox::*)(T)>(&QCheckBox::toggled);
   auto widget_setter = static_cast<void (QCheckBox::*)(T)>(&QCheckBox::setChecked);

   binder->BindWidget(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, connection_type, transient);
   binder->SetByValue(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, transient);
}


template<class V, class U, class T>
BoundControl<QGroupBox,V,U,T>::BoundControl(ControlBinder* binder, QString name, QGroupBox* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T), Qt::ConnectionType connection_type, bool transient)
{
   auto widget_signal = static_cast<void (QGroupBox::*)(T)>(&QGroupBox::toggled);
   auto widget_setter = static_cast<void (QGroupBox::*)(T)>(&QGroupBox::setChecked);

   binder->BindWidget(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, connection_type, transient);
   binder->SetByValue(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, transient);
}


template<class V, class U>
BoundControl<QLineEdit,V,U,const QString&>::BoundControl(ControlBinder* binder, QString name, QLineEdit* widget, V* obj, void(U::*setter)(const QString&), const QString&(U::*getter)(void), void (U::*signal)(const QString&), Qt::ConnectionType connection_type, bool transient)
{
   auto widget_signal = static_cast<void (QLineEdit::*)(const QString&)>(&QLineEdit::textEdited);
   auto widget_setter = static_cast<void (QLineEdit::*)(const QString&)>(&QLineEdit::setText);

   binder->BindWidget(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, connection_type, transient);
   binder->SetByReference(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, transient);
}



template<class V, class U, class T>
BoundControl<QComboBox,V,U,T>::BoundControl(ControlBinder* binder, QString name, QComboBox* widget, V* obj, void(U::*setter)(T), T(U::*getter)(void), void (U::*signal)(T), Qt::ConnectionType connection_type, bool transient)
{
   auto widget_signal = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
   auto widget_setter = static_cast<void (QComboBox::*)(int)>(&QComboBox::setCurrentIndex);

   binder->BindWidget(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, connection_type, transient);
   binder->SetByValue(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, transient);
}


template<class V, class U>
BoundFilenameControl<V,U>::BoundFilenameControl(ControlBinder* binder, QString name, QLineEdit* widget, QPushButton* button, QString filter, V* obj, void(U::*setter)(const QString&), const QString&(U::*getter)(void), void (U::*signal)(const QString&), Qt::ConnectionType connection_type, bool transient)
{
   auto widget_signal = static_cast<void (QLineEdit::*)(const QString&)>(&QLineEdit::textEdited);
   auto widget_setter = static_cast<void (QLineEdit::*)(const QString&)>(&QLineEdit::setText);

   QObject::connect(button, &QPushButton::pressed, [filter, obj, getter, setter]()
   {
      QString filename = QFileDialog::getSaveFileName(nullptr, "Choose File Name", (obj->*getter)(), filter);
      if (!filename.isEmpty())
         (obj->*setter)(filename);
   });

   binder->BindWidget(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, connection_type, transient);
   binder->SetByReference(name, widget, widget_setter, widget_signal, static_cast<U*>(obj), setter, getter, signal, transient);
}