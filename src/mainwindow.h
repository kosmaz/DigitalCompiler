#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "Bank.h"

namespace Ui
{
    class MainWindow;
}

class QLabel;
class QTimer;
class QSound;
class QAction;
class QDialog;
class QGroupBox;
class QScrollArea;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QRadioButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void newTest();
    void continueTest();
    void loadTest();
    void saveTest();
    void saveTestAs();
    void setupAction();
    void exitAction();
    void helpAction();
    void aboutAction();
    void checkForUpdatesAction();
    void registrationAction();

    inline void answerOne();
    inline void answerTwo();
    inline void answerThree();
    inline void answerFour();
    inline void answerFive();
    inline void noAnswer();

    inline void setTimeLimit(int);
    inline void setDefaultTime(int);
    inline void setDefaultPath();
    inline void setAudio(bool);
    void setAudioPlayBack(bool);

    inline void setYellow();
    inline void setPink();
    inline void setLightBlue();
    inline void setLightGreen();
    inline void setLightRed();
    void setBackgroundColor(int);
    inline void setOK();
    inline void setCancel();
    inline void changePix();

    void cancelTest();
    void continueLater();
    void previous();
    void next();
    void submit();
    void setPlayPause(bool);
    void timeMonitor();

    inline void dummy(){startTest(0);}
    inline void dummy1(){startTest(1);}
    inline void dummy2(){startTest(2);}

signals:
    void audioPlayBack(bool);
    void colorChanged(int);

private:

    void setupActions();
    void setupMenus();
    void setupCentralWidget(int when=0);
    void setupTestWindow();
    void pixSlide();
    void otherLabel();
    QString arrangeString(QString);
    QMenu* setupExamMenu(QString,int,int,QVector<QString>&,QVector<QAction*>&);

    void startTest(unsigned, bool=false);
    QuestionBank* selectTestType(unsigned);
    void displayQuestionAnswer(int);
    void concludeTest();

    void save(bool type=0);
    bool loadSettings();
    bool saveSettings(bool);

    Ui::MainWindow* ui;

    /*members for setting up the main windows interface, includes all the
     *necessary actions required for this setup
     */
    QAction* fNew_Test;
    QAction* fContinue_Test;
    QAction* fLoad_Test;
    QAction* fSave_Test;
    QAction* fSave_As_Test;
    QAction* fSetup_Action;
    QAction* fExit_Action;
    QAction* fHelp_Action;
    QAction* fAbout_Action;
    QAction* fCheck_For_Updates_Action;
    QAction* fRegistration_Action;

    /*The two main general layouts for both modes of operation of this program*/
    QHBoxLayout* mainwindow;
    QVBoxLayout* testwindow;

    //main buttons on the non-test mode central widget
    QPushButton* newtest_button;
    QPushButton* help_button;
    QPushButton* about_button;
    QPushButton* exit_button;

    //used to request from the users their choice of timelimit and the particular test to embark on
    QDialog* choice_dialog;

    /*members that stores the general settings of this program and are to be initiaized from
     * file when the program is started
     */
    QString* fDefault_file_path;
    QString* fLast_Save;
    int default_time;
    int color_choice;
    bool disable_audio;
    QDialog* settings_dialog;

    //String for holding present working test name and its file path on system if any was given
    QString* fWorking_File;
    QString* fTest_Name;

    //objects for handling questions and answers. interface between the user and the program during test
    QScrollArea* question_scrollArea;
    QLabel* question;
    QScrollArea* answer_scrollArea;
    QRadioButton* answer_1;
    QRadioButton* answer_2;
    QRadioButton* answer_3;
    QRadioButton* answer_4;
    QRadioButton* answer_5;
    QRadioButton* no_answer;

    //for grouping the radiobuttons together
    QGroupBox* question_box;
    QGroupBox* answer_box;
    QGroupBox* answer_box2;

    //main buttons on the test layout for accepting input events from the user
    QLabel* number;
    QLabel* number_label;
    QLabel* time_label;
    QLabel* time_display;
    QPushButton* continue_later_button;
    QPushButton* previous_button;
    QPushButton* next_button;
    QPushButton* submit_button;
    QPushButton* cancel_button;
    QPushButton* pause_play_button;

    //the QuestionBank object that is been presently worked with by the user
    QuestionBank* fRunning;

    /*data members that are to be saved by a QuestionBank object when asked to save itself
     * and when asked to load itself
     */
    QVector<QuestionAnswer>* fQuestions;
    QVector<unsigned>* fAnswers;
    bool fState;
    unsigned fTime_Limit;        //holds time value in minutes
    unsigned fPresent_Time;     //holds time value in seconds
    unsigned fBank_ID;
    unsigned fPresent_Question;
    unsigned fMax_Questions;

    QTimer* fTimer;         //handles the timing event during a test
    QSound* fSound;         //handles the sound playing when the sound option is enabled

    int fPix_Max_Time;
    int fPix_Time_Count;
    int fPix;
    QTimer* fCentral_Timer;
    QLabel* fCentral_Label;
    QLabel* fOther_Label;
};


inline void MainWindow::answerOne()
{
    fAnswers->replace(fPresent_Question,1);
    return;
}


inline void MainWindow::answerTwo()
{
    fAnswers->replace(fPresent_Question,2);
    return;
}


inline void MainWindow::answerThree()
{
    fAnswers->replace(fPresent_Question,3);
    return;
}


inline void MainWindow::answerFour()
{
    fAnswers->replace(fPresent_Question,4);
    return;
}


inline void MainWindow::answerFive()
{
    fAnswers->replace(fPresent_Question,5);
    return;
}


inline void MainWindow::noAnswer()
{
    fAnswers->replace(fPresent_Question,0);
    return;
}


inline void MainWindow::setTimeLimit(int time)
{
    fTime_Limit=time;
    return;
}


inline void MainWindow::setDefaultTime(int dtime)
{
    default_time=dtime;
    return;
}


inline void MainWindow::setDefaultPath()
{
    *fDefault_file_path=QFileDialog::getExistingDirectory(this);
    return;
}


inline void MainWindow::setOK()
{
    saveSettings(true);
    settings_dialog->close();
    return;
}


inline void MainWindow::setCancel()
{
    saveSettings(false);
    settings_dialog->close();
    return;
}


inline void MainWindow::setAudio(bool play)
{
    disable_audio=play;
    emit audioPlayBack(play);
    return;
}


inline void MainWindow::setYellow()
{
    color_choice=1;
    if(testwindow)
        emit colorChanged(color_choice);
    return;
}

inline void MainWindow::setPink()
{
    color_choice=2;
    if(testwindow)
        emit colorChanged(color_choice);
    return;
}


inline void MainWindow::setLightBlue()
{
    color_choice=3;
    if(testwindow)
        emit colorChanged(color_choice);
    return;
}


inline void MainWindow::setLightGreen()
{
    color_choice=4;
    if(testwindow)
        emit colorChanged(color_choice);
    return;
}


inline void MainWindow::setLightRed()
{
    color_choice=5;
    if(testwindow)
        emit colorChanged(color_choice);
    return;
}


inline void MainWindow::changePix()
{
    ++fPix_Time_Count;
    if(fPix_Max_Time==fPix_Time_Count)
    {
        pixSlide();
        fPix_Time_Count=0;
    }
    return;
}

#endif // MAINWINDOW_H
