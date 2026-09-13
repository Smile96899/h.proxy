#pragma once

#include "main/NekoGui.hpp"
#include "ProxyEntity.hpp"

namespace NekoGui {
    class Group : public JsonStore {
    public:
        int id = -1;
        bool archive = false;
        bool skip_auto_update = false;
        QString name = "";
        QString url = "";
        QString info = "";
        qint64 sub_last_update = 0;
        int front_proxy_id = -1;


        bool manually_column_width = false;
        QList<int> column_width;
        QList<int> order;

        Group();


        [[nodiscard]] QList<std::shared_ptr<ProxyEntity>> Profiles() const;


        [[nodiscard]] QList<std::shared_ptr<ProxyEntity>> ProfilesWithOrder() const;
    };
}
