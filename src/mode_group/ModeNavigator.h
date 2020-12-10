#ifndef ACE_UTILS_MODE_GROUP_MODE_NAVIGATOR_H
#define ACE_UTILS_MODE_GROUP_MODE_NAVIGATOR_H

#include <AceCommon.h> // incrementMod()
#include "ModeGroup.h"

namespace ace_utils {
namespace mode_group {

/**
 * A class that helps navigate the hierarchical ModeGroup tree defined by the
 * `rootModeGroup`. Currently, this class supports only a 2-level ModeGroup
 * tree, the rootGroup and the array of childGroups, because that's the
 * navigation needs of my various clocks which use 2 buttons to expose various
 * functionalities without using a menu system. I think a hierarchy with more
 * than 2-levels would require a menu to help users avoid getting lost, so this
 * class currently does not support that. Maybe in the future.
 */
class ModeNavigator {
  public:
    /** Constructor. Initialize the navigator using the root ModeGroup. */
    ModeNavigator(ModeGroup const* rootModeGroup) :
        mCurrentModeGroup(rootModeGroup) {
    }

    /** Return the current mode identifier. */
    uint8_t mode() const { return mMode; }

    /**
     * Activate the navigator by setting the mode to be the first mode
     * in the ModeGroup hierarchy. It is no longer kModeUnknown.
     */
    void setup() {
      mMode = getCurrentMode(mCurrentModeIndex);
    }

    /** Move to the next sibling mode and wrap to 0 if the end is reached. */
    void changeMode() {
      ace_common::incrementMod(mCurrentModeIndex, mCurrentModeGroup->numModes);
      mMode = getCurrentMode(mCurrentModeIndex);
    }

    /**
     * Alternate between a root group and a child group. This class currently
     * supports only a 2-level hierarchy.
     */
    void changeGroup() {
      const ModeGroup* parentGroup = mCurrentModeGroup->parentGroup;

      if (parentGroup) {
        mCurrentModeGroup = parentGroup;
        mCurrentModeIndex = mTopLevelIndexSave;
      } else {
        const ModeGroup* const* childGroups = mCurrentModeGroup->childGroups;
        const ModeGroup* childGroup = childGroups
            ? childGroups[mCurrentModeIndex]
            : nullptr;
        if (childGroup) {
          mCurrentModeGroup = childGroup;
          // Save the current top level index so that we can go back to it.
          mTopLevelIndexSave = mCurrentModeIndex;
          mCurrentModeIndex = 0;
        }
      }

      mMode = getCurrentMode(mCurrentModeIndex);
    }

  private:
    uint8_t getCurrentMode(uint8_t index) const {
      return mCurrentModeGroup->modes[index];
    }

    ModeGroup const* mCurrentModeGroup;
    uint8_t mTopLevelIndexSave = 0;
    uint8_t mCurrentModeIndex = 0;
    uint8_t mMode = kModeUnknown;
};

} // mode_group
} // ace_utils

#endif
