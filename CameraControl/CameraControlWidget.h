#pragma once

#include "AbstractStreamingCamera.h"

#include <QMainWindow>
#include <QHBoxLayout>
#include <QString>
#include <QFormLayout>
#include <QLabel>
#include <QWindow>
#include <QPushbutton>
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

class CameraControlWidget : public QObject
{
   Q_OBJECT

public:
   CameraControlWidget(AbstractStreamingCamera* camera, QFormLayout* parent, const QString& control, ControlType type, const QString& suffix = "", bool use_timer = false, bool auto_init = true);
   virtual ~CameraControlWidget() {};   
   
   void SetControlLock(bool locked);
   void SetWidgetValue();

signals:

   void ValueChanged();

protected:

   void Init();
   bool event(QEvent *event);

protected:
   QString control;


private:

   bool CreateWidget();

   void WidgetUpdated(QVariant value);
   void BoolWidgetUpdated(bool value);
   void IntWidgetUpdated(int value);
   void FloatWidgetUpdated(double value);
   void TextWidgetUpdated(QString value);
   void EnumerationWidgetUpdated(int index);
   void LimitClicked(Limit limit);

   void SendValueToCamera();
   QVariant proposed_value;

   ControlType type;
   QString suffix;

   QFormLayout* layout;
   QWidget* obj;
   QPushButton* min_btn;
   QPushButton* max_btn;

   QTimer* timer;
   QTimer* value_timer;
   AbstractStreamingCamera* camera;

   int is_implemented;
   bool use_timer;

   QSettings* settings;

   QMutex* mutex;
};