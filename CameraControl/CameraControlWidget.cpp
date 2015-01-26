#include "CameraControlWidget.h"

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

CameraControlWidget::CameraControlWidget(AbstractStreamingCamera* camera, QFormLayout* parent, const QString& control, ControlType type, const QString& suffix, bool use_timer, bool auto_init) :
QObject(parent),
camera(camera),
layout(parent),
mutex(camera->control_mutex),
control(control),
type(type),
use_timer(use_timer),
max_btn(nullptr),
min_btn(nullptr)
{
   // Timer to delay setting of value after entry (for typing)
   value_timer = new QTimer();
   value_timer->setSingleShot(true);
   value_timer->setInterval(500);
   connect(value_timer, &QTimer::timeout, this, &CameraControlWidget::SendValueToCamera);

   if (auto_init)
      Init();
}

/*
   Subclass contructor should call this after appropriate checks
*/
void CameraControlWidget::Init()
{
   timer = new QTimer(this);

   CreateWidget();
   SetWidgetValue();

   settings = new QSettings(this);
   settings->beginGroup(camera->objectName());

   // Check if settings file contains an entry for this value, if so update accordingly. 
   if (settings->contains(control))
   {
      proposed_value = settings->value(control);
      SendValueToCamera();
   }

   if (use_timer)
   {
      connect(timer, &QTimer::timeout, this, &CameraControlWidget::SetWidgetValue);
      timer->start(500);
   }

   connect(camera, &AbstractStreamingCamera::ControlLockUpdated, this, &CameraControlWidget::SetControlLock, Qt::QueuedConnection);
};


bool CameraControlWidget::event(QEvent *event)
{
   switch (event->type()) {
   case WidgetEvent::ValueUpdated:
      SetWidgetValue();
      return true;
   default:
      return QObject::event(event);
   }
}


void CameraControlWidget::SetControlLock(bool locked)
{
   bool enabled = (!locked) && camera->IsParameterWritable(control);
   obj->setEnabled(enabled);

   if (min_btn != nullptr)
      min_btn->setEnabled(enabled);
   if (max_btn != nullptr)
      max_btn->setEnabled(enabled);
}

bool CameraControlWidget::CreateWidget()
{
   QMutexLocker lk(mutex);

   // TODO: do we need to expose read only?
   bool is_read_only = camera->IsParameterReadOnly(control);

   if (type == Integer)
   {
      QSpinBox* spinbox = new QSpinBox();
      if (suffix.length() > 0)
         spinbox->setSuffix(suffix);
      obj = spinbox;

      QVariant min_step = camera->GetParameterMinIncrement(control, type);
      if (min_step.isValid())
         spinbox->setSingleStep(min_step.toInt());

      int mn = camera->GetParameterLimit(control, type, Min).toInt();
      spinbox->setMinimum(mn);
      int mx = camera->GetParameterLimit(control, type, Max).toInt();
      spinbox->setMinimum(mx);

      connect(spinbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
         this, &CameraControlWidget::WidgetUpdated);
   }
   else if (type == Float)
   {
      QDoubleSpinBox* spinbox = new QDoubleSpinBox();
      if (suffix.length() > 0)
         spinbox->setSuffix(suffix);
      obj = spinbox;

      int mn = camera->GetParameterLimit(control, type, Min).toDouble();
      spinbox->setMinimum(mn);
      int mx = camera->GetParameterLimit(control, type, Max).toDouble();
      spinbox->setMinimum(mx);

      connect(spinbox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
         this, &CameraControlWidget::FloatWidgetUpdated);
   }
   else if (type == Boolean)
   {
      QCheckBox* checkbox = new QCheckBox();
      obj = checkbox;

      connect(checkbox, &QCheckBox::stateChanged, this, &CameraControlWidget::BoolWidgetUpdated);
   }
   else if (type == Text)
   {
      QLineEdit* edit = new QLineEdit();
      edit->setMaxLength(1023);
      obj = edit;

      connect(edit, &QLineEdit::textEdited, this, &CameraControlWidget::TextWidgetUpdated);
   }
   else if (type == Enumeration)
   {
      QComboBox* combobox = new QComboBox();
      obj = combobox;

      connect(combobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
         this, &CameraControlWidget::EnumerationWidgetUpdated);
   }

   if (!(is_read_only) && (type == Integer || type == Float))
   {
      min_btn = new QPushButton();
      max_btn = new QPushButton();

      connect(min_btn, &QPushButton::clicked,
         [this](bool b){ LimitClicked(Min); });

      connect(max_btn, &QPushButton::clicked,
         [this](bool b){ LimitClicked(Max); });

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


void CameraControlWidget::SetWidgetValue()
{
   if (obj == NULL)
      return;

   QMutexLocker lk(mutex);

   int is_read_only = camera->IsParameterReadOnly(control);

   SetControlLock(false);

   QVariant value = camera->GetParameter(control, type);

   if (type == Integer)
   {
      int value_min = camera->GetParameterLimit(control, type, Limit::Min).toInt();
      int value_max = camera->GetParameterLimit(control, type, Limit::Max).toInt();

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
      double value_min = camera->GetParameterLimit(control, type, Limit::Min).toDouble();
      double value_max = camera->GetParameterLimit(control, type, Limit::Max).toDouble();

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
      EnumerationList list = camera->GetEnumerationList(control);      
      QComboBox* combobox = static_cast<QComboBox*>(obj);

      // suppress signals while we're updating box
      disconnect(combobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
         this, &CameraControlWidget::EnumerationWidgetUpdated);

      combobox->clear();
      for (auto& p : list)
      {
         QVariant data(p.second);
         combobox->addItem(p.first, data);
      }

      int qidx = combobox->findData(value.toInt());
      combobox->setCurrentIndex(qidx);

      connect(combobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
         this, &CameraControlWidget::EnumerationWidgetUpdated);
   }

};

void CameraControlWidget::LimitClicked(Limit limit)
{
   proposed_value = camera->GetParameterLimit(control, type, limit);
   SendValueToCamera();
}



void CameraControlWidget::WidgetUpdated(QVariant value)
{
   proposed_value = value;
   std::cout << "Widget Updated, proposed value: " << value.toString().toStdString() << "\n";

   // Don't set value immediately - very annoying if you're tring
   // to type and camera constrains values as you enter them
   // Instead use timer to set value after a short delay
   value_timer->stop();
   value_timer->start();

   // stop update timer
   if (use_timer)
      timer->stop();
}

void CameraControlWidget::SendValueToCamera()
{
   QMutexLocker lk(mutex);
   if (!camera->IsParameterWritable(control)) return;

   try
   {
      std::cout << "Sending value to camera: " << proposed_value.toString().toStdString() << "\n";

      camera->SetParameter(control, type, proposed_value);
   }
   catch (std::exception e)
   {
      // In case of error setting value get current value from camera
      std::cout << "Error setting parameter: " << e.what() << "\n";
      SetWidgetValue();
   }

   QVariant actual_value = camera->GetParameter(control, type);
   settings->setValue(control, actual_value);

   // restart update timer
   if (use_timer && !timer->isActive())
      timer->start();
   
   emit ValueChanged();
}

void CameraControlWidget::IntWidgetUpdated(int value)
{
   WidgetUpdated(QVariant(value));
}

void CameraControlWidget::BoolWidgetUpdated(bool value)
{
   WidgetUpdated(QVariant(value));
}


void CameraControlWidget::FloatWidgetUpdated(double value)
{
   WidgetUpdated(QVariant(value));
}

void CameraControlWidget::TextWidgetUpdated(QString value)
{
   WidgetUpdated(QVariant(value));
}

void CameraControlWidget::EnumerationWidgetUpdated(int index)
{
   QComboBox* combo = static_cast<QComboBox*>(obj);
   QVariant var = combo->currentData();
   int64_t idx = var.toInt();

   WidgetUpdated(QVariant(idx));
}
