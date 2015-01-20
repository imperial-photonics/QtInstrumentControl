#include "XimeaControlDisplay.h"
#include "XimeaCameraPrivate.h"
#include <xiApi.h>

#include <QApplication>
#include <QInputDialog>
#include <QErrorMessage>
#include <QGroupBox>

#include <iostream>


XimeaCameraPrivate* GetXimeaFromUser(bool choose_camera, QObject* parent)
{
   QErrorMessage msg;

   QStringList camera_list = XimeaCameraPrivate::GetConnectedCameras();

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

//   try
//   {
      return new XimeaCameraPrivate(selected_idx, parent);
//   }
//   catch (...)
//   {
//      msg.showMessage("Could not connect to camera");
//      return nullptr;
//   }
}


XimeaControlDisplay::XimeaControlDisplay(XimeaCameraPrivate* camera, QWidget* parent) :
camera(camera), QWidget(parent)
{

   // ROI Controls
   //===================================================
   QFormLayout* roi_layout = new QFormLayout();
   AddWidget(roi_layout, XI_PRM_WIDTH, Integer);
   AddWidget(roi_layout, XI_PRM_OFFSET_X, Integer);
   AddWidget(roi_layout, XI_PRM_HEIGHT, Integer);
   AddWidget(roi_layout, XI_PRM_OFFSET_Y, Integer);

   QGroupBox* roi_group = new QGroupBox("ROI");
   roi_group->setLayout(roi_layout);
   
   // Acqusition Controls
   //===================================================
   QFormLayout* acq_layout = new QFormLayout();
   
   AddWidget(acq_layout, XI_PRM_IMAGE_DATA_FORMAT, Enumeration);
   AddWidget(acq_layout, XI_PRM_EXPOSURE, Integer, " us", true); // use timer -> auto exposure is possible
   AddWidget(acq_layout, XI_PRM_GAIN, Float, " dB", true); // use timer -> auto gain is possible
   AddWidget(acq_layout, XI_PRM_DOWNSAMPLING, Integer);
   AddWidget(acq_layout, XI_PRM_FRAMERATE, Float, " Hz");
   AddWidget(acq_layout, XI_PRM_TRG_SOURCE, Enumeration);
   AddWidget(acq_layout, XI_PRM_TRG_SELECTOR, Enumeration);
   AddWidget(acq_layout, XI_PRM_ACQ_TIMING_MODE, Enumeration);

   QGroupBox* acq_group = new QGroupBox("Acquisition");
   acq_group->setLayout(acq_layout);

   // Auto exposure Controls
   //===================================================
   QFormLayout* auto_layout = new QFormLayout();

   AddWidget(auto_layout, XI_PRM_AEAG, Boolean);
   AddWidget(auto_layout, XI_PRM_AE_MAX_LIMIT, Integer, " us");
   AddWidget(auto_layout, XI_PRM_AG_MAX_LIMIT, Float, " dB");
   AddWidget(auto_layout, XI_PRM_EXP_PRIORITY, Float);
   AddWidget(auto_layout, XI_PRM_AEAG_LEVEL, Float);

   QGroupBox* auto_group = new QGroupBox("Auto Exposure/Gain");
   auto_group->setLayout(auto_layout);

   
   // Add control panels to window
   //==================================================
   QVBoxLayout* side_layout = new QVBoxLayout();
   side_layout->addWidget(roi_group);
   side_layout->addWidget(acq_group);
   side_layout->addWidget(auto_group);
   side_layout->setMargin(0);

   setLayout(side_layout);
}

