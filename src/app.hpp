#ifndef KLINK_APP_INCLUDED
#define KLINK_APP_INCLUDED

#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/radioaction.h>
#include <geser/svg_widget.hpp>

const char constexpr *ui_info = 
    u8"<ui>"
    u8"	<menubar name='MenuBar'>"
    u8"		<menu action='FileMenu'>"
    u8"			<menuitem action='Open'/>"
    u8"			<menuitem action='Quit'/>"
    u8"		</menu>"
    u8"	</menubar>"
    u8"	<toolbar name='ToolsBar'>"
    u8"		<toolitem action='Cursor'/>"
    u8"		<toolitem action='LinkPoint'/>"
    u8"	</toolbar>"
    u8"</ui>";

class App : public Gtk::Application
{
    public:
	virtual ~App() = default;

    public:
	static Glib::RefPtr<App> create(int _argc, char *_argv[]);

    protected:
	App(int _argc, char *_argv[]);

    private:
	void open_file();


    public:
	Gtk::Window window;
	Glib::RefPtr<Gtk::UIManager> manager;
	Glib::RefPtr<Gtk::ActionGroup> actions;
	Glib::RefPtr<Gtk::RadioAction> cursor_action, link_point_action;
	geser::SvgWidget svg_widget;
};

#endif //KLINK_APP_INCLUDED
