#pragma once

#include "ui_ImageRenderWindow.h"

class ImageRenderWindow : public QWidget, protected Ui::ImageRenderWindow
{
   Q_OBJECT

public:

   ImageRenderWindow(QString title, ImageSource* source = nullptr) 
   {   
      setupUi(this);

      setWindowTitle(title);

      render_widget->SetSource(source);
      render_widget->SetBitShift(8);
      
      connect(contrast_slider, &QSlider::valueChanged, render_widget, &ImageRenderWidget::SetBitShift);
      contrast_slider->setValue(8);

      connect(reset_button, &QToolButton::clicked, render_widget, &ImageRenderWidget::ResetROI);
      connect(zoom_button,  &QToolButton::clicked, render_widget, &ImageRenderWidget::SelectROI);
      connect(save_button,  &QToolButton::clicked, render_widget, &ImageRenderWidget::SaveImageWithPrompt);

      connect(render_widget,   &ImageRenderWidget::ROISelectionUpdated, zoom_button, &QToolButton::setChecked);
      connect(show_overlay_button, &QToolButton::toggled, render_widget, &ImageRenderWidget::SetOverlayDisplay);

      image_scroll->setValue(render_widget->GetImageIndex());
      connect(image_scroll, &QScrollBar::valueChanged, render_widget, &ImageRenderWidget::SetImageIndex);
      connect(render_widget, &ImageRenderWidget::ImageIndexChanged, image_scroll, &QScrollBar::setValue);
      connect(render_widget, &ImageRenderWidget::MaxImageIndexChanged, image_scroll, &QScrollBar::setMaximum);

      connect(render_widget, &ImageRenderWidget::LabelChanged, widget_label, &QLabel::setText);

   }

   ImageRenderWidget* GetRenderWidget() { return render_widget; };

};
