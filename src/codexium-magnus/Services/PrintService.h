#ifndef PRINTSERVICE_H
#define PRINTSERVICE_H

#include "IPrintService.h"
#include <QWebEngineView>
#include <QString>
#include <QPrinter>

namespace CodexiumMagnus::Services {

/**
 * Implementation of IPrintService using Qt WebEngine.
 * 
 * Uses Qt WebEngine's printing capabilities to render HTML content
 * for printing to physical printers or PDF files. Handles asynchronous
 * page loading and print operations to ensure content is fully rendered
 * before printing.
 */
class PrintService : public IPrintService {
    Q_OBJECT

public:
    /**
     * Construct a new PrintService instance.
     * 
     * @param webEngineView The QWebEngineView to use for rendering content
     * @param parent Parent QObject for memory management
     */
    explicit PrintService(QWebEngineView *webEngineView, QObject *parent = nullptr);
    
    /**
     * Destructor.
     */
    ~PrintService();

    /**
     * Print HTML content to a physical printer.
     * 
     * Loads the HTML content into the WebEngine view, waits for it to load,
     * then displays a print dialog for the user to select printer and options.
     * 
     * @param htmlContent The HTML content to print
     */
    void printContent(const QString& htmlContent) override;

    /**
     * Print HTML content to a PDF file.
     * 
     * Loads the HTML content into the WebEngine view, waits for it to load,
     * then renders it as a PDF file at the specified path. The PDF is
     * formatted for A4 page size with portrait orientation.
     * 
     * @param htmlContent The HTML content to print
     * @param outputPath The file path where the PDF should be saved
     */
    void printToPdf(const QString& htmlContent, const QString& outputPath) override;

private slots:
    /**
     * Handle completion of PDF printing operation.
     * 
     * @param filePath The path where the PDF was saved
     * @param success true if printing succeeded, false otherwise
     */
    void onPdfPrintingFinished(const QString& filePath, bool success);
    
    /**
     * Handle completion of page loading for print operations.
     */
    void onPageLoadFinished(bool success);

private:
    /**
     * Setup printer with default settings for document printing.
     * 
     * @param printer The printer to configure
     */
    void setupPrinter(QPrinter& printer);

    QWebEngineView *m_webEngineView;  ///< WebEngine view for rendering content
    QString m_pendingHtmlContent;     ///< HTML content waiting to be printed
    QString m_pendingPdfPath;         ///< PDF path for pending print operation
    bool m_isPrintPending;            ///< Flag indicating if a print operation is pending
    bool m_isPrintToPhysicalPrinter;  ///< Flag indicating if PDF is for physical printer (temp file)
};

} // namespace CodexiumMagnus::Services

#endif // PRINTSERVICE_H