#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRadioButton>
#include <QBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QScrollArea>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>
#include <QAction>
#include <QColor>
#include <QLabel>
#include <QTimer>
#include <QSound>
#include <QMenu>
#include <QFile>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow),fState(false),fTime_Limit(0),
    fPresent_Time(0),fBank_ID(0),fPresent_Question(0),fMax_Questions(0),fPix_Max_Time(10),fPix_Time_Count(0),fPix(0),
    fCentral_Label(nullptr),fOther_Label(nullptr)
{
    if(!loadSettings())delete this;
    ui->setupUi(this);
    setupActions();
    setupMenus();
    setupCentralWidget(1);
    setWindowIcon(QIcon(":/images/ICONS/book.ico"));
    fWorking_File=new QString;
    connect(this,SIGNAL(audioPlayBack(bool)),this,SLOT(setAudioPlayBack(bool)));
    connect(this,SIGNAL(colorChanged(int)),this,SLOT(setBackgroundColor(int)));
    if(disable_audio==false)
        setAudioPlayBack(disable_audio);
    testwindow=nullptr;
    this->adjustSize();
}


MainWindow::~MainWindow()
{
    if(fState==true)
        exitAction();
    QFile save("ppconfig.ini");
    save.open(QIODevice::WriteOnly);

    QDataStream out(&save);
    out.setVersion(QDataStream::Qt_5_4);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out<<*fDefault_file_path<<*fLast_Save<<(qint32)default_time<<(qint32)color_choice<<(quint8)disable_audio;
    save.close();
    QApplication::restoreOverrideCursor();

    if(disable_audio==false)
        delete fSound;
    delete fWorking_File;
    delete fDefault_file_path;
    delete fLast_Save;
}


void MainWindow::setupActions()
{

    fNew_Test=new QAction(QIcon(":/images/ICONS/new.png"),tr("&New test"),this);
    fNew_Test->setShortcuts(QKeySequence::New);
    fNew_Test->setStatusTip(tr("Start a new test"));
    connect(fNew_Test,SIGNAL(triggered()),this,SLOT(newTest()));


    fContinue_Test=new QAction(QIcon(":/images/ICONS/continue.png"),tr("&Continue test"),this);
    fContinue_Test->setStatusTip(tr("Continue with your last test"));
    connect(fContinue_Test,SIGNAL(triggered()),this,SLOT(continueTest()));

    fLoad_Test=new QAction(QIcon(":/images/ICONS/open.png"),tr("&Load test"),this);
    fLoad_Test->setShortcuts(QKeySequence::Open);
    fLoad_Test->setStatusTip(tr("Load from a list of old tests"));
    connect(fLoad_Test,SIGNAL(triggered()),this,SLOT(loadTest()));

    fSave_Test=new QAction(QIcon(":/images/ICONS/save.png"),tr("&Save test"),this);
    fSave_Test->setShortcuts(QKeySequence::Save);
    fSave_Test->setStatusTip(tr("Save test progress to file"));
    connect(fSave_Test,SIGNAL(triggered()),this,SLOT(saveTest()));

    fSave_As_Test=new QAction(QIcon(":/images/ICONS/save_as.png"),tr("Save test &As"),this);
    fSave_As_Test->setShortcuts(QKeySequence::SaveAs);
    fSave_As_Test->setStatusTip(tr("Save test with a different name"));
    connect(fSave_As_Test,SIGNAL(triggered()),this,SLOT(saveTestAs()));

    fSetup_Action=new QAction(QIcon(":/images/ICONS/Settings.png"),tr("S&etup"),this);
    fSetup_Action->setStatusTip(tr("Settings"));
    connect(fSetup_Action,SIGNAL(triggered()),this,SLOT(setupAction()));

    fHelp_Action=new QAction(QIcon(":/images/ICONS/Help.png"),tr("Hel&p"),this);
    fHelp_Action->setStatusTip(tr("Help contents"));
    connect(fHelp_Action,SIGNAL(triggered()),this,SLOT(helpAction()));

    fAbout_Action=new QAction(QIcon(":/images/ICONS/about.png"),tr("A&bout"),this);
    fAbout_Action->setStatusTip(tr("Information about program"));
    connect(fAbout_Action,SIGNAL(triggered()),this,SLOT(aboutAction()));

    fCheck_For_Updates_Action=new QAction(QIcon(":/images/ICONS/update.png"),tr("Check for &updates"),this);
    fCheck_For_Updates_Action->setStatusTip(tr("Check online for updates"));
    connect(fCheck_For_Updates_Action,SIGNAL(triggered()),this,SLOT(checkForUpdatesAction()));

    fRegistration_Action=new QAction(QIcon(":/images/ICONS/register.png"),tr("&Register product"),this);
    fRegistration_Action->setStatusTip(tr("Register your product online for full fuctionality"));
    connect(fRegistration_Action,SIGNAL(triggered()),this,SLOT(registrationAction()));

    fExit_Action=new QAction(QIcon(":/images/ICONS/exit.png"),tr("E&xit"),this);
    fExit_Action->setShortcuts(QKeySequence::Quit);
    fExit_Action->setStatusTip(tr("Close program"));
    connect(fExit_Action,SIGNAL(triggered()),this,SLOT(exitAction()));

    return;
}


void MainWindow::setupMenus()
{
    QMenu* File_Menu=menuBar()->addMenu(tr("&File"));
    File_Menu->addAction(fNew_Test);
    File_Menu->addAction(fContinue_Test);
    File_Menu->addAction(fLoad_Test);
    File_Menu->addSeparator();
    File_Menu->addAction(fSave_Test);
    File_Menu->addAction(fSave_As_Test);
    File_Menu->addSeparator();
    File_Menu->addAction(fSetup_Action);
    File_Menu->addSeparator();
    File_Menu->addAction(fExit_Action);

    QMenu* Help_Menu=menuBar()->addMenu(tr("&Help"));
    Help_Menu->addAction(fHelp_Action);
    Help_Menu->addAction(fCheck_For_Updates_Action);
    Help_Menu->addAction(fRegistration_Action);
    Help_Menu->addSeparator();
    Help_Menu->addAction(fAbout_Action);

    return;
}


void MainWindow::setupCentralWidget(int when)
{
    setWindowTitle(tr("PAST QUESTIONS AND ANSWERS"));

    fPresent_Time=0;
    fPresent_Question=0;
    fNew_Test->setEnabled(true);
    fContinue_Test->setEnabled(true);
    fLoad_Test->setEnabled(true);
    fSave_As_Test->setEnabled(false);
    fSave_Test->setEnabled(false);

    newtest_button=new QPushButton(tr("&New test"),this);
    newtest_button->setDefault(true);
    newtest_button->setIcon(QIcon(":/images/ICONS/add.png"));
    newtest_button->setStatusTip(tr("Start a new test"));
    connect(newtest_button,SIGNAL(clicked()),this,SLOT(newTest()));

    help_button=new QPushButton(tr("Hel&p"),this);
    help_button->setIcon(QIcon(":/images/ICONS/Help.png"));
    help_button->setStatusTip(tr("Help contents"));
    connect(help_button,SIGNAL(clicked()),this,SLOT(helpAction()));

    about_button=new QPushButton(tr("A&bout"),this);
    about_button->setIcon(QIcon(":/images/ICONS/about.png"));
    about_button->setStatusTip(tr("Information about program"));
    connect(about_button,SIGNAL(clicked()),this,SLOT(aboutAction()));

    exit_button=new QPushButton(tr("E&xit"),this);
    exit_button->setIcon(QIcon(":/images/ICONS/exit.png"));
    exit_button->setStatusTip(tr("Close program"));
    connect(exit_button,SIGNAL(clicked()),this,SLOT(exitAction()));

    QHBoxLayout* hboxlayout=new QHBoxLayout();
    hboxlayout->addWidget(newtest_button);
    hboxlayout->addWidget(help_button);
    hboxlayout->addWidget(about_button);
    hboxlayout->addWidget(exit_button);

    if(when==0)
    {
        /*modify later by using a QWidget Vector to hold pointer to all QWidget objects
         * to make deleting, enabling and disabling easier to perform by using loops
         */
        delete question,question=nullptr;
        delete question_scrollArea,question_scrollArea=nullptr;
        delete question_box,question_box=nullptr;
        delete answer_1,answer_1=nullptr;
        delete answer_2,answer_2=nullptr;
        delete answer_3,answer_3=nullptr;
        delete answer_4,answer_4=nullptr;
        delete answer_5,answer_5=nullptr;
        delete no_answer,no_answer=nullptr;
        delete answer_box2,answer_box2=nullptr;
        delete answer_scrollArea,answer_scrollArea=nullptr;
        delete answer_box,answer_box=nullptr;
        delete pause_play_button,pause_play_button=nullptr;
        delete number,number=nullptr;
        delete number_label,number_label=nullptr;
        delete time_label,time_label=nullptr;
        delete time_display,time_display=nullptr;
        delete continue_later_button,continue_later_button=nullptr;
        delete previous_button,previous_button=nullptr;
        delete next_button,next_button=nullptr;
        delete submit_button,submit_button=nullptr;
        delete cancel_button,cancel_button=nullptr;
        QLayoutItem* item;
        while((item=testwindow->takeAt(0))!=0)
            delete item;
        delete testwindow,testwindow=nullptr;
    }

    pixSlide();
    otherLabel();
    QVBoxLayout* label_layout=new QVBoxLayout;
    label_layout->addWidget(fCentral_Label,4);
    label_layout->addLayout(hboxlayout,1);

    mainwindow=new QHBoxLayout(centralWidget());
    mainwindow->addLayout(label_layout,5);
    mainwindow->addWidget(fOther_Label,1);
    QPalette bpalette=centralWidget()->palette();
    bpalette.setColor(QPalette::Window,QColor(127,127,127));
    centralWidget()->setAutoFillBackground(true);
    centralWidget()->setPalette(bpalette);

    fCentral_Timer=new QTimer(this);
    fCentral_Timer->start(1000);
    connect(fCentral_Timer,SIGNAL(timeout()),this,SLOT(changePix()));

    return;
}


void MainWindow::pixSlide()
{
    if(fCentral_Label==nullptr)
        fCentral_Label=new QLabel(this);
    switch(fPix)
    {
        case 0:
        {
            QImage pix(":/images/ICONS/main.jpg");
            fCentral_Label->setPixmap(QPixmap::fromImage(pix));
            fCentral_Label->setScaledContents(true);
        }
            break;
        case 1:
        {
            QImage pix(":/images/ICONS/main1.jpg");
            fCentral_Label->setPixmap(QPixmap::fromImage(pix));
            fCentral_Label->setScaledContents(true);
        }
            break;
        case 2:
        {
            QImage pix(":/images/ICONS/main2.jpg");
            fCentral_Label->setPixmap(QPixmap::fromImage(pix));
            fCentral_Label->setScaledContents(true);
        }
            break;
        case 3:
        {
            QImage pix(":/images/ICONS/main3.jpg");
            fCentral_Label->setPixmap(QPixmap::fromImage(pix));
            fCentral_Label->setScaledContents(true);
        }
            break;
        case 4:
        {
            QImage pix(":/images/ICONS/main4.jpg");
            fCentral_Label->setPixmap(QPixmap::fromImage(pix));
            fCentral_Label->setScaledContents(true);
        }
    }
    if(fPix==4)
        fPix=0;
    else
        ++fPix;
    return;
}


void MainWindow::otherLabel()
{
    if(fOther_Label==nullptr)
        fOther_Label=new QLabel(this);
    fOther_Label->setPixmap(QPixmap::fromImage(QImage(":/images/ICONS/otherLabel.jpg")));
    fOther_Label->setScaledContents(true);
    return;
}


void MainWindow::setupTestWindow()
{
    fCentral_Timer->stop(),delete fCentral_Timer,fPix_Time_Count=0;
    delete fCentral_Label,fCentral_Label=nullptr;
    delete fOther_Label,fOther_Label=nullptr;
    delete newtest_button,newtest_button=nullptr;
    delete help_button,help_button=nullptr;
    delete about_button,about_button=nullptr;
    delete exit_button,exit_button=nullptr;

    fNew_Test->setEnabled(false);
    fContinue_Test->setEnabled(false);
    fLoad_Test->setEnabled(false);
    fSave_As_Test->setEnabled(true);
    fSave_Test->setEnabled(true);

    question=new QLabel(tr("none"),this);
    question->setWordWrap(true);
    question->setTextFormat(Qt::RichText);
    question->setMinimumSize(740,850);

    question_scrollArea=new QScrollArea(this);
    question_scrollArea->setWidget(question);

    answer_1=new QRadioButton(tr("none"),this);
    connect(answer_1,SIGNAL(clicked()),this,SLOT(answerOne()));
    answer_2=new QRadioButton(tr("none"),this);
    connect(answer_2,SIGNAL(clicked()),this,SLOT(answerTwo()));
    answer_3=new QRadioButton(tr("none"),this);
    connect(answer_3,SIGNAL(clicked()),this,SLOT(answerThree()));
    answer_4=new QRadioButton(tr("none"),this);
    connect(answer_4,SIGNAL(clicked()),this,SLOT(answerFour()));
    answer_5=new QRadioButton(tr("none"),this);
    connect(answer_5,SIGNAL(clicked()),this,SLOT(answerFive()));
    no_answer=new QRadioButton(tr("SKIP QUESTION"),this);
    no_answer->setChecked(true);
    connect(no_answer,SIGNAL(clicked()),this,SLOT(noAnswer()));

    question_box=new QGroupBox(tr("Question\t\t\t(Scroll down to see question if not visible at first)"),this);
    QHBoxLayout* qlayout=new QHBoxLayout;
    qlayout->addWidget(question_scrollArea);
    question_box->setLayout(qlayout);

    previous_button=new QPushButton(tr("Previous"),this);
    previous_button->setIcon(QIcon(":/images/ICONS/previous.ico"));
    connect(previous_button,SIGNAL(clicked()),this,SLOT(previous()));
    next_button=new QPushButton(tr("Next"),this);
    next_button->setIcon(QIcon(":/images/ICONS/next.ico"));
    connect(next_button,SIGNAL(clicked()),this,SLOT(next()));
    submit_button=new QPushButton(tr("Submit"),this);
    submit_button->setDefault(true);
    submit_button->setIcon(QIcon(":/images/ICONS/submit.ico"));
    connect(submit_button,SIGNAL(clicked()),this,SLOT(submit()));
    QVBoxLayout* prev_next_layout=new QVBoxLayout();
    prev_next_layout->addWidget(previous_button);
    prev_next_layout->addWidget(next_button);
    prev_next_layout->addWidget(submit_button);

    QVBoxLayout* answer_layout=new QVBoxLayout();
    answer_layout->addWidget(answer_1);
    answer_layout->addWidget(answer_2);
    answer_layout->addWidget(answer_3);
    answer_layout->addWidget(answer_4);
    answer_layout->addWidget(answer_5);
    answer_layout->addWidget(no_answer);

    answer_box2=new QGroupBox;
    answer_box2->setLayout(answer_layout);
    answer_box2->setMinimumSize(430,550);

    answer_scrollArea=new QScrollArea;
    answer_scrollArea->setWidget(answer_box2);

    QVBoxLayout* answer_scrollArea_layout=new QVBoxLayout;
    answer_scrollArea_layout->addWidget(answer_scrollArea);

    answer_box=new QGroupBox(tr("Answer"),this);
    answer_box->setLayout(answer_scrollArea_layout);

    QHBoxLayout* middle_layout=new QHBoxLayout;
    middle_layout->addWidget(question_box,5);
    middle_layout->addLayout(prev_next_layout);
    middle_layout->addWidget(answer_box,3);

    number=new QLabel(tr("0/0"),this);
    number_label=new QLabel(tr("Question number:"),this);
    time_display=new QLabel(tr("Unlimited"),this);
    time_label=new QLabel(tr("Time Remaining:"),this);
    QHBoxLayout* mlayout_1=new QHBoxLayout;
    mlayout_1->addWidget(number_label,1);
    mlayout_1->addWidget(number,2);
    mlayout_1->addSpacing(20);
    mlayout_1->addWidget(time_label,1);
    mlayout_1->addWidget(time_display,3);
    mlayout_1->addSpacing(155);

    continue_later_button=new QPushButton(tr("Continue Later"),this);
    continue_later_button->setIcon(QIcon(":/images/ICONS/refresh.ico"));
    connect(continue_later_button,SIGNAL(clicked()),this,SLOT(continueLater()));
    cancel_button=new QPushButton(tr("Cancel"),this);
    connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelTest()));
    QHBoxLayout* mlayout_2=new QHBoxLayout;
    mlayout_1->addSpacing(30);
    mlayout_2->addWidget(continue_later_button);
    mlayout_2->addWidget(cancel_button);

    QHBoxLayout* last_layout=new QHBoxLayout;
    last_layout->addLayout(mlayout_1,4);
    last_layout->addLayout(mlayout_2,1);

    pause_play_button=new QPushButton(tr("Pause Test"),this);
    pause_play_button->setIcon(QIcon(":/images/ICONS/pause.ico"));
    pause_play_button->setCheckable(true);
    pause_play_button->setChecked(false);
    pause_play_button->setMaximumWidth(100);
    connect(pause_play_button,SIGNAL(toggled(bool)),this,SLOT(setPlayPause(bool)));
    QHBoxLayout* top_layout=new QHBoxLayout;
    top_layout->addSpacing(300);
    top_layout->addWidget(pause_play_button);
    top_layout->addSpacing(300);

    QLayoutItem* item;
    while((item=mainwindow->takeAt(0))!=0)
        delete item;
    delete mainwindow;
    testwindow=new QVBoxLayout(centralWidget());
    testwindow->addLayout(top_layout,2);
    testwindow->addLayout(middle_layout,5);
    testwindow->addLayout(last_layout,1);
    if(choice_dialog)
        choice_dialog->close();
    setBackgroundColor(color_choice);
    setWindowTitle(*fTest_Name);
    return;
}


void MainWindow::setBackgroundColor(int choice)
{
    QPalette bpalette=palette();
    centralWidget()->setAutoFillBackground(true);
    switch(choice)
    {
        case 1:bpalette.setColor(QPalette::Window,QColor(255,192,0));          //yellow
            break;
        case 2:bpalette.setColor(QPalette::Window,QColor(255,105,180));        //pink
            break;
        case 3:bpalette.setColor(QPalette::Window,QColor(176,224,230));        //light blue
            break;
        case 4:bpalette.setColor(QPalette::Window,QColor(152,251,152));        //light green
            break;
        case 5:bpalette.setColor(QPalette::Window,QColor(250,128,114));        //light red
            break;
    }
    centralWidget()->setPalette(bpalette);
    return;
}


void MainWindow::setupAction()
{
    settings_dialog=new QDialog(centralWidget());

    QSpinBox* timespin=new QSpinBox(settings_dialog);
    //max minutes allowed for each test is 120 minutes, 2hours
    timespin->setRange(0,120);
    timespin->setValue(default_time);
    connect(timespin,SIGNAL(valueChanged(int)),this,SLOT(setDefaultTime(int)));
    QLabel* timelabel=new QLabel(tr("Choose your default time in Minutes:"),settings_dialog);
    QHBoxLayout* timelayout=new QHBoxLayout;
    timelayout->addWidget(timelabel,2);
    timelayout->addWidget(timespin,1);

    QPushButton* path=new QPushButton(tr("File Path"),settings_dialog);
    connect(path,SIGNAL(clicked()),this,SLOT(setDefaultPath()));
    QLabel* pathlabel=new QLabel(tr("Choose the default file directory to save your tests to:"),settings_dialog);
    QHBoxLayout* pathlayout=new QHBoxLayout;
    pathlayout->addWidget(pathlabel,2);
    pathlayout->addWidget(path,1);

    QCheckBox* audio_check=new QCheckBox(tr("Disable audio"),settings_dialog);
    audio_check->setChecked(disable_audio);
    connect(audio_check,SIGNAL(toggled(bool)),this,SLOT(setAudio(bool)));

    QAction* yellow=new QAction(tr("Yellow"),settings_dialog);
    yellow->setIcon(QIcon(":/images/ICONS/yellow.ico"));
    connect(yellow,SIGNAL(triggered()),this,SLOT(setYellow()));
    QAction* pink=new QAction(tr("Pink"),settings_dialog);
    pink->setIcon(QIcon(":/images/ICONS/pink.ico"));
    connect(pink,SIGNAL(triggered()),this,SLOT(setPink()));
    QAction* lightblue=new QAction(tr("Light Blue"),settings_dialog);
    lightblue->setIcon(QIcon(":/images/ICONS/lightBlue.ico"));
    connect(lightblue,SIGNAL(triggered()),this,SLOT(setLightBlue()));
    QAction* lightgreen=new QAction(tr("Light Green"),settings_dialog);
    lightgreen->setIcon(QIcon(":/images/ICONS/lightGreen.ico"));
    connect(lightgreen,SIGNAL(triggered()),this,SLOT(setLightGreen()));
    QAction* lightcoral=new QAction(tr("Light Coral "),settings_dialog);
    lightcoral->setIcon(QIcon(":/images/ICONS/lightCoral.ico"));
    connect(lightcoral,SIGNAL(triggered()),this,SLOT(setLightRed()));
    QMenu* color_menu=new QMenu(settings_dialog);
    color_menu->addAction(yellow);
    color_menu->addAction(pink);
    color_menu->addAction(lightblue);
    color_menu->addAction(lightgreen);
    color_menu->addAction(lightcoral);
    QPushButton* temp_color=new QPushButton(tr("Color"),settings_dialog);
    temp_color->setMenu(color_menu);
    QLabel* color_label=new QLabel(tr("Choose the Test window's background color from the list of\ncolors provided here -->"),settings_dialog);
    QHBoxLayout* color_layout=new QHBoxLayout;
    color_layout->addWidget(color_label,2);
    color_layout->addWidget(temp_color,1);

    QVBoxLayout* setlayout=new QVBoxLayout;
    setlayout->addLayout(timelayout);
    setlayout->addSpacing(20);
    setlayout->addLayout(pathlayout);
    setlayout->addSpacing(20);
    setlayout->addWidget(audio_check);
    setlayout->addSpacing(20);
    setlayout->addLayout(color_layout);

    QPushButton* okButton=new QPushButton(tr("Ok"),settings_dialog);
    okButton->setDefault(true);
    connect(okButton,SIGNAL(clicked()),this,SLOT(setOK()));
    QPushButton* cancelButton=new QPushButton(tr("Cancel"),settings_dialog);
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(setCancel()));
    QVBoxLayout* buttonlayout=new QVBoxLayout;
    buttonlayout->addWidget(okButton);
    buttonlayout->addWidget(cancelButton);

    QHBoxLayout* homelayout=new QHBoxLayout(settings_dialog);
    homelayout->addLayout(setlayout);
    homelayout->addSpacing(50);
    homelayout->addLayout(buttonlayout);

    settings_dialog->setLayout(homelayout);
    settings_dialog->setWindowTitle(tr("Settings"));
    settings_dialog->setWindowIcon(QIcon(":/images/ICONS/Settings.png"));
    settings_dialog->setWindowModality(Qt::ApplicationModal);
    settings_dialog->show();
    return;
    return;
}


void MainWindow::newTest()
{
    choice_dialog=new QDialog(centralWidget());

    QVector<QAction*> jamb_action_list;
    QVector<QAction*> waec_action_list;
    QVector<QAction*> neco_action_list;
    QVector<QAction*> gce_action_list;
    QVector<QString> subject_menu;
    subject_menu.push_back("English");
    subject_menu.push_back("Maths");
    subject_menu.push_back("Physics");
    QMenu* jamb_menu=setupExamMenu("Jamb",2008,2009,subject_menu,jamb_action_list);
    QMenu* waec_menu=setupExamMenu("Waec",2008,2009,subject_menu,waec_action_list);
    QMenu* neco_menu=setupExamMenu("Neco",2008,2009,subject_menu,neco_action_list);
    QMenu* gce_menu=setupExamMenu("GCE",2008,2009,subject_menu,gce_action_list);

    connect(jamb_action_list.at(0),SIGNAL(triggered()),this,SLOT(dummy1()));
    //connect(jamb_action_list.at(1),SIGNAL(triggered()),this,SLOT(dummy2()));

    for(int i=1; i<jamb_action_list.size(); ++i)
        connect(jamb_action_list.at(i),SIGNAL(triggered()),this,SLOT(dummy()));
    for(int i=0; i<jamb_action_list.size(); ++i)
        connect(waec_action_list.at(i),SIGNAL(triggered()),this,SLOT(dummy()));
    for(int i=0; i<jamb_action_list.size(); ++i)
        connect(neco_action_list.at(i),SIGNAL(triggered()),this,SLOT(dummy()));
    for(int i=0; i<jamb_action_list.size(); ++i)
        connect(gce_action_list.at(i),SIGNAL(triggered()),this,SLOT(dummy()));

    QMenu* exam_menu=new QMenu(choice_dialog);
    exam_menu->addMenu(jamb_menu);
    exam_menu->addMenu(waec_menu);
    exam_menu->addMenu(neco_menu);
    exam_menu->addMenu(gce_menu);

    QPushButton* pushbutton=new QPushButton(tr("Choose from here"),choice_dialog);
    pushbutton->setMenu(exam_menu);
    pushbutton->setMaximumWidth(120);
    QLabel* option=new QLabel(tr("Choose from the list given below\nThe exam type\nThe exam's year\nThe subject"),choice_dialog);
    QVBoxLayout* vboxlayout_1=new QVBoxLayout;
    vboxlayout_1->addWidget(option);
    vboxlayout_1->addWidget(pushbutton);

    QSpinBox* time_spinbox=new QSpinBox(choice_dialog);
    //max minutes allowed for each test is 120 minutes, 2hours
    time_spinbox->setRange(0,120);
    time_spinbox->setValue(default_time);
    time_spinbox->setMaximumWidth(100);
    fTime_Limit=default_time;
    connect(time_spinbox,SIGNAL(valueChanged(int)),this,SLOT(setTimeLimit(int)));
    QLabel* time_label=new QLabel(tr("Select the time limit for each question in Minutes\nChoose 0 Minutes to disable timing"),choice_dialog);
    QVBoxLayout* vboxlayout_2=new QVBoxLayout;
    vboxlayout_2->addWidget(time_label);
    vboxlayout_2->addWidget(time_spinbox);

    QHBoxLayout* hboxlayout=new QHBoxLayout(choice_dialog);
    hboxlayout->addLayout(vboxlayout_1);
    hboxlayout->addSpacing(20);
    hboxlayout->addLayout(vboxlayout_2);

    choice_dialog->setLayout(hboxlayout);
    choice_dialog->setWindowTitle(tr("New Test"));
    choice_dialog->setWindowModality(Qt::ApplicationModal);
    choice_dialog->setWindowIcon(QIcon(":/images/ICONS/new.png"));
    choice_dialog->show();
    return;
}


QMenu* MainWindow::setupExamMenu(QString exam_name, int year_start, int year_end, QVector<QString>& subject_names,
                                 QVector<QAction*>& action_list)
{
    QMenu* exam_type=new QMenu(exam_name,choice_dialog);
    for(int i=year_start; i<=year_end; ++i)
    {
        QMenu* exam_year=exam_type->addMenu(QString::number(i));
        for(int j=0; j<subject_names.size(); ++j)
        {
            QAction* subject=exam_year->addAction(subject_names.at(j));
            action_list.push_back(subject);
        }
    }
    return exam_type;
}


void MainWindow::continueTest()
{
    if(fLast_Save->size()==0)
    {
        QMessageBox::information(this,tr("Continue test"),tr("There is no saved test on this system!"));
        return;
    }
    fRunning=new QuestionBank;
    Format temp;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if(!fRunning->load(temp,*fLast_Save))
    {
        delete fRunning;
        QApplication::restoreOverrideCursor();
        return;
    }
    fBank_ID=temp.fid;
    fPresent_Question=temp.present_question;
    fPresent_Time=temp.present_time;
    fTime_Limit=temp.time_limit;
    fAnswers=temp.temp_answers;
    QApplication::restoreOverrideCursor();
    delete fRunning;
    choice_dialog=nullptr;
    startTest(fBank_ID,true);
    return;
}


void MainWindow::loadTest()
{
    *fWorking_File=QFileDialog::getOpenFileName(this,tr("Load test"),*fDefault_file_path,tr("Test file(*.tst)"));
    fRunning=new QuestionBank;
    Format temp;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if(!fRunning->load(temp,*fWorking_File))
    {
        delete fRunning;
        QApplication::restoreOverrideCursor();
        return;
    }
    fBank_ID=temp.fid;
    fPresent_Question=temp.present_question;
    fPresent_Time=temp.present_time;
    fTime_Limit=temp.time_limit;
    fAnswers=temp.temp_answers;
    QApplication::restoreOverrideCursor();
    delete fRunning;
    choice_dialog=nullptr;
    startTest(fBank_ID,true);
    return;
}


void MainWindow::saveTest()
{
    save();
    return;
}


void MainWindow::saveTestAs()
{
    save(true);
    return;
}


void MainWindow::save(bool type)
{
    QString temp_str(*fWorking_File);
    Format temp;
    temp.fid=fBank_ID;
    temp.present_question=fPresent_Question;
    temp.present_time=fPresent_Time;
    temp.time_limit=fTime_Limit;
    temp.temp_answers=fAnswers;
    if(type==true)
    {
        temp_str=QFileDialog::getSaveFileName(this,tr("Save Test As"),*fDefault_file_path,tr("Test file(*.tst)"));
        *fWorking_File=temp_str;
        fRunning->save(temp,temp_str);
        *fLast_Save=temp_str;
    }
    else
    {
        if(temp_str.size()==0)
        {
            temp_str=QFileDialog::getSaveFileName(this,tr("Save Test"),*fDefault_file_path,tr("Test file(*.tst)"));
            *fWorking_File=temp_str;
        }
        fRunning->save(temp,temp_str);
        *fLast_Save=temp_str;
    }
    return;
}


bool MainWindow::loadSettings()
{
    QFile load("ppconfig.ini");
    if(!load.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this,tr("Error"),
            tr("This application setup file is missing and hereby "
                "this app will be terminated\nPlease contact the developer for help"),QMessageBox::Ok);
        return false;
    }

    QDataStream in(&load);
    in.setVersion(QDataStream::Qt_5_4);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    fDefault_file_path=new QString;
    fLast_Save=new QString;
    qint32 temptime;
    qint32 tempcolor;
    quint8 tempaudio;
    in>>*fDefault_file_path>>*fLast_Save>>temptime>>tempcolor>>tempaudio;
    default_time=temptime;
    color_choice=tempcolor;
    disable_audio=tempaudio;
    if(in.status()!=QDataStream::Ok)
        QMessageBox::warning(this,tr("Load Error"),tr("Encountered an unknown error while trying to\nread from configurations settings file.\n"
                                                       "The file must have been modified by an external\nuser or program. This will not affect the normal\n"
                                                       "runtime of the program."),QMessageBox::Ok);
    load.close();
    QApplication::restoreOverrideCursor();
    return true;
}


bool MainWindow::saveSettings(bool y_n)
{
    if(y_n==false)
        return true;
    else
    {
        QFile save("ppconfig.ini");
        save.open(QIODevice::WriteOnly);

        QDataStream out(&save);
        out.setVersion(QDataStream::Qt_5_4);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        out<<*fDefault_file_path<<*fLast_Save<<(qint32)default_time<<(qint32)color_choice<<(quint8)disable_audio;
        if(out.status()!=QDataStream::Ok)
            QMessageBox::warning(0,tr("Save Error"),tr("Error ecountered while trying to write settings to file"));
        save.close();
        QApplication::restoreOverrideCursor();
    }
    return true;
}


void MainWindow::helpAction()
{
    QMessageBox::information(this,tr("Help"),tr("This is an easy to use program but if you encounter any "
                                                "form\nof problem or problems while using it feel free to contact us\n"
                                                "+2348139278033\n"
                                                "kosmaz2009@yahoo.com"));
    return;
}


void MainWindow::aboutAction()
{
    QMessageBox::about(this,tr("About"),tr("Past Questions And Answers\n"
                                           "\tVersion 0.1.0.0\n\n"
                                           "Developed and marketed by -k0$m@3- Inc.\n"
                                           "Contact us:\n08139278033\nkosmaz2009@yahoo.com"));
    return;
}


//! START
void MainWindow::checkForUpdatesAction()
{
    QMessageBox::information(this,tr("Update"),tr("No Online Update available"));
    return;
}


void MainWindow::registrationAction()
{
    QMessageBox::information(this,tr("Register"),tr("This package is a beta version.\nNo registration is required"));
    return;
}
//! END


void MainWindow::exitAction()
{
    if(fState==true)
    {
        int choice=QMessageBox::warning(this,tr("Unsaved test"
                                                " in progress!"),tr("You have "
                                                                   "a running test "
                                                                   "that is unsaved.\n"
                                                                   "Do you want to save the test progress now?"),
                                                                    QMessageBox::Yes |QMessageBox::No,QMessageBox::Yes);
        switch(choice)
        {
            case QMessageBox::Yes:
            {
                save();
                close();
            }
            break;

            case QMessageBox::No: close();
            break;
        }
    }
    else
        close();
    return;
}


void MainWindow::continueLater()
{
    save();
    fWorking_File->clear();
    delete fRunning;
    if(fTime_Limit!=0)
        delete fTimer;
    setupCentralWidget();
    fState=false;
    return;
}


void MainWindow::cancelTest()
{
    int choice=QMessageBox::warning(this,tr("Cancel test"),
                                    tr("Are you sure you want to cancel the present test?\n"
                                       "All test progress will be lost"),QMessageBox::Yes | QMessageBox::No,QMessageBox::No);
    switch(choice)
    {
        case QMessageBox::Yes:
        {
            delete fRunning;
            if(fTime_Limit!=0)
                delete fTimer;
            if(fWorking_File->size()!=0)fWorking_File->clear();
            setupCentralWidget();
            fState=false;
        }
        break;

        case QMessageBox::No:
        break;
    }
    return;
}


void MainWindow::setPlayPause(bool toggled)
{
    if(toggled==true)
    {
        question->setText("");
        answer_1->setText("");
        answer_2->setText("");
        answer_3->setText("");
        answer_4->setText("");
        answer_5->setText("");
        number->setText("");

        question_box->setEnabled(false);
        answer_box->setEnabled(false);
        next_button->setEnabled(false);
        number->setEnabled(false);
        time_display->setEnabled(false);
        submit_button->setEnabled(false);
        previous_button->setEnabled(false);
        pause_play_button->setText(tr("Continue Test"));
        pause_play_button->setIcon(QIcon(":/images/ICONS/play.ico"));
        if(fTime_Limit!=0)
            fTimer->stop();
    }
    else
    {
        question_box->setEnabled(true);
        answer_box->setEnabled(true);
        next_button->setEnabled(true);
        submit_button->setEnabled(true);
        previous_button->setEnabled(true);
        number->setEnabled(true);
        time_display->setEnabled(true);
        pause_play_button->setText(tr("Pause Test"));
        pause_play_button->setIcon(QIcon(":/images/ICONS/pause.ico"));

        displayQuestionAnswer(fPresent_Question);
        if(fTime_Limit!=0)
            fTimer->start(1000);
    }
    return;
}


void MainWindow::setAudioPlayBack(bool which)
{
    if(which==true)
    {
        fSound->stop();
        delete fSound;
    }
    else
    {
        fSound=new QSound(":/sounds/audio.wav",this);
        fSound->setLoops(QSound::Infinite);
        fSound->play();
    }
    return;
}


void MainWindow::timeMonitor()
{
    unsigned temp_time=fTime_Limit*60;
    ++fPresent_Time;
    unsigned display=temp_time-fPresent_Time;
    unsigned mins=display/60;
    unsigned secs=display%60;
    time_display->setText(QString::number(mins)+" min(s) "+QString::number(secs)+" sec(s)");
    if(fPresent_Time==temp_time)
    {
        QMessageBox::information(this,tr("Out of Time"),tr("Your time has been exhausted!\nEvaluating your test now"
                                                           " based on the ones you have attempted"));
        concludeTest();
    }
    return;
}


void MainWindow::startTest(unsigned id, bool where)
{
    fRunning=selectTestType(id);
    if(!fRunning)
        return;
    fMax_Questions=fRunning->getMaxQuestion();
    if(where==false)
    {
        fAnswers=new QVector<unsigned>(fMax_Questions,0);
        fBank_ID=fRunning->getID();
    }
    fTest_Name=new QString(*fRunning->getName());
    fQuestions=new QVector<QuestionAnswer>(*fRunning->getQuestions());
    setupTestWindow();
    displayQuestionAnswer(fPresent_Question);
    if(fTime_Limit!=0)
    {
        fTimer=new QTimer(this);
        connect(fTimer,SIGNAL(timeout()),this,SLOT(timeMonitor()));
        fTimer->start(1000);
    }
    fState=true;
    return;
}


void MainWindow::displayQuestionAnswer(int num)
{
    if(fQuestions->at(num).question.size()==0)
        question->setPixmap(fQuestions->at(num).pixmap);
    else
        question->setText(fQuestions->at(num).question);

    answer_1->setText(arrangeString(fQuestions->at(num).a));
    answer_1->setEnabled((answer_1->text().size()==0)? false:true);
    answer_2->setText(arrangeString(fQuestions->at(num).b));
    answer_2->setEnabled((answer_2->text().size()==0)? false:true);
    answer_3->setText(arrangeString(fQuestions->at(num).c));
    answer_3->setEnabled((answer_3->text().size()==0)? false:true);
    answer_4->setText(arrangeString(fQuestions->at(num).d));
    answer_4->setEnabled((answer_4->text().size()==0)? false:true);
    answer_5->setText(arrangeString(fQuestions->at(num).e));
    answer_5->setEnabled((answer_5->text().size()==0)? false:true);

    no_answer->setEnabled((fQuestions->at(num).no_answer==false)? true:false);

    if((fQuestions->at(num).no_answer==false))
        switch(fAnswers->at(num))
        {
            case 0:no_answer->setChecked(true);
            break;
            case 1:answer_1->setChecked(true);
            break;
            case 2:answer_2->setChecked(true);
            break;
            case 3:answer_3->setChecked(true);
            break;
            case 4:answer_4->setChecked(true);
            break;
            case 5:answer_5->setChecked(true);
            break;
        }

    if((fQuestions->at(num).no_answer==false))
    {
        number->setText(fQuestions->at(num).label);
        number->setEnabled(true);
    }
    else
    {
        number->setText("");
        number->setEnabled(false);
    }

    if(fPresent_Question>0)
        previous_button->setEnabled(true);
    else
        previous_button->setEnabled(false);

    if(fPresent_Question<fMax_Questions-1)
        next_button->setEnabled(true);
    else
        next_button->setEnabled(false);

    return;
}


QString MainWindow::arrangeString(QString disp)
{
    for(int i=0,j=0; i<disp.size(); ++i,++j)
        if(j==86)
        {
            if(disp.at(i)!=' ')
            {
                for(; i>0; --i)
                    if(disp.at(i)==' ')
                    {
                        disp.replace(i,1,'\n');
                        break;
                    }
            }
            else
                disp.replace(i,1,'\n');
            j=0;
        }
    return disp;
}


void MainWindow::next()
{
    ++fPresent_Question;
    displayQuestionAnswer(fPresent_Question);
    return;
}


void MainWindow::previous()
{
    --fPresent_Question;
     displayQuestionAnswer(fPresent_Question);
    return;
}


void MainWindow::submit()
{
    fTimer->stop();
    int r=QMessageBox::warning(this,tr("Submit?"),tr("Are you sure you want to submit your test?"),
                               QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
    if(r==QMessageBox::Yes)
        concludeTest();
    else
        fTimer->start(1000);
    return;
}


void MainWindow::concludeTest()
{
    if(fTime_Limit!=0)
    {
        fTimer->stop();
        delete fTimer;
    }
    fRunning->evaluateTest(*fAnswers,fPresent_Time);
    delete fQuestions;
    delete fRunning;
    fState=false;
    setupCentralWidget();
    return;
}


QuestionBank* MainWindow::selectTestType(unsigned id)
{
    QuestionBank* choice=nullptr;

    switch(id)
    {
        case 1:choice=new Jamb2008English;
            break;
        case 2:choice=new Jamb2008Maths;
            break;
        case 3:choice=new Jamb2009English;
            break;
        case 4:choice=new Jamb2009Maths;
            break;
        default:choice=new QuestionBank;
            break;
    }

    return choice;
}
