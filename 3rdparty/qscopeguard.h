






































#ifndef QSCOPEGUARD_H
#define QSCOPEGUARD_H

#include <QtCore/qglobal.h>


QT_BEGIN_NAMESPACE


template <typename F> class QScopeGuard;
template <typename F> QScopeGuard<F> qScopeGuard(F f);

template <typename F>
class QScopeGuard
{
public:
    QScopeGuard(QScopeGuard &&other) Q_DECL_NOEXCEPT
        : m_func(std::move(other.m_func))
        , m_invoke(other.m_invoke)
    {
        other.dismiss();
    }

    ~QScopeGuard()
    {
        if (m_invoke)
            m_func();
    }

    void dismiss() Q_DECL_NOEXCEPT
    {
        m_invoke = false;
    }

private:
    explicit QScopeGuard(F f) Q_DECL_NOEXCEPT
        : m_func(std::move(f))
    {
    }

    Q_DISABLE_COPY(QScopeGuard)

    F m_func;
    bool m_invoke = true;
    friend QScopeGuard qScopeGuard<F>(F);
};


template <typename F>
QScopeGuard<F> qScopeGuard(F f)
{
    return QScopeGuard<F>(std::move(f));
}

QT_END_NAMESPACE

#endif
