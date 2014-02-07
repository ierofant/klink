#ifndef KLINK_APP_INCLUDED
#define KLINK_APP_INCLUDED

#include <glibmm/propertyproxy.h>
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/radioaction.h>
#include "svg_widget.hpp"

const char constexpr *ui_info = 
    u8"<ui>"
    u8"	<menubar name='MenuBar'>"
    u8"		<menu action='FileMenu'>"
    u8"			<menuitem action='Open'/>"
    u8"			<menuitem action='SaveAs'/>"
    u8"			<menuitem action='Quit'/>"
    u8"		</menu>"
    u8"	</menubar>"
    u8"	<toolbar name='ToolsBar'>"
    u8"		<toolitem action='Cursor'/>"
    u8"		<!--toolitem action='LinkPoint'/-->"
    u8"		<toolitem action='Link'/>"
    u8"	</toolbar>"
    u8"	<toolbar name='NavigateBar'>"
    u8"		<toolitem action='GoToTop'/>"
    u8"		<toolitem action='GoUp'/>"
    u8"		<toolitem action='GoDown'/>"
    u8"	</toolbar>"
    u8"</ui>";

enum Mode
{
    CURSOR,
    LINK_POINT,
    LINK
};

class App : public Gtk::Application
{
    public:
	virtual ~App() = default;

    public:
	static Glib::RefPtr<App> get();
	static Glib::RefPtr<App> create(int _argc, char *_argv[]);

    public:
	Mode get_mode() const;
	SvgWidget& get_svg_widget();
	int run();
	Glib::SignalProxyProperty signal_change_mode();

    protected:
	App(int _argc, char *_argv[]);

    private:
	void open_file();
	void save_as();
	void on_change_current_element();
	void on_change_root_group();
	void on_change_mode();
	void on_cursor_mode_activate();
	void on_link_point_mode_activate();
	void on_link_mode_activate();
	void on_goto_top_action_activate();
	void on_go_up_action_activate();
	void on_go_down_action_activate();
	bool on_key_press_event(GdkEventKey *_event);

    private:
	static Glib::RefPtr<App> application;
	Glib::Property<Mode> mode;
	Gtk::Window window;
	Gtk::Statusbar current_statusbar, root_statusbar;
	Glib::RefPtr<Gtk::UIManager> manager;
	Glib::RefPtr<Gtk::ActionGroup> actions;
	Glib::RefPtr<Gtk::RadioAction> cursor_action, link_point_action, link_action;
	SvgWidget svg_widget;
};

#endif //KLINK_APP_INCLUDED
