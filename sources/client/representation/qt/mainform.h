#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>
#include <QTimer>
#include <QTextEdit>
#include <QLineEdit>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QMap>
#include <QVector>
#include <QElapsedTimer>

namespace Ui {
class MainForm;
}

class Representation;

class MainForm : public QWidget
{
    Q_OBJECT
public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();
public slots:
    void addSytemTextToTab(const QString& tab, const QString& text);
    void clearSystemTexts();
    void removeEmptyTabs();

    void startGameLoop(int id, QString map);
    void connectionFailed(QString reason);

    void insertHtmlIntoChat(QString html);

    void oocPrefixToLineEdit();

    void uploadStarted();
    void uploadFinished();
protected:
    void closeEvent(QCloseEvent* event);
    void resizeEvent(QResizeEvent* event);
private slots:
    void setFocusOnLineEdit();

    void on_command_line_edit_returnPressed();

    void on_splitter_splitterMoved(int pos, int index);

    void connectToHost();
signals:
    void closing();
    void generateUnsync();
private:
    void AddSystemTexts();

    QElapsedTimer close_request_timer_;
    bool map_sending_;

    void RemoveFirstBlockFromTextEditor();

    QMap<QString, QTextEdit*> texts_;

    int left_column_;
    int right_column_;

    int argc_;
    char** argv_;
    Ui::MainForm *ui;
    int fps_cap_;

    int current_fps_;
    qint64 represent_max_ms_;

    QVector<QString> chat_messages_;
    Representation* representation_;
};

#endif // MAINFORM_H
