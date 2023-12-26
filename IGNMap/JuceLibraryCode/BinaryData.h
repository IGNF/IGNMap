/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   NoSelectable_png;
    const int            NoSelectable_pngSize = 1281;

    extern const char*   NoView_png;
    const int            NoView_pngSize = 1295;

    extern const char*   Options_png;
    const int            Options_pngSize = 4937;

    extern const char*   Selectable_png;
    const int            Selectable_pngSize = 1267;

    extern const char*   View_png;
    const int            View_pngSize = 1279;

    extern const char*   Translation_fr_txt;
    const int            Translation_fr_txtSize = 2072;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 6;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
