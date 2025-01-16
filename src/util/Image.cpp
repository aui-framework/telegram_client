// AUIgram - example of using AUI Framework
// Copyright (C) 2024 Alex2772 and Contributors
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

//
// Created by alex2772 on 11/14/24.
//

#include "Image.h"

#include <AUI/Image/AImageLoaderRegistry.h>

_<IDrawable> util::image::from(const td::td_api::minithumbnail& minithumbnail) {
    class MinithumbnailDrawable : public IDrawable {
    public:
        explicit MinithumbnailDrawable(_unique<IDrawable> base) noexcept : mBase(std::move(base)) {}
        void draw(IRenderer& render, const IDrawable::Params& params) override {
            mBase-> draw(render, params);
            auto relation = glm::ivec2(params.size) / mBase->getSizeHint();
            ass::Backdrop::Any backdrops[] = {
                ass::Backdrop::GaussianBlurCustom { .radius = 2_dp, .downscale = glm::max(relation.x, relation.y) },
            };
            render.backdrops({}, params.size, backdrops);
        }
        glm::ivec2 getSizeHint() override { return mBase->getSizeHint(); }
    private:
        _unique<IDrawable> mBase;
    };

    return _new<MinithumbnailDrawable>(std::make_unique<AImageDrawable>(AImage::fromBuffer(AByteBufferView(minithumbnail.data_))));
}
