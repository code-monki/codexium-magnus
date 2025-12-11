#ifndef IPRINTSERVICE_H
#define IPRINTSERVICE_H

#include <QObject>
#include <QString>

namespace CodexiumMagnus::Services {

/**
 * Service interface for printing content.
 * 
 * Handles print rendering via PDF (FR-10). Provides functionality to print
 * HTML content either to a physical printer or to a PDF file. Ensures that
 * content is printed without truncation and maintains proper formatting
 * for A4/Letter page sizes.
 * 
 * The service uses Qt WebEngine's printing capabilities to render HTML
 * content for printing, ensuring consistent output across platforms.
 */
class IPrintService : public QObject {
    Q_OBJECT

public:
    explicit IPrintService(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IPrintService() = default;

    /**
     * Print HTML content to a physical printer.
     * 
     * Displays a print dialog allowing the user to select printer and
     * print options, then prints the content. The content is rendered
     * using Qt WebEngine to ensure proper formatting.
     * 
     * @param htmlContent The HTML content to print
     */
    virtual void printContent(const QString& htmlContent) = 0;

    /**
     * Print HTML content to a PDF file.
     * 
     * Renders the HTML content and saves it as a PDF file at the specified
     * path. The PDF is formatted for A4 or Letter page size with proper
     * margins and page breaks to prevent content truncation.
     * 
     * @param htmlContent The HTML content to print
     * @param outputPath The file path where the PDF should be saved
     */
    virtual void printToPdf(const QString& htmlContent, const QString& outputPath) = 0;

signals:
    /**
     * Emitted when a print operation completes successfully.
     * 
     * This signal is emitted after the content has been successfully
     * sent to the printer or saved as a PDF file.
     */
    void printCompleted();

    /**
     * Emitted when an error occurs during printing.
     * 
     * @param errorMessage Description of the error that occurred
     */
    void printError(const QString& errorMessage);
};

} // namespace CodexiumMagnus::Services

#endif // IPRINTSERVICE_H