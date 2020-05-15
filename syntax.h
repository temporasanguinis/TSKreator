#ifndef TS_SYNTAX_H
#define TS_SYNTAX_H
#pragma warning(push, 0)
#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>
#pragma warning(pop)

class QTextDocument;

class Syntax : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Syntax(QTextDocument *parent = 0);
    Syntax& operator=(const Syntax& a) {
        highlightingRules = a.highlightingRules;
    }

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

};

#endif
