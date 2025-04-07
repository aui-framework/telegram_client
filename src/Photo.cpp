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
#include "Photo.h"
#include "App.h"
#include <AUI/Image/AImageLoaderRegistry.h>

namespace {
struct SizeOption {
    struct RemoteFile {
        int32_t fileId;
    };
    struct LocalFile {
        std::string path;
    };
    struct Loading {};
    struct Loaded {
        AImageDrawable drawable;
    };

    using State = std::variant<RemoteFile, Loading, LocalFile, Loaded>;
    State state;
};

SizeOption::State handleFile(const td::td_api::file& file) {
    if (file.local_ && file.local_->is_downloading_completed_) {
        return SizeOption::LocalFile { .path = file.local_->path_ };
    }
    return SizeOption::RemoteFile { .fileId = file.id_ };
}

class Photo : public IDrawable, public std::enable_shared_from_this<Photo> {
public:

    explicit Photo(_<App> app, const td::td_api::object_ptr<td::td_api::minithumbnail>& minithumbnail, AVector<SizeOption> sizes)
      : mApp(std::move(app)), mSizes(std::move(sizes)) {
        if (minithumbnail != nullptr) {
            mMinithumbnail.emplace(AImage::fromBuffer(AByteBufferView(minithumbnail->data_)));
        }
    }

    void draw(IRenderer& render, const IDrawable::Params& params) override {
        if (!mSizes.empty()) {
            auto& t = mSizes.first();
        paint:
            if (auto i = std::get_if<SizeOption::Loaded>(&t.state)) {
                i->drawable.draw(render, params);
                return;
            }
            if (auto i = std::get_if<SizeOption::LocalFile>(&t.state)) {
                t.state = SizeOption::Loaded { .drawable = AImageDrawable(AImage::fromFile(APath(i->path))) };
                goto paint;
            }
            if (auto i = std::get_if<SizeOption::RemoteFile>(&t.state)) {
                mApp->sendQuery(td::td_api::downloadFile(i->fileId, 1, 0, 0, true), [this, self = shared_from_this(), &t = t](td::td_api::file& file) {
                    t.state = handleFile(file);
                });
                t.state = SizeOption::Loading{};
            }
        }

        if (!mMinithumbnail) {
            return;
        }
        mMinithumbnail->draw(render, params);
        auto relation = glm::ivec2(params.size) / mMinithumbnail->getSizeHint();
        ass::Backdrop::Any backdrops[] = {
            ass::Backdrop::GaussianBlurCustom { .radius = 2_dp, .downscale = glm::max(relation.x, relation.y) },
        };
        render.backdrops({}, params.size, backdrops);
    }
    glm::ivec2 getSizeHint() override {
        if (mMinithumbnail) {
            return mMinithumbnail->getSizeHint();
        }
        return { 0, 0 };
    }

private:
    _<App> mApp;
    AOptional<AImageDrawable> mMinithumbnail;

    AVector<SizeOption> mSizes;
};

}   // namespace

_<IDrawable> photo::Cache::get(const _<App>& app, const td::td_api::object_ptr<td::td_api::chatPhotoInfo>& photo) {
    if (photo == nullptr) {
        return nullptr;
    }
    auto id = photo->small_->id_;
    if (auto cached = LRUCache::get(id)) {
        return *cached;
    }

    std::array variants { &*photo->small_, &*photo->big_ };
    return _new<Photo>(
        app,
        photo->minithumbnail_,
        variants | ranges::views::transform([](const td::td_api::file* file) {
            return SizeOption { .state = handleFile(*file) };
        }) | ranges::to_vector);
}

_<IDrawable> photo::Cache::get(const _<App>& app, const td::td_api::object_ptr<td::td_api::photo>& photo) {
    if (photo == nullptr) {
        return nullptr;
    }
    auto id = photo->sizes_.front()->photo_->id_;
    if (auto cached = LRUCache::get(id)) {
        return *cached;
    }

    return _new<Photo>(
        app,
        photo->minithumbnail_,
        photo->sizes_ | ranges::views::transform([](const td::td_api::object_ptr<td::td_api::photoSize>& file) {
          return SizeOption {
                .state = handleFile(*file->photo_)
            };
        }) | ranges::to_vector);
}
