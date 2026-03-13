#pragma once

#include <LegendEngine/Common/Defs.hpp>

namespace le
{
    class Resource
    {
    public:
        Resource();
        virtual ~Resource() = 0;
        LEGENDENGINE_NO_COPY(Resource);
    };
}