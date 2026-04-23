// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace toucan
{
    //! Options for fetchURL.
    struct URLFetchOptions
    {
        //! Connect timeout in milliseconds. Mapped to FFmpeg's `timeout` option.
        int openTimeoutMs = 10'000;

        //! Read/write timeout in milliseconds. Mapped to FFmpeg's `rw_timeout`.
        int rwTimeoutMs = 30'000;

        //! Hard cap on response size. Default 512 MiB.
        size_t maxBytes = static_cast<size_t>(512) << 20;
    };

    //! Fetch an http(s) URL into memory using FFmpeg's AVIO.
    //! Throws std::runtime_error on timeout, scheme mismatch, size overflow,
    //! or any AVIO error.
    std::vector<uint8_t> fetchURL(
        const std::string& url,
        const URLFetchOptions& = {});
}
