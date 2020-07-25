/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2018 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QtDebug>
#include "theme_configuration.h"

namespace xe {
namespace ui {
namespace qt {

bool ThemeConfiguration::LoadFromFile() { return LoadFromFile(file_); }

bool ThemeConfiguration::LoadFromFile(const QString& filename) {
  // check file exists
  if (filename.isNull() || filename.isEmpty()) {
    return false;
  }

  // open file for parsing

  QFile file;
  file.setFileName(filename);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }

  auto text = file.readAll();

  QJsonDocument json_doc = QJsonDocument::fromJson(text);
  QJsonObject json_obj = json_doc.object();

  QJsonValue name_obj = json_obj.value("name");
  QJsonValue desc_obj = json_obj.value("description");

  // check correct data types from json file
  if (name_obj.isString() && desc_obj.isString()) {
    name_ = name_obj.toString();
    description_ = desc_obj.toString();
  } else {
    return false;
  }

  QJsonValue colors_obj = json_obj.value("colors");

  // load in custom colors, these are optional so do not fail if not found
  if (colors_obj.isUndefined() && !colors_obj.isArray()) {
    return true;
  }

  QJsonArray colors_array = colors_obj.toArray();
  for (QJsonValue color_value : colors_array) {
    // check color value is correct data type
    if (!color_value.isUndefined() && color_value.isObject()) {
      QJsonObject color_obj = color_value.toObject();

      // retrieve name and value for color
      const auto& color_name = color_obj.value("name").toString();
      const auto& color_value = color_obj.value("value").toString();

      // store color in cache
      colors_.emplace_back(color_name, color_value);
    }
  }

  // Sort color macros by length to avoid similarly named colors partially
  // overriding eachother
  qSort(colors_.begin(), colors_.end(),
        [](const ThemeColor& a, const ThemeColor& b) {
          return a.first.length() > b.first.length();
        });

  return true;
}

bool ThemeConfiguration::SaveToFile() { return SaveToFile(file_); }

bool ThemeConfiguration::SaveToFile(const QString& filename) {
  if (filename.isNull() || filename.isEmpty()) {
    return false;
  }

  QJsonObject root;
  root["name"] = name_;
  root["description"] = description_;

  // check if this theme has any custom colors
  if (colors_.size() > 0) {
    QJsonArray colors_array;
    // loop over each custom color and append it to the JSON document
    for (const auto& color : colors_) {
      QJsonObject color_obj;
      color_obj["name"] = color.first;
      color_obj["value"] = color.second.name();

      colors_array.append(color_obj);
    }
    root["colors"] = colors_array;
  }

  // save JSON document to disk

  QJsonDocument document(root);

  QFile file;
  file.setFileName(filename);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }

  if (file.write(document.toJson()) < 0) {
    return false;
  }

  file.flush();
  file.close();

  return true;
}

QColor ThemeConfiguration::ColorForKey(const QString& key) const {
  for (const auto& color : colors_) {
    if (color.first == key) {
      return color.second;
    }
  }
  return QColor();
}

void ThemeConfiguration::add_color(const QString& name, const QColor& color) {
  colors_.emplace_back(name, color);
}

void ThemeConfiguration::add_color(const QString& name, const QString& hex) {
  return add_color(name, QColor(hex));
}

void ThemeConfiguration::add_color(const QString& name, int r, int g, int b) {
  return add_color(name, QColor(r, g, b));
}

void ThemeConfiguration::add_color(const QString& name, int r, int g, int b,
                                   int a) {
  return add_color(name, QColor(r, g, b, a));
}

}  // namespace qt
}  // namespace ui
}  // namespace xe
