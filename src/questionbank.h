#ifndef QUESTIONBANK_H
#define QUESTIONBANK_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QPixmap>

class QPushButton;
class QScrollArea;
class QDialog;
class QWidget;
class QLabel;

struct QuestionAnswer
{
    QuestionAnswer(){}

    QuestionAnswer(QString _question,QString _a,QString _b,QString _c,QString _d, QString _e, QString _label,
                   bool _no_answer=false):question(_question),a(_a),b(_b),c(_c),d(_d),e(_e),label(_label),no_answer(_no_answer){}

    QuestionAnswer(QPixmap _pixmap,QString _a,QString _b,QString _c,QString _d, QString _e, QString _label,
                   bool _no_answer=false):a(_a),b(_b),c(_c),d(_d),e(_e),label(_label),no_answer(_no_answer),pixmap(_pixmap){}

    QString question,a,b,c,d,e,label;
    bool no_answer=false;
    QPixmap pixmap;
};


struct Format
{
    unsigned fid,present_question,present_time,time_limit;
    QVector<unsigned>* temp_answers;
};


class QuestionBank : public QObject
{
    Q_OBJECT
public:
    explicit QuestionBank(int=0, QString="");
    virtual ~QuestionBank();

    bool load(Format&,QString);
    bool save(Format&,QString);

    inline unsigned getID() const;
    inline unsigned getMaxQuestion() const;
    inline QString* getName() const;
    inline QVector<QuestionAnswer>* getQuestions() const;
    void evaluateTest(QVector<unsigned>,int);

protected slots:
    void passed(bool);
    void failed(bool);
    void unanswered(bool);
    void finish();

    void Left();
    void Right();
    void displayResult();


protected:
    struct pair{unsigned question,answer;};
    virtual void setupQuestions();
    virtual void setupAnswers();
    void showResult(int);

    /*used to identify the right test save file to avoid errors during reading or writing to file if the
     *user changed the extension name of a non test file to the extension of a test file
     */
    quint64 file_matcher;

    //Basic members of a QuestionBank object
    unsigned fID;
    unsigned fMax_Question;
    QString* fMy_Name;

    //Questions and their respective answers
    QVector<QuestionAnswer>* fQuestions;
    QVector<unsigned>* fAnswers;

    //used to store the results evaluated from the test
    QVector<pair>* fPassed;
    QVector<pair>* fFailed;
    QVector<pair>* fUnanswered;

    //used to display results at the end of test sessions
    QDialog* fResult_Dialog;
    QWidget* fMore_Widget;
    QLabel* fWidget_Display0;
    QLabel* fWidget_Display1;
    QLabel* fWidget_Display2;
    QScrollArea* fWidget_Display1_ScrollArea;
    QPushButton* fNext_Button;
    QPushButton* fPrevious_Button;
    QPushButton* fPassed_Button;
    QPushButton* fFailed_Button;
    QPushButton* fUnanswered_Button;
    QVector<pair>* fWorking_On;
    int fPos;
};

inline unsigned QuestionBank::getMaxQuestion() const
{
    return fMax_Question;
}


inline QString* QuestionBank::getName() const
{
    return fMy_Name;
}


inline QVector<QuestionAnswer>* QuestionBank::getQuestions() const
{
    return fQuestions;
}


inline unsigned QuestionBank::getID() const
{
    return fID;
}

#endif // QUESTIONBANK_H
