#include "chonkylauncher.h"
#include <QtWidgets/QApplication>
#include <QtCore/QStandardPaths>
#include <QtCore/QThread>
#include <QtCore/QDir>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtWidgets/QComboBox>

ChonkyLauncher::ChonkyLauncher(QWidget* parent)
	: QMainWindow(parent)
	, m_centralWidget(nullptr)
	, m_mainLayout(nullptr)
	, m_chonkyPathLayout(nullptr)
	, m_chonkyPathLabel(nullptr)
	, m_chonkyPathEdit(nullptr)
	, m_chonkyBrowseButton(nullptr)
	, m_versionLabel(nullptr)
	, m_gamesPathLayout(nullptr)
	, m_gamesPathLabel(nullptr)
	, m_pathsContainer(nullptr)
	, m_pathsContainerLayout(nullptr)
	, m_pathSelectionComboBox(nullptr)
	, m_addPathButton(nullptr)
	, m_removePathButton(nullptr)
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
	, m_themeComboBox(nullptr)
	, m_themeLayout(nullptr)
	, m_themeLabel(nullptr)
	, m_networkManager(nullptr)
	, m_currentReply(nullptr)
	, m_currentVersion(QApplication::applicationVersion())
	, m_latestVersion("")
	, m_lastInstalledReleaseId("")
	, m_tempDir(nullptr)
	, m_progressBar(nullptr)
	, m_statusLabel(nullptr)
	, m_gameProcess(nullptr)
{
	QString appDir = QCoreApplication::applicationDirPath();
	m_configFilePath = appDir + "/config.json";
	
	QFile configFile(m_configFilePath);
	if (!configFile.exists()) {
		QSettings oldSettings("ChonkyLauncher", "Settings");
		oldSettings.clear();
		oldSettings.sync();
	}
	
	m_networkManager = new QNetworkAccessManager(this);

	setupUI();

	loadSettings();

	if (!m_lastInstalledReleaseId.isEmpty()) {
		QString installedVersion = m_lastInstalledReleaseId;
		if (installedVersion.startsWith("v")) {
			installedVersion = installedVersion.mid(1);
		}
		m_currentVersion = installedVersion;
	}
	else {
		m_currentVersion = QApplication::applicationVersion();
	}

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

	m_versionLabel = new QLabel("Version: " + m_currentVersion);
	m_versionLabel->setAlignment(Qt::AlignCenter);
	m_versionLabel->setStyleSheet("QLabel { font-weight: bold; color: #666; padding: 5px; }");

	m_chonkyPathLayout = new QHBoxLayout();
	m_chonkyPathLabel = new QLabel("ChonkyStation Executable:");
	m_chonkyPathEdit = new QLineEdit();
	m_chonkyPathEdit->setPlaceholderText("Select ChonkyStation4.exe...");
	m_chonkyPathEdit->setText(m_chonkyExecutablePath);
	m_chonkyBrowseButton = new QPushButton("Browse...");

	m_chonkyPathLayout->addWidget(m_chonkyPathLabel);
	m_chonkyPathLayout->addWidget(m_chonkyPathEdit);
	m_chonkyPathLayout->addWidget(m_chonkyBrowseButton);

	m_gamesPathLabel = new QLabel("Games Folders:");

	m_pathsContainer = new QWidget();
	m_pathsContainerLayout = new QVBoxLayout(m_pathsContainer);
	m_pathsContainerLayout->setContentsMargins(0, 0, 0, 0);

	m_pathSelectionComboBox = new QComboBox();
	m_pathSelectionComboBox->setMinimumWidth(200);

	m_addPathButton = new QPushButton("Add Path");
	m_removePathButton = new QPushButton("Remove Selected");
	m_scanButton = new QPushButton("Scan All Paths");
	m_scanButton->setEnabled(false);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(new QLabel("Select Path:"));
	buttonLayout->addWidget(m_pathSelectionComboBox);
	buttonLayout->addWidget(m_addPathButton);
	buttonLayout->addWidget(m_removePathButton);
	buttonLayout->addWidget(m_scanButton);

	m_pathsContainerLayout->addLayout(buttonLayout);

	QHBoxLayout* gamesPathMainLayout = new QHBoxLayout();
	gamesPathMainLayout->addWidget(m_gamesPathLabel);
	gamesPathMainLayout->addWidget(m_pathsContainer, 1);

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

	m_themeLayout = new QHBoxLayout();
	m_themeLabel = new QLabel("Theme:");
	m_themeComboBox = new QComboBox();
	m_themeComboBox->setMinimumWidth(150);

	m_themeLayout->addWidget(m_themeLabel);
	m_themeLayout->addWidget(m_themeComboBox);
	m_themeLayout->addStretch();

	m_progressBar = new QProgressBar();
	m_progressBar->setVisible(false);
	m_statusLabel = new QLabel("Ready");

	m_mainLayout->addWidget(m_versionLabel);
	m_mainLayout->addLayout(m_chonkyPathLayout);
	m_mainLayout->addLayout(gamesPathMainLayout);
	m_mainLayout->addLayout(m_gameControlsLayout);
	m_mainLayout->addWidget(m_gamesListLabel);
	m_mainLayout->addWidget(m_gamesList);
	m_mainLayout->addLayout(m_iconSizeLayout);
	m_mainLayout->addLayout(m_updateLayout);
	m_mainLayout->addLayout(m_themeLayout);
	m_mainLayout->addWidget(m_progressBar);
	m_mainLayout->addWidget(m_statusLabel);

	connect(m_chonkyBrowseButton, &QPushButton::clicked, this, &ChonkyLauncher::selectChonkyExecutable);
	connect(m_addPathButton, &QPushButton::clicked, this, &ChonkyLauncher::addPath);
	connect(m_removePathButton, &QPushButton::clicked, [this]() {
		int currentIndex = m_pathSelectionComboBox->currentIndex();
		if (currentIndex >= 0) {
			removePath(currentIndex);
		}
		});
	connect(m_scanButton, &QPushButton::clicked, this, &ChonkyLauncher::scanAllPaths);
	connect(m_playButton, &QPushButton::clicked, this, &ChonkyLauncher::launchSelectedGame);
	connect(m_stopButton, &QPushButton::clicked, this, &ChonkyLauncher::stopGame);
	connect(m_gamesList, &QListWidget::itemDoubleClicked, this, &ChonkyLauncher::onGameItemDoubleClicked);
	connect(m_gamesList, &QListWidget::itemSelectionChanged, this, [this]() {
		bool hasSelection = m_gamesList->currentItem() != nullptr;
		bool isGameRunning = m_gameProcess && m_gameProcess->state() == QProcess::Running;
		m_playButton->setEnabled(hasSelection && !isGameRunning);
		});
	connect(m_chonkyPathEdit, &QLineEdit::textChanged, [this]() {
		static bool isInitialized = false;
		if (!isInitialized) {
			isInitialized = true;
			return;
		}
		
		m_chonkyExecutablePath = m_chonkyPathEdit->text();
		saveSettings();
		updateScanButtonState();
		});
	connect(m_iconSizeSlider, &QSlider::valueChanged, this, &ChonkyLauncher::onIconSizeChanged);
	connect(m_autoUpdateCheckBox, &QCheckBox::toggled, this, &ChonkyLauncher::onAutoUpdateToggled);
	connect(m_autoInstallCheckBox, &QCheckBox::toggled, this, &ChonkyLauncher::saveSettings);
	connect(m_checkUpdateButton, &QPushButton::clicked, this, &ChonkyLauncher::checkForUpdates);
	connect(m_themeComboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged), this, &ChonkyLauncher::onThemeChanged);

	loadThemes();

	updateScanButtonState();
}

void ChonkyLauncher::updateScanButtonState()
{
	bool canScan = !m_gamesFolderPaths.isEmpty() && !m_chonkyExecutablePath.isEmpty();
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

void ChonkyLauncher::addPath()
{
	QString folderPath = QFileDialog::getExistingDirectory(
		this,
		"Select Games Folder",
		QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);

	if (!folderPath.isEmpty() && !m_gamesFolderPaths.contains(folderPath)) {
		m_gamesFolderPaths.append(folderPath);

		QString displayName = QString("Path %1: %2").arg(m_pathSelectionComboBox->count() + 1).arg(QDir(folderPath).dirName());
		m_pathSelectionComboBox->addItem(displayName);

		QHBoxLayout* pathRow = new QHBoxLayout();
		QLineEdit* pathEdit = new QLineEdit(folderPath);
		pathEdit->setReadOnly(true);
		QPushButton* browseButton = new QPushButton("Browse...");

		pathRow->addWidget(pathEdit, 1);
		pathRow->addWidget(browseButton);

		m_pathRows.append(pathRow);
		m_pathEdits.append(pathEdit);
		m_pathBrowseButtons.append(browseButton);

		m_pathsContainerLayout->insertLayout(m_pathRows.size(), pathRow);

		connect(browseButton, &QPushButton::clicked, [this, pathEdit]() {
			QString newPath = QFileDialog::getExistingDirectory(
				this,
				"Select Games Folder",
				pathEdit->text(),
				QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
			);
			if (!newPath.isEmpty() && !m_gamesFolderPaths.contains(newPath)) {
				int index = m_pathEdits.indexOf(pathEdit);
				if (index >= 0) {
					m_gamesFolderPaths[index] = newPath;
					pathEdit->setText(newPath);

					// Update combo box display
					QString newDisplayName = QString("Path %1: %2").arg(index + 1).arg(QDir(newPath).dirName());
					m_pathSelectionComboBox->setItemText(index, newDisplayName);

					// Save settings
					saveSettings();
				}
			}
			});

		updatePathButtons();
		saveSettings();
		updateScanButtonState();
	}
}

void ChonkyLauncher::removePath(int index)
{
	if (index < 0 || index >= m_pathRows.size()) return;

	QHBoxLayout* row = m_pathRows.takeAt(index);
	QLineEdit* edit = m_pathEdits.takeAt(index);
	QPushButton* button = m_pathBrowseButtons.takeAt(index);
	QString path = m_gamesFolderPaths.takeAt(index);

	m_pathsContainerLayout->removeItem(row);
	m_pathSelectionComboBox->removeItem(index);

	delete edit;
	delete button;
	delete row;

	m_pathSelectionComboBox->clear();
	for (int i = 0; i < m_gamesFolderPaths.size(); ++i) {
		QString displayName = QString("Path %1: %2").arg(i + 1).arg(QDir(m_gamesFolderPaths[i]).dirName());
		m_pathSelectionComboBox->addItem(displayName);
	}

	updatePathButtons();
	saveSettings();
	updateScanButtonState();
}

void ChonkyLauncher::updatePathButtons()
{
	m_removePathButton->setEnabled(!m_pathRows.isEmpty());
}

void ChonkyLauncher::scanAllPaths()
{
	m_gamesList->clear();

	if (m_chonkyExecutablePath.isEmpty()) {
		QMessageBox::warning(this, "Warning", "Please select ChonkyStation executable first.");
		return;
	}

	m_statusLabel->setText("Scanning paths...");

	for (const QString& path : m_gamesFolderPaths) {
		QDir dir(path);
		if (dir.exists()) {
			scanDirectory(dir, path);
		}
	}

	m_statusLabel->setText(QString("Found %1 games").arg(m_gamesList->count()));
}

void ChonkyLauncher::onIconSizeChanged(int size)
{
	m_gamesList->setIconSize(QSize(size, size));
	m_iconSizeValue->setText(QString::number(size) + "px");

	QFile configFile(m_configFilePath);
	QJsonObject config;

	if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QByteArray data = configFile.readAll();
		configFile.close();

		QJsonDocument doc = QJsonDocument::fromJson(data);
		if (doc.isObject()) {
			config = doc.object();
		}
	}

	config["iconSize"] = size;

	if (configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QJsonDocument doc(config);
		configFile.write(doc.toJson());
		configFile.close();
	}

	for (int i = 0; i < m_gamesList->count(); ++i) {
		QListWidgetItem* item = m_gamesList->item(i);
		QString gamePath = item->data(Qt::UserRole).toString();
		QIcon icon = getGameIcon(gamePath);
		item->setIcon(icon);
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
	static bool isLoading = false;
	if (isLoading) return;
	isLoading = true;
	
	QFile configFile(m_configFilePath);
	
	if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QByteArray data = configFile.readAll();
		configFile.close();
		
		QJsonDocument doc = QJsonDocument::fromJson(data);
		if (doc.isObject()) {
			QJsonObject config = doc.object();
			
			m_chonkyExecutablePath = config["chonkyExecutable"].toString();
			m_chonkyPathEdit->setText(m_chonkyExecutablePath);
			
			QJsonArray pathsArray = config["gamesFolderPaths"].toArray();
			m_gamesFolderPaths.clear();
			for (const QJsonValue& value : pathsArray) {
				m_gamesFolderPaths.append(value.toString());
			}
			
			qDebug() << "Loaded" << m_gamesFolderPaths.size() << "paths:" << m_gamesFolderPaths;
			
			m_autoUpdateCheckBox->setChecked(config["autoUpdate"].toBool(false));
			m_autoInstallCheckBox->setChecked(config["autoInstall"].toBool(false));
			m_lastInstalledReleaseId = config["lastInstalledReleaseId"].toString();
			
			int iconSize = config["iconSize"].toInt(30);
			m_iconSizeSlider->setValue(iconSize);
			m_iconSizeValue->setText(QString::number(iconSize) + "px");
			m_gamesList->setIconSize(QSize(iconSize, iconSize));
			
			m_checkUpdateButton->setEnabled(m_autoUpdateCheckBox->isChecked());
		}
	}
	else {
		qDebug() << "Config file not found, using defaults";
		m_chonkyExecutablePath = "";
		m_gamesFolderPaths.clear();
		m_lastInstalledReleaseId = "";
		
		m_chonkyPathEdit->setText("");
		m_autoUpdateCheckBox->setChecked(false);
		m_autoInstallCheckBox->setChecked(false);
		m_iconSizeSlider->setValue(30);
		m_iconSizeValue->setText("30px");
		m_gamesList->setIconSize(QSize(30, 30));
		m_checkUpdateButton->setEnabled(false);
		
		updatePathButtons();
		updateScanButtonState();
	}
	
	if (!m_pathRows.isEmpty()) {
		qDebug() << "Clearing" << m_pathRows.size() << "existing UI elements";
		m_pathSelectionComboBox->clear();
		for (int i = 0; i < m_pathRows.size(); ++i) {
			QHBoxLayout* row = m_pathRows[i];
			QLineEdit* edit = m_pathEdits[i];
			QPushButton* button = m_pathBrowseButtons[i];
			
			m_pathsContainerLayout->removeItem(row);
			delete edit;
			delete button;
			delete row;
		}
		m_pathRows.clear();
		m_pathEdits.clear();
		m_pathBrowseButtons.clear();
	}
	
	qDebug() << "Creating UI for" << m_gamesFolderPaths.size() << "paths";
	for (int i = 0; i < m_gamesFolderPaths.size(); ++i) {
		QString displayName = QString("Path %1: %2").arg(i + 1).arg(QDir(m_gamesFolderPaths[i]).dirName());
		m_pathSelectionComboBox->addItem(displayName);

		QHBoxLayout* pathRow = new QHBoxLayout();
		QLineEdit* pathEdit = new QLineEdit(m_gamesFolderPaths[i]);
		pathEdit->setReadOnly(true);
		QPushButton* browseButton = new QPushButton("Browse...");

		pathRow->addWidget(pathEdit, 1);
		pathRow->addWidget(browseButton);

		m_pathRows.append(pathRow);
		m_pathEdits.append(pathEdit);
		m_pathBrowseButtons.append(browseButton);

		m_pathsContainerLayout->insertLayout(m_pathRows.size(), pathRow);

		connect(browseButton, &QPushButton::clicked, [this, pathEdit, i]() {
			QString newPath = QFileDialog::getExistingDirectory(
				this,
				"Select Games Folder",
				pathEdit->text(),
				QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
			);
			if (!newPath.isEmpty() && !m_gamesFolderPaths.contains(newPath)) {
				m_gamesFolderPaths[i] = newPath;
				pathEdit->setText(newPath);

				QString newDisplayName = QString("Path %1: %2").arg(i + 1).arg(QDir(newPath).dirName());
				m_pathSelectionComboBox->setItemText(i, newDisplayName);
				
				saveSettings();
			}
			});
	}

	updatePathButtons();
	updateScanButtonState();
	
	if (!m_gamesFolderPaths.isEmpty() && !m_chonkyExecutablePath.isEmpty()) {
		scanAllPaths();
	}
	
	qDebug() << "loadSettings() finished with" << m_gamesFolderPaths.size() << "paths";
	isLoading = false;
}

void ChonkyLauncher::saveSettings()
{
	qDebug() << "saveSettings() called with" << m_gamesFolderPaths.size() << "paths:" << m_gamesFolderPaths;
	
	QJsonObject config;
	config["chonkyExecutable"] = m_chonkyExecutablePath;

	QJsonArray pathsArray;
	for (const QString& path : m_gamesFolderPaths) {
		pathsArray.append(path);
	}
	config["gamesFolderPaths"] = pathsArray;

	config["autoUpdate"] = m_autoUpdateCheckBox ? m_autoUpdateCheckBox->isChecked() : false;
	config["autoInstall"] = m_autoInstallCheckBox ? m_autoInstallCheckBox->isChecked() : false;
	config["lastInstalledReleaseId"] = m_lastInstalledReleaseId;
	config["iconSize"] = m_iconSizeSlider ? m_iconSizeSlider->value() : 30;
	config["selectedTheme"] = m_themeComboBox->currentData().toString();

	QJsonDocument doc(config);

	QFile configFile(m_configFilePath);
	if (configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		configFile.write(doc.toJson());
		configFile.close();
		qDebug() << "Settings saved successfully to" << m_configFilePath;
	} else {
		qDebug() << "Failed to save settings to" << m_configFilePath;
	}
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
		}
		else {
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

#ifdef Q_OS_WIN
	QString appPath = QCoreApplication::applicationDirPath();
	QString tempDirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Temp";
	QDir().mkpath(tempDirPath);

	if (createUpdateScript(zipPath, appPath)) {
		QString scriptPath = tempDirPath + "/update.ps1";

		if (m_autoInstallCheckBox && m_autoInstallCheckBox->isChecked()) {
			m_statusLabel->setText("Update downloaded - restarting for installation...");

			QProcess::startDetached("powershell.exe", QStringList()
				<< "-ExecutionPolicy" << "Bypass"
				<< "-File" << scriptPath);

			QTimer::singleShot(1000, this, &QApplication::quit);
		}
		else {
			m_statusLabel->setText("Update ready - restart to install");

			int result = QMessageBox::information(this, "Update Ready",
				"Update downloaded successfully!\n\n"
				"The application will restart to complete the update.",
				QMessageBox::Ok | QMessageBox::Cancel);

			if (result == QMessageBox::Ok) {
				QProcess::startDetached("powershell.exe", QStringList()
					<< "-ExecutionPolicy" << "Bypass"
					<< "-File" << scriptPath);

				QTimer::singleShot(1000, this, &QApplication::quit);
			}
		}
	}
	else {
		m_statusLabel->setText("Failed to create update script");
	}
#else
	m_statusLabel->setText("Update ready - manual restart required");
	QMessageBox::information(this, "Update Complete",
		"Update downloaded successfully!\n\n"
		"Please restart the application to complete the update.");
#endif

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

bool ChonkyLauncher::createUpdateScript(const QString& zipPath, const QString& appPath)
{
	QString tempDirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Temp";
	QString scriptPath = tempDirPath + "/update.ps1";
	QString extractPath = tempDirPath + "/extract";

	QDir().mkpath(tempDirPath);
	QDir().mkpath(extractPath);

	QString tempZipPath = extractPath + "/temp_download_update.zip";
	if (!QFile::copy(zipPath, tempZipPath)) {
		qDebug() << "Failed to copy zip file to" << tempZipPath;
		return false;
	}

	QString scriptContent = QStringLiteral(
		"Set-ExecutionPolicy Bypass -Scope Process -Force\n"
		"Write-Output \"Starting ChonkyLauncher update...\"\n"
		"Write-Output \"Extracting update from: %1\"\n"
		"Write-Output \"Extracting to: %2\"\n"
		"Write-Output \"Installing to: %3\"\n"
		"\n"
		"Expand-Archive -Path '%1' -DestinationPath '%2' -Force\n"
		"if ($?) {\n"
		"    Write-Output \"Extraction successful\"\n"
		"} else {\n"
		"    Write-Output \"Extraction failed\"\n"
		"    exit 1\n"
		"}\n"
		"\n"
		"Start-Sleep -Seconds 2\n"
		"Write-Output \"Copying files to application directory...\"\n"
		"\n"
		"Copy-Item -Recurse -Force '%2\\*' '%3\\'\n"
		"if ($?) {\n"
		"    Write-Output \"File copy successful\"\n"
		"} else {\n"
		"    Write-Output \"File copy failed\"\n"
		"    exit 1\n"
		"}\n"
		"\n"
		"Start-Sleep -Seconds 2\n"
		"Write-Output \"Cleaning up temporary files...\"\n"
		"\n"
		"Remove-Item -Force -LiteralPath '%3\\update.ps1' -ErrorAction SilentlyContinue\n"
		"Remove-Item -Force -LiteralPath '%3\\temp_download_update.zip' -ErrorAction SilentlyContinue\n"
		"Remove-Item -Recurse -Force '%2' -ErrorAction SilentlyContinue\n"
		"\n"
		"Write-Output \"Starting updated application...\"\n"
		"Start-Process -FilePath '%3\\ChonkyLauncher.exe' -WorkingDirectory '%3'\n"
		"Write-Output \"Update completed successfully!\"\n"
	).arg(tempZipPath, extractPath, appPath);

	QFile scriptFile(scriptPath);
	if (!scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "Failed to create script file:" << scriptPath;
		return false;
	}

	QTextStream out(&scriptFile);
	out << scriptContent;
	scriptFile.close();

	qDebug() << "Update script created at:" << scriptPath;
	return true;
}

bool ChonkyLauncher::isNewerRelease(const QJsonObject& latestRelease, const QString& currentVersion, const QString& lastInstalledReleaseId)
{
	QString latestVersion = latestRelease["tag_name"].toString();
	QString latestReleaseId = latestRelease["id"].toString();

	if (latestVersion.startsWith("v")) {
		latestVersion = latestVersion.mid(1);
	}

	if (!lastInstalledReleaseId.isEmpty()) {
		QString lastInstalledVersion = lastInstalledReleaseId;
		if (lastInstalledVersion.startsWith("v")) {
			lastInstalledVersion = lastInstalledVersion.mid(1);
		}
		return isNewerVersion(latestVersion, lastInstalledVersion);
	}

	return isNewerVersion(latestVersion, currentVersion);
}

void ChonkyLauncher::loadThemes()
{
	QString appDir = QCoreApplication::applicationDirPath();
	QString themesDir = appDir + "/themes";

	m_themeComboBox->clear();
	m_themeComboBox->addItem("Default", "");

	if (!QDir(themesDir).exists()) {
		QDir().mkpath(themesDir);
	}

	QDir dir(themesDir);
	QStringList qssFiles = dir.entryList(QStringList() << "*.qss", QDir::Files);

	for (const QString& fileName : qssFiles) {
		QString themeName = fileName.left(fileName.length() - 4);
		m_themeComboBox->addItem(themeName, fileName);
	}

	QFile configFile(m_configFilePath);
	if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QByteArray data = configFile.readAll();
		configFile.close();

		QJsonDocument doc = QJsonDocument::fromJson(data);
		if (doc.isObject()) {
			QJsonObject config = doc.object();
			QString savedTheme = config["selectedTheme"].toString();
			if (!savedTheme.isEmpty()) {
				int index = m_themeComboBox->findData(savedTheme);
				if (index >= 0) {
					m_themeComboBox->setCurrentIndex(index);
					QString themeFile = m_themeComboBox->currentData().toString();
					if (!themeFile.isEmpty()) {
						applyTheme(themeFile.left(themeFile.length() - 4));
					}
				}
			}
		}
	}
}

void ChonkyLauncher::applyTheme(const QString& themeFile)
{
	if (themeFile.isEmpty()) {
		qApp->setStyleSheet("");
		return;
	}

	QString appDir = QCoreApplication::applicationDirPath();
	QString themePath = appDir + "/themes/" + themeFile + ".qss";

	QFile file(themePath);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QString stylesheet = QTextStream(&file).readAll();
		qApp->setStyleSheet(stylesheet);
		file.close();
	}
}

void ChonkyLauncher::onThemeChanged(const QString& themeName)
{
	static bool isLoading = true;

	if (isLoading) {
		isLoading = false;
		return;
	}

	QString themeFile = m_themeComboBox->currentData().toString();

	if (themeFile.isEmpty()) {
		applyTheme("");
	}
	else {
		applyTheme(themeFile.left(themeFile.length() - 4));
	}

	QFile configFile(m_configFilePath);
	QJsonObject config;

	if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QByteArray data = configFile.readAll();
		configFile.close();

		QJsonDocument doc = QJsonDocument::fromJson(data);
		if (doc.isObject()) {
			config = doc.object();
		}
	}

	config["selectedTheme"] = themeFile;

	if (configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QJsonDocument doc(config);
		configFile.write(doc.toJson());
		configFile.close();
	}
}
