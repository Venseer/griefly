#include "qtopengl.h"

#include "../Screen.h"
#include "representation/Representation.h"

#include <QMouseEvent>
#include <QKeyEvent>

QtOpenGL::QtOpenGL(QWidget *parent)
    : QGLWidget(parent),
      representation_(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
    SetGLContext(this);
}

QtOpenGL::~QtOpenGL()
{
    // Nothing
}

void QtOpenGL::SetRepresentation(Representation* representation)
{
    representation_ = representation;
}

void QtOpenGL::handlePassedKey(QKeyEvent* event)
{
    keyPressEvent(event);
}

void QtOpenGL::mousePressEvent(QMouseEvent* event)
{
    if ((event->button() == Qt::LeftButton) && IsRepresentationValid())
    {
        GetRepresentation().Click(event->x(), event->y());
    }
}

void QtOpenGL::keyPressEvent(QKeyEvent* event)
{
    if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return))
    {
        emit enterPressed();
        return;
    }
    if (event->key() == Qt::Key_F2)
    {
        emit f2Pressed();
        emit enterPressed();
        return;
    }
    if (IsRepresentationValid())
    {
        GetRepresentation().HandleKeyboardDown(event);
    }
}

void QtOpenGL::keyReleaseEvent(QKeyEvent *event)
{
    if (IsRepresentationValid())
    {
        GetRepresentation().HandleKeyboardUp(event);
    }
}

void QtOpenGL::focusOutEvent(QFocusEvent *event)
{
    emit focusLost();
}

void QtOpenGL::initializeGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, AREA_SIZE_W, AREA_SIZE_H, 0, 0, 1); // TODO: check

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    glClearColor(0, 0, 0, 0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void QtOpenGL::paintGL()
{
    if (IsRepresentationValid())
    {
        GetRepresentation().Process();
    }
}

void QtOpenGL::resizeGL(int width, int height)
{
    if (IsScreenValid())
    {
        GetScreen().PerformSizeUpdate();
    }
}

bool QtOpenGL::IsRepresentationValid()
{
    return representation_ != nullptr;
}

Representation& QtOpenGL::GetRepresentation()
{
    return *representation_;
}

QtOpenGL* widget = nullptr;

QtOpenGL* GetGLWidget()
{
    return widget;
}

void MakeCurrentGLContext()
{
    if (widget)
    {
        widget->makeCurrent();
    }
}

void SetGLContext(QtOpenGL* new_widget)
{
    widget = new_widget;
}

