#include "luna_window.h"
#include "window_subsystem.h"

namespace luna
{

LWindow::WindowHandle window_id = 0;

LWindow::LWindow()
{
	m_id = window_id++;
}

}