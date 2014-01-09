#ifndef TITLE_DIALOG_HPP_INCLUDED
#define TITLE_DIALOG_HPP_INCLUDED

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>

class TitleDialog : public Gtk::Dialog
{
    public:
	TitleDialog();
	virtual ~TitleDialog() = default;

    public:
	Glib::ustring get_title() const;

    private:
	Gtk::Entry entry;
};

#endif //TITLE_DIALOG_HPP_INCLUDED
