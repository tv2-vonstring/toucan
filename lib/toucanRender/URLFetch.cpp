// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "URLFetch.h"

#include "FFmpeg.h"
#include "Util.h"

extern "C"
{
#include <libavformat/avio.h>
#include <libavutil/dict.h>
}

#include <stdexcept>

namespace toucan
{
    namespace
    {
        constexpr size_t readChunkSize = 64 * 1024;
    }

    std::vector<uint8_t> fetchURL(const std::string& url, const URLFetchOptions& options)
    {
        if (!isRemoteURL(url))
        {
            throw std::runtime_error("Refusing to fetch non-http(s) URL: " + url);
        }

        AVDictionary* avOptions = nullptr;
        // FFmpeg takes these as microseconds.
        const int64_t openTimeoutUs = static_cast<int64_t>(options.openTimeoutMs) * 1000;
        const int64_t rwTimeoutUs = static_cast<int64_t>(options.rwTimeoutMs) * 1000;
        av_dict_set_int(&avOptions, "timeout", openTimeoutUs, 0);
        av_dict_set_int(&avOptions, "rw_timeout", rwTimeoutUs, 0);
        // Disallow protocol redirection to file:// etc.
        av_dict_set(&avOptions, "protocol_whitelist", "http,https,tcp,tls", 0);

        AVIOContext* ctx = nullptr;
        int r = avio_open2(&ctx, url.c_str(), AVIO_FLAG_READ, nullptr, &avOptions);
        av_dict_free(&avOptions);
        if (r < 0 || !ctx)
        {
            throw std::runtime_error("Cannot open URL '" + url + "': " + ffmpeg::getErrorLabel(r));
        }

        std::vector<uint8_t> out;
        try
        {
            const int64_t reportedSize = avio_size(ctx);
            if (reportedSize > 0)
            {
                if (static_cast<size_t>(reportedSize) > options.maxBytes)
                {
                    throw std::runtime_error("URL response exceeds maxBytes: " + url);
                }
                out.reserve(static_cast<size_t>(reportedSize));
            }

            std::vector<uint8_t> chunk(readChunkSize);
            while (true)
            {
                const int n = avio_read(ctx, chunk.data(), static_cast<int>(chunk.size()));
                if (n == AVERROR_EOF || n == 0)
                {
                    break;
                }
                if (n < 0)
                {
                    throw std::runtime_error("Error reading URL '" + url + "': " + ffmpeg::getErrorLabel(n));
                }
                if (out.size() + static_cast<size_t>(n) > options.maxBytes)
                {
                    throw std::runtime_error("URL response exceeds maxBytes: " + url);
                }
                out.insert(out.end(), chunk.begin(), chunk.begin() + n);
            }
        }
        catch (...)
        {
            avio_closep(&ctx);
            throw;
        }
        avio_closep(&ctx);
        return out;
    }
}
