#include "AndorControlDisplay.h"

#include <QApplication>
#include <QInputDialog>
#include <QErrorMessage>
#include <QGroupBox>
//#include <QSurfaceFormat>

#include <iostream>


AndorCamera* GetCameraFromUser(bool choose_camera, QObject* parent)
{
   QErrorMessage msg;
   QStringList camera_list;
   QVector<int> camera_idx;

   AndorCamera::GetConnectedCameras(camera_list, camera_idx);
   
   // Check if any cameras are connected
   //=========================================
   if (camera_list.length() == 0)
   {
      msg.showMessage("No cameras connected!");
      return 0;
   }


   // Choose camera
   //=========================================
   int selected_idx = 0;
   if (choose_camera)
   {
      bool ok = true;
      QInputDialog dialog;
      QString selected = dialog.getItem(NULL, "Choose Camera", "Connected Cameras", camera_list, 0, false, &ok);
   
      if (!ok) return 0;

      for (int i = 0; i<camera_list.length(); i++)
      {
         if (camera_list[i].compare(selected) == 0)
            selected_idx = i;
      }
   }

   try
   {
      return new AndorCamera(selected_idx, parent);
   }
   catch(...)
   {
      msg.showMessage("Could not connect to camera");
      return nullptr;
   }
}


AndorControlDisplay::AndorControlDisplay(AndorCamera* camera, QWidget* parent) :
   camera(camera), QWidget(parent)
{
   // Temperature Controls
   //===================================================
   QFormLayout* temp_layout = new QFormLayout();
   AddWidget(temp_layout, "SensorTemperature", Float, " C", true);
   AddWidget(temp_layout, "TemperatureStatus", Enumeration, "", true);
   AddWidget(temp_layout, "SensorCooling"    , Boolean);
   AddWidget(temp_layout, "TemperatureControl", Enumeration);
   AddWidget(temp_layout, "FanSpeed", Enumeration);

   QGroupBox* temp_group = new QGroupBox("Temperature");
   temp_group->setLayout(temp_layout);

   // ROI Controls
   //===================================================
   QFormLayout* roi_layout = new QFormLayout();
   AddWidget(roi_layout, "AOIBinning", Enumeration);
   AddWidget(roi_layout, "AOIWidth"  , Integer);
   AddWidget(roi_layout, "AOILeft"   , Integer);
   AddWidget(roi_layout, "AOIHeight" , Integer);
   AddWidget(roi_layout, "AOITop"    , Integer);

   QGroupBox* roi_group = new QGroupBox("ROI");
   roi_group->setLayout(roi_layout);

   // Acqusition Controls
   //===================================================
   QFormLayout* acq_layout = new QFormLayout();
   AddWidget(acq_layout, "ExposureTime"            , Float, " s");
   AddWidget(acq_layout, "FrameRate"               , Float, " Hz");
   AddWidget(acq_layout, "ElectronicShutteringMode", Enumeration);
   AddWidget(acq_layout, "Overlap"                 , Boolean);
   AddWidget(acq_layout, "SimplePreAmpGainControl" , Enumeration);
   AddWidget(acq_layout, "TriggerMode"             , Enumeration);
   AddWidget(acq_layout, "MetadataEnable"          , Boolean);
   AddWidget(acq_layout, "PixelEncoding"           , Enumeration);
   AddWidget(acq_layout, "PixelReadoutRate"        , Enumeration);
   AddWidget(acq_layout, "MaxInterfaceTransferRate", Float, " Hz");
   AddWidget(acq_layout, "ReadoutTime"             , Float);

   QGroupBox* acq_group = new QGroupBox("Acquisition");
   acq_group->setLayout(acq_layout);
 
   // Add control panels to window
   //==================================================
   QVBoxLayout* side_layout = new QVBoxLayout();
   side_layout->addWidget(temp_group);
   side_layout->addWidget(roi_group);
   side_layout->addWidget(acq_group);
   side_layout->setMargin(0);

   setLayout(side_layout);
}

