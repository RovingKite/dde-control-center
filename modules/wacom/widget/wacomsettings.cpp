#include "wacomsettings.h"

WacomSettings::WacomSettings(QWidget *parent):
    TranslucentFrame(parent)
{
    m_mainGroup = new SettingsGroup;
    m_mainLayout = new QVBoxLayout;
    m_pressureSlider = new TitledSliderItem(tr("Pressure Sensitive"));
    m_pressureSlider->slider()->setType(DCCSlider::Vernier);
    m_pressureSlider->slider()->setTickPosition(QSlider::TicksBelow);
    m_pressureSlider->slider()->setRange(0, 6);
    m_pressureSlider->slider()->setTickInterval(1);

    m_mainGroup->appendItem(m_pressureSlider);
    m_mainLayout->addWidget(m_mainGroup);
    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);
    setObjectName("WacomSettings");
    m_preSlider = m_pressureSlider->slider();
    connect(m_preSlider, &QSlider::valueChanged, this, &WacomSettings::requestSetPressureValue);
}

void WacomSettings::setModel(WacomModelBase *const baseSettings)
{
    m_baseSettings = baseSettings;

    connect(m_baseSettings, &WacomModelBase::pressureValueChanged, this, &WacomSettings::setPressureValue);
    setPressureValue(m_baseSettings->getPressureValue());
}

void WacomSettings::setPressureValue(const int &value)
{
    m_preSlider->setValue(value);
}