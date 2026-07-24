/*
 * ADMC - AD Management Center
 *
 * Copyright (C) 2020-2025 BaseALT Ltd.
 * Copyright (C) 2026 Kozyrev Yuri
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "console_impls/pinned_impl.h"

#include "ad_defines.h"
#include "console_impls/item_type.h"
#include "console_impls/object_impl/object_impl.h"
#include "console_impls/policy_impl.h"
#include "console_widget/results_view.h"
#include "create_dialogs/create_policy_dialog.h"
#include "fsmo/fsmo_utils.h"
#include "globals.h"
#include "managers/icon_manager.h"
#include "object_impl/console_object_operations.h"
#include "status.h"
#include "utils.h"

#include <QAction>
#include <QList>
#include <QMessageBox>
#include <QStandardItem>
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qobject.h>

PinnedImpl::PinnedImpl(ConsoleWidget *console_arg)
: ConsoleImpl(console_arg) {
    set_results_view(new ResultsView(console_arg));
    clear_action = new QAction(tr("Clear"), this);
}

void PinnedImpl::fetch(const QModelIndex &index) {
    AdInterface ad;
    if (ad_failed(ad, console)) {
        return;
    }
    auto pinned = console->get_pinned();
    ConsoleObjectTreeOperations::add_objects_to_console_from_dn_list(
        console, ad, {pinned.begin(), pinned.end()}, index);
}

void PinnedImpl::refresh(const QList<QModelIndex> &index_list) {
    const QModelIndex index = index_list[0];

    console->delete_children(index);
    fetch(index);
}

QList<QAction *> PinnedImpl::get_all_custom_actions() const {
    return {clear_action};
}

QSet<QAction *> PinnedImpl::get_custom_actions(
    const QModelIndex &index, const bool single_selection) const {
    UNUSED_ARG(index);
    UNUSED_ARG(single_selection);

    auto all_actions = get_all_custom_actions();
    return {all_actions.begin(), all_actions.end()};
}

QSet<StandardAction> PinnedImpl::get_standard_actions(
    const QModelIndex &index, const bool single_selection) const {
    UNUSED_ARG(index);
    UNUSED_ARG(single_selection);

    return {StandardAction_Refresh};
}

QList<QString> PinnedImpl::column_labels() const {
    return ConsoleObjectTreeOperations::object_impl_column_labels();
}

QList<int> PinnedImpl::default_columns() const {
    return ConsoleObjectTreeOperations::object_impl_default_columns();
}

void console_pinned_tree_init(ConsoleWidget *console) {
    auto pinned_tree_head = console->add_scope_item(
        ItemType_Pinned, console->domain_info_index())[0];
    pinned_tree_head->setText(QObject::tr("Pinned"));
    pinned_tree_head->setDragEnabled(false);
    pinned_tree_head->setIcon(
        g_icon_manager->category_icon(ADMC_CATEGORY_GP_OBJECTS));
    console->set_item_sort_index(pinned_tree_head->index(), 0);
}

QModelIndex get_pinned_tree_root(ConsoleWidget *console) {
    return console->search_item(
        console->domain_info_index(), {ItemType_Pinned});
}
