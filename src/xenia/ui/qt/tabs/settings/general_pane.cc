#include "general_pane.h"

#include <QLabel>
#include <QVBoxLayout>

#include "xenia/ui/qt/widgets/checkbox.h"
#include "xenia/ui/qt/widgets/combobox.h"
#include "xenia/ui/qt/widgets/groupbox.h"
#include "xenia/ui/qt/widgets/scroll_area.h"

DECLARE_bool(show_debug_tab)

    namespace xe {
  namespace ui {
  namespace qt {

  const QStringList game_languages = {
      "English", "Japanese", "German",  "French",    "Spanish",
      "Italian", "Korean",   "Chinese", "Portugese", "Polish",
      "Russian", "Swedish",  "Turkish", "Norwegian", "Dutch"};

  void GeneralPane::Build() {
    QWidget* base_widget = new QWidget();
    base_widget->setSizePolicy(QSizePolicy::MinimumExpanding,
                               QSizePolicy::MinimumExpanding);

    // Setup scroll area for settings pane
    XScrollArea* scroll_area = new XScrollArea(this);
    scroll_area->setWidget(base_widget);
    scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_area->setWidgetResizable(true);

    QVBoxLayout* layout = new QVBoxLayout();
    base_widget->setLayout(layout);

    layout->setSpacing(16);
    layout->setContentsMargins(32, 16, 32, 16);

    // Add settings groupboxes to layout
    layout->addWidget(CreateGeneralGroupBox());
    layout->addWidget(CreateUpdateGroupBox());
    layout->addWidget(CreateWindowGroupBox());
    layout->addWidget(CreateLogGroupBox());
    layout->addStretch();

    set_widget(scroll_area);
  }

  XGroupBox* GeneralPane::CreateGeneralGroupBox() {
    XGroupBox* groupbox = new XGroupBox("General Settings");

    QVBoxLayout* groupbox_layout = new QVBoxLayout();
    groupbox_layout->setContentsMargins(16, 16, 16, 16);
    groupbox->setLayout(groupbox_layout);

    XCheckBox* discord_presence_checkbox = new XCheckBox();
    discord_presence_checkbox->setText("Discord Rich Presence");

    connect(discord_presence_checkbox, &XCheckBox::stateChanged,
            [&](bool value) {
              update_config_var(cvars::cv_show_debug_tab, value);
            });
    groupbox_layout->addWidget(discord_presence_checkbox);

    XCheckBox* game_icon_checkbox = new XCheckBox();
    game_icon_checkbox->setText("Show Game Icon in Taskbar");
    groupbox_layout->addWidget(game_icon_checkbox);

    QHBoxLayout* game_language_layout = new QHBoxLayout();
    game_language_layout->setContentsMargins(0, 0, 0, 0);
    game_language_layout->setSpacing(16);

    QLabel* game_language_label = new QLabel("Game Language");
    XComboBox* game_language_combobox = new XComboBox();
    game_language_combobox->addItems(game_languages);

    game_language_layout->addWidget(game_language_label);
    game_language_layout->addWidget(game_language_combobox);
    game_language_layout->addStretch();

    groupbox_layout->addLayout(game_language_layout);

    return groupbox;
  }

  XGroupBox* GeneralPane::CreateUpdateGroupBox() {
    return new XGroupBox("Update Settings");
  }

  XGroupBox* GeneralPane::CreateWindowGroupBox() {
    return new XGroupBox("Window Settings");
  }

  XGroupBox* GeneralPane::CreateLogGroupBox() {
    return new XGroupBox("Log Settings");
  }

}  // namespace qt
}  // namespace ui
}  // namespace xe