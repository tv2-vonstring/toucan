// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanRender/MemoryMap.h>

#include <opentimelineio/externalReference.h>
#include <opentimelineio/timeline.h>

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <vector>

namespace toucan
{
    class IReadNode;

    //! Timeline wrapper that supports .otiod and .otioz files.
    class TimelineWrapper : public std::enable_shared_from_this<TimelineWrapper>
    {
    public:
        TimelineWrapper(const std::filesystem::path&);

        ~TimelineWrapper();

        const std::filesystem::path& getPath() const;

        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& getTimeline() const;

        const OTIO_NS::TimeRange& getTimeRange() const;

        std::string getMediaPath(const std::string& url) const;

        std::shared_ptr<IReadNode> createReadNode(const OTIO_NS::MediaReference*);

    private:
        MemoryReference _getMemoryReference(const std::string& url) const;
        MemoryReference _getOrFetchMemoryRef(const std::string& url);

        std::filesystem::path _path;
        //std::filesystem::path _tmpPath;
        std::unique_ptr<MemoryMap> _memoryMap;
        MemoryReferences _memoryReferences;
        std::map<std::string, std::shared_ptr<std::vector<uint8_t>>> _fetchedBytes;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> _timeline;
        OTIO_NS::TimeRange _timeRange;
    };
}
