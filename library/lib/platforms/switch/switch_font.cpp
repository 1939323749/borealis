/*
    Copyright 2019 p-sam
    Copyright 2021 natinusala

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <switch.h>

#include <borealis/core/application.hpp>
#include <borealis/core/i18n.hpp>
#include <borealis/core/logger.hpp>
#include <borealis/platforms/switch/switch_font.hpp>

namespace brls
{

// Compile-time flags to save RAM:
//  - SW_STANDARD_FONT_ONLY: only load the Standard (Latin / Japanese) and symbols shared fonts
//  - SW_DISABLE_FONT_FALLBACK: only load the shared fonts needed by the current locale,
//    even when running as an application (by default, every shared font is loaded
//    when running as an application, and only the current locale ones in applet mode)

void SwitchFontLoader::loadFonts()
{
    std::string locale = Application::getLocale();

    this->fallbackOrder.clear();

    // Get every shared font, ordered by priority for the system language
    PlFontData fonts[PlSharedFontType_Total];
    s32 totalFonts = 0;
    Result rc      = 1;

    uint64_t languageCode = 0;
    Result langRc         = setGetSystemLanguage(&languageCode);
    if (R_SUCCEEDED(langRc))
        rc = plGetSharedFont(languageCode, fonts, PlSharedFontType_Total, &totalFonts);
    else
        Logger::error("switch: could not get system language: {:#x}", langRc);

    PlFontData* fontsByType[PlSharedFontType_Total] = {};

    if (R_SUCCEEDED(rc))
    {
        for (s32 i = 0; i < totalFonts && i < PlSharedFontType_Total; i++)
        {
            if (fonts[i].type >= PlSharedFontType_Total)
                continue;

            fontsByType[fonts[i].type] = &fonts[i];
            this->fallbackOrder.push_back(fonts[i].type);
        }
    }
    else
    {
        // Fallback to the standard and symbols fonts only
        if (R_SUCCEEDED(langRc))
            Logger::error("switch: could not get shared fonts list: {:#x}", rc);

        uint32_t types[] = { PlSharedFontType_Standard, PlSharedFontType_NintendoExt };
        for (uint32_t type : types)
        {
            rc = plGetSharedFontByType(&fonts[type], (PlSharedFontType)type);
            if (R_SUCCEEDED(rc))
            {
                fontsByType[type] = &fonts[type];
                this->fallbackOrder.push_back(type);
            }
            else
            {
                Logger::error("switch: could not load shared font {}: {:#x}", type, rc);
            }
        }
    }

    // Decide which fonts to load
    bool loadFont[PlSharedFontType_Total]  = {};
    loadFont[PlSharedFontType_Standard]    = true;
    loadFont[PlSharedFontType_NintendoExt] = true;

#ifdef SW_STANDARD_FONT_ONLY
    Logger::warning("switch: saving RAM: only the standard shared font will be loaded");
#else
    // Load every font when running as an application, there is enough RAM for it
    AppletType appletType = appletGetAppletType();
    bool loadAll          = appletType == AppletType_Application || appletType == AppletType_SystemApplication;

#ifdef SW_DISABLE_FONT_FALLBACK
    loadAll = false;
    Logger::warning("switch: saving RAM: full font fallback is disabled");
#endif

    if (loadAll)
        Logger::info("switch: application mode, every shared font will be loaded");
    else
        Logger::info("switch: applet mode or font fallback disabled, only the shared fonts for the current locale will be loaded");

    if (loadAll || locale == LOCALE_ZH_CN || locale == LOCALE_ZH_HANS)
    {
        loadFont[PlSharedFontType_ChineseSimplified]    = true;
        loadFont[PlSharedFontType_ExtChineseSimplified] = true;
    }

    if (loadAll || locale == LOCALE_ZH_TW || locale == LOCALE_ZH_HANT)
        loadFont[PlSharedFontType_ChineseTraditional] = true;

    if (loadAll || locale == LOCALE_KO)
        loadFont[PlSharedFontType_KO] = true;
#endif

    for (int type = 0; type < PlSharedFontType_Total; type++)
    {
        if (!loadFont[type])
            continue;

        if (fontsByType[type])
            Application::loadFontFromMemory(HOS_SHARED_FONTS_NAMES[type], fontsByType[type]->address, fontsByType[type]->size, false);
        else
            Logger::error("switch: shared font {} is not available", type);
    }

    // Material icons
    if (!this->loadMaterialFromResources())
        Logger::error("switch: could not load Material icons font from resources");
}

bool SwitchFontLoader::getSharedFontFallbackOrder(SharedFontFallbackOrder* order)
{
    *order = this->fallbackOrder;
    return true;
}

} // namespace brls
