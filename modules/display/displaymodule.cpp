#include "displaymodule.h"
#include "contentwidget.h"
#include "displaywidget.h"

using namespace dcc;
using namespace dcc::display;

DisplayModule::DisplayModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),

      m_displayModel(nullptr),
      m_displayWorker(nullptr),
      m_displayWidget(nullptr)
{

}

DisplayModule::~DisplayModule()
{
    m_displayModel->deleteLater();
    m_displayWorker->deleteLater();
}

void DisplayModule::initialize()
{
    m_displayModel = new DisplayModel;
    m_displayWorker = new DisplayWorker(m_displayModel);

    m_displayWorker->moveToThread(qApp->thread());
    m_displayModel->moveToThread(qApp->thread());
}

void DisplayModule::reset()
{

}

void DisplayModule::moduleActive()
{

}

void DisplayModule::moduleDeactive()
{

}

void DisplayModule::contentPopped(ContentWidget * const w)
{
    w->deleteLater();
}

const QString DisplayModule::name() const
{
    return QStringLiteral("display");
}

ModuleWidget *DisplayModule::moduleWidget()
{
    if (m_displayWidget)
        return m_displayWidget;

    m_displayWidget = new DisplayWidget;
    connect(m_displayWidget, &DisplayWidget::requestRotate, m_displayWorker, &DisplayWorker::rotate);

    return m_displayWidget;
}