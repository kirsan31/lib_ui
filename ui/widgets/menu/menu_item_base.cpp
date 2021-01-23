// This file is part of Desktop App Toolkit,
// a set of libraries for developing nice desktop applications.
//
// For license and copyright information please follow this link:
// https://github.com/desktop-app/legal/blob/master/LEGAL
//
#include "ui/widgets/menu/menu_item_base.h"

namespace Ui::Menu {

ItemBase::ItemBase(
	not_null<RpWidget*> parent,
	const style::Menu &st)
: RippleButton(parent, st.ripple) {
	init();
}

void ItemBase::setSelected(
		bool selected,
		TriggeredSource source) {
	if (!isEnabled()) {
		return;
	}
	if (_selected.current() != selected) {
		setMouseTracking(!selected);
		_lastTriggeredSource = source;
		_selected = selected;
		update();
	}
}

bool ItemBase::isSelected() const {
	return _selected.current();
}

rpl::producer<CallbackData> ItemBase::selects() const {
	return _selected.changes(
	) | rpl::map([=](bool selected) -> CallbackData {
		return { action(), y(), _lastTriggeredSource, _index, selected };
	});
}

TriggeredSource ItemBase::lastTriggeredSource() const {
	return _lastTriggeredSource;
}

int ItemBase::index() const {
	return _index;
}

void ItemBase::setIndex(int index) {
	_index = index;
}

void ItemBase::setClicked(TriggeredSource source) {
	if (isEnabled()) {
		_lastTriggeredSource = source;
		_clicks.fire({});
	}
}

rpl::producer<CallbackData> ItemBase::clicks() const {
	return rpl::merge(
		AbstractButton::clicks() | rpl::to_empty,
		_clicks.events()
	) | rpl::filter([=] {
		return isEnabled();
	}) | rpl::map([=]() -> CallbackData {
		return { action(), y(), _lastTriggeredSource, _index, true };
	});
}

rpl::producer<int> ItemBase::minWidthValue() const {
	return _minWidth.value();
}

int ItemBase::minWidth() const {
	return _minWidth.current();
}

void ItemBase::init() {
	events(
	) | rpl::filter([=](not_null<QEvent*> e) {
		return isEnabled()
			&& isSelected()
			&& (e->type() == QEvent::MouseButtonRelease);
	}) | rpl::to_empty | rpl::start_with_next([=] {
		const auto point = mapFromGlobal(QCursor::pos());
		if (!rect().contains(point)) {
			setSelected(false);
		}
	}, lifetime());
}

void ItemBase::initResizeHook(rpl::producer<QSize> &&size) {
	std::move(
		size
	) | rpl::start_with_next([=](QSize s) {
		resize(s.width(), contentHeight());
	}, lifetime());
}

void ItemBase::setMinWidth(int w) {
	_minWidth = w;
}

void ItemBase::finishAnimating() {
	RippleButton::finishAnimating();
}

void ItemBase::enableMouseSelecting() {
	events(
	) | rpl::filter([=](not_null<QEvent*> e) {
		return action()->isEnabled()
			&& ((e->type() == QEvent::Leave)
				|| (e->type() == QEvent::Enter)
				|| (e->type() == QEvent::MouseMove));
	}) | rpl::map([=](not_null<QEvent*> e) {
		return (e->type() != QEvent::Leave);
	}) | rpl::start_with_next([=](bool selected) {
		setSelected(selected);
	}, lifetime());
}

} // namespace Ui::Menu
