#ifndef ACE_UTILS_MODE_GROUP_MODE_GROUP_H
#define ACE_UTILS_MODE_GROUP_MODE_GROUP_H

namespace ace_utils {
namespace mode_group {

/** A mode identifier guaranteed to be invalid. */
const uint8_t kModeUnknown = 0;

/**
 * A data structure that captures the group of sibliing clock modes which can
 * be cycled through using the "Mode" button. An alternative is to represent
 * each mode as a hierarchy of "directories" and "files". But that requires
 * more memory. Grouping all the siblings together into a single object is more
 * memory efficient, but less aesthetically pleasing.
 */
struct ModeGroup {
  /** Pointer to the parent ModeGroup. Set to nullptr for the root group. */
  const ModeGroup* const parentGroup;

  /** Number of modes. */
  const uint8_t numModes;

  /** Array of mode identifiers of size numModes. */
  const uint8_t* const modes;

  /**
   * List of child ModeGroup corresponding to each element in 'modes'. If
   * childGroups is set to nullptr, that is equivalent to setting each element
   * to nullptr. In other words, if there are 3 elements in modes, then we
   * could make childGroups point to an array of {nullptr, nullptr, nullptr}.
   * But it's more space efficient to set childGroups = nullptr.
   */
  const ModeGroup* const* const childGroups;
};

} // mode_group
} // ace_utils

#endif
