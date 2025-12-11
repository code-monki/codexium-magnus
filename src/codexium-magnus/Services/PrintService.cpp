#include "PrintService.h"
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QPrintDialog>
#include <QPrinter>
#include <QPageLayout>
#include <QPageSize>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QTemporaryFile>
#include <QDesktopServices>
#include <QUrl>

namespace CodexiumMagnus::Services {

PrintService::PrintService(QWebEngineView *webEngineView, QObject *parent)
    : IPrintService(parent)
    , m_webEngineView(webEngineView)
    , m_isPrintPending(false)
    , m_isPrintToPhysicalPrinter(false)
{
    if (m_webEngineView && m_webEngineView->page()) {
        // Connect to page load finished signal for async printing
        connect(m_webEngineView->page(), &QWebEnginePage::loadFinished,
                this, &PrintService::onPageLoadFinished);
        
        // Connect to PDF printing finished signal
        connect(m_webEngineView->page(), &QWebEnginePage::pdfPrintingFinished,
                this, &PrintService::onPdfPrintingFinished);
    }
}

PrintService::~PrintService() {
    // Qt parent system handles cleanup
}

void PrintService::printContent(const QString& htmlContent) {
    if (!m_webEngineView || !m_webEngineView->page()) {
        emit printError("WebEngine view not available");
        return;
    }

    if (m_isPrintPending) {
        emit printError("Print operation already in progress");
        return;
    }

    // Store content and set flag
    m_pendingHtmlContent = htmlContent;
    m_pendingPdfPath.clear();
    m_isPrintPending = true;

    // Load HTML content - will trigger onPageLoadFinished when ready
    m_webEngineView->setHtml(htmlContent);
}

void PrintService::printToPdf(const QString& htmlContent, const QString& outputPath) {
    if (!m_webEngineView || !m_webEngineView->page()) {
        emit printError("WebEngine view not available");
        return;
    }

    if (outputPath.isEmpty()) {
        emit printError("Output path is empty");
        return;
    }

    if (m_isPrintPending) {
        emit printError("Print operation already in progress");
        return;
    }

    // Ensure output directory exists
    QFileInfo fileInfo(outputPath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            emit printError(QString("Failed to create output directory: %1").arg(dir.absolutePath()));
            return;
        }
    }

    // Check if file is writable
    if (fileInfo.exists() && !QFileInfo(outputPath).isWritable()) {
        emit printError(QString("Output file is not writable: %1").arg(outputPath));
        return;
    }

    // Store content and path, set flag
    m_pendingHtmlContent = htmlContent;
    m_pendingPdfPath = outputPath;
    m_isPrintPending = true;
    m_isPrintToPhysicalPrinter = false;

    // Load HTML content - will trigger onPageLoadFinished when ready
    m_webEngineView->setHtml(htmlContent);
}

void PrintService::onPageLoadFinished(bool success) {
    if (!m_isPrintPending) {
        return; // Not our print operation
    }

    if (!success) {
        m_isPrintPending = false;
        m_pendingHtmlContent.clear();
        m_pendingPdfPath.clear();
        emit printError("Failed to load content for printing");
        return;
    }

    if (!m_pendingPdfPath.isEmpty()) {
        // Print to PDF
        QPageLayout layout(QPageSize::A4, QPageLayout::Portrait, QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);
        m_webEngineView->page()->printToPdf(m_pendingPdfPath, layout);
        // Note: onPdfPrintingFinished will be called when PDF is ready
    } else {
        // Print to physical printer
        // In Qt 6, QWebEnginePage::print() doesn't exist.
        // We'll use printToPdf() to a temp file, then open it with system print dialog.
        QTemporaryFile tempFile;
        tempFile.setFileTemplate(QDir::temp().absoluteFilePath("codexium-print-XXXXXX.pdf"));
        if (!tempFile.open()) {
            m_isPrintPending = false;
            m_pendingHtmlContent.clear();
            emit printError("Failed to create temporary file for printing");
            return;
        }
        
        QString tempPath = tempFile.fileName();
        tempFile.close();
        
        // Store temp path for cleanup after printing
        m_pendingPdfPath = tempPath;
        m_isPrintToPhysicalPrinter = true;
        
        // Print to PDF first, then we'll open it with system print dialog
        QPageLayout layout(QPageSize::A4, QPageLayout::Portrait, QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);
        m_webEngineView->page()->printToPdf(tempPath, layout);
        // Note: onPdfPrintingFinished will be called when PDF is ready
    }
}

void PrintService::onPdfPrintingFinished(const QString& filePath, bool success) {
    m_isPrintPending = false;
    m_pendingHtmlContent.clear();
    
    if (success) {
        if (m_isPrintToPhysicalPrinter) {
            // Open PDF with system print dialog
            QUrl url = QUrl::fromLocalFile(filePath);
            if (QDesktopServices::openUrl(url)) {
                emit printCompleted();
            } else {
                emit printError("Failed to open PDF for printing");
            }
            // Clean up temp file after a delay (system print dialog needs it)
            // Note: In a production app, you might want to use QTimer to delete it later
        } else {
            emit printCompleted();
        }
    } else {
        emit printError(QString("Failed to print to PDF: %1").arg(filePath));
    }
    
    m_pendingPdfPath.clear();
    m_isPrintToPhysicalPrinter = false;
}

void PrintService::setupPrinter(QPrinter& printer) {
    // Set default printer settings
    printer.setPageSize(QPageSize::A4);
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);
    printer.setColorMode(QPrinter::Color);
    printer.setOutputFormat(QPrinter::NativeFormat);
}

} // namespace CodexiumMagnus::Services