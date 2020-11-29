// This file is part of Desktop App Toolkit,
// a set of libraries for developing nice desktop applications.
//
// For license and copyright information please follow this link:
// https://github.com/desktop-app/legal/blob/master/LEGAL
//
#pragma once

#include "base/unique_qptr.h"
#include "ui/effects/animations.h"
#include "ui/widgets/call_button.h"

namespace Ui {

class BlobsWidget;

enum class CallMuteButtonType {
	Connecting,
	Active,
	Muted,
	ForceMuted,
};

struct CallMuteButtonState {
	QString text;
	CallMuteButtonType type = CallMuteButtonType::Connecting;
};

class CallMuteButton final {
public:
	explicit CallMuteButton(
		not_null<RpWidget*> parent,
		CallMuteButtonState initial = CallMuteButtonState());

	void setState(const CallMuteButtonState &state);
	void setLevel(float level);
	[[nodiscard]] rpl::producer<Qt::MouseButton> clicks() const;

	[[nodiscard]] QSize innerSize() const;
	[[nodiscard]] QRect innerGeometry() const;
	void moveInner(QPoint position);

	void setVisible(bool visible);
	void show() {
		setVisible(true);
	}
	void hide() {
		setVisible(false);
	}
	void raise();
	void lower();

	[[nodiscard]] rpl::lifetime &lifetime();

private:
	CallMuteButtonState _state;
	float _level = 0.;

	const base::unique_qptr<BlobsWidget> _blobs;
	CallButton _content;
	CallButton _connecting;

	Animations::Simple _switchAnimation;

};

} // namespace Ui
