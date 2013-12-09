#include <gtkmm/box.h>
#include <gtkmm/stock.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/table.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/filechooserdialog.h>
#include "app.hpp"

Glib::RefPtr<App> App::create(int _argc, char *_argv[])
{
    return Glib::RefPtr<App>(new App(_argc, _argv));
}

App::App::App(int _argc, char *_argv[])
    : Gtk::Application(_argc, _argv, "ru.nilksa.klink"),
      manager(Gtk::UIManager::create()),
      actions(Gtk::ActionGroup::create())
{
    actions->add(Gtk::Action::create("FileMenu", "Файл"));
    actions->add(Gtk::Action::create("Open", "Открыть"), sigc::mem_fun(*this, &App::open_file));
    actions->add(Gtk::Action::create("Quit", "Выход"), sigc::mem_fun(*this, &App::quit));

    Gtk::RadioAction::Group tools_group;
    cursor_action = Gtk::RadioAction::create(tools_group, "Cursor", "C");
    link_point_action = Gtk::RadioAction::create(tools_group, "LinkPoint", "P");
    actions->add(cursor_action);
    actions->add(link_point_action);

    manager->insert_action_group(actions);
    manager->add_ui_from_string(ui_info);

    auto *menubar = manager->get_widget("/MenuBar");
    auto *toolsbar = manager->get_widget("/ToolsBar");
    toolsbar->set_property("toolbar-style", Gtk::TOOLBAR_TEXT);
    toolsbar->set_property("orientation", Gtk::ORIENTATION_VERTICAL);

    auto *table = Gtk::manage(new Gtk::Table(1, 2));
    auto *swindow = Gtk::manage(new Gtk::ScrolledWindow());
    swindow->add(svg_widget);
    
    table->attach(*toolsbar, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL | Gtk::EXPAND);
    table->attach(*swindow, 1, 2, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND);

    auto *vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    vbox->pack_start(*menubar, false, false);
    vbox->pack_start(*table);

    window.set_title("klink");
    window.set_position(Gtk::WIN_POS_CENTER);
    window.set_default_size(800, 600);
    window.add(*vbox);
    window.show_all();
    add_window(window);
}

void App::open_file()
{
    Gtk::FileChooserDialog fc_dialog(window, "Выберите файл");
    fc_dialog.add_button(Gtk::Stock::OK, GTK_RESPONSE_OK);
    if(fc_dialog.run() == GTK_RESPONSE_OK) svg_widget.set_source_file(fc_dialog.get_filename());
}
