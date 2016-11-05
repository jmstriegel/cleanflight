//
// CMS things for blackbox and flashfs.
// 

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "platform.h"

#include "build/version.h"

#ifdef CMS

#include "drivers/system.h"

#include "config/config_profile.h"
#include "config/config_master.h"
#include "config/feature.h"

#include "io/cms.h"
#include "io/cms_types.h"
#include "io/cms_blackbox.h"

#include "io/flashfs.h"

#ifdef USE_FLASHFS
static long cmsx_EraseFlash(displayPort_t *pDisplay, void *ptr)
{
    UNUSED(ptr);

    displayClear(pDisplay);
    displayWrite(pDisplay, 5, 3, "ERASING FLASH...");
    displayResync(pDisplay); // Was max7456RefreshAll(); Why at this timing?

    flashfsEraseCompletely();
    while (!flashfsIsReady()) {
        delay(100);
    }

    displayClear(pDisplay);
    displayResync(pDisplay); // Was max7456RefreshAll(); wedges during heavy SPI?

    return 0;
}
#endif // USE_FLASHFS

static bool featureRead = false;
static uint8_t cmsx_FeatureBlackbox;

static long cmsx_Blackbox_FeatureRead(void)
{
    if (!featureRead) {
        cmsx_FeatureBlackbox = feature(FEATURE_BLACKBOX) ? 1 : 0;
        featureRead = true;
    }

    return 0;
}

static long cmsx_Blackbox_FeatureWriteback(void)
{
    if (cmsx_FeatureBlackbox)
        featureSet(FEATURE_BLACKBOX);
    else
        featureClear(FEATURE_BLACKBOX);

    return 0;
}

static OSD_UINT8_t entryBlackboxRateDenom = {&masterConfig.blackbox_rate_denom,1,32,1};

static OSD_Entry cmsx_menuBlackboxEntries[] =
{
    {"--- BLACKBOX ---", OME_Label, NULL, NULL, 0},
    {"ENABLED", OME_Bool, NULL, &cmsx_FeatureBlackbox, 0},
    {"RATE DENOM", OME_UINT8, NULL, &entryBlackboxRateDenom, 0},
#ifdef USE_FLASHFS
    {"ERASE FLASH", OME_Submenu, cmsx_EraseFlash, NULL, 0},
#endif // USE_FLASHFS
    {"BACK", OME_Back, NULL, NULL, 0},
    {NULL, OME_END, NULL, NULL, 0}
};

CMS_Menu cmsx_menuBlackbox = {
    "MENUBB",
    OME_MENU,
    cmsx_Blackbox_FeatureRead,
    NULL,
    cmsx_Blackbox_FeatureWriteback,
    cmsx_menuBlackboxEntries,
};
#endif
