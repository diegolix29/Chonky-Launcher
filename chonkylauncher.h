#ifndef CHONKYLUNCHER_H
#define CHONKYLUNCHER_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QCheckBox>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>
#include <QtGui/QPainter>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QStandardPaths>
#include <QtCore/QTemporaryDir>

class ChonkyLauncher : public QMainWindow
{
	Q_OBJECT

public:
	ChonkyLauncher(QWidget* parent = nullptr);
	~ChonkyLauncher();

private slots:
	void selectChonkyExecutable();
	void selectGamesFolder();
	void scanGamesFolder();
	void launchSelectedGame();
	void stopGame();
	void onGameItemDoubleClicked(QListWidgetItem* item);
	void updateScanButtonState();
	void onGameProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void onIconSizeChanged(int size);
	void checkForUpdates();
	void onAutoUpdateToggled(bool enabled);
	void onReleasesFetched();
	void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void onDownloadFinished();

private:
	void setupUI();
	void loadSettings();
	void saveSettings();
	bool hasGameFiles(const QDir& directory);
	void scanDirectory(const QDir& directory, const QString& basePath);
	void launchGame(const QString& gamePath);
	QIcon getGameIcon(const QString& gamePath);
	void downloadAndInstallUpdate(const QString& downloadUrl);
	void extractAndInstall(const QString& zipPath);
	bool isNewerVersion(const QString& latestVersion, const QString& currentVersion);
	bool isNewerRelease(const QJsonObject& latestRelease, const QString& currentVersion, const QString& lastInstalledReleaseId);

	QWidget* m_centralWidget;
	QVBoxLayout* m_mainLayout;

	QHBoxLayout* m_chonkyPathLayout;
	QLabel* m_chonkyPathLabel;
	QLineEdit* m_chonkyPathEdit;
	QPushButton* m_chonkyBrowseButton;

	QHBoxLayout* m_gamesPathLayout;
	QLabel* m_gamesPathLabel;
	QLineEdit* m_gamesPathEdit;
	QPushButton* m_gamesBrowseButton;
	QPushButton* m_scanButton;

	QHBoxLayout* m_gameControlsLayout;
	QLabel* m_gamesListLabel;
	QListWidget* m_gamesList;
	QPushButton* m_playButton;
	QPushButton* m_stopButton;
	
	QHBoxLayout* m_iconSizeLayout;
	QLabel* m_iconSizeLabel;
	QSlider* m_iconSizeSlider;
	QLabel* m_iconSizeValue;
	
	QHBoxLayout* m_updateLayout;
	QCheckBox* m_autoUpdateCheckBox;
	QCheckBox* m_autoInstallCheckBox;
	QPushButton* m_checkUpdateButton;
	
	QNetworkAccessManager* m_networkManager;
	QNetworkReply* m_currentReply;
	QString m_currentVersion;
	QString m_latestVersion;
	QString m_downloadUrl;
	QString m_lastInstalledReleaseId;
	QTemporaryDir* m_tempDir;
	QFile m_downloadFile;

	QProgressBar* m_progressBar;
	QLabel* m_statusLabel;

	QString m_chonkyExecutablePath;
	QString m_gamesFolderPath;
	QSettings* m_settings;
	QProcess* m_gameProcess;
};

#endif
