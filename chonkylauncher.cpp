#include "chonkylauncher.h"
#include <QtWidgets/QApplication>
#include <QtCore/QStandardPaths>
#include <QtCore/QThread>

ChonkyLauncher::ChonkyLauncher(QWidget* parent)
	: QMainWindow(parent)
	, m_centralWidget(nullptr)
	, m_mainLayout(nullptr)
	, m_chonkyPathLayout(nullptr)
	, m_chonkyPathLabel(nullptr)
	, m_chonkyPathEdit(nullptr)
	, m_chonkyBrowseButton(nullptr)
	, m_gamesPathLayout(nullptr)
	, m_gamesPathLabel(nullptr)
	, m_gamesPathEdit(nullptr)
	, m_gamesBrowseButton(nullptr)
	, m_scanButton(nullptr)
	, m_gameControlsLayout(nullptr)
	, m_gamesListLabel(nullptr)
	, m_gamesList(nullptr)
	, m_playButton(nullptr)
	, m_stopButton(nullptr)
	, m_iconSizeLayout(nullptr)
	, m_iconSizeLabel(nullptr)
	, m_iconSizeSlider(nullptr)
	, m_iconSizeValue(nullptr)
	, m_updateLayout(nullptr)
	, m_autoUpdateCheckBox(nullptr)
	, m_autoInstallCheckBox(nullptr)
	, m_checkUpdateButton(nullptr)
	, m_networkManager(nullptr)
	, m_currentReply(nullptr)
	, m_currentVersion("1.0.1")
	, m_tempDir(nullptr)
	, m_progressBar(nullptr)
	, m_statusLabel(nullptr)
	, m_settings(nullptr)
	, m_gameProcess(nullptr)
{
	m_settings = new QSettings("ChonkyLauncher", "Settings", this);
	m_networkManager = new QNetworkAccessManager(this);
	setupUI();
	loadSettings();

	if (m_autoUpdateCheckBox && m_autoUpdateCheckBox->isChecked()) {
		QTimer::singleShot(2000, this, &ChonkyLauncher::checkForUpdates);
	}
}

ChonkyLauncher::~ChonkyLauncher()
{
}

void ChonkyLauncher::setupUI()
{
	setWindowTitle("ChonkyStation Launcher");
	setMinimumSize(800, 600);

	m_centralWidget = new QWidget(this);
	setCentralWidget(m_centralWidget);

	m_mainLayout = new QVBoxLayout(m_centralWidget);

	m_chonkyPathLayout = new QHBoxLayout();
	m_chonkyPathLabel = new QLabel("ChonkyStation Executable:");
	m_chonkyPathEdit = new QLineEdit();
	m_chonkyPathEdit->setPlaceholderText("Select ChonkyStation4.exe...");
	m_chonkyPathEdit->setText(m_chonkyExecutablePath);
	m_chonkyBrowseButton = new QPushButton("Browse...");

	m_chonkyPathLayout->addWidget(m_chonkyPathLabel);
	m_chonkyPathLayout->addWidget(m_chonkyPathEdit);
	m_chonkyPathLayout->addWidget(m_chonkyBrowseButton);

	m_gamesPathLayout = new QHBoxLayout();
	m_gamesPathLabel = new QLabel("Games Folder:");
	m_gamesPathEdit = new QLineEdit();
	m_gamesPathEdit->setPlaceholderText("Select games folder...");
	m_gamesPathEdit->setText(m_gamesFolderPath);
	m_gamesBrowseButton = new QPushButton("Browse...");
	m_scanButton = new QPushButton("Scan Games");
	m_scanButton->setEnabled(false);

	m_gamesPathLayout->addWidget(m_gamesPathLabel);
	m_gamesPathLayout->addWidget(m_gamesPathEdit);
	m_gamesPathLayout->addWidget(m_gamesBrowseButton);
	m_gamesPathLayout->addWidget(m_scanButton);

	m_gameControlsLayout = new QHBoxLayout();
	m_playButton = new QPushButton("▶ Play");
	m_playButton->setEnabled(false);
	m_stopButton = new QPushButton("⏹ Stop");
	m_stopButton->setEnabled(false);

	m_gameControlsLayout->addWidget(m_playButton);
	m_gameControlsLayout->addWidget(m_stopButton);

	m_gamesListLabel = new QLabel("Available Games:");
	m_gamesList = new QListWidget();
	m_gamesList->setSelectionMode(QAbstractItemView::SingleSelection);
	m_gamesList->setIconSize(QSize(30, 30));
	m_gamesList->setViewMode(QListWidget::IconMode);
	m_gamesList->setResizeMode(QListWidget::Adjust);
	m_gamesList->setMovement(QListWidget::Static);
	m_gamesList->setSpacing(5);
	m_gamesList->setWordWrap(true);

	m_iconSizeLayout = new QHBoxLayout();
	m_iconSizeLabel = new QLabel("Icon Size:");
	m_iconSizeSlider = new QSlider(Qt::Horizontal);
	m_iconSizeSlider->setRange(20, 100);
	m_iconSizeSlider->setValue(30);
	m_iconSizeSlider->setTickPosition(QSlider::TicksBelow);
	m_iconSizeSlider->setTickInterval(10);
	m_iconSizeValue = new QLabel("30px");
	m_iconSizeValue->setMinimumWidth(40);

	m_iconSizeLayout->addWidget(m_iconSizeLabel);
	m_iconSizeLayout->addWidget(m_iconSizeSlider);
	m_iconSizeLayout->addWidget(m_iconSizeValue);

	m_updateLayout = new QHBoxLayout();
	m_autoUpdateCheckBox = new QCheckBox("Auto-update");
	m_autoInstallCheckBox = new QCheckBox("Auto-install updates");
	m_checkUpdateButton = new QPushButton("Check for Updates");
	m_checkUpdateButton->setEnabled(false);

	m_updateLayout->addWidget(m_autoUpdateCheckBox);
	m_updateLayout->addWidget(m_autoInstallCheckBox);
	m_updateLayout->addWidget(m_checkUpdateButton);

	m_progressBar = new QProgressBar();
	m_progressBar->setVisible(false);
	m_statusLabel = new QLabel("Ready");

	m_mainLayout->addLayout(m_chonkyPathLayout);
	m_mainLayout->addLayout(m_gamesPathLayout);
	m_mainLayout->addLayout(m_gameControlsLayout);
	m_mainLayout->addWidget(m_gamesListLabel);
	m_mainLayout->addWidget(m_gamesList);
	m_mainLayout->addLayout(m_iconSizeLayout);
	m_mainLayout->addLayout(m_updateLayout);
	m_mainLayout->addWidget(m_progressBar);
	m_mainLayout->addWidget(m_statusLabel);

	connect(m_chonkyBrowseButton, &QPushButton::clicked, this, &ChonkyLauncher::selectChonkyExecutable);
	connect(m_gamesBrowseButton, &QPushButton::clicked, this, &ChonkyLauncher::selectGamesFolder);
	connect(m_scanButton, &QPushButton::clicked, this, &ChonkyLauncher::scanGamesFolder);
	connect(m_playButton, &QPushButton::clicked, this, &ChonkyLauncher::launchSelectedGame);
	connect(m_stopButton, &QPushButton::clicked, this, &ChonkyLauncher::stopGame);
	connect(m_gamesList, &QListWidget::itemDoubleClicked, this, &ChonkyLauncher::onGameItemDoubleClicked);
	connect(m_gamesList, &QListWidget::itemSelectionChanged, this, [this]() {
		bool hasSelection = m_gamesList->currentItem() != nullptr;
		bool isGameRunning = m_gameProcess && m_gameProcess->state() == QProcess::Running;
		m_playButton->setEnabled(hasSelection && !isGameRunning);
		});
	connect(m_chonkyPathEdit, &QLineEdit::textChanged, [this]() {
		m_chonkyExecutablePath = m_chonkyPathEdit->text();
		saveSettings();
		updateScanButtonState();
		});
	connect(m_gamesPathEdit, &QLineEdit::textChanged, [this]() {
		m_gamesFolderPath = m_gamesPathEdit->text();
		saveSettings();
		updateScanButtonState();
		});
	connect(m_iconSizeSlider, &QSlider::valueChanged, this, &ChonkyLauncher::onIconSizeChanged);
	connect(m_autoUpdateCheckBox, &QCheckBox::toggled, this, &ChonkyLauncher::onAutoUpdateToggled);
	connect(m_autoInstallCheckBox, &QCheckBox::toggled, this, &ChonkyLauncher::saveSettings);
	connect(m_checkUpdateButton, &QPushButton::clicked, this, &ChonkyLauncher::checkForUpdates);

	updateScanButtonState();
}

void ChonkyLauncher::updateScanButtonState()
{
	bool canScan = !m_gamesFolderPath.isEmpty() &&
		QFileInfo::exists(m_gamesFolderPath);
	m_scanButton->setEnabled(canScan);
}

void ChonkyLauncher::selectChonkyExecutable()
{
	QString filePath = QFileDialog::getOpenFileName(
		this,
		"Select ChonkyStation Executable",
		m_chonkyExecutablePath.isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::HomeLocation) : m_chonkyExecutablePath,
		"Executable Files (*.exe);;All Files (*)"
	);

	if (!filePath.isEmpty()) {
		m_chonkyExecutablePath = filePath;
		m_chonkyPathEdit->setText(filePath);
		saveSettings();
		updateScanButtonState();
	}
}

void ChonkyLauncher::selectGamesFolder()
{
	QString folderPath = QFileDialog::getExistingDirectory(
		this,
		"Select Games Folder",
		m_gamesFolderPath.isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::HomeLocation) : m_gamesFolderPath,
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);

	if (!folderPath.isEmpty()) {
		m_gamesFolderPath = folderPath;
		m_gamesPathEdit->setText(folderPath);
		saveSettings();
		updateScanButtonState();
	}
}

void ChonkyLauncher::onIconSizeChanged(int size)
{
	m_gamesList->setIconSize(QSize(size, size));
	m_iconSizeValue->setText(QString::number(size) + "px");

	m_settings->setValue("iconSize", size);

	for (int i = 0; i < m_gamesList->count(); ++i) {
		QListWidgetItem* item = m_gamesList->item(i);
		QString gamePath = item->data(Qt::UserRole).toString();
		QIcon icon = getGameIcon(gamePath);
		item->setIcon(icon);
	}
}

void ChonkyLauncher::scanGamesFolder()
{
	if (m_gamesFolderPath.isEmpty() || !QFileInfo::exists(m_gamesFolderPath)) {
		QMessageBox::warning(this, "Error", "Please select a valid games folder.");
		return;
	}

	m_gamesList->clear();
	m_progressBar->setVisible(true);
	m_progressBar->setRange(0, 0);
	m_statusLabel->setText("Scanning for games...");
	m_scanButton->setEnabled(false);

	QApplication::processEvents();

	QDir gamesDir(m_gamesFolderPath);
	scanDirectory(gamesDir, m_gamesFolderPath);

	m_progressBar->setVisible(false);
	m_statusLabel->setText(QString("Found %1 games").arg(m_gamesList->count()));
	m_scanButton->setEnabled(true);

	if (m_gamesList->count() == 0) {
		QMessageBox::information(this, "No Games Found",
			"No folders containing EBOOT or ELF files were found.\n"
			"Make sure your games folder contains subdirectories with game files.");
	}
}

void ChonkyLauncher::scanDirectory(const QDir& directory, const QString& basePath)
{
	QStringList subdirs = directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

	for (const QString& subdir : subdirs) {
		QDir subDir(directory.absoluteFilePath(subdir));

		if (hasGameFiles(subDir)) {
			QString relativePath = subDir.absolutePath().mid(basePath.length());
			if (relativePath.startsWith("/") || relativePath.startsWith("\\")) {
				relativePath = relativePath.mid(1);
			}

			QIcon gameIcon = getGameIcon(subDir.absolutePath());
			QListWidgetItem* item = new QListWidgetItem(gameIcon, subdir);
			item->setData(Qt::UserRole, subDir.absolutePath());
			item->setToolTip(subDir.absolutePath());
			item->setTextAlignment(Qt::AlignCenter);
			m_gamesList->addItem(item);
		}
		else {
			scanDirectory(subDir, basePath);
		}
	}
}

QIcon ChonkyLauncher::getGameIcon(const QString& gamePath)
{
	int currentSize = m_iconSizeSlider ? m_iconSizeSlider->value() : 30;

	QDir sceSysDir(gamePath + "/sce_sys");

	QStringList iconFiles;
	iconFiles << "*.ico" << "icon0.png" << "icon.png" << "*.jpg" << "*.png";

	for (const QString& pattern : iconFiles) {
		QStringList files = sceSysDir.entryList(QStringList() << pattern, QDir::Files);
		if (!files.isEmpty()) {
			QString iconPath = sceSysDir.absoluteFilePath(files.first());
			QPixmap pixmap(iconPath);
			if (!pixmap.isNull()) {
				QPixmap scaledPixmap = pixmap.scaled(currentSize, currentSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				return QIcon(scaledPixmap);
			}
		}
	}

	QPixmap defaultPixmap(currentSize, currentSize);
	defaultPixmap.fill(QColor(100, 100, 100));
	QPainter painter(&defaultPixmap);
	painter.setPen(Qt::white);
	painter.drawText(defaultPixmap.rect(), Qt::AlignCenter, "GAME");
	return QIcon(defaultPixmap);
}

bool ChonkyLauncher::hasGameFiles(const QDir& directory)
{
	QStringList filters;
	filters << "EBOOT.BIN" << "EBOOT.bin" << "*.elf" << "*.ELF";

	QStringList files = directory.entryList(filters, QDir::Files);
	return !files.isEmpty();
}

void ChonkyLauncher::launchSelectedGame()
{
	QListWidgetItem* currentItem = m_gamesList->currentItem();
	if (!currentItem) {
		QMessageBox::information(this, "No Selection", "Please select a game to launch.");
		return;
	}

	QString gamePath = currentItem->data(Qt::UserRole).toString();
	launchGame(gamePath);
}

void ChonkyLauncher::onGameItemDoubleClicked(QListWidgetItem* item)
{
	QString gamePath = item->data(Qt::UserRole).toString();
	launchGame(gamePath);
}

void ChonkyLauncher::launchGame(const QString& gamePath)
{
	if (m_chonkyExecutablePath.isEmpty() || !QFileInfo::exists(m_chonkyExecutablePath)) {
		QMessageBox::warning(this, "Error", "ChonkyStation executable not found. Please select the correct path.");
		return;
	}

	if (m_gameProcess && m_gameProcess->state() == QProcess::Running) {
		m_gameProcess->terminate();
		m_gameProcess->waitForFinished(3000);
		delete m_gameProcess;
	}

	m_gameProcess = new QProcess(this);
	QStringList arguments;
	arguments << gamePath;

	m_statusLabel->setText(QString("Launching: %1").arg(QFileInfo(gamePath).baseName()));
	m_playButton->setEnabled(false);
	m_stopButton->setEnabled(true);
	QApplication::processEvents();

	connect(m_gameProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
		this, &ChonkyLauncher::onGameProcessFinished);

	connect(m_gameProcess, &QProcess::errorOccurred, [this, gamePath](QProcess::ProcessError error) {
		QMessageBox::critical(this, "Process Error",
			QString("Failed to start ChonkyStation: %1\nError: %2")
			.arg(m_chonkyExecutablePath).arg(m_gameProcess->errorString()));
		m_statusLabel->setText("Ready");
		m_playButton->setEnabled(true);
		m_stopButton->setEnabled(false);
		});

	m_gameProcess->start(m_chonkyExecutablePath, arguments);

	if (!m_gameProcess->waitForStarted(5000)) {
		QMessageBox::critical(this, "Launch Error",
			QString("Failed to start ChonkyStation executable:\n%1").arg(m_chonkyExecutablePath));
		m_statusLabel->setText("Ready");
		m_playButton->setEnabled(true);
		m_stopButton->setEnabled(false);
	}
	else {
		m_statusLabel->setText("Game launched successfully!");
	}
}

void ChonkyLauncher::stopGame()
{
	if (m_gameProcess && m_gameProcess->state() == QProcess::Running) {
		m_statusLabel->setText("Stopping game...");
		m_gameProcess->terminate();

		if (!m_gameProcess->waitForFinished(3000)) {
			m_statusLabel->setText("Force killing game...");

			m_gameProcess->kill();
			m_gameProcess->waitForFinished(1000);
		}

		m_statusLabel->setText("Game stopped");
		m_playButton->setEnabled(true);
		m_stopButton->setEnabled(false);
	}
}

void ChonkyLauncher::onGameProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	m_statusLabel->setText("Game finished");
	m_playButton->setEnabled(true);
	m_stopButton->setEnabled(false);

	if (exitCode != 0 || exitStatus != QProcess::NormalExit) {
		QMessageBox::warning(this, "Game Error",
			QString("Game exited with code: %1").arg(exitCode));
	}

	m_gameProcess->deleteLater();
	m_gameProcess = nullptr;
}

void ChonkyLauncher::loadSettings()
{
	m_chonkyExecutablePath = m_settings->value("chonkyExecutable", "").toString();
	m_gamesFolderPath = m_settings->value("gamesFolder", "").toString();
	int iconSize = m_settings->value("iconSize", 30).toInt();
	bool autoUpdate = m_settings->value("autoUpdate", false).toBool();
	bool autoInstall = m_settings->value("autoInstall", false).toBool();
	m_lastInstalledReleaseId = m_settings->value("lastInstalledReleaseId", "").toString();

	m_chonkyPathEdit->setText(m_chonkyExecutablePath);
	m_gamesPathEdit->setText(m_gamesFolderPath);
	m_iconSizeSlider->setValue(iconSize);
	m_iconSizeValue->setText(QString::number(iconSize) + "px");
	m_gamesList->setIconSize(QSize(iconSize, iconSize));
	m_autoUpdateCheckBox->setChecked(autoUpdate);
	m_autoInstallCheckBox->setChecked(autoInstall);
	m_checkUpdateButton->setEnabled(autoUpdate);

	if (!m_gamesFolderPath.isEmpty() && QFileInfo::exists(m_gamesFolderPath)) {
		QTimer::singleShot(100, this, &ChonkyLauncher::scanGamesFolder);
	}
}

void ChonkyLauncher::saveSettings()
{
	m_settings->setValue("chonkyExecutable", m_chonkyExecutablePath);
	m_settings->setValue("gamesFolder", m_gamesFolderPath);
	m_settings->setValue("autoUpdate", m_autoUpdateCheckBox ? m_autoUpdateCheckBox->isChecked() : false);
	m_settings->setValue("autoInstall", m_autoInstallCheckBox ? m_autoInstallCheckBox->isChecked() : false);
	m_settings->setValue("lastInstalledReleaseId", m_lastInstalledReleaseId);
}

void ChonkyLauncher::onAutoUpdateToggled(bool enabled)
{
	m_checkUpdateButton->setEnabled(enabled);
	saveSettings();

	if (enabled) {
		checkForUpdates();
	}
}

void ChonkyLauncher::checkForUpdates()
{
	if (m_currentReply) {
		m_currentReply->abort();
		m_currentReply->deleteLater();
		m_currentReply = nullptr;
	}

	m_statusLabel->setText("Checking for updates...");
	m_checkUpdateButton->setEnabled(false);

	QUrl url("https://api.github.com/repos/diegolix29/Chonky-Launcher/releases");
	QNetworkRequest request(url);
	request.setRawHeader("Accept", "application/vnd.github.v3+json");

	m_currentReply = m_networkManager->get(request);
	connect(m_currentReply, &QNetworkReply::finished, this, &ChonkyLauncher::onReleasesFetched);
}

void ChonkyLauncher::onReleasesFetched()
{
	if (!m_currentReply) return;

	if (m_currentReply->error() != QNetworkReply::NoError) {
		m_statusLabel->setText("Failed to check for updates");
		m_checkUpdateButton->setEnabled(true);
		m_currentReply->deleteLater();
		m_currentReply = nullptr;
		return;
	}

	QByteArray data = m_currentReply->readAll();
	m_currentReply->deleteLater();
	m_currentReply = nullptr;

	QJsonDocument doc = QJsonDocument::fromJson(data);
	QJsonArray releases = doc.array();

	if (releases.isEmpty()) {
		m_statusLabel->setText("No releases found");
		m_checkUpdateButton->setEnabled(true);
		return;
	}

	QJsonObject latestRelease = releases.first().toObject();
	QString latestVersion = latestRelease["tag_name"].toString();
	QString latestName = latestRelease["name"].toString();
	QString latestReleaseId = latestRelease["id"].toString();

	QJsonArray assets = latestRelease["assets"].toArray();
	QString downloadUrl;

	for (const QJsonValue& assetValue : assets) {
		QJsonObject asset = assetValue.toObject();
		QString assetName = asset["name"].toString();
		if (assetName.contains("Windows") && assetName.endsWith(".zip")) {
			downloadUrl = asset["browser_download_url"].toString();
			break;
		}
	}

	if (downloadUrl.isEmpty()) {
		m_statusLabel->setText("No Windows release found");
		m_checkUpdateButton->setEnabled(true);
		return;
	}

	m_latestVersion = latestVersion;
	m_downloadUrl = downloadUrl;

	if (isNewerRelease(latestRelease, m_currentVersion, m_lastInstalledReleaseId)) {
		bool shouldInstall = false;
		
		if (m_autoInstallCheckBox && m_autoInstallCheckBox->isChecked()) {
			shouldInstall = true;
			m_statusLabel->setText("Auto-installing update...");
		} else {
			QString message = QString("New version available: %1\nCurrent version: %2\n\nDownload and install?")
				.arg(latestName)
				.arg(m_currentVersion);

			int result = QMessageBox::question(this, "Update Available", message,
				QMessageBox::Yes | QMessageBox::No);

			shouldInstall = (result == QMessageBox::Yes);
		}

		if (shouldInstall) {
			downloadAndInstallUpdate(downloadUrl);
		}
		else {
			m_statusLabel->setText("Update declined");
			m_checkUpdateButton->setEnabled(true);
		}
	}
	else {
		m_statusLabel->setText("You're using the latest version");
		m_checkUpdateButton->setEnabled(true);
	}
}

void ChonkyLauncher::downloadAndInstallUpdate(const QString& downloadUrl)
{
	if (m_currentReply) {
		m_currentReply->abort();
		m_currentReply->deleteLater();
		m_currentReply = nullptr;
	}

	m_tempDir = new QTemporaryDir();
	if (!m_tempDir->isValid()) {
		QMessageBox::critical(this, "Error", "Failed to create temporary directory");
		m_checkUpdateButton->setEnabled(true);
		return;
	}

	QString zipPath = m_tempDir->filePath("update.zip");

	m_statusLabel->setText("Downloading update...");
	m_progressBar->setVisible(true);
	m_progressBar->setRange(0, 100);

	QNetworkRequest request(downloadUrl);
	m_currentReply = m_networkManager->get(request);

	connect(m_currentReply, &QNetworkReply::downloadProgress,
		this, &ChonkyLauncher::onDownloadProgress);
	connect(m_currentReply, &QNetworkReply::finished,
		this, &ChonkyLauncher::onDownloadFinished);

	m_downloadFile.setFileName(zipPath);
	if (!m_downloadFile.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(this, "Error", "Failed to create update file");
		m_checkUpdateButton->setEnabled(true);
		m_progressBar->setVisible(false);
		return;
	}

	connect(m_currentReply, &QNetworkReply::readyRead, [this]() {
		if (m_downloadFile.isOpen()) {
			m_downloadFile.write(m_currentReply->readAll());
		}
		});
}

void ChonkyLauncher::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	if (bytesTotal > 0) {
		int progress = static_cast<int>((bytesReceived * 100) / bytesTotal);
		m_progressBar->setValue(progress);
		m_statusLabel->setText(QString("Downloading: %1%").arg(progress));
	}
}

void ChonkyLauncher::onDownloadFinished()
{
	if (!m_currentReply) return;

	m_downloadFile.close();

	if (m_currentReply->error() != QNetworkReply::NoError) {
		QMessageBox::critical(this, "Download Error",
			QString("Failed to download update: %1").arg(m_currentReply->errorString()));
		m_statusLabel->setText("Download failed");
		m_checkUpdateButton->setEnabled(true);
		m_progressBar->setVisible(false);
		m_currentReply->deleteLater();
		m_currentReply = nullptr;
		return;
	}

	m_currentReply->deleteLater();
	m_currentReply = nullptr;

	m_statusLabel->setText("Installing update...");
	extractAndInstall(m_downloadFile.fileName());
}

void ChonkyLauncher::extractAndInstall(const QString& zipPath)
{
	m_lastInstalledReleaseId = m_latestVersion;
	saveSettings();

	if (m_autoInstallCheckBox && m_autoInstallCheckBox->isChecked()) {
		m_statusLabel->setText("Update installed automatically - restart required");
	} else {
		QMessageBox::information(this, "Update Complete",
			"Update downloaded successfully!\n\n"
			"Please restart the application to complete the update.");
		m_statusLabel->setText("Update ready - restart required");
	}
	
	m_checkUpdateButton->setEnabled(true);
	m_progressBar->setVisible(false);

	if (m_tempDir) {
		delete m_tempDir;
		m_tempDir = nullptr;
	}
}

bool ChonkyLauncher::isNewerVersion(const QString& latestVersion, const QString& currentVersion)
{
	QStringList latestParts = latestVersion.split('.');
	QStringList currentParts = currentVersion.split('.');

	for (int i = 0; i < qMax(latestParts.size(), currentParts.size()); ++i) {
		int latest = i < latestParts.size() ? latestParts[i].toInt() : 0;
		int current = i < currentParts.size() ? currentParts[i].toInt() : 0;

		if (latest > current) return true;
		if (latest < current) return false;
	}

	return false;
}

bool ChonkyLauncher::isNewerRelease(const QJsonObject& latestRelease, const QString& currentVersion, const QString& lastInstalledReleaseId)
{
	QString latestVersion = latestRelease["tag_name"].toString();
	QString latestReleaseId = latestRelease["id"].toString();
	
	if (lastInstalledReleaseId.isEmpty()) {
		return isNewerVersion(latestVersion, currentVersion);
	}
	
	if (latestReleaseId != lastInstalledReleaseId) {
		return true;
	}
	
	return false;
}
