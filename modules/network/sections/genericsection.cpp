#include "genericsection.h"

#include <QLineEdit>

using namespace NetworkManager;
using namespace dcc::widgets;

GenericSection::GenericSection(NetworkManager::ConnectionSettings::Ptr connSettings, QFrame *parent)
    : AbstractSection(tr("General"), parent),
      m_connIdItem(new LineEditWidget(this)),
      m_autoConnItem(new SwitchWidget(this)),
      m_connSettings(connSettings)
{
    initUI();
}

GenericSection::~GenericSection()
{
}

bool GenericSection::allInputValid()
{
    bool valid = true;

    valid = !m_connIdItem->textEdit()->text().isEmpty();

    m_connIdItem->setIsErr(!valid);

    return valid;
}

void GenericSection::saveSettings()
{
    m_connSettings->setId(m_connIdItem->text());
    m_connSettings->setAutoconnect(m_autoConnItem->checked());
}

void GenericSection::initUI()
{
    m_connIdItem->setTitle(tr("Name"));
    m_connIdItem->setText(m_connSettings->id());

    m_autoConnItem->setTitle(tr("Automatically connect"));
    m_autoConnItem->setChecked(m_connSettings->autoconnect());

    appendItem(m_connIdItem);
    appendItem(m_autoConnItem);

    connect(m_connIdItem->textEdit(), &QLineEdit::editingFinished, this, &GenericSection::allInputValid);
}
