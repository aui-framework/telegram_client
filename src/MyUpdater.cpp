// AUI Telegram Client - example of using AUI Framework
// Copyright (C) 2025 Alex2772 and Contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <range/v3/all.hpp>

#include "MyUpdater.h"
#include <AUI/Util/kAUI.h>
#include <AUI/Updater/GitHub.h>
#include <AUI/Updater/Semver.h>
#include <AUI/Updater/AppropriatePortablePackagePredicate.h>

static constexpr auto LOG_TAG = "MyUpdater";

AFuture<void> MyUpdater::checkForUpdatesImpl() {
    return async {
        try {
            ALogger::info(LOG_TAG) << "Checking for updates...";
            auto githubLatestRelease = aui::updater::github::latestRelease("aui-framework", "telegram_client");
            ALogger::info(LOG_TAG) << "Found latest release: " << githubLatestRelease.tag_name;
            auto ourVersion = aui::updater::Semver::fromString(AUI_PP_STRINGIZE(AUI_CMAKE_PROJECT_VERSION));
            auto theirVersion = aui::updater::Semver::fromString(githubLatestRelease.tag_name);

            if (theirVersion <= ourVersion) {
                return;
            }
            aui::updater::AppropriatePortablePackagePredicate predicate {};
            auto it = ranges::find_if(
                githubLatestRelease.assets, predicate, &aui::updater::github::LatestReleaseResponse::Asset::name);
            if (it == ranges::end(githubLatestRelease.assets)) {
                ALogger::warn(LOG_TAG)
                    << "Newer version was found but a package appropriate for your platform is not available. "
                       "Expected: "
                    << predicate.getQualifierDebug() << ", got: "
                    << (githubLatestRelease.assets |
                        ranges::view::transform(&aui::updater::github::LatestReleaseResponse::Asset::name));
                return;
            }
            ALogger::info(LOG_TAG) << "To download: " << (mDownloadUrl = it->browser_download_url);

            downloadUpdate();
        } catch (const AException& e) {
            ALogger::err(LOG_TAG) << "Can't check for updates: " << e;
        }
    };
}

AFuture<void> MyUpdater::downloadUpdateImpl(const APath& unpackedUpdateDir) {
    return async {
        try {
            AUI_ASSERTX(!mDownloadUrl.empty(), "make a successful call to checkForUpdates first");
            downloadAndUnpack(mDownloadUrl, unpackedUpdateDir);
            reportReadyToApplyAndRestart(makeDefaultInstallationCmdline());
        } catch (const AException& e) {
            ALogger::err(LOG_TAG) << "Can't check for updates: " << e;
        }
    };
}
