#include "chonkylauncher.h"
#include <QtWidgets/QApplication>
#include <QtCore/QStandardPaths>
#include <QtCore/QThread>

ChonkyLauncher::ChonkyLauncher(QWidget *parent)
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
    , m_gamesListLabel(nullptr)
    , m_gamesList(nullptr)
    , m_launchButton(nullptr)
    , m_progressBar(nullptr)
    , m_statusLabel(nullptr)
    , m_settings(nullptr)
{
    m_settings = new QSettings("ChonkyLauncher", "Settings", this);
    loadSettings();
    setupUI();
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
    
    m_gamesListLabel = new QLabel("Available Games:");
    m_gamesList = new QListWidget();
    m_gamesList->setSelectionMode(QAbstractItemView::SingleSelection);
    
    m_launchButton = new QPushButton("Launch Selected Game");
    m_launchButton->setEnabled(false);
    
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    m_statusLabel = new QLabel("Ready");
    
    m_mainLayout->addLayout(m_chonkyPathLayout);
    m_mainLayout->addLayout(m_gamesPathLayout);
    m_mainLayout->addWidget(m_gamesListLabel);
    m_mainLayout->addWidget(m_gamesList);
    m_mainLayout->addWidget(m_launchButton);
    m_mainLayout->addWidget(m_progressBar);
    m_mainLayout->addWidget(m_statusLabel);
    
    connect(m_chonkyBrowseButton, &QPushButton::clicked, this, &ChonkyLauncher::selectChonkyExecutable);
    connect(m_gamesBrowseButton, &QPushButton::clicked, this, &ChonkyLauncher::selectGamesFolder);
    connect(m_scanButton, &QPushButton::clicked, this, &ChonkyLauncher::scanGamesFolder);
    connect(m_launchButton, &QPushButton::clicked, this, &ChonkyLauncher::launchSelectedGame);
    connect(m_gamesList, &QListWidget::itemDoubleClicked, this, &ChonkyLauncher::onGameItemDoubleClicked);
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
}

void ChonkyLauncher::updateScanButtonState()
{
    bool canScan = !m_chonkyExecutablePath.isEmpty() && 
                   QFileInfo::exists(m_chonkyExecutablePath) &&
                   !m_gamesFolderPath.isEmpty() && 
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
            
            QListWidgetItem* item = new QListWidgetItem(subdir);
            item->setData(Qt::UserRole, subDir.absolutePath());
            item->setToolTip(subDir.absolutePath());
            m_gamesList->addItem(item);
        } else {
            scanDirectory(subDir, basePath);
        }
    }
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
    
    m_statusLabel->setText(QString("Launching: %1").arg(QFileInfo(gamePath).baseName()));
    QApplication::processEvents();
    
    QProcess* process = new QProcess(this);
    QStringList arguments;
    arguments << gamePath;
    
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [this, process, gamePath](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitCode != 0 || exitStatus != QProcess::NormalExit) {
                QMessageBox::warning(this, "Launch Error", 
                    QString("Failed to launch game from: %1\nExit code: %2")
                    .arg(gamePath).arg(exitCode));
            }
            process->deleteLater();
            m_statusLabel->setText("Ready");
        });
    
    connect(process, &QProcess::errorOccurred, [this, process, gamePath](QProcess::ProcessError error) {
        QMessageBox::critical(this, "Process Error", 
            QString("Failed to start ChonkyStation: %1\nError: %2")
            .arg(m_chonkyExecutablePath).arg(process->errorString()));
        process->deleteLater();
        m_statusLabel->setText("Ready");
    });
    
    process->start(m_chonkyExecutablePath, arguments);
    
    if (!process->waitForStarted(5000)) {
        QMessageBox::critical(this, "Launch Error", 
            QString("Failed to start ChonkyStation executable:\n%1").arg(m_chonkyExecutablePath));
        process->deleteLater();
        m_statusLabel->setText("Ready");
    } else {
        m_statusLabel->setText("Game launched successfully!");
    }
}

void ChonkyLauncher::loadSettings()
{
    m_chonkyExecutablePath = m_settings->value("chonkyExecutable", "").toString();
    m_gamesFolderPath = m_settings->value("gamesFolder", "").toString();
}

void ChonkyLauncher::saveSettings()
{
    m_settings->setValue("chonkyExecutable", m_chonkyExecutablePath);
    m_settings->setValue("gamesFolder", m_gamesFolderPath);
}
