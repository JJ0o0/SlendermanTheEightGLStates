#include <playable/flashlight.hpp>

void Flashlight::Bind(int unit) const {
    m_cookie.Bind(unit);
}

void Flashlight::Unbind() const {
    m_cookie.Unbind();
}