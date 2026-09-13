#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QDropEvent>
#include <QDebug>
#include <functional>
#include <utility>

class MyTableWidget : public QTableWidget {
public:
    explicit MyTableWidget(QWidget *parent = nullptr) : QTableWidget(parent) {

        this->setDragDropMode(QAbstractItemView::InternalMove);
        this->setDropIndicatorShown(true);
        this->setSelectionBehavior(QAbstractItemView::SelectRows);
    };

    QList<int> order;
    std::map<int, int> id2Row;
    QList<int> row2Id;

    std::function<void()> callback_save_order;
    std::function<void(int id)> refresh_data;

    void _save_order(bool saveToFile) {
        order.clear();
        id2Row.clear();
        for (int i = 0; i < this->rowCount(); i++) {
            auto id = row2Id[i];
            order += id;
            id2Row[id] = i;
        }
        if (callback_save_order != nullptr && saveToFile)
            callback_save_order();
    }

    void update_order(bool saveToFile) {
        if (order.isEmpty()) {
            _save_order(false);
            return;
        }


        bool needSave = false;
        auto deleted_profiles = order;
        for (int i = 0; i < this->rowCount(); i++) {
            auto id = row2Id[i];
            deleted_profiles.removeAll(id);
        }
        for (auto deleted_profile: deleted_profiles) {
            needSave = true;
            order.removeAll(deleted_profile);
        }


        QMap<int, int> newRows;
        for (int i = 0; i < this->rowCount(); i++) {
            auto id = row2Id[i];
            auto dst = order.indexOf(id);
            if (dst == i) continue;
            if (dst == -1) {

                needSave = true;
                continue;
            }
            newRows[dst] = id;
        }

        for (int i = 0; i < this->rowCount(); i++) {
            if (!newRows.contains(i)) continue;
            row2Id[i] = newRows[i];
        }


        _save_order(needSave || saveToFile);
    };

protected:






    void dropEvent(QDropEvent *event) override {
        if (order.isEmpty()) order = row2Id;


        int row_src, row_dst;
        row_src = this->currentRow();
        auto id_src = row2Id[row_src];
        QTableWidgetItem *item = this->itemAt(event->pos());
        if (item != nullptr) {

            row_dst = item->row();

            order.removeAt(row_src);
            order.insert(row_dst, id_src);
        } else {

            return;
        }


        clearSelection();
        update_order(true);
        refresh_data(-1);
    };
};
