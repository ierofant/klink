#include <iostream>
#include <sstream>
#include <glibmm/fileutils.h>
#include <gtkmm/box.h>
#include <gtkmm/stock.h>
#include <gtkmm/icontheme.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/table.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/filechooserdialog.h>
#include <klink_config.hpp>
#include "app.hpp"

Glib::RefPtr<App> App::application;

Glib::RefPtr<App> App::get()
{
    return application;
}

Glib::RefPtr<App> App::create(int _argc, char *_argv[])
{
    if(!application) application = Glib::RefPtr<App>(new App(_argc, _argv));
    return application;
}

Mode App::get_mode() const
{
    return mode;
}

SvgWidget& App::get_svg_widget()
{
    return svg_widget;
}

int App::run()
{
    return Gtk::Application::run(window);
}

Glib::SignalProxyProperty App::signal_change_mode()
{
    return mode.get_proxy().signal_changed();
}

App::App::App(int _argc, char *_argv[])
    : Glib::ObjectBase(typeid(*this)),
      Gtk::Application(_argc, _argv, "ru.nilksa.klink"),
      mode(*this, "mode", CURSOR),
      manager(Gtk::UIManager::create()),
      actions(Gtk::ActionGroup::create())
{
    try
    {
	auto theme = Gtk::IconTheme::get_default();
	theme->add_builtin_icon("klink-cursor", 32, Gdk::Pixbuf::create_from_file(resource_dir + "/images/cursor.svg"));
        theme->add_builtin_icon("klink-link", 32, Gdk::Pixbuf::create_from_file(resource_dir + "/images/link.svg"));
    }
    catch(Glib::FileError const &_e)
    {
	std::cerr << _e.what() << std::endl;
    }

    actions->add(Gtk::Action::create("FileMenu", "Файл"));
    actions->add(Gtk::Action::create("Open", "Открыть"), sigc::mem_fun(*this, &App::open_file));
    actions->add(Gtk::Action::create("Save", "Сохранить"), sigc::mem_fun(*this, &App::save));
    actions->add(Gtk::Action::create("SaveAs", "Сохранить как"), sigc::mem_fun(*this, &App::save_as));
    actions->add(Gtk::Action::create("Quit", "Выход"), sigc::mem_fun(*this, &App::quit));

    Gtk::RadioAction::Group tools_group;
    auto cursor_action = Gtk::RadioAction::create_with_icon_name(tools_group, "Cursor", "klink-cursor", "cursor", "Выделение и удаление объектов");
    auto link_action = Gtk::RadioAction::create_with_icon_name(tools_group, "Link", "klink-link", "link", "Рисование соединителей");
    actions->add(cursor_action, sigc::mem_fun(*this, &App::on_change_mode));
    actions->add(link_action);

    actions->add(Gtk::Action::create("GoToTop", Gtk::Stock::GOTO_TOP, "goto-top", "Переход к главной группе"), sigc::mem_fun(*this, &App::on_goto_top_action_activate));
    actions->add(Gtk::Action::create("GoUp", Gtk::Stock::GO_UP, "go-up", "Переход на группу вверх"), sigc::mem_fun(*this, &App::on_go_up_action_activate));
    actions->add(Gtk::Action::create("GoDown", Gtk::Stock::GO_DOWN, "go-down", "Переход на группу вниз"), sigc::mem_fun(*this, &App::on_go_down_action_activate));
    actions->get_action("Save")->set_sensitive(false);

    manager->insert_action_group(actions);
    manager->add_ui_from_string(ui_info);

    auto *menubar = manager->get_widget("/MenuBar");
    auto *toolsbar = manager->get_widget("/ToolsBar");
    toolsbar->set_property("toolbar-style", Gtk::TOOLBAR_ICONS);
    toolsbar->set_property("orientation", Gtk::ORIENTATION_VERTICAL);

    auto *navibar = manager->get_widget("/NavigateBar");
    navibar->set_property("toolbar-style", Gtk::TOOLBAR_ICONS);
    navibar->set_property("orientation", Gtk::ORIENTATION_VERTICAL);
    
    auto *table = Gtk::manage(new Gtk::Table(2, 3));
    auto *swindow = Gtk::manage(new Gtk::ScrolledWindow());
    swindow->add(svg_widget);
    
    auto *hbox = Gtk::manage(new Gtk::Box());
    hbox->pack_start(current_statusbar, false, false, 10);
    hbox->pack_start(root_statusbar, false, false);
    
    table->attach(*toolsbar, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL | Gtk::EXPAND);
    table->attach(*swindow, 1, 2, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND);
    table->attach(*navibar, 2, 3, 0, 1, Gtk::FILL, Gtk::FILL | Gtk::EXPAND);
    table->attach(*hbox, 0, 3, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::FILL);

    auto *vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    vbox->pack_start(*menubar, false, false);
    vbox->pack_start(*table);

    svg_widget.signal_change_current_element().connect(sigc::mem_fun(*this, &App::on_change_current_element));
    svg_widget.signal_change_root_group().connect(sigc::mem_fun(*this, &App::on_change_root_group));
    signal_change_mode().connect(sigc::mem_fun(svg_widget, &SvgWidget::on_change_mode));

    window.set_title("klink");
    window.set_position(Gtk::WIN_POS_CENTER);
    window.set_default_size(800, 600);
    window.signal_key_press_event().connect(sigc::mem_fun(*this, &App::on_key_press_event));
    window.add(*vbox);
    window.show_all_children();
}

void App::open_file()
{
    Gtk::FileChooserDialog fc_dialog(window, "Выберите файл");
    fc_dialog.add_button(Gtk::Stock::CANCEL, GTK_RESPONSE_CANCEL);
    fc_dialog.add_button(Gtk::Stock::OPEN, GTK_RESPONSE_OK);
    if(fc_dialog.run() == GTK_RESPONSE_OK)
    {
	filename = fc_dialog.get_filename();
	svg_widget.set_source_file(filename);
	actions->get_action("Save")->set_sensitive(true);
    }
}

void App::save()
{
    if(!filename.empty())
    {
	auto *doc = svg_widget.get_document();
	if(doc) doc->write_to_file_formatted(filename);
    }
}

void App::save_as()
{
    Gtk::FileChooserDialog fc_dialog(window, "Выберите файл", Gtk::FILE_CHOOSER_ACTION_SAVE);
    fc_dialog.add_button(Gtk::Stock::CANCEL, GTK_RESPONSE_CANCEL);
    fc_dialog.add_button(Gtk::Stock::SAVE, GTK_RESPONSE_OK);
    if(fc_dialog.run() == GTK_RESPONSE_OK)
    {
	auto *doc = svg_widget.get_document();
	if(doc) doc->write_to_file_formatted(fc_dialog.get_filename());
    }
}

void App::on_change_current_element()
{
    auto go_down_action = actions->get_action("GoDown");
    auto *current_element = svg_widget.get_current_element();
    Glib::ustring id;
    if(current_element)
    {
	go_down_action->set_sensitive(!current_element->get_children().empty());
	id = current_element->get_attribute_value("id");
    }
    else
    {
	go_down_action->set_sensitive(false);
	id = "none";
    }
    current_statusbar.push("current: " + id);
}

void App::on_change_root_group()
{
    auto go_up_action = actions->get_action("GoUp");
    auto *root_group = svg_widget.get_root_group();
    Glib::ustring id;
    if(root_group)
    {
	go_up_action->set_sensitive(root_group->get_parent());
	id = root_group->get_attribute_value("id");
    }
    else 
    {
	go_up_action->set_sensitive(false);
	id = "none";
    }
    root_statusbar.push("root: " + id);
}

void App::on_change_mode()
{
    auto cursor_action = Glib::RefPtr<Gtk::RadioAction>::cast_static(actions->get_action("Cursor"));
    auto link_action = Glib::RefPtr<Gtk::RadioAction>::cast_static(actions->get_action("Link"));
    if(cursor_action->get_active()) on_cursor_mode_activate();
    else if(link_action->get_active()) on_link_mode_activate();
}

void App::on_cursor_mode_activate()
{
    mode = CURSOR;
}

void App::on_link_mode_activate()
{
    mode = LINK;
}

void App::on_goto_top_action_activate()
{
    svg_widget.set_root_group(svg_widget.get_document()->get_root_node());
}

void App::on_go_up_action_activate()
{
    svg_widget.set_root_group(svg_widget.get_root_group()->get_parent());
}

void App::on_go_down_action_activate()
{
    svg_widget.set_root_group(svg_widget.get_current_element());
    svg_widget.set_current_element(nullptr);
}

bool App::on_key_press_event(GdkEventKey *_event)
{
    svg_widget.event((GdkEvent*) _event);
    return false;
}
