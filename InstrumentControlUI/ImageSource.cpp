
/*
#include <QFileDialog>
#include <opencv2/highgui/highgui.hpp>

void ImageSource::SaveImage()
{
   QString filename = QFileDialog::getSaveFileName(0, "Choose File Name", "", "tif file (*.tif)");

   if (filename.isEmpty())
      return;

   cv::imwrite(filename.toStdString(), GetImage());
}
*/