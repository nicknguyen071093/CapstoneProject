#include "wordDTO.h"

WordDTO::WordDTO()
{
}

WordDTO::WordDTO( int i, QString le,  QString li)
{
    id= i;
    letter=le;
    link= li;
}
void WordDTO::SetId(int i)
{
    id=i;
}
int WordDTO::GetId() const
{
    return id;
}
void WordDTO::SetLink(QString s)
{
   link=s;
}
QString WordDTO::GetLink() const
{
   return link;
}
void WordDTO::SetLetter(QString i)
{
   letter=i;
}
QString WordDTO::GetLetter() const
{
   return letter;
}
