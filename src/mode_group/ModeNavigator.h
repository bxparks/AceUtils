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
        mCurrModeIterator{rootModeGroup, 0} {
      updateCurrentModeId();
    }

    /** Return the current mode identifier. */
    uint8_t modeId() const { return mModeId; }

    /** Move to the next sibling mode and wrap to 0 if the end is reached. */
    void changeMode() {
      ace_common::incrementMod(
          mCurrModeIterator.recordIndex,
          mCurrModeIterator.group->numModes);
      updateCurrentModeId();
    }

    /**
     * Alternate between a root group and a child group, going to the first mode
     * of the group.
     *
     * This class currently supports only a 2-level hierarchy. Supporting an
     * arbitrary number of levels would require keeping a stack of the
     * traversal, which would require memory allocation.
     */
    void changeGroup() {
      const ModeGroup* parentGroup = mCurrModeIterator.group->parentGroup;

      if (parentGroup) {
        // Go back to the parent.
        mCurrModeIterator = mPrevModeIterator;
      } else {
        // Go down to the child group.
        const ModeRecord* children = mCurrModeIterator.group->children;
        const ModeRecord* childRecord = children
            ? &children[mCurrModeIterator.recordIndex]
            : nullptr;
        if (childRecord && childRecord->childGroup) {
          mPrevModeIterator = mCurrModeIterator;
          mCurrModeIterator.group = childRecord->childGroup;
          mCurrModeIterator.recordIndex = 0;
        }
      }

      updateCurrentModeId();
    }

  private:
    void updateCurrentModeId() {
      mModeId = mCurrModeIterator.group->children
          ? mCurrModeIterator.group->children[
              mCurrModeIterator.recordIndex].modeId
          : 0;
    }

    ModeIterator mCurrModeIterator;
    ModeIterator mPrevModeIterator;
    uint8_t mModeId = kModeUnknown;
};

} // mode_group
} // ace_utils

#endif
