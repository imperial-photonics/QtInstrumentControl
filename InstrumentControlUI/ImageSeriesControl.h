#include "ui_ImageSeriesControl.h"
#include "AbstractImageWriter.h"
#include "ControlBinder.h"

class ImageSeriesControl : public QWidget, private ControlBinder, private Ui::ImageSeriesControl
{
   Q_OBJECT

public:
   ImageSeriesControl(QObject* parent) : 
      ControlBinder(parent, objectName())
   {
      setupUi(this);
   }

   void SetImageWriter(AbstractImageWriter* image_writer_) 
   { 
      image_writer = image_writer_; 

      connect(camera_folder_choose_button, &QPushButton::clicked, image_writer, &AbstractImageWriter::ChooseFolder, Qt::DirectConnection); // This is read only
      Bind(camera_folder_edit, image_writer, &AbstractImageWriter::SetFolder, &AbstractImageWriter::GetFolder, &AbstractImageWriter::FolderChanged);
      Bind(camera_filename_root_edit, image_writer, &AbstractImageWriter::SetFilenameRoot, &AbstractImageWriter::GetFilenameRoot, &AbstractImageWriter::FilenameRootChanged);
      Bind(buffer_size_spin, image_writer, &AbstractImageWriter::SetBufferSize, &AbstractImageWriter::GetBufferSize, &AbstractImageWriter::BufferSizeChanged);
   }

private:
   AbstractImageWriter* image_writer;

};