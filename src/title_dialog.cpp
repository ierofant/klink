#include <gtkmm/stock.h>
#include "title_dialog.hpp"

TitleDialog::TitleDialog()
{
    set_title("link-point's title");

    add_button(Gtk::Stock::OK, GTK_RESPONSE_OK);
    add_button(Gtk::Stock::CANCEL, GTK_RESPONSE_CANCEL);

    auto *vbox = get_vbox();
    vbox->pack_start(entry, false, false);
    show_all_children();
}


Glib::ustring TitleDialog::get_title() const
{
    return entry.get_text();
}
