#include "syntax.h"
using namespace std;

Syntax::Syntax(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    QTextCharFormat multiLineCommentFormat;


    multiLineCommentFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}01.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}02.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkYellow);
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}03.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkBlue);
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}04.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}05.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);

	multiLineCommentFormat.setForeground(Qt::darkCyan);
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}06.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);

	multiLineCommentFormat.setForeground(Qt::lightGray);
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}07.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);

	multiLineCommentFormat.setForeground(Qt::darkGray);
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}08.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);

	multiLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}09.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);

	multiLineCommentFormat.setForeground(Qt::green);
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}10.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);

	multiLineCommentFormat.setForeground(Qt::yellow);
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}11.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);

	multiLineCommentFormat.setForeground(QColor(20,20,255));
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}12.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);

	multiLineCommentFormat.setForeground(Qt::magenta);
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}13.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);

	multiLineCommentFormat.setForeground(Qt::cyan);
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}14.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);

	multiLineCommentFormat.setForeground(Qt::white);
    rule.pattern = QRegExp("\\$[c,C][0-9]{2,2}15.*");
    rule.format = multiLineCommentFormat;
    highlightingRules.append(rule);


}

void Syntax::highlightBlock(const QString &text)
{
    try
    {
		setCurrentBlockState(0);
		QRegExp commentEndExpression("\\$[c,C][0-9]{4,4}");
        int formattedTo = 0;
        bool onlyOnce = false;
		for (size_t i = 0; i < highlightingRules.size(); i++)
		{
			HighlightingRule rule = highlightingRules.at(i);
			rule.pattern.setMinimal(true);
			int startIndex = 0;
			int prevState = previousBlockState();

			startIndex = text.indexOf(rule.pattern);

			if (startIndex == -1 && i + 1 != prevState) continue;
			if (!onlyOnce && startIndex == -1 && prevState > 0) {
                onlyOnce = true;
				int endIndex = text.indexOf(commentEndExpression, 0);
                if (formattedTo < 1) {
                    setFormat(0, endIndex > -1 ? endIndex : text.length(), highlightingRules.at(prevState - 1).format);
                    setCurrentBlockState(i + 1);
                }
			}
			else if (!onlyOnce && startIndex != -1 && prevState > 0) {
                onlyOnce = true;
                formattedTo = max(formattedTo, startIndex);
				setFormat(0, startIndex, highlightingRules.at(prevState - 1).format);
				setCurrentBlockState(i + 1);
			}

			while (startIndex >= 0) {
				int endIndex = text.indexOf(commentEndExpression, startIndex + 1);
				int commentLength;

				if (endIndex == -1) {
					setCurrentBlockState(i + 1);
					commentLength = text.length() - startIndex;
				}
				else {
					int currentState = 0;
					for (size_t x = 0; x < highlightingRules.size(); x++)
					{
						HighlightingRule rule = highlightingRules.at(x);
						rule.pattern.setMinimal(true);
						int nextRule = text.indexOf(rule.pattern, startIndex + 1);
						if (nextRule >= 0) {
							currentState = x + 1;
							break;
						}
					}
					commentLength = endIndex - startIndex + commentEndExpression.matchedLength() - 6;
					setCurrentBlockState(currentState);
				}
                formattedTo = max(formattedTo, startIndex + commentLength);
				setFormat(startIndex, commentLength, rule.format);
				startIndex = text.indexOf(rule.pattern, startIndex + commentLength);
			}
		}
    }
    catch (const std::exception&)
    {
    }
}
