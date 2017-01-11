#include "questionbank.h"
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QBoxLayout>
#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QFile>


QuestionBank::QuestionBank(int fid,QString name):file_matcher(0xFF357AC),fID(fid),fMax_Question(0),
    fMy_Name(new QString(name)),fQuestions(new QVector<QuestionAnswer>),fAnswers(new QVector<unsigned>),
    fWorking_On(nullptr),fPos(0){}


QuestionBank::~QuestionBank()
{
    delete fQuestions;
    delete fAnswers;
    delete fMy_Name;
}


bool QuestionBank::load(Format& ld,QString filename)
{
    QFile openfile(filename);
    if(!openfile.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0,tr("Open test"),tr("The specified file could not be opened"));
        return false;
    }

    QDataStream in(&openfile);
    in.setVersion(QDataStream::Qt_5_4);
    quint64 temp_filematcher;
    in>>temp_filematcher;
    if(temp_filematcher!=file_matcher)
    {
        QMessageBox::critical(0,tr("Error"),tr("The selected file is not a test file"));
        return false;
    }
    quint32 fid;
    quint32 present_question;
    quint32 present_time;
    quint32 time_limit;
    QVector<unsigned>* temp_data=new QVector<unsigned>;

    in>>fid>>present_question>>present_time>>time_limit>>*temp_data;
    ld.fid=fid;
    ld.present_question=present_question;
    ld.present_time=present_time;
    ld.time_limit=time_limit;
    ld.temp_answers=temp_data;
    if(in.status()!=QDataStream::Ok)
        QMessageBox::warning(0,tr("Load Error"),tr("Error ecountered while trying to read data from file"));
    openfile.close();
    return true;
}


bool QuestionBank::save(Format& sv,QString filename)
{
    QFile savefile(filename);
    if(!savefile.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(0,tr("Save test"),tr("The specified file could not be opened"));
        return false;
    }

    QDataStream out(&savefile);
    out.setVersion(QDataStream::Qt_5_4);
    out<<(quint64)file_matcher;
    out<<(quint32)sv.fid<<(quint32)sv.present_question<<(quint32)sv.present_time<<(quint32)sv.time_limit<<*sv.temp_answers;
    if(out.status()!=QDataStream::Ok)
        QMessageBox::warning(0,tr("Save Error"),tr("Error ecountered while trying to write data to file"));
    savefile.close();
    return true;
}


void QuestionBank::setupQuestions(){}


void QuestionBank::setupAnswers(){}


void QuestionBank::evaluateTest(QVector<unsigned> answers,int total_time)
{
    fPassed=new QVector<pair>;
    fFailed=new QVector<pair>;
    fUnanswered=new QVector<pair>;

    for(int i=0,j=0; i<answers.size(); ++i)
    {
        if(fQuestions->at(i).no_answer==false)
        {
            if(answers.at(i)==fAnswers->at(j))
            {
                pair temp_pair;
                temp_pair.question=i;
                temp_pair.answer=fAnswers->at(j);
                fPassed->push_back(temp_pair);
            }
            else if(answers.at(i)==0)
            {
                pair temp_pair;
                temp_pair.question=i;
                temp_pair.answer=fAnswers->at(j);
                fUnanswered->push_back(temp_pair);
            }
            else
            {
                pair temp_pair;
                temp_pair.question=i;
                temp_pair.answer=fAnswers->at(j);
                fFailed->push_back(temp_pair);
            }
            ++j;
        }
    }

    showResult(total_time);
    return;
}


void QuestionBank::showResult(int time)
{
    fResult_Dialog=new QDialog;
    fResult_Dialog->setWindowIcon(QIcon(":/images/ICONS/result.ico"));
    fResult_Dialog->setWindowTitle(tr("Result"));
    fResult_Dialog->setMinimumHeight(500);

    fMore_Widget=new QWidget(fResult_Dialog);
    QDialogButtonBox* button_box=new QDialogButtonBox(Qt::Horizontal,fResult_Dialog);

    fPassed_Button=button_box->addButton(tr("Passed"),QDialogButtonBox::ResetRole);
    fPassed_Button->setCheckable(true);
    connect(fPassed_Button,SIGNAL(toggled(bool)),this,SLOT(passed(bool)));

    fFailed_Button=button_box->addButton(tr("Failed"),QDialogButtonBox::ResetRole);
    fFailed_Button->setCheckable(true);
    connect(fFailed_Button,SIGNAL(toggled(bool)),this,SLOT(failed(bool)));

    fUnanswered_Button=button_box->addButton(tr("Unanswered"),QDialogButtonBox::ResetRole);
    fUnanswered_Button->setCheckable(true);
    connect(fUnanswered_Button,SIGNAL(toggled(bool)),this,SLOT(unanswered(bool)));

    QPushButton* finish_button=button_box->addButton(tr("Finish"),QDialogButtonBox::ApplyRole);
    finish_button->setDefault(true);
    connect(finish_button,SIGNAL(clicked()),this,SLOT(finish()));

    fPrevious_Button=new QPushButton(fMore_Widget);
    fPrevious_Button->setIcon(QIcon(":/images/ICONS/previous.ico"));
    connect(fPrevious_Button,SIGNAL(clicked()),this,SLOT(Left()));

    fNext_Button=new QPushButton(fMore_Widget);
    fNext_Button->setIcon(QIcon(":/images/ICONS/next.ico"));
    connect(fNext_Button,SIGNAL(clicked()),this,SLOT(Right()));

    QVBoxLayout* button_layout=new QVBoxLayout;
    button_layout->addWidget(fPrevious_Button);
    button_layout->addWidget(fNext_Button);

    fWidget_Display0=new QLabel(fMore_Widget);

    fWidget_Display1=new QLabel(fMore_Widget);
    fWidget_Display1->setMinimumHeight(350);
    fWidget_Display1->setTextFormat(Qt::RichText);
    fWidget_Display1->setScaledContents(true);
    fWidget_Display1->setWordWrap(true);
    fWidget_Display1->setMaximumSize(350,350);

    fWidget_Display2=new QLabel(fMore_Widget);
    fWidget_Display2->setWordWrap(true);
    fWidget_Display1->setMinimumSize(600,500);

    fWidget_Display1_ScrollArea=new QScrollArea(fMore_Widget);
    fWidget_Display1_ScrollArea->setWidget(fWidget_Display1);

    QVBoxLayout* widget_display_layout=new QVBoxLayout;
    widget_display_layout->addWidget(fWidget_Display0,1);
    widget_display_layout->addWidget(fWidget_Display1_ScrollArea,5);    //!
    widget_display_layout->addWidget(fWidget_Display2,1);   //!


    QHBoxLayout* widget_layout=new QHBoxLayout;
    widget_layout->addLayout(widget_display_layout,5);      //!
    widget_layout->addLayout(button_layout,1);              //!
    fMore_Widget->setLayout(widget_layout);
    fMore_Widget->hide();

    int mins=time/60;
    int secs=time%60;
    QLabel* dialog_display=new QLabel(fResult_Dialog);
    dialog_display->setText("Test Name:                                                 "+*fMy_Name+
                            "\n\nTotal Number of Questions:                        "+QString::number(fAnswers->size())+
                            "\n\nNumber of Questions Answered:                "+QString::number(fPassed->size()+fFailed->size())+
                            "\n\nNumber of Questions Unanswered:            "+QString::number(fUnanswered->size())+
                            "\n\nNumber of Questions Passed:                     "+QString::number(fPassed->size())+
                            "\n\nNumber of Questions Failed:                       "+QString::number(fFailed->size())+
                            "\n\nTotal time taken:                                          "+QString::number(mins)+" min(s) "+QString::number(secs)+" sec(s)"+
                            "\n");
    QVBoxLayout* result_layout=new QVBoxLayout;
    result_layout->addWidget(dialog_display);
    result_layout->addWidget(button_box);
    result_layout->addWidget(fMore_Widget);
    fResult_Dialog->setLayout(result_layout);
    fResult_Dialog->setFixedWidth(550);

    fResult_Dialog->exec();
    if(fWorking_On)
        delete fWorking_On;

    delete fPassed;
    delete fFailed;
    delete fUnanswered;

    return;
}


void QuestionBank::passed(bool on)
{
    if(on==true)
    {
        fFailed_Button->setChecked(false);
        fUnanswered_Button->setChecked(false);
        if(fWorking_On)
           delete fWorking_On;
        fWorking_On=new QVector<pair>(*fPassed);
        fPos=0;
        if(fWorking_On->size()==0)
        {
            fMore_Widget->hide();
            fResult_Dialog->adjustSize();
        }
        else
        {
            displayResult();
            fMore_Widget->show();
        }
    }
    else
    {
        if(fWorking_On)
        {
            delete fWorking_On;
            fWorking_On=nullptr;
        }
        fMore_Widget->hide();
        fResult_Dialog->adjustSize();
    }
    return;
}


void QuestionBank::failed(bool on)
{
    if(on==true)
    {
        fPassed_Button->setChecked(false);
        fUnanswered_Button->setChecked(false);
        if(fWorking_On)
           delete fWorking_On;
        fWorking_On=new QVector<pair>(*fFailed);
        fPos=0;
        if(fWorking_On->size()==0)
        {
            fMore_Widget->hide();
            fResult_Dialog->adjustSize();
        }
        else
        {
            displayResult();
            fMore_Widget->show();
        }

    }
    else
    {
        if(fWorking_On)
        {
            delete fWorking_On;
            fWorking_On=nullptr;
        }
        fMore_Widget->hide();
        fResult_Dialog->adjustSize();
    }
    return;
}


void QuestionBank::unanswered(bool on)
{
    if(on==true)
    {
        fPassed_Button->setChecked(false);
        fFailed_Button->setChecked(false);
        if(fWorking_On)
           delete fWorking_On;
        fWorking_On=new QVector<pair>(*fUnanswered);
        fPos=0;
        if(fWorking_On->size()==0)
        {
            fMore_Widget->hide();
            fResult_Dialog->adjustSize();
        }
        else
        {
            displayResult();
            fMore_Widget->show();
        }
    }
    else
    {
        if(fWorking_On)
        {
            delete fWorking_On;
            fWorking_On=nullptr;
        }
        fMore_Widget->hide();
        fResult_Dialog->adjustSize();
    }
    return;
}


void QuestionBank::finish()
{
    fResult_Dialog->close();
    return;
}


void QuestionBank::Left()
{
    --fPos;
    displayResult();
    return;
}


void QuestionBank::Right()
{
    ++fPos;
    displayResult();
    return;
}


void QuestionBank::displayResult()
{
    pair temp;
    temp.answer=fWorking_On->at(fPos).answer;
    temp.question=fWorking_On->at(fPos).question;

    fWidget_Display0->setText("Question Number:     "+fQuestions->at(temp.question).label+"\t(Scroll down to see question if not visible at first)");
    if(fQuestions->at(temp.question).question.size()==0)
        fWidget_Display1->setPixmap(fQuestions->at(temp.question).pixmap);
    else
        fWidget_Display1->setText(fQuestions->at(temp.question).question);

    switch(temp.answer)
    {
        case 1:fWidget_Display2->setText("Answer:   "+fQuestions->at(temp.question).a);
            break;
        case 2:fWidget_Display2->setText("Answer:   "+fQuestions->at(temp.question).b);
            break;
        case 3:fWidget_Display2->setText("Answer:   "+fQuestions->at(temp.question).c);
            break;
        case 4:fWidget_Display2->setText("Answer:   "+fQuestions->at(temp.question).d);
            break;
        case 5:fWidget_Display2->setText("Answer:   "+fQuestions->at(temp.question).e);
            break;
    }

    if(fPos>0)
        fPrevious_Button->setEnabled(true);
    else
        fPrevious_Button->setEnabled(false);

    if(fPos<fWorking_On->size()-1)
        fNext_Button->setEnabled(true);
    else
        fNext_Button->setEnabled(false);
    return;
}
