// This file is part of Desktop App Toolkit,
// a set of libraries for developing nice desktop applications.
//
// For license and copyright information please follow this link:
// https://github.com/desktop-app/legal/blob/master/LEGAL
//
#pragma once

#include "ui/rp_widget.h"
#include "ui/widgets/scroll_area.h"
#include "ui/text/text.h"
#include "ui/painter.h"

namespace style {
struct SideBarMenu;
} // namespace style

namespace Ui {

class RippleAnimation;

class SideBarMenu final {
public:
	struct Item {
		QString id;
		QString title;
		QString badge;
		not_null<const style::icon*> icon;
		not_null<const style::icon*> iconActive;
		int iconTop = 0;
	};

	SideBarMenu(not_null<QWidget*> parent, const style::SideBarMenu &st);
	~SideBarMenu();

	[[nodiscard]] not_null<const Ui::RpWidget*> widget() const;

	void setGeometry(QRect geometry);
	void setItems(std::vector<Item> items);
	void setActive(
		const QString &id,
		anim::type animated = anim::type::normal);
	[[nodiscard]] rpl::producer<QString> activateRequests() const;

	[[nodiscard]] rpl::lifetime &lifetime();

private:
	struct MenuItem {
		Item data;
		Ui::Text::String text;
		mutable std::unique_ptr<Ui::RippleAnimation> ripple;
		int top = 0;
		int height = 0;
	};
	void setup();
	void paint(Painter &p, QRect clip) const;
	[[nodiscard]] int countContentHeight(int width, int outerHeight);

	void mouseMove(QPoint position);
	void mousePress(Qt::MouseButton button);
	void mouseRelease(Qt::MouseButton button);

	void setSelected(int selected);
	void setPressed(int pressed);
	void addRipple(MenuItem &item, QPoint position);
	void repaint(const QString &id);
	[[nodiscard]] MenuItem *itemById(const QString &id);

	const style::SideBarMenu &_st;

	Ui::RpWidget _outer;
	const not_null<Ui::ScrollArea*> _scroll;
	const not_null<Ui::RpWidget*> _inner;
	std::vector<MenuItem> _items;
	int _selected = -1;
	int _pressed = -1;

	QString _activeId;
	rpl::event_stream<QString> _activateRequests;

};

} // namespace Ui
