/*
  Copyright (c) 2015, BuildmLearn Contributors listed at http://buildmlearn.org/people/
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

#include "templates/piccollection/piccollectioneditor.h"

#include "templates/piccollection/piccollectionimage.h"
#include "templates/piccollection/piccollectioncore.h"
#include "templates/piccollection/piccollectionentrypoint.h"
#include "miscellaneous/iconfactory.h"
#include "miscellaneous/iofactory.h"
#include "core/templatefactory.h"

#include <QTimer>
#include <QFileDialog>

PicCollectionEditor::PicCollectionEditor(TemplateCore *core, QWidget *parent)
  : TemplateEditor(core, parent), m_ui(new Ui::PicCollectionEditor) {
  m_ui->setupUi(this);

  // Set validators.
  QRegExpValidator *author_validator = new QRegExpValidator(this);
  QRegExpValidator *title_validator = new QRegExpValidator(this);

  author_validator->setRegExp(QRegExp(".{,50}"));
  title_validator->setRegExp(QRegExp(".{,100}"));

  m_ui->m_txtAuthor->lineEdit()->setValidator(author_validator);
  m_ui->m_txtName->lineEdit()->setValidator(title_validator);

  // Set validators.
  QRegExpValidator *picTitle_validator = new QRegExpValidator(this);
  
  picTitle_validator->setRegExp(QRegExp(".{,100}"));
  
  m_ui->m_txtTitle->lineEdit()->setValidator(picTitle_validator);
  
  // Set tab order.
  QList<QWidget*> tab_order_widgets;
  tab_order_widgets << m_ui->m_txtTitle->lineEdit() << m_ui->m_btnPictureSelect  <<
                       m_ui->m_txtDescription << m_ui->m_txtAuthor->lineEdit() << 
                       m_ui->m_txtName->lineEdit() << m_ui->m_listImages << 
                       m_ui->m_btnImageAdd << m_ui->m_btnImageRemove <<
                       m_ui->m_btnImageUp << m_ui->m_btnImageDown;

  for (int i = 1; i < tab_order_widgets.size(); i++) {
    setTabOrder(tab_order_widgets.at(i - 1), tab_order_widgets.at(i));
  }

  m_ui->m_txtNumberOfImages->lineEdit()->setEnabled(false);

  m_ui->m_lblPictureFile->label()->setWordWrap(true);
  m_ui->m_txtTitle->lineEdit()->setPlaceholderText(tr("Title of the image"));
  m_ui->m_lblPictureFile->setStatus(WidgetWithStatus::Error, QString(), tr("No picture selected"));
  m_ui->m_txtAuthor->lineEdit()->setPlaceholderText(tr("Author of this Collection"));
  m_ui->m_txtName->lineEdit()->setPlaceholderText(tr("Name of this Collection"));

  IconFactory *factory = IconFactory::instance();

  m_ui->m_btnImageAdd->setIcon(factory->fromTheme("item-add"));
  m_ui->m_btnImageRemove->setIcon(factory->fromTheme("item-remove"));
  m_ui->m_btnImageUp->setIcon(factory->fromTheme("move-up"));
  m_ui->m_btnImageDown->setIcon(factory->fromTheme("move-down"));

  connect(m_ui->m_btnPictureSelect, SIGNAL(clicked()), this, SLOT(selectPicture()));
  connect(m_ui->m_txtAuthor->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(onAuthorChanged(QString)));
  connect(m_ui->m_txtTitle->lineEdit(), SIGNAL(textEdited(QString)), this, SLOT(onTitleChanged(QString)));
  connect(m_ui->m_txtName->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(onNameChanged(QString)));
  connect(m_ui->m_txtDescription, SIGNAL(textChanged()), this, SLOT(saveImage()));
  connect(m_ui->m_btnImageAdd, SIGNAL(clicked()), this, SLOT(addImage()));
  connect(m_ui->m_btnImageRemove, SIGNAL(clicked()), this, SLOT(removeImage()));
  connect(m_ui->m_listImages, SIGNAL(currentRowChanged(int)), this, SLOT(loadImage(int)));
  connect(m_ui->m_btnImageUp, SIGNAL(clicked()), this, SLOT(moveImageUp()));
  connect(m_ui->m_btnImageDown, SIGNAL(clicked()), this, SLOT(moveImageDown()));

  setEditorsEnabled(false);
  updateImageCount();

  qRegisterMetaType<PicCollectionImage>("PicCollectionImage");

  checkAuthor();
  checkName();
  loadImage(-1);
}

PicCollectionEditor::~PicCollectionEditor() {
  qDebug("Destroying PicCollectionEditor instance.");

  delete m_ui;
}

void PicCollectionEditor::updateImageCount() {
  m_ui->m_txtNumberOfImages->lineEdit()->setText(QString::number(m_ui->m_listImages->count()));

  if (m_ui->m_listImages->count() > 0) {
    m_ui->m_txtNumberOfImages->setStatus(WidgetWithStatus::Ok, tr("Collection contains at least one image."));
  }
  else {
    m_ui->m_txtNumberOfImages->setStatus(WidgetWithStatus::Error, tr("Collection does not contain any images."));
  }
}

bool PicCollectionEditor::canGenerateApplications() {
  return
      !m_ui->m_txtName->lineEdit()->text().simplified().isEmpty() &&
      !m_ui->m_txtAuthor->lineEdit()->text().simplified().isEmpty() &&
      !activeImages().isEmpty();
}

QString PicCollectionEditor::generateBundleData() {
  /*if (!canGenerateApplications()) {
    return QString();
  }*/

  QDomDocument source_document = qApp->templateManager()->generateBundleHeader(core()->entryPoint()->typeIndentifier(),
                                                                               m_ui->m_txtAuthor->lineEdit()->text(),
                                                                               QString(),
                                                                               m_ui->m_txtName->lineEdit()->text(),
                                                                               QString(),
                                                                               "1");
  FIND_DATA_ELEMENT(data_element, source_document);

  foreach (const PicCollectionImage &image, activeImages()) {
    QDomElement item_element = source_document.createElement("item");

    // Fill in details about image.
    QDomElement title_element = source_document.createElement("title");
    QDomElement description_element = source_document.createElement("description");
    QDomElement image_element = source_document.createElement("image");

    title_element.appendChild(source_document.createTextNode(image.title()));
    description_element.appendChild(source_document.createTextNode(image.description()));

    // Read file with image, convert it to base64 and insert into XML bundle.
    QByteArray picture_encoded = IOFactory::fileToBase64(image.picturePath());

    if (picture_encoded.isEmpty() || picture_encoded.isNull()) {
      return QString();
    }

    image_element.appendChild(source_document.createTextNode(QString::fromUtf8(picture_encoded)));
    item_element.appendChild(title_element);
    item_element.appendChild(description_element);
    item_element.appendChild(image_element);

    data_element.appendChild(item_element);
  }

  return source_document.toString(XML_BUNDLE_INDENTATION);
}

bool PicCollectionEditor::loadBundleData(const QString &bundle_data) {
  QDomDocument bundle_document;
  bundle_document.setContent(bundle_data);

  QDomNodeList items = bundle_document.documentElement().elementsByTagName("item");

  for (int i = 0; i < items.size(); i++) {
    QDomNode item = items.at(i);

    if (item.isElement()) {
      QString title = item.namedItem("title").toElement().text();
      QString description = item.namedItem("description").toElement().text();
      QString image_data = item.namedItem("image").toElement().text();

      if (title.isEmpty() || image_data.isEmpty()) {
        // TODO: error
        continue;
      }
      else {
        // TODO: POKRACOVAT TADY, prevadeni z base64 do souboru blbne.
        // https://www.google.cz/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#q=qt%20base64%20to%20file
        QString output_directory = qApp->templateManager()->tempDirectory();
        QString target_image_file = output_directory +
                                    QString("/image_%1.png").arg(i);

        if (IOFactory::base64ToFile(image_data, target_image_file)) {
          // Picture from the item was saved to disk.
          addImage(title, description, target_image_file);
        }
        else {
          // TODO: errro
        }
      }
    }
    else {
      continue;
    }
  }

  // Load author & name.
  m_ui->m_txtAuthor->lineEdit()->setText(bundle_document.documentElement().namedItem("author").namedItem("name").toElement().text());
  m_ui->m_txtName->lineEdit()->setText(bundle_document.documentElement().namedItem("title").toElement().text());

  return true;
}

QList<PicCollectionImage> PicCollectionEditor::activeImages() const {
  QList<PicCollectionImage> images;

  for (int i = 0; i < m_ui->m_listImages->count(); i++) {
    images.append(m_ui->m_listImages->item(i)->data(Qt::UserRole).value<PicCollectionImage>());
  }

  return images;
}

QString PicCollectionEditor::projectName() {
  return m_ui->m_txtName->lineEdit()->text();
}

QString PicCollectionEditor::authorName() {
  return m_ui->m_txtAuthor->lineEdit()->text();
}

void PicCollectionEditor::checkAuthor() {
  if (m_ui->m_txtAuthor->lineEdit()->text().isEmpty()) {
    m_ui->m_txtAuthor->setStatus(WidgetWithStatus::Error,
                                 tr("No author is specified."));
  }
  else {
    m_ui->m_txtAuthor->setStatus(WidgetWithStatus::Ok,
                                 tr("Author is specified."));
  }
}

void PicCollectionEditor::checkImage() {
  if (m_ui->m_txtTitle->lineEdit()->text().isEmpty()) {
    m_ui->m_txtTitle->setStatus(WidgetWithStatus::Warning,
                               tr("Image is not specified."));
  }
  else {
    m_ui->m_txtTitle->setStatus(WidgetWithStatus::Ok,
                               tr("Image is specified."));
  }
}

void PicCollectionEditor::checkTitle() {
  if (m_ui->m_txtTitle->lineEdit()->text().isEmpty()) {
    m_ui->m_txtTitle->setStatus(WidgetWithStatus::Error,
                                 tr("Title is not specified."));
  }
  else {
    m_ui->m_txtTitle->setStatus(WidgetWithStatus::Ok,
                                 tr("Title is specified."));
  }
}

void PicCollectionEditor::checkName() {
  if (m_ui->m_txtName->lineEdit()->text().isEmpty()) {
    m_ui->m_txtName->setStatus(WidgetWithStatus::Error,
                               tr("No collection name is specified."));
  }
  else {
    m_ui->m_txtName->setStatus(WidgetWithStatus::Ok,
                               tr("Collection name is specified."));
  }
}

void PicCollectionEditor::configureUpDown() {
  if (m_ui->m_listImages->count() > 1) {
    int index = m_ui->m_listImages->currentRow();

    if (index == 0) {
      m_ui->m_btnImageUp->setEnabled(false);
      m_ui->m_btnImageDown->setEnabled(true);
    }
    else if (index == m_ui->m_listImages->count() - 1) {
      m_ui->m_btnImageUp->setEnabled(true);
      m_ui->m_btnImageDown->setEnabled(false);
    }
    else {
      m_ui->m_btnImageUp->setEnabled(true);
      m_ui->m_btnImageDown->setEnabled(true);
    }
  }
  else {
    m_ui->m_btnImageUp->setEnabled(false);
    m_ui->m_btnImageDown->setEnabled(false);
  }
}

void PicCollectionEditor::moveImageUp() {
  int index = m_ui->m_listImages->currentRow();

  m_ui->m_listImages->insertItem(index - 1, m_ui->m_listImages->takeItem(index));
  m_ui->m_listImages->setCurrentRow(index - 1);

  emit changed();
}

void PicCollectionEditor::moveImageDown() {
  int index = m_ui->m_listImages->currentRow();

  m_ui->m_listImages->insertItem(index + 1, m_ui->m_listImages->takeItem(index));
  m_ui->m_listImages->setCurrentRow(index + 1);

  emit changed();
}

void PicCollectionEditor::loadPicture(const QString& picture_path) {
  if (!picture_path.isEmpty()) {
    m_ui->m_lblPictureView->setPixmap(QPixmap(picture_path).scaled(m_ui->m_lblPictureView->size(),
                                                                   Qt::KeepAspectRatio));
    m_ui->m_lblPictureFile->setStatus(WidgetWithStatus::Ok,
                                      tr("Picture is selected."),
                                      tr("Picture is selected."));
  }
  else {
    m_ui->m_lblPictureView->setPixmap(QPixmap());
    m_ui->m_lblPictureFile->setStatus(WidgetWithStatus::Error,
                                      tr("Picture is not selected."),
                                      tr("No picture is selected."));
  }

  m_ui->m_lblPictureFile->label()->setToolTip(QDir::toNativeSeparators(picture_path));
}

void PicCollectionEditor::addImage(const QString &title,
                                  const QString &description,
                                  const QString &picture_path) {
  int marked_image = m_ui->m_listImages->currentRow();
  PicCollectionImage new_image;
  QListWidgetItem *new_item = new QListWidgetItem();

  new_image.setTitle(title);
  new_image.setDescription(description);
  new_image.setPicturePath(picture_path);

  new_item->setText(new_image.title());
  new_item->setData(Qt::UserRole, QVariant::fromValue(new_image));

  if (m_ui->m_listImages->count() == 0) {
    // We are adding first image.
    setEditorsEnabled(true);

    m_ui->m_btnImageRemove->setEnabled(true);

    m_ui->m_listImages->insertItem(0, new_item);
    m_ui->m_listImages->setCurrentRow(0);
  }
  else {
    m_ui->m_listImages->insertItem(marked_image + 1, new_item);
    m_ui->m_listImages->setCurrentRow(marked_image + 1);
  }

  updateImageCount();
}

void PicCollectionEditor::addImage() {
  addImage(tr("White Roses"),
              tr("This is a white rose."),
              APP_TEMPLATES_PATH + QDir::separator() +
              core()->entryPoint()->baseFolder() + QDir::separator() +
              "whiteRoses.png");
  launch();
  emit changed();
}

void PicCollectionEditor::setEditorsEnabled(bool enabled) {
  m_ui->m_groupImageEditor->setEnabled(enabled);
}

void PicCollectionEditor::loadImage(int index) {
  m_ui->m_txtTitle->blockSignals(true);
  m_ui->m_lblPictureFile->label()->blockSignals(true);

  if (index >= 0) {
    PicCollectionImage image = m_ui->m_listImages->item(index)->data(Qt::UserRole).value<PicCollectionImage>();

    m_ui->m_txtTitle->lineEdit()->setText(image.title());
    loadPicture(image.picturePath());
	m_ui->m_txtDescription->setText(image.description());
    
    m_activeImage = image;
  }
  else {
    m_ui->m_txtTitle->lineEdit()->setText(QString());
    loadPicture(QString());
  }

  m_ui->m_txtTitle->blockSignals(false);
  m_ui->m_lblPictureFile->label()->blockSignals(false);

  checkTitle();
  
  QTimer::singleShot(0, this, SLOT(configureUpDown()));
}

void PicCollectionEditor::saveImage() {
  m_activeImage.setTitle(m_ui->m_txtTitle->lineEdit()->text());
  m_activeImage.setDescription(m_ui->m_txtDescription->toPlainText());
  m_activeImage.setPicturePath(m_ui->m_lblPictureFile->label()->toolTip());

  m_ui->m_listImages->currentItem()->setData(Qt::UserRole, QVariant::fromValue(m_activeImage));
  m_ui->m_listImages->currentItem()->setText(m_activeImage.title());

  emit changed();
}

void PicCollectionEditor::removeImage() {
  int current_row = m_ui->m_listImages->currentRow();

  if (current_row >= 0) {
    if (m_ui->m_listImages->count() == 1) {
      // We are removing last visible image.
      setEditorsEnabled(false);
	  m_ui->m_txtDescription->clear();
      m_ui->m_btnImageRemove->setEnabled(false);
    }

    delete m_ui->m_listImages->takeItem(current_row);
  }

  updateImageCount();
  launch();
  emit changed();
}

void PicCollectionEditor::onTitleChanged(const QString& new_title) {
  Q_UNUSED(new_title)
  
  checkTitle();
  saveImage();
}

void PicCollectionEditor::onAuthorChanged(const QString& new_author) {
  Q_UNUSED(new_author)

  checkAuthor();

  launch();
  emit changed();
}

void PicCollectionEditor::onNameChanged(const QString& new_name) {
  Q_UNUSED(new_name)

  checkName();

  launch();
  emit changed();
}

void PicCollectionEditor::selectPicture() {
  QString selected_picture = QFileDialog::getOpenFileName(this, tr("Select picture for image"),
                                                          m_ui->m_lblPictureFile->label()->toolTip(),
                                                          tr("Images (*.gif *.jpg *.png)"),
                                                          0);

  if (!selected_picture.isEmpty()) {
    loadPicture(selected_picture);
    saveImage();
  }
}
