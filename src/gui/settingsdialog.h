#pragma once

#include <QDialog>
#include <QSettings>
#include <QLineEdit>      // ← AGGIUNGI
#include <QPushButton>    // ← AGGIUNGI
#include <QSpinBox>       // ← AGGIUNGI
#include <QComboBox>      // ← AGGIUNGI
#include <QCheckBox>      // ← AGGIUNGI

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    int getCompressionLevel() const;
    int getCodecPreference() const;
    bool isAutoCodecEnabled() const;
    QString getExtractPath() const;

private slots:
    void saveSettings();
    void loadSettings();
    void onBrowseClicked();

private:
    void setupUI();

    QSpinBox* m_compressionLevel;
    QComboBox* m_codecPreference;
    QCheckBox* m_autoCodec;
    QLineEdit* m_extractPath;
    QPushButton* m_browseButton;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;

    QSettings m_settings;
};
