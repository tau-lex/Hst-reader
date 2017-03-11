/*****************************************************************************/
/*                                                                           */
/*   HST-Reader                                                              */
/*   https://github.com/terentjew-alexey/Hst-reader                          */
/*                                                                           */
/*   H S T R E A D E R   C L A S S   H E A D E R                             */
/*                                                                           */
/*   Aleksey Terentew                                                        */
/*   terentew.aleksey@ya.ru                                                  */
/*                                                                           */
/*****************************************************************************/

#ifndef HSTREADER_H
#define HSTREADER_H

#include <QObject>
#include "include/imt4reader.h"

class HstReader : public IMt4Reader
{
public:
    HstReader(QString fName);
    bool readFile();
};

#endif // HSTREADER_H
