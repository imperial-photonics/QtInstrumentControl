#include <QFileDialog>
#include <opencv2/highgui/highgui.hpp>

#include "ImageRenderWidget.h"

/*
   Copy a cv::Mat into a QImage.
   Currently supports mono 8,16 bit integer and 32 bit FP images
*/
QImage CopyToQImage(cv::Mat& cv_image, int bit_shift)
{
   cv::Size size = cv_image.size();
   QImage q_image(size.width, size.height, QImage::Format_Indexed8);
   
   int image_type = cv_image.type();

   for (int y = 0; y < size.height; y++)
   {
      unsigned char* image_ptr = q_image.scanLine(y);

      if (image_type == CV_8U)
      {
         unsigned char* data_ptr = cv_image.row(y).data;
         for (int x = 0; x < size.width; x++)
         {
            unsigned char d = data_ptr[x] << (8 - bit_shift);
            image_ptr[x] = d;
         }

      }
      else if (image_type == CV_16U)
      {
         unsigned short* data_ptr = reinterpret_cast<unsigned short*>(cv_image.row(y).data);
         for (int x=0; x < size.width; x++)
         {
            unsigned short d = data_ptr[x] >> bit_shift;
            if (d > 255)
               d = 255;
            image_ptr[x] = d;
         }
      }
      else if (image_type == CV_32F)
      {
         float* data_ptr = reinterpret_cast<float*>(cv_image.row(y).data);
         for (int x=0; x < size.width; x++)
            image_ptr[x] = 255 * data_ptr[x];
      }
   }
   return q_image;
}



ImageRenderWidget::ImageRenderWidget(QWidget *parent)
 : source(nullptr), 
   QWidget(parent), 
   image(NULL), 
   bit_shift(0),
   use_overlay(true), 
   selecting_roi(false), 
   use_roi(false),
   cur_index(-1)
{
   ratio = 1;
   setMinimumSize(400, 400);
   CreateColorMap();

   timer = new QTimer(this);
   connect(timer, &QTimer::timeout, this, &ImageRenderWidget::GetImageFromSource);
   timer->start(16);

}

void ImageRenderWidget::SetBitShift(int bit_shift_)
{ 
   bit_shift = bit_shift_; 
   Redraw();
}


void ImageRenderWidget::Redraw()
{
   update();
}

void ImageRenderWidget::SelectROI(bool checked)
{
   selecting_roi = checked;
   has_roi_start = false;

   if (checked)
      setCursor(Qt::CrossCursor);
   else
      setCursor(Qt::ArrowCursor);

   emit ROISelectionUpdated(checked);
}

void ImageRenderWidget::SetImageIndex(unsigned int cur_index_) 
{ 
   if (cur_index_ >= cv_image.size())
      return;

   cur_index = cur_index_; 
   emit ImageIndexChanged(cur_index);

   Redraw();
}

void ImageRenderWidget::EnforceAspectRatio()
{

   int heightFromWidth = (int)(width()*ratio);
   int widthFromHeight = (int)(height() / ratio);

   if (heightFromWidth <= height())
      sz = QSize(width(), heightFromWidth);
   else
      sz = QSize(widthFromHeight, height());


   resize(sz);
}

void ImageRenderWidget::resizeEvent(QResizeEvent* event)
{
   EnforceAspectRatio();
}

void ImageRenderWidget::mousePressEvent(QMouseEvent* event)
{

   if (selecting_roi && !has_roi_start)
   {
      roi_start = GetTruePos(event->pos());
      has_roi_start = true;
   }
}

/*
   Convert a point @pos from screen coordinates to image coordinates
*/
QPoint ImageRenderWidget::GetTruePos(QPoint pos)
{
   float scale_w = static_cast<float>(roi.width()) / size().width();
   float scale_h = static_cast<float>(roi.height()) / size().height();

   QPoint scaled_pos(pos.x() * scale_w, pos.y() * scale_h);
   scaled_pos += roi.topLeft();

   return scaled_pos;

}

void ImageRenderWidget::mouseReleaseEvent(QMouseEvent* event)
{
   if (cur_index >= cv_image.size())
      return;

   selected_pos = GetTruePos(event->pos());

   int a = cv_image[cur_index].type();
   cv::Size s = cv_image[cur_index].size();


   if (selecting_roi && has_roi_start)
   {

      if (selected_pos.x() > roi_start.x() && selected_pos.y() > roi_start.y())
      {
         QPoint top_left = roi.topLeft() + roi_start;
         QPoint bottom_right = roi.topLeft() + selected_pos;

         roi = QRect(top_left, bottom_right);
         use_roi = true;
      }
      else
      {
         use_roi = false;
      }
      has_roi_start = false;
   }
   else
   {
      emit PointClicked(selected_pos);
   }

}

/*
   If we're connected to a source then get the latest image. 
*/
void ImageRenderWidget::GetImageFromSource()
{
   if (source == nullptr)
      return;
   
   if (cv_image.empty())
   {
      cv_image.push_back(cv::Mat(1, 1, CV_8U));
      image_labels.push_back("");
      cur_index = 0;
   }

   // Get the latest image from the source
   cv_image[cur_index] = source->GetImage();

   Redraw();
}

/*
   Add an image to the display and show the new image
*/
void ImageRenderWidget::AddImage(cv::Mat image, QString label)
{ 
   // Append label first since counts are based on cv_image size
   image_labels.push_back(label);
   cv_image.push_back(image);

   int sz = static_cast<int>(cv_image.size()) - 1;
   emit MaxImageIndexChanged(sz);
   SetImageIndex(sz);

   Redraw();
}

/*
   Update the display
*/
void ImageRenderWidget::paintEvent(QPaintEvent *event)
{
   if (cur_index >= cv_image.size())
      return;

   cv::Mat im = cv_image[cur_index];
   QString im_label = image_labels[cur_index];

   QPainter painter;
   painter.begin(this);

   cv::Size image_size = im.size();

   if (image_size.area() > 0)
   {
      float new_ratio = (float)image_size.height / image_size.width;

      if (new_ratio != ratio)
      {
         ratio = new_ratio;
         EnforceAspectRatio();
      }

      QImage image = CopyToQImage(im, bit_shift);


      image.setColorTable(colors);


      QRect window_rect(QPoint(0, 0), size());

      if (!use_roi)
         roi = QRect(0, 0, image_size.width, image_size.height);

      if ((image_size.width > 1) && (image_size.height > 1))
      {
         painter.drawImage(window_rect, image, roi);
      }

      float scale_w = static_cast<float>(roi.width()) / size().width();
      float scale_h = static_cast<float>(roi.height()) / size().height();

      if (use_overlay)
      {
         painter.setPen(QPen(Qt::red, 1));

         for (auto rect : overlay_rects)
         {

            rect = rect.translated(-roi.topLeft());
            QPoint top_left(rect.topLeft().x() / scale_w, rect.topLeft().y() / scale_h);
            QPoint bottom_right(rect.bottomRight().x() / scale_w, rect.bottomRight().y() / scale_h);

            QRect r(top_left, bottom_right);
            painter.drawRect(r);
         }

         painter.setPen(QPen(Qt::green, 3));

         for (auto point : overlay_points)
         {
            point -= roi.topLeft();
            QPoint p(point.x() / scale_w, point.y() / scale_h);
            painter.drawPoint(p);
         }
      }
   }

   painter.end();

   uint16_t v = 0;
   if (selected_pos.x() < image_size.width &&
      selected_pos.x() >= 0 &&
      selected_pos.y() < image_size.height &&
      selected_pos.y() >= 0)
   {
      if (im.type() == CV_16U)
         v = im.at<uint16_t>(selected_pos.y(), selected_pos.x());
      else if (im.type() == CV_8U)
         v = im.at<uint8_t>(selected_pos.y(), selected_pos.x());
      else if (im.type() == CV_32F)
         v = im.at<float>(selected_pos.y(), selected_pos.x());
   }

   double meanv = 0;
   if (image_size.area() > 0)
      meanv = cv::mean(im)[0];


   if (!im_label.isEmpty())
      im_label.append(". ");
   QString label = QString("%1Point (%2, %3) : %4,  Average : %5").arg(im_label).arg(selected_pos.x()).arg(selected_pos.y()).arg(v).arg(meanv);
   emit LabelChanged(label);

}

void ImageRenderWidget::CreateColorMap()
{
   colors.reserve(255);

   for (int i = 0; i < 255; i++)
   {
      QColor c(i, i, i);
      colors.push_back(c.rgb());
   }
}

/*
   Prompt the user for a filename and save the image
*/
void ImageRenderWidget::SaveImageWithPrompt()
{
   QString filename = QFileDialog::getSaveFileName(0, "Choose File Name", "", "tif file (*.tif)");
   SaveImage(filename);
}

/*
   Save the images to a file with a specfied name
   The image labels are appended to the filename
*/
void ImageRenderWidget::SaveImage(QString filename)
{
   if (filename.isEmpty())
      return;

   for (int i = 0; i < cv_image.size(); i++)
   {
      QString fname = filename;
      if (image_labels[i].size() > 0)
      {
         QString fsub = QString(" %1.tif").arg(image_labels[i]);
         fname.replace(".tif", fsub);
      }
      cv::imwrite(fname.toStdString(), cv_image[i]);
   }
}