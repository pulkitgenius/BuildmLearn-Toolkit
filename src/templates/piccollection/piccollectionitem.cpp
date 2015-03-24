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

#include "templates/piccollection/piccollectionitem.h"

#include "definitions/definitions.h"


PicCollectionItem::PicCollectionItem(QWidget *parent) : QWidget(parent), m_ui(new Ui::PicCollectionItem) {
  m_ui->setupUi(this);
  m_ui->m_lblPicture->setFixedHeight((int) (SIMULATOR_CONTENTS_HEIGHT * 0.4));

  QFont caption_font = m_ui->m_lblImageNumber->font();
  caption_font.setPointSize(caption_font.pointSize() + SIMULATOR_HEADER_SIZE_INCREASE);
  m_ui->m_lblImageNumber->setFont(caption_font);

  connect(m_ui->m_btnNext, SIGNAL(clicked()), this, SIGNAL(nextImageRequested()));
  connect(m_ui->m_btnPrevious, SIGNAL(clicked()), this, SIGNAL(previousImageRequested()));
  connect(m_ui->m_btnGallery, SIGNAL(clicked()), this, SIGNAL(galleryRequested()));
}

PicCollectionItem::~PicCollectionItem() {
  delete m_ui;
}

void PicCollectionItem::setImage(const PicCollectionImage &image, int image_number, int total_images) {
  m_ui->m_btnPrevious->setEnabled(image_number != 1);
  m_ui->m_btnNext->setEnabled(image_number != total_images);
  m_ui->m_lblImageNumber->setText(tr("Image number %1 of %2").arg(QString::number(image_number),
                                                                        QString::number(total_images)));
  m_ui->m_lblImageTitle->setText(image.title());
  m_ui->m_lblDescription->setText(image.description());
  m_ui->m_lblPicture->setPixmap(QPixmap(image.picturePath()).scaled(m_ui->m_lblPicture->size(), Qt::KeepAspectRatio));
}
