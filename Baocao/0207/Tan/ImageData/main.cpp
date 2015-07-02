#include <QCoreApplication>
#include <QtSql>
#include <QString>
#include <QList>
#include <sstream>
#include <iostream>

using namespace std;
QSqlDatabase db;
class ImageObject
{
private:
    int id;
    QString link;
    QString letterId;
public:
    void SetId(int i)
    {
        id=i;
    }
    int GetId() const
    {
        return id;
    }
   void SetLink(QString s)
   {
       link=s;
   }
   QString GetLink() const
   {
       return link;
   }
   void SetLetterId(QString i)
   {
       letterId=i;
   }
   QString GetLetterId() const
   {
       return letterId;
   }
};
string itos(int i) // convert int to string
{
    stringstream s;
    s << i;
    return s.str();
}
void getImage(){
   db = QSqlDatabase::addDatabase("QSQLITE");
   db.setDatabaseName("/home/seven7up/ImageData.sqlite");

   if(!db.open()){
       cout<< "connect to database fail";
   }
   else{
       QList<ImageObject> myList;
       ImageObject object;
       QSqlQuery q(db);
       int r;
       q.exec("select * from (LinkImage join Alphabet) where (LinkImage.idLetter = Alphabet.id)");
       for (r = 0, q.first(); q.isValid(); q.next(),++r) {

           object.SetId(q.value(0).toInt());
           object.SetLink(q.value(1).toString());
           object.SetLetterId(q.value(4).toString());
           myList.append(object);
       }

       foreach (const ImageObject &o, myList) {
               cout<< itos(o.GetId())+ "---" + o.GetLink().toStdString() + "--- "+ o.GetLetterId().toStdString() << endl;
       }
   }
}
void getStudyImage(){
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/home/seven7up/ImageData.sqlite");

    if(!db.open()){
        cout<< "connect to database fail";
    }
    else{
        QList<ImageObject> myList;
        ImageObject object;
        QSqlQuery q(db);
        int r;
        q.exec("select * from (StudyImage join Alphabet) where (LinkImage.idLetter = Alphabet.id)");
        for (r = 0, q.first(); q.isValid(); q.next(),++r) {

            object.SetId(q.value(0).toInt());
            object.SetLink(q.value(1).toString());
            object.SetLetterId(q.value(4).toString());
            myList.append(object);
        }

        foreach (const ImageObject &o, myList) {
                cout<< itos(o.GetId())+ "---" + o.GetLink().toStdString() + "--- "+ o.GetLetterId().toStdString() << endl;
        }
    }
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    getImage();
    return a.exec();
}
