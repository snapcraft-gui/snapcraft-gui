#include "highlighter.h"

//! [0]
Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkMagenta);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bname\\b" << "\\bsummary\\b" << "\\bconfinement\\b"
                    << "\\bdescription\\b" << "\\bcommand\\b" << "\\bstage-packages\\b"
                    << "\\bafter\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
//! [0]
    }

//! [1]
//! rules
//! [1]

//! [2]
    classFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("\\b: [A--'Za-z0-9_-. \n,\r]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);
//! [2]

//! [3]
    singleLineCommentFormat.setForeground(Qt::green);
    singleLineCommentFormat.setFontItalic(true);
    rule.pattern = QRegExp("#[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

//! [3]

//! [4]
    bracks.setForeground(Qt::red);
    rule.pattern = QRegExp("\\[([A--'Za-z0-9_-. \n,\r]+)\\]");
    rule.format = bracks;
    highlightingRules.append(rule);
//! [4]

//! [5]
    quotationFormat.setForeground(Qt::darkMagenta);
    quotationFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp(".*:");
    rule.format = quotationFormat;
    highlightingRules.append(rule);
//! [5]

//! [6]
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("- .*");
    rule.format = functionFormat;
    highlightingRules.append(rule);
//! [6]

//! [7]
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}
//! [7]

//! [8]
void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
//! [8]

//! [9]
    setCurrentBlockState(0);
//! [9]


}

