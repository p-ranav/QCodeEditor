// QCodeEditor
#include <QSyntaxStyle>

// Qt
#include <QDebug>
#include <QXmlStreamReader>
#include <QFile>

// C++
#include <string_view>

QSyntaxStyle::QSyntaxStyle(QObject* parent) :
    QObject(parent),
    m_name(),
    m_data(),
    m_loaded(false)
{

}

bool QSyntaxStyle::load(QString fl)
{
    QXmlStreamReader reader(fl);

    while (!reader.atEnd() && !reader.hasError())
    {
        auto token = reader.readNext();

        if(token == QXmlStreamReader::StartElement)
        {
            const QString style_schema_literal("style-schema");
            const QString style_literal("style");

            if (reader.name() == QStringView{style_schema_literal.data(), style_schema_literal.size()})
            {
                if (reader.attributes().hasAttribute("name"))
                {
                    m_name = reader.attributes().value("name").toString();
                }
            }
            else if (reader.name() == QStringView{style_literal.data(), style_literal.size()})
            {
                auto attributes = reader.attributes();

                auto name = attributes.value("name");

                QTextCharFormat format;

                if (attributes.hasAttribute("background"))
                {
                    format.setBackground(QColor(attributes.value("background").toString()));
                }

                if (attributes.hasAttribute("foreground"))
                {
                    format.setForeground(QColor(attributes.value("foreground").toString()));
                }

                const QString true_literal("true");

                if (attributes.hasAttribute("bold") &&
                    attributes.value("bold") == QStringView(true_literal.data(), true_literal.size()))
                {
                    format.setFontWeight(QFont::Weight::Bold);
                }

                if (attributes.hasAttribute("italic") &&
                    attributes.value("italic") == QStringView(true_literal.data(), true_literal.size()))
                {
                    format.setFontItalic(true);
                }

                if (attributes.hasAttribute("underlineStyle"))
                {
                    auto underline = attributes.value("underlineStyle");

                    auto s = QTextCharFormat::UnderlineStyle::NoUnderline;

                    const QString single_underline_literal("SingleUnderline");
                    const QString dash_underline_literal("DashUnderline");
                    const QString dot_line_literal("DotLine");
                    const QString dash_dot_line_literal("DashDotLine");
                    const QString dash_dot_dot_line_literal("DashDotDotLine");
                    const QString wave_underline_literal("WaveUnderline");
                    const QString spell_check_underline_literal("SpellCheckUnderline");

                    if (underline == QStringView{single_underline_literal.data(), single_underline_literal.size()})
                    {
                        s = QTextCharFormat::UnderlineStyle::SingleUnderline;
                    }
                    else if (underline == QStringView{dash_underline_literal.data(), dash_underline_literal.size()})
                    {
                        s = QTextCharFormat::UnderlineStyle::DashUnderline;
                    }
                    else if (underline == QStringView{dot_line_literal.data(), dot_line_literal.size()})
                    {
                        s = QTextCharFormat::UnderlineStyle::DotLine;
                    }
                    else if (underline == QStringView{dash_dot_line_literal.data(), dash_dot_line_literal.size()})
                    {
                        s = QTextCharFormat::DashDotLine;
                    }
                    else if (underline == QStringView{dash_dot_dot_line_literal.data(), dash_dot_dot_line_literal.size()})
                    {
                        s = QTextCharFormat::DashDotDotLine;
                    }
                    else if (underline == QStringView{wave_underline_literal.data(), wave_underline_literal.size()})
                    {
                        s = QTextCharFormat::WaveUnderline;
                    }
                    else if (underline == QStringView{spell_check_underline_literal.data(), spell_check_underline_literal.size()})
                    {
                        s = QTextCharFormat::SpellCheckUnderline;
                    }
                    else
                    {
                        qDebug() << "Unknown underline value " << underline;
                    }

                    format.setUnderlineStyle(s);
                }

                m_data[name.toString()] = format;
            }
        }
    }

    m_loaded = !reader.hasError();

    return m_loaded;
}

QString QSyntaxStyle::name() const
{
    return m_name;
}

QTextCharFormat QSyntaxStyle::getFormat(QString name) const
{
    auto result = m_data.find(name);

    if (result == m_data.end())
    {
        return QTextCharFormat();
    }

    return result.value();
}

bool QSyntaxStyle::isLoaded() const
{
    return m_loaded;
}

QSyntaxStyle* QSyntaxStyle::defaultStyle()
{
    static QSyntaxStyle style;

    if (!style.isLoaded())
    {
        Q_INIT_RESOURCE(qcodeeditor_resources);
        QFile fl(":/default_style.xml");

        if (!fl.open(QIODevice::ReadOnly))
        {
            return &style;
        }

        if (!style.load(fl.readAll()))
        {
            qDebug() << "Can't load default style.";
        }
    }

    return &style;
}
