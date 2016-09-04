#include "highlighter.h"

//! [0]
Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(QBrush(QColor("#00733C")));
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

//! [2] text after key
    classFormat.setForeground(QBrush(QColor("#EE736C")));
    rule.pattern = QRegExp("\\b: [A--'Za-z0-9_-. \n,\r]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);
//! [2]

//! [3] single line comment
    singleLineCommentFormat.setForeground(Qt::gray);
    singleLineCommentFormat.setFontItalic(true);
//    singleLineCommentFormat.setFontPointSize(10);
    rule.pattern = QRegExp("#[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

//! [3]

//! [4] detect bracks after key []
    bracks.setForeground(QBrush(QColor("#EE736C")));
    rule.pattern = QRegExp("\\[([A--'Za-z0-9_-. \n,\r]+)\\]");
    rule.format = bracks;
    highlightingRules.append(rule);
//! [4]

//! [5] key
    quotationFormat.setForeground(QBrush(QColor("#00733C")));
    quotationFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp(".*:");
    rule.format = quotationFormat;
    highlightingRules.append(rule);
//! [5]

//! [6] text after key in new line
//! for example
//! stage-packages:
//! - libqt5core5a    <--this text
//! - libqt5gui5      <--this text
//! - libqt5webkit5   <--this text
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(QBrush(QColor(10,92,179)));
    rule.pattern = QRegExp("- .*");
    rule.format = functionFormat;
    highlightingRules.append(rule);
//! [6]

//! [7]

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

//! [9]process is repeated until the last occurrence of the pattern in the current text block is found
    setCurrentBlockState(0);
//! [9]


}

