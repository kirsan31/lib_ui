// This file is part of Desktop App Toolkit,
// a set of libraries for developing nice desktop applications.
//
// For license and copyright information please follow this link:
// https://github.com/desktop-app/legal/blob/master/LEGAL
//
#include "ui/effects/fade_animation.h"

#include "ui/ui_utility.h"
#include "ui/painter.h"

namespace Ui {
namespace {

constexpr int kWideScale = 5;

} // namespace

FadeAnimation::FadeAnimation(TWidget *widget, float64 scale)
: _widget(widget)
, _scale(scale) {
}

bool FadeAnimation::paint(QPainter &p) {
	if (_cache.isNull()) return false;

	const auto cache = _cache;
	auto opacity = _animation.value(_visible ? 1. : 0.);
	p.setOpacity(opacity);
	if (_scale < 1.) {
		PainterHighQualityEnabler hq(p);
		auto targetRect = QRect(
			(1 - kWideScale) / 2 * _size.width(),
			(1 - kWideScale) / 2 * _size.height(),
			kWideScale * _size.width(),
			kWideScale * _size.height());
		auto scale = opacity + (1. - opacity) * _scale;
		auto shownWidth = anim::interpolate(
			(1 - kWideScale) / 2 * _size.width(),
			0,
			scale);
		auto shownHeight = anim::interpolate(
			(1 - kWideScale) / 2 * _size.height(),
			0,
			scale);
		auto margins = QMargins(
			shownWidth,
			shownHeight,
			shownWidth,
			shownHeight);
		p.drawPixmap(targetRect.marginsAdded(margins), cache);
	} else {
		p.drawPixmap(0, 0, cache);
	}
	return true;
}

void FadeAnimation::refreshCache() {
	if (!_cache.isNull()) {
		_cache = QPixmap();
		_cache = grabContent();
		Assert(!_cache.isNull());
	}
}

QPixmap FadeAnimation::grabContent() {
	SendPendingMoveResizeEvents(_widget);
	_size = _widget->size();
	const auto pixelRatio = style::DevicePixelRatio();
	if (_size.isEmpty()) {
		auto image = QImage(
			pixelRatio,
			pixelRatio,
			QImage::Format_ARGB32_Premultiplied);
		image.fill(Qt::transparent);
		return PixmapFromImage(std::move(image));
	}
	auto widgetContent = GrabWidget(_widget);
	if (_scale < 1.) {
		auto result = QImage(kWideScale * _size * pixelRatio, QImage::Format_ARGB32_Premultiplied);
		result.setDevicePixelRatio(pixelRatio);
		result.fill(Qt::transparent);
		{
			Painter p(&result);
			p.drawPixmap((kWideScale - 1) / 2 * _size.width(), (kWideScale - 1) / 2 * _size.height(), widgetContent);
		}
		return PixmapFromImage(std::move(result));
	}
	return widgetContent;
}

void FadeAnimation::setFinishedCallback(FinishedCallback &&callback) {
	_finishedCallback = std::move(callback);
}

void FadeAnimation::setUpdatedCallback(UpdatedCallback &&callback) {
	_updatedCallback = std::move(callback);
}

void FadeAnimation::show() {
	_visible = true;
	stopAnimation();
}

void FadeAnimation::hide() {
	_visible = false;
	stopAnimation();
}

void FadeAnimation::stopAnimation() {
	_animation.stop();
	if (!_cache.isNull()) {
		_cache = QPixmap();
		if (_finishedCallback) {
			_finishedCallback();
		}
	}
	if (_visible == _widget->isHidden()) {
		_widget->setVisible(_visible);
	}
}

void FadeAnimation::fadeIn(int duration) {
	if (_visible) return;

	_visible = true;
	startAnimation(duration);
}

void FadeAnimation::fadeOut(int duration) {
	if (!_visible) return;

	_visible = false;
	startAnimation(duration);
}

void FadeAnimation::startAnimation(int duration) {
	if (_cache.isNull()) {
		_cache = grabContent();
		Assert(!_cache.isNull());
	}
	auto from = _visible ? 0. : 1.;
	auto to = _visible ? 1. : 0.;
	_animation.start([this]() { updateCallback(); }, from, to, duration);
	updateCallback();
	if (_widget->isHidden()) {
		_widget->show();
	}
}

void FadeAnimation::updateCallback() {
	_widget->update();
	if (_updatedCallback) {
		_updatedCallback(_animation.value(_visible ? 1. : 0.));
	}
	if (!_animation.animating()) {
		stopAnimation();
	}
}

} // namespace Ui
