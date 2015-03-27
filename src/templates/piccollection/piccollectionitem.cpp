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
#include <QTimer> 
#include <iostream>

PicCollectionItem::PicCollectionItem(QWidget *parent) : QWidget(parent), m_ui(new Ui::PicCollectionItem) {
  m_ui->setupUi(this);

  m_picHeightUnclicked = (int) (SIMULATOR_CONTENTS_HEIGHT * 0.5);
  m_picWidthUnclicked = (int) (SIMULATOR_CONTENTS_WIDTH * 0.90);
  m_picHeightClicked = (int) (SIMULATOR_CONTENTS_HEIGHT * 0.90);
  m_picWidthClicked = (int) (SIMULATOR_CONTENTS_WIDTH * 0.95);
  m_rotationNeeded = false;
  m_ui->m_lblImageTitle->setFixedHeight(((int) (SIMULATOR_CONTENTS_HEIGHT * 0.125))+1);
  m_ui->m_lblDescription->setFixedHeight((int) (SIMULATOR_CONTENTS_HEIGHT * 0.125));
  
  m_angle = 0;
  m_startAngle = 0;

  connect(m_ui->m_lblPicture, SIGNAL(clicked(int)), this, SLOT(imageClicked()));
  QString style = "QPushButton{min-height:1.5em; font:1em; margin:0 1px 0 1px; color: white;\
							   background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #329932, stop: 1 #004C00);\
							   border-style: outset;border-radius: 3px; border-width: 1px; border-color: #50873a;}\
							   QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #004C00, stop: 1 #329932);}";
  
  m_ui->m_btnNext->setStyleSheet(style);
  m_ui->m_btnPrevious->setStyleSheet(style);

  style = "QPushButton{min-height:1.5em; font:1em; margin:0 1px 0 1px; color: white;\
					   background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #ff3232, stop: 1 #e50000);\
					   border-style: outset;border-radius: 3px; border-width: 1px; border-color: #ff0000;}\
					   QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e50000, stop: 1 #ff3232);}";
  
  m_ui->m_btnGallery->setStyleSheet(style);
  
  style = "QLabel{color: #9b8442;}";
  m_ui->m_lblImageTitle->setStyleSheet(style);

  QFont caption_font = m_ui->m_lblImageNumber->font();
  caption_font.setPointSize(caption_font.pointSize() + 5);
  m_ui->m_lblImageNumber->setFont(caption_font);
  
  QFont image_title("Impact", 20, QFont::Bold);
  m_ui->m_lblImageTitle->setFont(image_title);

  connect(m_ui->m_btnNext, SIGNAL(clicked()), this, SIGNAL(nextImageRequested()));
  connect(m_ui->m_btnPrevious, SIGNAL(clicked()), this, SIGNAL(previousImageRequested()));
  connect(m_ui->m_btnGallery, SIGNAL(clicked()), this, SIGNAL(galleryRequested()));
}

PicCollectionItem::~PicCollectionItem() {
  delete m_ui;
}

void PicCollectionItem::setImage(const PicCollectionImage &image, int image_number, int total_images) {
  m_ui->m_btnPrevious->setEnabled(total_images != 1);
  m_ui->m_btnNext->setEnabled(total_images != 1);
  m_ui->m_lblImageNumber->setText(tr("%1 / %2").arg(QString::number(image_number),
                                                                        QString::number(total_images)));
  m_ui->m_lblImageTitle->setText(image.title());
  m_ui->m_lblDescription->setText(image.description());
  
  m_image.load(image.picturePath());
  m_imageSmall = m_image.scaledToWidth(m_picWidthUnclicked);
  scaleImage(m_picWidthUnclicked, m_picHeightUnclicked, false);
  
  if (m_image.width() > m_image.height()) 
    m_rotationNeeded = true;
  else 
    m_rotationNeeded = false;
}

void PicCollectionItem::imageClicked()
{
	if (m_angle == m_startAngle) {
		if (m_ui->m_lblImageNumber->isVisible()) {	
			m_ui->m_container->setStyleSheet("background-color:black;");
			m_ui->m_lblImageNumber->setVisible(false);
			m_ui->m_line->setVisible(false);
			m_ui->m_lblImageTitle->setVisible(false);
			//m_ui->m_lblDummy->setVisible(false);
			m_ui->m_lblDescription->setVisible(false);
			m_ui->m_btnNext->setVisible(false);
			m_ui->m_btnPrevious->setVisible(false);
			m_ui->m_btnGallery->setVisible(false);
			std::cout<<"Inside if\n";
			//scaleImage(m_picWidthUnclicked, m_picHeightUnclicked, false);
			if (m_rotationNeeded) {
				//m_startAngle = 0;
				rotateImage(90);
				
				//scaleImage(m_picWidthUnclicked, m_picHeightUnclicked, false);
			}
			scaleAfterRotation();
			//scaleImage(m_picWidthClicked, m_picHeightClicked);
		}
		else {
			scaleImage(m_picWidthUnclicked, m_picHeightUnclicked);
			rotateAfterScale();
		}
	}
}

void PicCollectionItem::rotateImage(int angle) {
  //QTransform transform;
  //m_image = m_image.transformed(transform.rotate(angle));
  //m_ui->m_lblPicture->setPixmap(m_image));
  m_angle = angle;
  m_startAngle = 0;
  rotateAnim();
}

void PicCollectionItem::rotateAnim() {
	static int test = 0;
  test+=1;
  
  if (m_startAngle != m_angle) {// && test<20) {
		QTransform transform;
		m_startAngle = m_angle>m_startAngle ? m_startAngle+1:m_startAngle-1;
		//m_image = m_image.transformed(transform.rotate(m_startAngle));
		//m_imageSmall = m_imageSmall.transformed(transform.rotate(m_angle));
		//scaleImage(m_picWidthUnclicked, m_picHeightUnclicked, false);
		m_ui->m_lblPicture->setPixmap(m_imageSmall.transformed(transform.rotate(m_startAngle)));
		/*std::cout<<"Inside rotateAnim, ";
		std::cout<<test;
		std::cout<<", ";
		std::cout<<m_startAngle;
		std::cout<<":";
		std::cout<<m_angle;
		std::cout<<"\n";*/
		QTimer::singleShot(1, this, SLOT(rotateAnim()));
	}
	else {
		QTransform transform;
		m_image = m_image.transformed(transform.rotate(m_startAngle));
		m_imageSmall = m_imageSmall.transformed(transform);
	}
}

void PicCollectionItem::scaleAfterRotation() {
	if (m_startAngle == m_angle) {
		scaleImage(m_picWidthClicked, m_picHeightClicked);
		m_rotation = true;
	}
	else
		QTimer::singleShot(50, this, SLOT(scaleAfterRotation()));
}

void PicCollectionItem::rotateAfterScale() {
	if ((m_picWidth == m_scaleWidth ||	m_picHeight == m_scaleHeight) && m_rotation) {
		if (m_rotationNeeded)
			rotateImage(-90);
		showItemsAfterRotate();
		m_rotation = false;
	}
	else if (m_rotation)
		QTimer::singleShot(50, this, SLOT(rotateAfterScale()));
}

void PicCollectionItem::showItemsAfterRotate() {
	if (m_startAngle == m_angle) {
		m_ui->m_container->setStyleSheet("");
		m_ui->m_lblImageNumber->setVisible(true);
		m_ui->m_line->setVisible(true);
		m_ui->m_lblImageTitle->setVisible(true);
		//m_ui->m_lblDummy->setVisible(true);
		m_ui->m_lblDescription->setVisible(true);
		m_ui->m_btnNext->setVisible(true);
		m_ui->m_btnPrevious->setVisible(true);
		m_ui->m_btnGallery->setVisible(true);
	}
	else
		QTimer::singleShot(50, this, SLOT(showItemsAfterRotate()));
}
	
/*  if (m_picWidth == m_scaleWidth ||	m_picHeight == m_scaleHeight) {
		std::cout<<"Inside showItems\n";
		if (m_rotationNeeded) {// && m_rotation) {
			std::cout<<"Inside if of showItems\n";
			//m_rotation = false;
			rotateImage(0);
			//scaleImage(m_picWidthUnclicked, m_picHeightUnclicked, false);
			if (m_startAngle == m_angle) {
				std::cout<<"Inside if if of showItems\n";
				m_ui->m_container->setStyleSheet("");
				m_ui->m_lblImageNumber->setVisible(true);
				m_ui->m_line->setVisible(true);
				m_ui->m_lblImageTitle->setVisible(true);
				//m_ui->m_lblDummy->setVisible(true);
				m_ui->m_lblDescription->setVisible(true);
				m_ui->m_btnNext->setVisible(true);
				m_ui->m_btnPrevious->setVisible(true);
				m_ui->m_btnGallery->setVisible(true);
			}
			else
				QTimer::singleShot(50, this, SLOT(showItems()));
		}
		else if (!m_rotationNeeded) {
			m_ui->m_container->setStyleSheet("");
			m_ui->m_lblImageNumber->setVisible(true);
			m_ui->m_line->setVisible(true);
			m_ui->m_lblImageTitle->setVisible(true);
			//m_ui->m_lblDummy->setVisible(true);
			m_ui->m_lblDescription->setVisible(true);
			m_ui->m_btnNext->setVisible(true);
			m_ui->m_btnPrevious->setVisible(true);
			m_ui->m_btnGallery->setVisible(true);
		}
	}
	else
		QTimer::singleShot(50, this, SLOT(showItems()));
}*/

void PicCollectionItem::scaleImage(int scaleWidth, int scaleHeight, bool anim) {
	
	if (anim) {
		m_scaleWidth = scaleWidth;
		m_scaleHeight = scaleHeight;
		m_picWidth = m_ui->m_lblPicture->pixmap()->width();
		m_picHeight = m_ui->m_lblPicture->pixmap()->height();
		scaleAnim();
	}
  else {
		if ((float)(m_image.width())/m_image.height() > (float)(scaleWidth)/scaleHeight)
			m_ui->m_lblPicture->setPixmap(m_image.scaledToWidth(scaleWidth));
		else
			m_ui->m_lblPicture->setPixmap(m_image.scaledToHeight(scaleHeight));
	}
}

void PicCollectionItem::scaleAnim() {
	static int test = 0;
  test+=1;
  
  if (m_picWidth != m_scaleWidth && 	m_picHeight != m_scaleHeight) {
		if ((float)(m_image.width())/m_image.height() > (float)(m_scaleWidth)/m_scaleHeight) {
			m_picWidth = m_picWidth<m_scaleWidth ? m_picWidth+1:m_picWidth-1;
			m_ui->m_lblPicture->setPixmap(m_image.scaledToWidth(m_picWidth));
		}
		else {
			m_picHeight = m_picHeight<m_scaleHeight ? m_picHeight+1:m_picHeight-1;
			m_ui->m_lblPicture->setPixmap(m_image.scaledToHeight(m_picHeight));
		}
		QTimer::singleShot(m_rotationNeeded ? 1:2, this, SLOT(scaleAnim()));
		/*std::cout<<"Inside ScaleAnim, ";
		std::cout<<test;
		std::cout<<", ";
		std::cout<<m_picWidth;
		std::cout<<":";
		std::cout<<m_scaleWidth;
		std::cout<<", ";
		std::cout<<m_picHeight;
		std::cout<<":";
		std::cout<<m_scaleHeight;
		std::cout<<"\n";*/
	}
  //std::cout<<"ScaleAnim done\n";
}
