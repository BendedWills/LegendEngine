#pragma once

#include <LegendEngine/Common/Defs.hpp>

namespace LegendEngine::Resources
{
    class Resource
    {
    public:
        Resource() = default;
        virtual ~Resource() = 0;
        LEGENDENGINE_NO_COPY(Resource);
    };
}