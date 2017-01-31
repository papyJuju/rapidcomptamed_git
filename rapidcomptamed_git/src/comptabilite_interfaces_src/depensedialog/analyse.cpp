#include "analyse.h"

AnalyseMovements::AnalyseMovements(QWidget * parent)
{
    setupUi(this);
}

AnalyseMovements::~AnalyseMovements(){}

void AnalyseMovements::getSums(const QString & textOfSums)
{
    m_text = textOfSums;
    textEdit->setHtml(m_text);
}

