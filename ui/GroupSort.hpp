#pragma once


namespace GroupSortMethod {
    enum GroupSortMethod {
        Raw,
        ByType,
        ByAddress,
        ByName,
        ByLatency,
        ById,
    };
}

struct GroupSortAction {
    GroupSortMethod::GroupSortMethod method = GroupSortMethod::Raw;
    bool save_sort = false;
    bool descending = false;
    bool scroll_to_started = false;
};
