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

#include "templates/piccollection/piccollectionsimulator.h"

#include "core/templatecore.h"
#include "templates/piccollection/piccollectioneditor.h"
#include "templates/piccollection/piccollectionitem.h"
#include "templates/piccollection/labelwithclick.h"
#include "definitions/definitions.h"
#include "miscellaneous/application.h"
#include "miscellaneous/skinfactory.h"


PicCollectionSimulator::PicCollectionSimulator(TemplateCore *core, QWidget *parent)
  : TemplateSimulator(core, parent),
    m_ui(new Ui::PicCollectionSimulator) {
  m_ui->setupUi(this);

  QFont caption_font = m_ui->m_lblHeading->font();
  caption_font.setPointSize(caption_font.pointSize() + SIMULATOR_HEADING_SIZE_INCREASE);
  m_ui->m_lblHeading->setFont(caption_font);

  connect(m_ui->m_btnStart, SIGNAL(clicked()), this, SLOT(start()));
}

PicCollectionSimulator::~PicCollectionSimulator() {
  qDebug("Destroying PicCollectionSimulator instance.");

  delete m_ui;
}

bool PicCollectionSimulator::startSimulation() {
  PicCollectionEditor *editor = static_cast<PicCollectionEditor*>(core()->editor());

  if (!editor->canGenerateApplications()) {
    // There are no active images or Collection does not
    // containt its name or author name.
    return false;
  }

  // Remove existing images.
  while (m_ui->m_phoneWidget->count() > 3) {
    QWidget *image_widget = m_ui->m_phoneWidget->widget(3);

    m_ui->m_phoneWidget->removeWidget(image_widget);
    image_widget->deleteLater();
  }

  // Remove exiting thumbnails.
  clearLayout(m_ui->m_gallery);

  // Load the images, setup the collection and start it.
  m_ui->m_btnStart->setEnabled(true);
  m_ui->m_lblAuthor->setText(editor->m_ui->m_txtAuthor->lineEdit()->text());
  m_ui->m_lblHeading->setText(editor->m_ui->m_txtName->lineEdit()->text());

  int image_number = 1;
  QList<PicCollectionImage> images = editor->activeImages();
  hlayout = new QHBoxLayout();
  
  foreach (const PicCollectionImage &image, images) {
    PicCollectionItem *item = new PicCollectionItem(m_ui->m_phoneWidget);

    connect(item, SIGNAL(nextImageRequested()), this, SLOT(moveToNextImage()));
    connect(item, SIGNAL(previousImageRequested()), this, SLOT(moveToPreviousImage()));
    connect(item, SIGNAL(galleryRequested()), this, SLOT(moveToGallery()));

    item->setImage(image, image_number++, images.size());
    m_ui->m_phoneWidget->insertWidget(m_ui->m_phoneWidget->count(), item);
    
    //For thumbnails
    int thumbNo = m_ui->m_phoneWidget->count() - 4;
    
    LabelWithClick *thumbnailpic = new LabelWithClick(0, thumbNo);
    connect(thumbnailpic, SIGNAL(clicked(int)), this, SLOT(goToSelectedImage(int)));
    thumbnailpic->setMaximumHeight((int) (SIMULATOR_CONTENTS_HEIGHT * 0.27));
	thumbnailpic->setMaximumWidth((int) (SIMULATOR_CONTENTS_WIDTH * 0.27));
    
    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);	
    m_ui->m_gallery->insertItem(-1, spacer);
    
    if (thumbNo%3 == 0) {
		hlayout = new QHBoxLayout();
		hlayout->setContentsMargins(QMargins(0,0,0,0));
		hlayout->insertWidget(0,thumbnailpic);
		QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
		hlayout->insertItem(-1,spacer);
		m_ui->m_gallery->insertLayout(thumbNo/3, hlayout);
	}
	else if (thumbNo%3 == 1) 
		hlayout->insertWidget(1,thumbnailpic);
	else {
		hlayout->insertWidget(2,thumbnailpic);
		QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
		hlayout->insertItem(0,spacer);
	}
		
    thumbnailpic->setPixmap(QPixmap(image.picturePath()).scaled(thumbnailpic->size(), Qt::KeepAspectRatio));
  }
  
  m_ui->m_numberofImages->setText(tr("%1 Images").arg(QString::number(images.size())));
  
  // Go to "start" page and begin.
  m_ui->m_phoneWidget->setCurrentIndex(1);
  return true;
}

bool PicCollectionSimulator::stopSimulation() {
  m_ui->m_phoneWidget->setCurrentIndex(0);

  emit canGoBackChanged(false);

  return true;
}

void PicCollectionSimulator::goToSelectedImage(int thumbNo) {
  m_ui->m_phoneWidget->slideInIdx(thumbNo + 3);
}

void PicCollectionSimulator::clearLayout(QLayout* layout) {
  while (QLayoutItem* item = layout->takeAt(0)) {
	if (QWidget* widget = item->widget())
	   delete widget;
	if (QLayout* childLayout = item->layout())
		clearLayout(childLayout);
	delete item;
  }
}

bool PicCollectionSimulator::goBack() {
  return false;
}

void PicCollectionSimulator::start() {
  moveToNextImage();
}

void PicCollectionSimulator::moveToNextImage() {
  //m_ui->m_phoneWidget->setCurrentIndex(m_ui->m_phoneWidget->currentIndex() + 1);
  if (m_ui->m_phoneWidget->currentIndex() == m_ui->m_phoneWidget->count() - 1)
	m_ui->m_phoneWidget->slideInIdx(3);
  else
	m_ui->m_phoneWidget->slideInNext();
}

void PicCollectionSimulator::moveToPreviousImage() {
  //m_ui->m_phoneWidget->setCurrentIndex(m_ui->m_phoneWidget->currentIndex() - 1);
  if (m_ui->m_phoneWidget->currentIndex() == 3)
	m_ui->m_phoneWidget->slideInIdx(m_ui->m_phoneWidget->count() - 1);
  else
	m_ui->m_phoneWidget->slideInPrev();
}

void PicCollectionSimulator::moveToGallery() {
  m_ui->m_phoneWidget->slideInIdx(2);
}
