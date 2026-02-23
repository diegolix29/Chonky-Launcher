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
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QSettings>

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
	void onGameItemDoubleClicked(QListWidgetItem* item);
	void updateScanButtonState();

private:
	void setupUI();
	void loadSettings();
	void saveSettings();
	bool hasGameFiles(const QDir& directory);
	void scanDirectory(const QDir& directory, const QString& basePath);
	void launchGame(const QString& gamePath);

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

	QLabel* m_gamesListLabel;
	QListWidget* m_gamesList;
	QPushButton* m_launchButton;

	QProgressBar* m_progressBar;
	QLabel* m_statusLabel;

	QString m_chonkyExecutablePath;
	QString m_gamesFolderPath;
	QSettings* m_settings;
};

#endif
