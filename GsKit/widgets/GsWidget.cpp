#include "GsWidget.h"

unsigned int GsWidget::mNumWidgets = 0;

GsWidget::GsWidget(const GsRect<float> &rect) :
    mRect(rect)
{
    mId = mNumWidgets;
    mNumWidgets++;
}

GsWidget::GsWidget(const GsRect<float> &rect,
                   std::shared_ptr<GsWidget> &parent):
    GsWidget(rect)
{
    mParent = parent;
}


GsWidget::~GsWidget()
{}
