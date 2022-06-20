// This file is part of MARTY.
//
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

#include "edge.h"
#include "curvature.h"
#include "diagramrenderer.h"
#include "diagramwidget.h"
#include "latexLink.h"
#include "node.h"
#include <QColorDialog>
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QStyleOptionGraphicsItem>
#include <cmath>
#include <iostream>

Edge::Edge(Diagram *diag)
    : graph(diag),
      first(nullptr),
      second(nullptr),
      flippedLabel(false),
      color(Qt::black),
      curvature(0),
      lineWidth(3),
      amplitude(1),
      particleType(static_cast<int>(drawer::ParticleType::Scalar)),
      selected(false)
{
    setZValue(Node::zValue - 1);
    setTransformOriginPoint(0, qreal(amplitude) / 2);
}

Edge::Edge(Diagram *diag, Node *t_first, Node *t_second) : Edge(diag)
{
    setSides(t_first, t_second);
}

Edge::~Edge()
{
}

QString Edge::getName() const
{
    return (label) ? label->data : "";
}

double Edge::getCurve() const
{
    return curvature;
}

void Edge::setCurve(double t_curve)
{
    setCurvature(t_curve);
}

const Node *Edge::getFirst() const
{
    return first;
}
const Node *Edge::getSecond() const
{
    return second;
}
Node *Edge::getFirst()
{
    return first;
}
Node *Edge::getSecond()
{
    return second;
}

bool Edge::isConnectedTo(const Node *node) const
{
    return first == node or second == node;
}

bool Edge::hasFocusInGraph() const
{
    return selected;
}

void Edge::focusInGraph()
{
    selected = !selected;
    update();
}

void Edge::unFocusInGraph()
{
    selected = false;
    update();
}

qreal Edge::length() const
{
    auto effNodes = Node::getEffectiveNodes(first, second);
    return std::sqrt(std::pow(effNodes.first.x() - effNodes.second.x(), 2)
                     + std::pow(effNodes.first.y() - effNodes.second.y(), 2));
}

qreal Edge::getCurvature() const
{
    return curvature / length();
}

void Edge::setCurvature(double t_curvature)
{
    double max  = 2;
    bool   less = std::fabs(2 * t_curvature) < std::fabs(curvature);
    curvature   = 2 * t_curvature;
    if (curvature > max)
        curvature = max;
    else if (curvature < -max)
        curvature = -max;
    if (less && std::fabs(curvature) < 0.1)
        curvature = 0;
    else if (std::fabs(curvature) < 0.01)
        curvature = 0;
    graph->renderer->modificationDone();
    update();
}

void Edge::setSides(Node *t_first, Node *t_second)
{
    if (first) {
        disconnect(first, SIGNAL(xChanged()), this, SLOT(nodeMoved()));
        disconnect(first, SIGNAL(yChanged()), this, SLOT(nodeMoved()));
    }
    if (second) {
        disconnect(second, SIGNAL(xChanged()), this, SLOT(nodeMoved()));
        disconnect(second, SIGNAL(yChanged()), this, SLOT(nodeMoved()));
    }
    first  = t_first;
    second = t_second;
    connect(first, SIGNAL(xChanged()), this, SLOT(nodeMoved()));
    connect(first, SIGNAL(yChanged()), this, SLOT(nodeMoved()));
    connect(second, SIGNAL(xChanged()), this, SLOT(nodeMoved()));
    connect(second, SIGNAL(yChanged()), this, SLOT(nodeMoved()));
    update();
}

void Edge::setParticleType(qint32 t_type)
{
    using namespace drawer;
    particleType = t_type;
    switch (std::abs(particleType)) {
    case static_cast<int>(ParticleType::Scalar):
    case static_cast<int>(ParticleType::Fermion):
    case static_cast<int>(ParticleType::Majorana):
    case static_cast<int>(ParticleType::ChargedScalar):
        amplitude = 1;
        break;
    case static_cast<int>(ParticleType::Vector):
    case static_cast<int>(ParticleType::Gluon):
    case static_cast<int>(ParticleType::Gaugino):
    case static_cast<int>(ParticleType::Gluino):
        amplitude = 5;
        break;
    case static_cast<int>(drawer::ParticleType::Ghost):
        amplitude = 7;
        break;
    }
    graph->renderer->modificationDone();
}

void Edge::setName(QString const &name)
{
    drawer::LatexLinker::Edge lEdge;
    lEdge.i       = graph->getPosNode(first);
    lEdge.j       = graph->getPosNode(second);
    lEdge.flipped = flippedLabel;
    lEdge.curve   = getCurve() / 2;
    graph->addEdgeLabel(this, lEdge, name);
    graph->renderer->modificationDone();
}

void Edge::setLineWidth(qint32 t_lineWidth)
{
    lineWidth = std::max(1, t_lineWidth);
    graph->renderer->modificationDone();
    update();
}

void Edge::release()
{
    selected = false;
    update();
}

void Edge::nodeMoved()
{
    if (first and second) {
        auto res = Node::getEffectiveNodes(first, second);
        setPos(res.first.x(), res.first.y());
        qreal dx    = second->x() - first->x();
        qreal dy    = second->y() - first->y();
        qreal angle = std::atan2(dy, dx);
        setRotation(angle * 180 / M_PI);
        if (label and label->data != "") {
            setName(label->data);
        }
        update();
    }
}

void Edge::changeEdgeType(QAction *action)
{
    using namespace drawer;
    QString text = action->text();
    if (text == "Scalar") {
        graph->setEdgeType(this, ParticleType::Scalar);
        setParticleType(qint32(ParticleType::Scalar));
    }
    else if (text == "Charged Scalar") {
        graph->setEdgeType(this, ParticleType::ChargedScalar);
        setParticleType(qint32(ParticleType::ChargedScalar));
    }
    else if (text == "Anti-Charged Scalar") {
        graph->setEdgeType(this, ParticleType::ChargedScalar, false);
        setParticleType(-qint32(ParticleType::ChargedScalar));
    }
    else if (text == "Ghost") {
        graph->setEdgeType(this, ParticleType::Ghost);
        setParticleType(qint32(ParticleType::Ghost));
    }
    else if (text == "Anti-Ghost") {
        graph->setEdgeType(this, ParticleType::Ghost, false);
        setParticleType(-qint32(ParticleType::Ghost));
    }
    else if (text == "Fermion") {
        graph->setEdgeType(this, ParticleType::Fermion);
        setParticleType(qint32(ParticleType::Fermion));
    }
    else if (text == "Majorana") {
        graph->setEdgeType(this, ParticleType::Majorana);
        setParticleType(qint32(ParticleType::Majorana));
    }
    else if (text == "Anti-Fermion") {
        graph->setEdgeType(this, ParticleType::Fermion, false);
        setParticleType(-qint32(ParticleType::Fermion));
    }
    else if (text == "Vector") {
        graph->setEdgeType(this, ParticleType::Vector);
        setParticleType(qint32(ParticleType::Vector));
    }
    else if (text == "Gaugino") {
        graph->setEdgeType(this, ParticleType::Gaugino);
        setParticleType(qint32(ParticleType::Gaugino));
    }
    else if (text == "Anti-Gaugino") {
        graph->setEdgeType(this, ParticleType::Gaugino);
        setParticleType(-qint32(ParticleType::Gaugino));
    }
    else if (text == "Gluon") {
        graph->setEdgeType(this, ParticleType::Gluon);
        setParticleType(qint32(ParticleType::Gluon));
    }
    else if (text == "Gluino") {
        graph->setEdgeType(this, ParticleType::Gluino);
        setParticleType(qint32(ParticleType::Gluino));
    }
    else if (text == "Anti-Gluino") {
        graph->setEdgeType(this, ParticleType::Gluino);
        setParticleType(-qint32(ParticleType::Gluino));
    }
    else if (text == "Set label") {
        changeName();
    }
    else if (text == "Flip label") {
        flipLabel();
    }
    else if (text == "Change color") {
        changeColor();
    }
    else if (text == "Open editor") {
        release();
        graph->renderer->openEdgeForm(this);
    }
    graph->renderer->modificationDone();
    update();
}

void Edge::changeName()
{
    bool    accepted;
    QString name   = (label) ? label->data : "";
    QString t_name = QInputDialog::getText(widget,
                                           "Change edge name",
                                           "Name (latex): ",
                                           QLineEdit::Normal,
                                           name,
                                           &accepted);
    if (accepted) {
        setName(t_name);
        graph->renderer->modificationDone();
    }
}

void Edge::changeColor()
{
    QColor c = QColorDialog::getColor(Qt::black, widget, "Pick a color");
    if (c.isValid()) {
        color = c;
        update();
        graph->renderer->modificationDone();
    }
}

QRectF Edge::boundingRect() const
{
    qreal  c = getCurvature();
    double curvatureHeight
        = (c == 0.) ? 0
                    : (1 - std::sqrt(1 - c * c * length() * length() / 4))
                          / std::abs(c);
    qreal offset = 30;
    if (c > 0)
        return QRectF(-amplitude - offset,
                      -curvatureHeight - amplitude - offset,
                      qint32(length()) + 2 * amplitude + 2 * offset,
                      curvatureHeight + 2 * amplitude + 2 * offset);
    return QRectF(-amplitude - offset,
                  -amplitude - offset,
                  qint32(length()) + 2 * amplitude + 2 * offset,
                  curvatureHeight + 2 * amplitude + 2 * offset);
}

QPainterPath Edge::shape() const
{
    qreal        c = getCurvature();
    QPainterPath path(QPointF(0, -amplitude - 10));
    double       L = curvature::getTotalLength(c, length());
    for (int i = 0; i != 50; ++i) {
        path.lineTo(QPointF(i * L / 50, amplitude + 10));
    }
    path.lineTo(QPointF(L, -amplitude - 10));
    for (int i = 0; i != 50; ++i) {
        path.lineTo(QPointF(L - i * L / 50, -amplitude - 10));
    }
    path.closeSubpath();
    curvature::applyCurvature(path, c, length());
    return path;
}

void Edge::paint(QPainter *painter,
                 QStyleOptionGraphicsItem const *,
                 QWidget *)
{
    if (not first or not second)
        return;
    switch (std::abs(particleType)) {

    case static_cast<int>(drawer::ParticleType::Scalar):
        paintDashedLine(painter);
        break;
    case static_cast<int>(drawer::ParticleType::ChargedScalar):
        paintDashedLine(painter);
        paintArrow(painter);
        break;
    case static_cast<int>(drawer::ParticleType::Ghost):
        paintDotLine(painter);
        paintArrow(painter);
        break;
    case static_cast<int>(drawer::ParticleType::Vector):
        paintSinusoid(painter);
        break;
    case static_cast<int>(drawer::ParticleType::Gaugino):
        paintSinusoid(painter);
        paintLine(painter);
        paintArrow(painter);
        break;
    case static_cast<int>(drawer::ParticleType::Fermion):
        paintLine(painter);
        paintArrow(painter);
        break;
    case static_cast<int>(drawer::ParticleType::Majorana):
        paintLine(painter);
        break;
    case static_cast<int>(drawer::ParticleType::Gluon):
        paintCycloid(painter);
        break;
    case static_cast<int>(drawer::ParticleType::Gluino):
        paintCycloid(painter);
        paintLine(painter);
        paintArrow(painter);
        break;
    }
}

void Edge::keyPressEvent(QKeyEvent *event)
{
    if (selected and event->key() == Qt::Key_F)
        flipLabel();
}

QColor Edge::getColor() const
{
    return color;
}

QColor Edge::getPaintColor() const
{
    return (selected) ? Qt::red : color;
}

void Edge::paintLine(QPainter *painter) const
{
    QPen pen(getPaintColor());
    pen.setWidth(lineWidth);
    if (curvature == 0.) {
        painter->setPen(pen);
        painter->drawLine(
            lineWidth / 2, 0, qint32(length() - lineWidth / 2), 0);
    }
    else {
        int          N = 100;
        qreal        c = getCurvature();
        QPainterPath path(QPointF(length(), 0));
        qreal        L         = length();
        qreal        theta_min = std::acos(c * L / 2);
        qreal        theta_max = M_PI - theta_min;
        qreal        theta     = theta_min;
        for (int i = 0; i != N; ++i) {
            QPointF posCurve(std::cos(theta) / c, -std::sin(theta) / c);
            QPointF newPoint = posCurve;
            newPoint += QPointF(L / 2, std::sqrt(1 - c * c * L * L / 4) / c);
            path.lineTo(newPoint);
            theta += (theta_max - theta_min) / N;
        }
        painter->setPen(pen);
        painter->drawPath(path);
    }
}

void Edge::paintDashedLine(QPainter *painter) const
{
    QPen pen(getPaintColor());
    pen.setWidth(lineWidth);
    pen.setStyle(Qt::PenStyle::DashLine);
    painter->setPen(pen);
    if (curvature == 0.) {
        painter->drawLine(
            lineWidth / 2, 0, qint32(length() - lineWidth / 2), 0);
    }
    else {
        int          N    = 100;
        qreal        c    = std::abs(getCurvature());
        int          sign = (curvature < 0) ? -1 : 1;
        QPainterPath path(QPointF(length(), 0));
        qreal        L         = length();
        qreal        theta_min = std::acos(c * L / 2);
        qreal        theta_max = M_PI - theta_min;
        qreal        theta     = theta_min;
        for (int i = 0; i != N; ++i) {
            QPointF posCurve(std::cos(theta) / c, -sign * std::sin(theta) / c);
            QPointF newPoint = posCurve;
            newPoint
                += QPointF(L / 2, sign * std::sqrt(1 - c * c * L * L / 4) / c);
            path.lineTo(newPoint);
            theta += (theta_max - theta_min) / N;
        }
        painter->setPen(pen);
        painter->drawPath(path);
    }
    pen.setStyle(Qt::PenStyle::SolidLine);
    painter->setPen(pen);
}

void Edge::paintDotLine(QPainter *painter) const
{
    qreal w    = 2 * M_PI / 20;
    qreal ds   = 2 * M_PI / w;
    qreal c    = std::abs(getCurvature());
    int   sign = (curvature < 0) ? -1 : 1;
    if (curvature == 0.)
        sign = 0;
    qreal L         = length();
    qreal S         = curvature::getTotalLength(c, L);
    w               = std::round(w * S) / S;
    qreal theta_min = std::acos(c * L / 2);
    qreal theta_max = M_PI - theta_min;
    qreal theta     = theta_min;
    int   N         = static_cast<int>(S / ds);
    painter->setBrush(Qt::SolidPattern);
    qreal  dtheta = (theta_max - theta_min) / N;
    qint32 size   = amplitude + lineWidth - 3;
    for (qreal s = ds; s < S; s += ds) {
        if (curvature != 0.) {
            QPointF posCurve(std::cos(theta) / c, -sign * std::sin(theta) / c);
            QPointF newPoint = posCurve;
            newPoint
                += QPointF(L / 2, sign * std::sqrt(1 - c * c * L * L / 4) / c);
            painter->drawEllipse(int(newPoint.x()) - size / 2,
                                 int(newPoint.y()) - size / 2,
                                 size,
                                 size);
        }
        else {
            painter->drawEllipse(int(s) - size / 2, -size / 2, size, size);
        }
        theta += dtheta;
    }
    painter->setBrush(Qt::NoBrush);
}

void Edge::paintSinusoid(QPainter *painter) const
{
    QPen pen(getPaintColor());
    pen.setWidth(lineWidth);
    int          N = 500;
    qreal        w = 0.35;
    QPainterPath path;
    qreal        s      = 0;
    qreal        offset = (curvature > 0) ? M_PI : 0;
    qreal        S      = curvature::getTotalLength(getCurvature(), length());
    qint32 k = static_cast<qint32>(std::round((w * S - M_PI) / (2 * M_PI)));
    w        = (2 * M_PI * k + M_PI) / S;
    for (int i = 0; i != N; ++i) {
        QPointF newPoint(s, amplitude * std::sin(w * s + offset));
        path.lineTo(newPoint);
        s += S / N;
    }
    curvature::applyCurvature(path, getCurvature(), length());
    painter->strokePath(path, pen);
}

void Edge::paintCycloid(QPainter *painter) const
{
    QPen pen(getPaintColor());
    pen.setWidth(lineWidth);
    QPainterPath path;

    qreal L     = curvature::getTotalLength(getCurvature(), length());
    qreal Delta = 6;
    qreal delta = 3;
    qreal A     = amplitude;
    qreal w     = 1;
    qreal alpha = 2 * w * (Delta - delta) / (2 + w);
    qreal beta  = 1.5 * (Delta / 2 - alpha * M_PI / (2 * w));
    qreal x0    = beta - alpha * M_PI / (2 * w);
    qreal t0    = M_PI / 2;
    beta        = 30 * beta;
    qreal t     = t0;
    qreal dt    = 0.05;
    while (true) {
        qreal x = x0 + alpha * t - beta * std::sin(w * t);
        if (x > L)
            break;
        QPointF newPoint(x0 + alpha * t - beta * std::sin(w * t),
                         -A * std::cos(w * t));
        path.lineTo(newPoint);
        t += dt;
    }
    if (path.currentPosition().x() < length()) {
        path.lineTo(QPointF(length(), 0));
    }
    curvature::applyCurvature(path, getCurvature(), length());
    painter->strokePath(path, pen);
}

void Edge::paintArrow(QPainter *painter) const
{
    const qreal  sizeArrow  = Diagram::scaleForGraph / 5. * (lineWidth / 3.);
    const qreal  ratioArrow = 1 / std::sqrt(2);
    QPainterPath path;
    bool         inverted = (particleType < 0);
    qreal        c        = getCurvature();
    qreal        L        = length();
    qreal        offsetY
        = (c == 0.) ? 0 : -(1 - std::sqrt(1 - c * c * L * L / 4)) / c;
    const qreal offset = (inverted) ? sizeArrow * 1 / std::sqrt(2)
                                    : sizeArrow * (1 - 1 / std::sqrt(2));
    const qreal posMin = L / 2 - offset;
    const qreal posMax = posMin + sizeArrow;
    if (inverted) {
        path.moveTo(posMax, offsetY + ratioArrow * sizeArrow / 2);
        path.lineTo(QPointF(posMin, offsetY));
        path.lineTo(QPointF(posMax, offsetY - ratioArrow * sizeArrow / 2));
    }
    else {
        path.moveTo(posMin, offsetY + ratioArrow * sizeArrow / 2);
        path.lineTo(QPointF(posMax, offsetY));
        path.lineTo(QPointF(posMin, offsetY - ratioArrow * sizeArrow / 2));
    }
    path.closeSubpath();
    painter->setPen(QPen(getPaintColor(), 1));
    painter->setBrush(QBrush(getPaintColor(), Qt::SolidPattern));
    painter->drawPath(path);
}

void Edge::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (graph->getInteractiveMode() != InsertionMode) {
        bool alreadyFocused = false;
        for (const auto &e : graph->edges)
            if (e->hasFocusInGraph()) {
                alreadyFocused = true;
                break;
            }
        if (graph->getInteractiveMode() == SelectionMode or !alreadyFocused)
            selected = true;
    }
    if (event->button() == Qt::RightButton) {
        QMenu *menu  = new QMenu(widget);
        QMenu *setPT = menu->addMenu("Set particle type");
        setPT->addAction("Scalar");
        setPT->addAction("Charged Scalar");
        setPT->addAction("Anti-Charged Scalar");
        setPT->addAction("Ghost");
        setPT->addAction("Anti-Ghost");
        setPT->addAction("Fermion");
        setPT->addAction("Anti-Fermion");
        setPT->addAction("Majorana");
        setPT->addAction("Vector");
        setPT->addAction("Gaugino");
        setPT->addAction("Anti-Gaugino");
        setPT->addAction("Gluon");
        setPT->addAction("Gluino");
        setPT->addAction("Anti-Gluino");
        menu->addAction("Set label");
        menu->addAction("Flip label");
        menu->addAction("Change color");
        menu->addAction("Open editor");
        connect(menu,
                SIGNAL(triggered(QAction *)),
                this,
                SLOT(changeEdgeType(QAction *)));
        menu->exec(QCursor::pos());
    }
    QGraphicsItem::mousePressEvent(event);
    if (graph->renderer->hasNodeForm() or graph->renderer->hasEdgeForm()) {
        graph->renderer->openEdgeForm(this);
    }
    update();
}

void Edge::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    update();
}

void Edge::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * /*event*/)
{
    release();
    graph->renderer->openEdgeForm(this);
}

void Edge::wheelEventCustom(int delta)
{
    if (hasFocusInGraph()) {
        double step = 0.002 * 1 / (1 + std::pow(std::fabs(curvature), 2));
        setCurvature((curvature + delta * step) / 2);
        if (label and label->data != "")
            setName(label->data);
        scene()->update();
    }
}
