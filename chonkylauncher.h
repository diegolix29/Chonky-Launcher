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
#include <QtCore/QList>
#include <QtCore/QFile>
#include <SDL3/SDL.h>

class ChonkyLauncher : public QMainWindow
{
	Q_OBJECT

public:
	ChonkyLauncher(QWidget* parent = nullptr);
	~ChonkyLauncher();

private slots:
	void selectChonkyExecutable();
	void launchSelectedGame();
	void stopGame();
	void onGameItemDoubleClicked(QListWidgetItem* item);
	void onGameProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void onIconSizeChanged(int size);
	void checkForUpdates();
	void onAutoUpdateToggled(bool enabled);
	void onReleasesFetched();
	void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void onDownloadFinished();
	void handleGamepadEvents();

private:
	void setupUI();
	void loadSettings();
	void saveSettings();
	bool hasGameFiles(const QDir& directory);
	void scanDirectory(const QDir& directory, const QString& basePath);
	void scanAllPaths();
	void addPath();
	void removePath(int index);
	void updatePathButtons();
	void updateScanButtonState();
	void launchGame(const QString& gamePath);
	QIcon getGameIcon(const QString& gamePath);
	void downloadAndInstallUpdate(const QString& downloadUrl);
	void extractAndInstall(const QString& zipPath);
	bool createUpdateScript(const QString& zipPath, const QString& appPath);
	bool isNewerVersion(const QString& latestVersion, const QString& currentVersion);
	bool isNewerRelease(const QJsonObject& latestRelease, const QString& currentVersion, const QString& lastInstalledReleaseId);
	void loadThemes();
	void applyTheme(const QString& themeName);
	void onThemeChanged(const QString& themeName);
	void initializeGamepad();
	void cleanupGamepad();
	void handleJoystickInput();
	void navigateGamesUp();
	void navigateGamesDown();
	void navigateGamesLeft();
	void navigateGamesRight();

	QWidget* m_centralWidget;
	QVBoxLayout* m_mainLayout;

	QHBoxLayout* m_chonkyPathLayout;
	QLabel* m_chonkyPathLabel;
	QLineEdit* m_chonkyPathEdit;
	QPushButton* m_chonkyBrowseButton;

	QLabel* m_versionLabel;

	QHBoxLayout* m_gamesPathLayout;
	QLabel* m_gamesPathLabel;
	QVBoxLayout* m_pathsContainerLayout;
	QWidget* m_pathsContainer;
	QComboBox* m_pathSelectionComboBox;
	QPushButton* m_addPathButton;
	QPushButton* m_removePathButton;
	QPushButton* m_scanButton;
	QList<QHBoxLayout*> m_pathRows;
	QList<QLineEdit*> m_pathEdits;
	QList<QPushButton*> m_pathBrowseButtons;

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

	QComboBox* m_themeComboBox;

	QHBoxLayout* m_themeLayout;
	QLabel* m_themeLabel;

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
	QStringList m_gamesFolderPaths;
	QString m_configFilePath;
	QProcess* m_gameProcess;

	// Gamepad support
	SDL_Gamepad* m_gamepad;
	QTimer* m_gamepadTimer;
	bool m_gamepadInitialized;
	bool m_gamepadFailed; // Track if gamepad mapping failed
	bool m_usingJoystickFallback; // Track if we're using joystick mode
	bool m_gamepadControlGUI; // Track if gamepad should control GUI instead of game
};

#endif
