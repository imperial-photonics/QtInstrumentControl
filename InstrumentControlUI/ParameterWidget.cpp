#include "ParameterWidget.h"

#include <QCoreApplication>
#include <QSpinBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QTimer>
#include <QPushButton>
#include <QMutexLocker>

#include <iostream>
#include <cstdint>

ParameterWidget::ParameterWidget(ParametricImageSource* camera, QFormLayout* parent, const QString& control, ParameterType type, const QString& suffix, bool use_timer, bool auto_init) :
QObject(parent),
control(control),
type(type),
layout(parent),
min_btn(nullptr),
max_btn(nullptr),
camera(camera),
use_timer(use_timer),
mutex(camera->control_mutex)
{
   // Timer to delay setting of value after entry (for typing)
   value_timer = new QTimer();
   value_timer->setSingleShot(true);
   value_timer->setInterval(500);
   connect(value_timer, &QTimer::timeout, this, &ParameterWidget::sendValueToCamera);

   if (auto_init)
      init();
}

/*
   Subclass contructor should call this after appropriate checks
*/
void ParameterWidget::init()
{
   timer = new QTimer(this);

   createWidget();
   setWidgetValue();

   settings = new QSettings(this);
   settings->beginGroup(camera->objectName());

   // Check if settings file contains an entry for this value, if so update accordingly. 
   if (settings->contains(control))
   {
      proposed_value = settings->value(control);
      sendValueToCamera();
   }

   if (use_timer)
   {
      connect(timer, &QTimer::timeout, this, &ParameterWidget::setWidgetValue);
      timer->start(500);
   }

   connect(camera, &ParametricImageSource::controlLockUpdated, this, &ParameterWidget::setControlLock, Qt::QueuedConnection);
};


bool ParameterWidget::event(QEvent *event)
{
   switch (event->type()) {
   case WidgetEvent::ValueUpdated:
      setWidgetValue();
      return true;
   default:
      return QObject::event(event);
   }
}


void ParameterWidget::setControlLock(bool locked)
{
   bool enabled = (!locked) && camera->isParameterWritable(control);
   obj->setEnabled(enabled);

   if (min_btn != nullptr)
      min_btn->setEnabled(enabled);
   if (max_btn != nullptr)
      max_btn->setEnabled(enabled);
}

bool ParameterWidget::createWidget()
{
   QMutexLocker lk(mutex);

   // TODO: do we need to expose read only?
   bool is_read_only = camera->isParameterReadOnly(control);

   if (type == Integer)
   {
      QSpinBox* spinbox = new QSpinBox();
      if (suffix.length() > 0)
         spinbox->setSuffix(suffix);
      obj = spinbox;

      QVariant min_step = camera->getParameterMinIncrement(control, type);
      if (min_step.isValid())
         spinbox->setSingleStep(min_step.toInt());

      int mn = camera->getParameterLimit(control, type, Min).toInt();
      spinbox->setMinimum(mn);
      int mx = camera->getParameterLimit(control, type, Max).toInt();
      spinbox->setMinimum(mx);

      connect(spinbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
         this, &ParameterWidget::widgetUpdated);
   }
   else if (type == Float)
   {
      QDoubleSpinBox* spinbox = new QDoubleSpinBox();
      if (suffix.length() > 0)
         spinbox->setSuffix(suffix);
      obj = spinbox;

      int mn = camera->getParameterLimit(control, type, Min).toDouble();
      spinbox->setMinimum(mn);
      int mx = camera->getParameterLimit(control, type, Max).toDouble();
      spinbox->setMinimum(mx);

      connect(spinbox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
         this, &ParameterWidget::floatWidgetUpdated);
   }
   else if (type == Boolean)
   {
      QCheckBox* checkbox = new QCheckBox();
      obj = checkbox;

      connect(checkbox, &QCheckBox::stateChanged, this, &ParameterWidget::boolWidgetUpdated);
   }
   else if (type == Text)
   {
      QLineEdit* edit = new QLineEdit();
      edit->setMaxLength(1023);
      obj = edit;

      connect(edit, &QLineEdit::textEdited, this, &ParameterWidget::textWidgetUpdated);
   }
   else if (type == Enumeration)
   {
      QComboBox* combobox = new QComboBox();
      obj = combobox;

      connect(combobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
         this, &ParameterWidget::enumerationWidgetUpdated);
   }

   if (!(is_read_only) && (type == Integer || type == Float))
   {
      min_btn = new QPushButton();
      max_btn = new QPushButton();

      connect(min_btn, &QPushButton::clicked,
         [this](bool b){ limitClicked(Min); });

      connect(max_btn, &QPushButton::clicked,
         [this](bool b){ limitClicked(Max); });

      QHBoxLayout* hlayout = new QHBoxLayout();
      hlayout->addWidget(obj);
      hlayout->addWidget(min_btn);
      hlayout->addWidget(max_btn);

      layout->addRow(new QLabel(control), hlayout);

   }
   else
   {
      layout->addRow(new QLabel(control), obj);

   }

   return true;
}


void ParameterWidget::setWidgetValue()
{
   if (obj == NULL)
      return;

   QMutexLocker lk(mutex);

   int is_read_only = camera->isParameterReadOnly(control);

   setControlLock(false);

   QVariant value = camera->getParameter(control, type);

   if (type == Integer)
   {
      int value_min = camera->getParameterLimit(control, type, Limit::Min).toInt();
      int value_max = camera->getParameterLimit(control, type, Limit::Max).toInt();

      QSpinBox* spinbox = static_cast<QSpinBox*>(obj);
      spinbox->setMinimum(value_min);
      spinbox->setMaximum(value_max);
      spinbox->setValue(value.toInt());

      if (!is_read_only)
      {
         min_btn->setText(QString::number(value_min));
         max_btn->setText(QString::number(value_max));
      }
   }
   else if (type == Float)
   {
      double value_min = camera->getParameterLimit(control, type, Limit::Min).toDouble();
      double value_max = camera->getParameterLimit(control, type, Limit::Max).toDouble();

      QDoubleSpinBox* spinbox = static_cast<QDoubleSpinBox*>(obj);
      spinbox->setMinimum(value_min);
      spinbox->setMaximum(value_max);
      spinbox->setDecimals(6);
      spinbox->setValue(value.toDouble());

      if (!is_read_only)
      {
         min_btn->setText(QString::number(value_min, 'g', 3));
         max_btn->setText(QString::number(value_max, 'g', 3));
      }
   }
   else if (type == Boolean)
   {
      QCheckBox* checkbox = static_cast<QCheckBox*>(obj);
      checkbox->setChecked(value.toBool());
   }
   else if (type == Text)
   {
      QLineEdit* edit = static_cast<QLineEdit*>(obj);
      edit->setText(value.toString());
   }
   else if (type == Enumeration)
   {
      EnumerationList list = camera->getEnumerationList(control);      
      QComboBox* combobox = static_cast<QComboBox*>(obj);

      // suppress signals while we're updating box
      disconnect(combobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
         this, &ParameterWidget::enumerationWidgetUpdated);

      combobox->clear();
      for (auto& p : list)
      {
         QVariant data(p.second);
         combobox->addItem(p.first, data);
      }

      int qidx = combobox->findData(value.toInt());
      combobox->setCurrentIndex(qidx);

      connect(combobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
         this, &ParameterWidget::enumerationWidgetUpdated);
   }

};

void ParameterWidget::limitClicked(Limit limit)
{
   proposed_value = camera->getParameterLimit(control, type, limit);
   sendValueToCamera();
}



void ParameterWidget::widgetUpdated(QVariant value)
{
   proposed_value = value;
   
   // Don't set value immediately - very annoying if you're tring
   // to type and camera constrains values as you enter them
   // Instead use timer to set value after a short delay
   value_timer->stop();
   value_timer->start();

   // stop update timer
   if (use_timer)
      timer->stop();
}

void ParameterWidget::sendValueToCamera()
{
   {
      QMutexLocker lk(mutex);
      if (!camera->isParameterWritable(control)) return;

      try
      {
         std::cout << "Setting parameter: " << control.toStdString() << " = " << proposed_value.toString().toStdString() << "\n";
         camera->setParameter(control, type, proposed_value);
      }
      catch (std::exception e)
      {
         // In case of error setting value get current value from camera
         std::cout << "Error setting parameter: " << e.what() << "\n";
         setWidgetValue();
      }

      QVariant actual_value = camera->getParameter(control, type);
      settings->setValue(control, actual_value);

      // restart update timer
      if (use_timer && !timer->isActive())
         timer->start();
   }
   emit valueChanged();
}

void ParameterWidget::intWidgetUpdated(int value)
{
   widgetUpdated(QVariant(value));
}

void ParameterWidget::boolWidgetUpdated(bool value)
{
   widgetUpdated(QVariant(value));
}


void ParameterWidget::floatWidgetUpdated(double value)
{
   widgetUpdated(QVariant(value));
}

void ParameterWidget::textWidgetUpdated(QString value)
{
   widgetUpdated(QVariant(value));
}

void ParameterWidget::enumerationWidgetUpdated(int index)
{
   QComboBox* combo = static_cast<QComboBox*>(obj);
   QVariant var = combo->currentData();
   int64_t idx = var.toInt();

   widgetUpdated(QVariant(idx));
}
