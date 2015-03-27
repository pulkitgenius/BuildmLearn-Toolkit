/*
  Copyright (c) 2012, BuildmLearn Contributors listed at http://buildmlearn.org/people/
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of the BuildmLearn nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef PICCOLLECTIONITEM_H
#define PICCOLLECTIONITEM_H

#include <QWidget>

#include "ui_piccollectionitem.h"

#include "templates/piccollection/piccollectionimage.h"


namespace Ui {
  class PicCollectionItem;
}

class PicCollectionItem : public QWidget {
    Q_OBJECT

  public:
    // Constructors and destructors.
    explicit PicCollectionItem(QWidget *parent = 0);
    virtual ~PicCollectionItem();

    /// \brief Sets new image for this widget.
    /// \param image Image object.
    /// \param image_number Number of the image.
    void setImage(const PicCollectionImage &image, int image_number, int total_images);

  signals:
    void nextImageRequested();
    void galleryRequested();
    void previousImageRequested();
    
  private slots:
    void imageClicked();
    void scaleAnim();
    void rotateAnim();
    void scaleAfterRotation();
    void rotateAfterScale();
    void showItemsAfterRotate();

  private:
    void rotateImage(int angle);
    void scaleImage(int scaleWidth, int scaleHeight, bool anim = true);
    QPixmap m_image;
    QPixmap m_imageSmall;
    int m_picHeightUnclicked;
    int m_picWidthUnclicked;
    int m_picHeightClicked;
    int m_picWidthClicked;
    int m_picWidth;
    int m_picHeight;
    int m_scaleWidth;
    int m_scaleHeight;
    int m_angle;
    int m_startAngle;
    bool m_rotationNeeded;
    bool m_rotation;
    Ui::PicCollectionItem *m_ui;
};

#endif // PICCOLLECTIONITEM_H
