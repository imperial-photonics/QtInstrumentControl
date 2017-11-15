template<class W, class T>
BoundPropertyControl<W,T>::BoundPropertyControl(ControlBinder* binder, QString name, W* widget, QObject* obj, const char* prop, T value)
{
   auto widget_signal = static_cast<void (W::*)(T)>(&W::valueChanged);
   auto widget_setter = static_cast<void (W::*)(T)>(&W::setValue);

   binder->BindWidgetToProperty(name, widget, widget_setter, widget_signal, obj, prop);
   binder->SetPropertyByValue(name, widget, widget_setter, widget_signal, obj, prop);
}

template<class T> 
BoundPropertyControl<QCheckBox,T>::BoundPropertyControl(ControlBinder* binder, QString name, QCheckBox* widget, QObject* obj, const char* prop, T value)
{
   auto widget_signal = &QCheckBox::toggled;
   auto widget_setter = &QCheckBox::setChecked;

   binder->BindWidgetToProperty(name, widget, widget_setter, widget_signal, obj, prop);
   binder->SetPropertyByValue(name, widget, widget_setter, widget_signal, obj, prop);
}

template<class T>
BoundPropertyControl<QGroupBox,T>::BoundPropertyControl(ControlBinder* binder, QString name, QGroupBox* widget, QObject* obj, const char* prop, T value)
{
   auto widget_signal = &QGroupBox::toggled;
   auto widget_setter = &QGroupBox::setChecked;

   binder->BindWidgetToProperty(name, widget, widget_setter, widget_signal, obj, prop);
   binder->SetPropertyByValue(name, widget, widget_setter, widget_signal, obj, prop);
}

template<class T>
BoundPropertyControl<QLineEdit,T>::BoundPropertyControl(ControlBinder* binder, QString name, QLineEdit* widget, QObject* obj, const char* prop, T value)
{
   auto widget_signal = &QLineEdit::textEdited;
   auto widget_setter = &QLineEdit::setText;

   binder->BindWidgetToProperty(name, widget, widget_setter, widget_signal, obj, prop);
   binder->SetPropertyByReference(name, widget, widget_setter, widget_signal, obj, prop);
}

template<class T>
BoundPropertyControl<QComboBox,T>::BoundPropertyControl(ControlBinder* binder, QString name, QComboBox* widget, QObject* obj, const char* prop, T value)
{
   auto widget_signal = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
   auto widget_setter = static_cast<void (QComboBox::*)(int)>(&QComboBox::setCurrentIndex);

   binder->BindWidgetToProperty(name, widget, widget_setter, widget_signal, obj, prop);
   binder->SetPropertyByValue(name, widget, widget_setter, widget_signal, obj, prop);
}
