#pragma once

#include "ArduinoCounter.h"

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>


class PMTStatusWidget : public QWidget
{
   Q_OBJECT

public:
   PMTStatusWidget(QWidget* parent) : 
      QWidget(parent)
   {
      status_label = new QLabel();

      enable_button = new QPushButton();
      enable_button->setText("Enable");
      enable_button->setCheckable(true);

      layout = new QHBoxLayout();
      layout->addWidget(status_label, 1);
      layout->addWidget(enable_button, 0);

      this->setLayout(layout);

      PMTEnabled(false);
      
   }

   void SetArduinoCounter(ArduinoCounter* counter)
   {
      connect(enable_button, &QPushButton::toggled, counter, &ArduinoCounter::SetPMTEnabled);
      connect(counter, &ArduinoCounter::OverloadOccured, this, &PMTStatusWidget::PMTOverloaded);
      connect(counter, &ArduinoCounter::PMTEnabled, this, &PMTStatusWidget::PMTEnabled);
   }

   void PMTOverloaded()
   {
      SetStatus("PMT Overload", "background-color: red; color: white;", false);
   }

   void PMTEnabled(bool enabled)
   {
      if (enabled)
         SetStatus("PMT Active", "color: green;", true);
      else
         SetStatus("PMT Inactive", "", false);
   }

signals:
   void PMTEnableToggled(bool enabled);

private:

   

   void SetStatus(QString text, QString stylesheet, bool enabled)
   {
      status_label->setText(text);
      status_label->setStyleSheet(stylesheet);

      enable_button->setChecked(enabled);
   }

   QString text;
   QString stylesheet;

   QHBoxLayout* layout;
   QLabel* status_label;
   QPushButton* enable_button;
};