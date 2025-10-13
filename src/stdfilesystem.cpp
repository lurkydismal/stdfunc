#include "stdfilesystem.hpp"

#include <linux/limits.h>
#include <unistd.h>

#include <array>
#include <filesystem>
#include <optional>

namespace stdfunc::filesystem {

[[nodiscard]] auto getApplicationDirectoryAbsolutePath()
    -> std::optional< std::filesystem::path > {
    std::array< char, PATH_MAX > l_executablePath{};

    // Get executable path
    {
        const ssize_t l_executablePathLength = readlink(
            "/proc/self/exe", l_executablePath.data(), ( PATH_MAX - 1 ) );

        if ( l_executablePathLength == -1 ) [[unlikely]] {
            return ( std::nullopt );
        }
    }

    return ( std::filesystem::path( std::string_view( l_executablePath ) )
                 .remove_filename() );
}

} // namespace stdfunc::filesystem
