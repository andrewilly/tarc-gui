#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QGroupBox>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_settings("TARC", "TARC-GUI")
{
    setupUI();
    loadSettings();
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::setupUI()
{
    setWindowTitle("Impostazioni");
    setModal(true);
    resize(450, 300);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Gruppo Compressione
    QGroupBox* compressionGroup = new QGroupBox("Compressione", this);
    QFormLayout* formLayout = new QFormLayout(compressionGroup);

    m_compressionLevel = new QSpinBox(this);
    m_compressionLevel->setRange(1, 22);
    m_compressionLevel->setToolTip("Livello di compressione (1=veloce, 22=massima compressione)");
    formLayout->addRow("Livello compressione:", m_compressionLevel);

    m_autoCodec = new QCheckBox("Codec automatico (consigliato)", this);
    formLayout->addRow("", m_autoCodec);

    m_codecPreference = new QComboBox(this);
    m_codecPreference->addItem("ZSTD (bilanciato)");
    m_codecPreference->addItem("LZ4 (veloce)");
    m_codecPreference->addItem("LZMA (massima compressione)");
    m_codecPreference->setEnabled(false);
    formLayout->addRow("Codec preferito:", m_codecPreference);

    // Collegamento per abilitare/disabilitare codec preference
    connect(m_autoCodec, &QCheckBox::toggled, m_codecPreference, &QComboBox::setDisabled);

    mainLayout->addWidget(compressionGroup);

    // Gruppo Estrazione
    QGroupBox* extractGroup = new QGroupBox("Estrazione", this);
    QHBoxLayout* extractLayout = new QHBoxLayout(extractGroup);

    m_extractPath = new QLineEdit(this);
    m_extractPath->setPlaceholderText("Cartella di destinazione predefinita");

    m_browseButton = new QPushButton("Sfoglia...", this);
    connect(m_browseButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseClicked);

    extractLayout->addWidget(m_extractPath);
    extractLayout->addWidget(m_browseButton);

    mainLayout->addWidget(extractGroup);

    // Pulsanti
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_okButton = new QPushButton("OK", this);
    m_cancelButton = new QPushButton("Annulla", this);

    connect(m_okButton, &QPushButton::clicked, this, &SettingsDialog::saveSettings);
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);

    mainLayout->addLayout(buttonLayout);
}

void SettingsDialog::loadSettings()
{
    m_compressionLevel->setValue(m_settings.value("compression/level", 3).toInt());
    m_autoCodec->setChecked(m_settings.value("compression/auto_codec", true).toBool());
    m_codecPreference->setCurrentIndex(m_settings.value("compression/preferred_codec", 0).toInt());
    m_extractPath->setText(m_settings.value("extraction/default_path", "").toString());
}

void SettingsDialog::saveSettings()
{
    m_settings.setValue("compression/level", m_compressionLevel->value());
    m_settings.setValue("compression/auto_codec", m_autoCodec->isChecked());
    m_settings.setValue("compression/preferred_codec", m_codecPreference->currentIndex());
    m_settings.setValue("extraction/default_path", m_extractPath->text());
    m_settings.sync();
    
    accept();
}

void SettingsDialog::onBrowseClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
        "Seleziona cartella di destinazione",
        m_extractPath->text());

    if (!dir.isEmpty()) {
        m_extractPath->setText(dir);
    }
}

int SettingsDialog::getCompressionLevel() const
{
    return m_compressionLevel->value();
}

int SettingsDialog::getCodecPreference() const
{
    return m_codecPreference->currentIndex();
}

bool SettingsDialog::isAutoCodecEnabled() const
{
    return m_autoCodec->isChecked();
}

QString SettingsDialog::getExtractPath() const
{
    return m_extractPath->text();
}
