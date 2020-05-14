#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QDebug>
#include <QKeyEvent>
#include <QEvent>
#include <memory>

#include "state.h"
#include "asrrecognizer.h"
#include "audiorecorder.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    std::unique_ptr<State> editorStatus;
    asr::AudioRecorder recorder{"__tmp.wav"};
    asr::ASRRecognizer recognizer{"__tmp.wav"};
    bool isRecording = false;
private slots:
    void saveAction();
    void newAction();
    void openAction();
    void saveasAction();
    void closeAction();
    void updateCaption();
    void textChangedStatus();
    void asrBegin();
    void asrEnd();
protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
