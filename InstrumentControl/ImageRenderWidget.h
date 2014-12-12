#pragma once

#include "AndorCamera.h"

#include <QGLWidget>
#include <QPainter>
#include <QImage>
#include <QLabel>
#include <QPaintEvent>
#include <QMainWindow>
#include <QTimer>
#include <QColor>
#include <QRgb>
#include <QRect>
#include <QStringList>

#include <stdint.h>
#include <string>


QImage CopyToQImage(cv::Mat& cv_image, int bit_shift);

class ImageRenderWidget : public QWidget
{
   Q_OBJECT

public:
   ImageRenderWidget(QWidget *parent = 0);

   void SetSource(ImageSource* source_) { source = source_; }
   void SetBitShift(int bit_shift_);
   void AddImage(cv::Mat image, QString label = QString(""));

   void SelectROI(bool checked);
   void ResetROI() { use_roi = false; }

   void SetOverlayRects(std::vector<QRect> overlay_rects_) { overlay_rects = overlay_rects_; }
   void SetOverlayPoints(std::vector<QPoint> overlay_points_) { overlay_points = overlay_points_; }

   void SetOverlayDisplay(bool overlay_on) { use_overlay = overlay_on; }
   bool GetOverlayDisplay() { return use_overlay; }

   void Redraw();

   void SaveImageWithPrompt();
   void SaveImage(QString filename);

   void SetImageIndex(unsigned int cur_index_);
   unsigned int GetImageIndex() { return cur_index; };

   int GetNumberOfImages() { return cv_image.size(); } 

signals:
   void ROISelectionUpdated(bool selecting);
   void PointClicked(QPointF point);
   void LabelChanged(QString label);

   void MaxImageIndexChanged(unsigned int n_images);
   void ImageIndexChanged(unsigned int index);

protected:
  
   void EnforceAspectRatio();
   QPoint GetTruePos(QPoint pos);

   void resizeEvent(QResizeEvent* event);
   void mouseReleaseEvent(QMouseEvent* event);
   void mousePressEvent(QMouseEvent* event);

   void paintEvent(QPaintEvent *event);
   void CreateColorMap();
      
   void GetImageFromSource();

private:
   QVector<QRgb> colors;
   QImage* image;
   ImageSource* source = nullptr;
   QTimer* timer;

   float ratio;
   int bit_shift;
   bool use_overlay;

   bool selecting_roi;
   bool has_roi_start;

   QPoint roi_start;

   bool use_roi;
   QRect roi;

   QPoint selected_pos;

   std::vector<QRect> overlay_rects;
   std::vector<QPoint> overlay_points;

   std::vector<cv::Mat> cv_image;
   QStringList image_labels;

   QSize sz;

   unsigned int cur_index;
};
