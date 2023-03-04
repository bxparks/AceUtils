#ifndef ACE_UTILS_MODE_GROUP_MODE_GROUP_H
#define ACE_UTILS_MODE_GROUP_MODE_GROUP_H

namespace ace_utils {
namespace mode_group {

/** A mode identifier guaranteed to be invalid. */
const uint8_t kModeUnknown = 0;

struct ModeGroup;

/** A record of a child of a ModeGroup. */
struct ModeRecord {
  /** Unique integer identifier of the mode. */
  uint8_t const modeId;

  /** ModeGroup containing children ModeRecords. Null if no children. */
  const ModeGroup* const childGroup;
};

/**
 * A data structure that captures the group of sibliing clock modes which can
 * be cycled through using the "Mode" button.
 *
 * An alternative is to represent each mode as a hierarchy of "directories" and
 * "files". But that requires more memory. Grouping all the siblings together
 * into a single object is more memory efficient, but less aesthetically
 * pleasing. A ModeGroup does not have a modeId because it does not represent a
 * specific rendering mode. It is a group of rendering modes.
 */
struct ModeGroup {
  /** Pointer to the parent ModeGroup. Set to nullptr for the root group. */
  const ModeGroup* const parentGroup;

  /** Number of modes. */
  uint8_t const numModes;

  /** Array of children mode groups. */
  const ModeRecord* const children;
};

/** Point to the current Mode. */
struct ModeIterator {
  const ModeGroup* group;
  uint8_t recordIndex;
};

} // mode_group
} // ace_utils

#endif
