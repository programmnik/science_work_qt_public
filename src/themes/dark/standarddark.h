#ifndef STANDARDDARK_H
#define STANDARDDARK_H

#include <QString>

class StandardDark {
public:
    static QString getGlobalStyle() {
        return R"(
            /* Глобальный фон для всех виджетов */
            QWidget {
                background-color: #292857;
                color: #e0e0e0;
                font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;
                font-size: 12px;
            }

            /* Для QMainWindow отдельно */
            QMainWindow {
                background-color: #1a1a2e;
            }

            /* Для QDockWidget */
            QDockWidget {
                background-color: #1a1a2e;
                color: #e0e0e0;
            }

            /* Для QMenuBar */
            QMenuBar {
                background-color: #2a2a4a;
                color: #e0e0e0;
            }
            QMenuBar::item:selected {
                background-color: #0d7377;
            }

            /* Для QMenu */
            QMenu {
                background-color: #2a2a4a;
                color: #e0e0e0;
                border: 1px solid #3a3a5a;
            }
            QMenu::item:selected {
                background-color: #0d7377;
            }

            /* Для QToolBar */
            QToolBar {
                background-color: #292857;
                border: none;
                color: #e0e0e0;
            }

            /* Для QStatusBar */
            QStatusBar {
                background-color: #1a1a2e;
                color: #e0e0e0;
            }

            /* Для QScrollBar */
            QScrollBar:vertical {
                background: #1a1a2e;
                width: 10px;
                margin: 0px;
            }
            QScrollBar::handle:vertical {
                background: #3a3a5a;
                border-radius: 5px;
                min-height: 20px;
            }
            QScrollBar::handle:vertical:hover {
                background: #4a4a6a;
            }
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                height: 0px;
            }
            QScrollBar:horizontal {
                background: #1a1a2e;
                height: 10px;
                margin: 0px;
            }
            QScrollBar::handle:horizontal {
                background: #3a3a5a;
                border-radius: 5px;
                min-width: 20px;
            }
            QScrollBar::handle:horizontal:hover {
                background: #4a4a6a;
            }
            QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
                width: 0px;
            }

            /* Для QLabel */
            QLabel {
                color: #e0e0e0;
            }

            /* Для QPushButton (базовый стиль) */
            QPushButton {
                background-color: #2a2a4a;
                border: 1px solid #3a3a5a;
                border-radius: 4px;
                color: #e0e0e0;
                padding: 5px 10px;
            }
            QPushButton:hover {
                background-color: #3a3a5a;
                border-color: #4a4a6a;
            }
            QPushButton:pressed {
                background-color: #1a1a3a;
            }
            QPushButton:disabled {
                background-color: #2a2a3a;
                color: #666666;
            }

            /* Для QLineEdit */
            QLineEdit {
                background-color: #2a2a4a;
                border: 1px solid #3a3a5a;
                border-radius: 4px;
                color: #e0e0e0;
                padding: 4px 8px;
                selection-background-color: #0d7377;
            }
            QLineEdit:focus {
                border-color: #0d7377;
            }
            QLineEdit::placeholder {
                color: #8888a0;
            }

            /* Для QTextEdit */
            QTextEdit {
                background-color: #1a1a2e;
                border: 1px solid #3a3a5a;
                border-radius: 4px;
                color: #e0e0e0;
            }

            /* Для QListWidget */
            QListWidget {
                background-color: #1a1a2e;
                border: 1px solid #3a3a5a;
                border-radius: 4px;
                color: #e0e0e0;
            }
            QListWidget::item {
                padding: 4px;
            }
            QListWidget::item:selected {
                background-color: #0d7377;
                color: white;
            }
            QListWidget::item:hover:!selected {
                background-color: #2a2a4a;
            }

            /* Для QTreeWidget */
            QTreeWidget {
                background-color: #1a1a2e;
                border: 1px solid #3a3a5a;
                color: #e0e0e0;
            }
            QTreeWidget::item:selected {
                background-color: #0d7377;
            }

            /* Для QTableWidget */
            QTableWidget {
                background-color: #1a1a2e;
                border: 1px solid #3a3a5a;
                color: #e0e0e0;
                gridline-color: #3a3a5a;
            }
            QTableWidget::item:selected {
                background-color: #0d7377;
            }
            QHeaderView::section {
                background-color: #2a2a4a;
                color: #e0e0e0;
                padding: 4px;
                border: 1px solid #3a3a5a;
            }

            /* Для QComboBox */
            QComboBox {
                background-color: #2a2a4a;
                border: 1px solid #3a3a5a;
                border-radius: 4px;
                color: #e0e0e0;
                padding: 4px 8px;
            }
            QComboBox:hover {
                border-color: #4a4a6a;
            }
            QComboBox::drop-down {
                border: none;
                background: transparent;
            }
            QComboBox::down-arrow {
                image: none;
                border-left: 5px solid transparent;
                border-right: 5px solid transparent;
                border-top: 5px solid #e0e0e0;
                margin-right: 4px;
            }
            QComboBox QAbstractItemView {
                background-color: #2a2a4a;
                color: #e0e0e0;
                border: 1px solid #3a3a5a;
                selection-background-color: #0d7377;
            }

            /* Для QSpinBox, QDoubleSpinBox */
            QSpinBox, QDoubleSpinBox {
                background-color: #2a2a4a;
                border: 1px solid #3a3a5a;
                border-radius: 4px;
                color: #e0e0e0;
                padding: 4px;
            }
            QSpinBox:focus, QDoubleSpinBox:focus {
                border-color: #0d7377;
            }

            /* Для QCheckBox */
            QCheckBox {
                color: #e0e0e0;
            }
            QCheckBox::indicator {
                width: 16px;
                height: 16px;
            }
            QCheckBox::indicator:unchecked {
                background-color: #2a2a4a;
                border: 1px solid #3a3a5a;
                border-radius: 3px;
            }
            QCheckBox::indicator:checked {
                background-color: #0d7377;
                border: 1px solid #0d7377;
                border-radius: 3px;
            }

            /* Для QRadioButton */
            QRadioButton {
                color: #e0e0e0;
            }
            QRadioButton::indicator {
                width: 16px;
                height: 16px;
            }
            QRadioButton::indicator:unchecked {
                background-color: #2a2a4a;
                border: 1px solid #3a3a5a;
                border-radius: 8px;
            }
            QRadioButton::indicator:checked {
                background-color: #0d7377;
                border: 1px solid #0d7377;
                border-radius: 8px;
            }

            /* Для QSlider */
            QSlider::groove:horizontal {
                height: 6px;
                background: #2a2a4a;
                border-radius: 3px;
            }
            QSlider::handle:horizontal {
                background: #0d7377;
                width: 14px;
                height: 14px;
                margin: -4px 0;
                border-radius: 7px;
            }
            QSlider::handle:horizontal:hover {
                background: #0f8b8f;
            }

            /* Для QTabWidget */
            QTabWidget::pane {
                background-color: #1a1a2e;
                border: 1px solid #3a3a5a;
            }
            QTabBar::tab {
                background-color: #2a2a4a;
                color: #e0e0e0;
                padding: 6px 12px;
                margin-right: 2px;
                border-top-left-radius: 4px;
                border-top-right-radius: 4px;
            }
            QTabBar::tab:selected {
                background-color: #0d7377;
            }
            QTabBar::tab:hover:!selected {
                background-color: #3a3a5a;
            }

            /* Для QGroupBox */
            QGroupBox {
                color: #e0e0e0;
                border: 1px solid #3a3a5a;
                border-radius: 4px;
                margin-top: 10px;
                padding-top: 10px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 5px;
                color: #e0e0e0;
            }

            /* Для QProgressBar */
            QProgressBar {
                background-color: #2a2a4a;
                border: 1px solid #3a3a5a;
                border-radius: 4px;
                color: #e0e0e0;
                text-align: center;
            }
            QProgressBar::chunk {
                background-color: #0d7377;
                border-radius: 3px;
            }

            /* Для QSplitter */
            QSplitter::handle {
                background-color: #3a3a5a;
            }
            QSplitter::handle:hover {
                background-color: #4a4a6a;
            }

            /* Для QMessageBox */
            QMessageBox {
                background-color: #1a1a2e;
            }
            QMessageBox QPushButton {
                min-width: 80px;
            }
        )";
    }
};

#endif // STANDARDDARK_H
