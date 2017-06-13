#pragma once

#include "ParametricImageSource.h"

#include <QMainWindow>
#include <QHBoxLayout>
#include <QString>
#include <QFormLayout>
#include <QLabel>
#include <QWindow>
#include <QPushButton>
#include <QTimer>
#include <QPair>
#include <QList>
#include <QSettings>

class WidgetEvent : public QEvent
{
public:
   static const QEvent::Type ValueUpdated = static_cast<QEvent::Type>(QEvent::User);
   WidgetEvent() : QEvent(ValueUpdated) {};
   ~WidgetEvent() {};
};

class ParameterWidget : public QObject
{
   Q_OBJECT

public:
   ParameterWidget(ParametricImageSource* camera, QFormLayout* parent, const QString& control, ParameterType type, const QString& suffix = "", bool use_timer = false, bool auto_init = true);
   virtual ~ParameterWidget() {};
   
   void setControlLock(bool locked);
   void setWidgetValue();

signals:

   void valueChanged();

protected:

   void init();
   bool event(QEvent *event);

protected:
   QString control;


private:

   bool createWidget();

   void widgetUpdated(QVariant value);
   void boolWidgetUpdated(bool value);
   void intWidgetUpdated(int value);
   void floatWidgetUpdated(double value);
   void textWidgetUpdated(QString value);
   void enumerationWidgetUpdated(int index);
   void limitClicked(Limit limit);

   void sendValueToCamera();
   QVariant proposed_value;

   ParameterType type;
   QString suffix;

   QFormLayout* layout;
   QWidget* obj;
   QPushButton* min_btn;
   QPushButton* max_btn;

   QTimer* timer;
   QTimer* value_timer;
   ParametricImageSource* camera;

   int is_implemented;
   bool use_timer;

   QSettings* settings;

   QMutex* mutex;
};
