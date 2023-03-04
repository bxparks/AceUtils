# Mode Group

**Deprecated**: The `mode_group.h` header and its `ModeGroup` and
`ModeNavigator` are deprecated. This was an attempt to encode the UI transitions
of a small application (e.g. a clock with an LCD or LED display, and some
buttons) in a way that was data driven. The alternative was to use a series of
switch-statements that dispatched to short code fragments to handle external
events (such as button presses, and periodic clock signals). It turned out that
while using switch-statements seemed simplistic and verbose, it was far easier
to understand and maintain compared to the  `ModeGroup` and `ModeNavigator`
framework which became difficult to understand after a few months away from the
code. I think this is another example where simple and verbose code wins over
clever code because the execution path of the simple code is explicit and easier
to follow.

The `ModeGroup` struct defines a tree of sibling and parent/child modes which
define the different modes of a clock that can be controlled by 2 buttons.

* Clicking the Mode button causes the current mode to change among the sibling
  modes of the current `ModeGroup`.
* Long Pressing the Mode button toggle between the root ModeGroup and a
  child ModeGroup of the current mode.
    * For example, if the current mode is a "view" mode, a LongPress will
      cause it to go into one of the "change" modes.
    * A subsequent LongPress goes back to the previous view mode.

The `ModeNavigator` extracts common navigation code to promote code reuse.

The advantage of using these classes is that the hierarchy of modes is
data-driven instead of being hardcoded into various `switch` statements. It is
far easier to rearrange the UI of the clock, placing modes in different order,
or at different levels, using a data-driven specification. When the navigation
was in code, it was quite difficult to make changes without spending a lot of
time fixing subtle UI bugs.

This package is used by:

* [OneZoneClock](https://github.com/bxparks/clocks/tree/master/OneZoneClock)
* [MultiZoneClock](https://github.com/bxparks/clocks/tree/master/MultiZoneClock)
* [WorldClock](https://github.com/bxparks/clocks/tree/master/WorldClock)
* [MedMinder](https://github.com/bxparks/clocks/tree/master/MedMinder)

## Usage

(Don't have the energy to write a full documentation of how to use this library.
I wrote some notes to myself below because I had trouble remembering how this
worked, but it's mostly for my own benefit. The next best thing is to look at
the source code for one of the above clocks.)

```C++
#include <AceUtils.h>
#include <mode_group/mode_group.h>
using ace_utils::mode_group::ModeGroup;
using ace_utils::mode_group::ModeRecord;
using ace_utils::mode_group::ModeNavigator;
```

The `ModeRecord` is a UI rendering mode, arranged in a hierarchical tree. It
contains a `modeId` that uniquely identifies the mode, and a nullable
`ModeGroup` pointer containing the children of this particular mode.

```C++
struct ModeRecord {
  /** Unique integer identifier of the mode. */
  uint8_t const modeId;

  /** ModeGroup containing children ModeRecords. Null if no children. */
  const ModeGroup* const childGroup;
};
```

The `ModeGroup` is an array of `ModeRecords` and a pointer back to the parent
`ModeGroup`.

```C++
struct ModeGroup {
  /** Pointer to the parent ModeGroup. Set to nullptr for the root group. */
  const ModeGroup* const parentGroup;

  /** Number of modes. */
  uint8_t const numModes;

  /** Array of children mode groups. */
  const ModeRecord* const children;
};
```

### Defining the Hierarchy

The tree hierarchy should be defined starting from the bottom leaf ModeRecords
up to the root ModeGroup. A single `rootModeGroup` needs to be defined. Since
each `ModeGroup` contains a pointer up to the parent, it is sometimes necessary
to define the parent with an `extern` statement to resolve the forward
reference.

For example, here is a UI hierarchy where a SingleClick of the Mode button
cycles through the siblings in the tree. But a LongPress of the Mode button
causes the UI to move down to the child, or move up to the parent if already at
the child:
```
- View DateTime
   - Change hour
   - Change minute
   - Change second
   - Change day
   - Change month
   - Change year
- View TimeZone
   - Change zone name
- Settings
   - Change contrast
   - Invert display
- View SystemClock
- About
```

The `ModeGroup` and `ModeRecord` definition looks something like this (see
[clocks/OneZoneClock/OneZoneClock.ino](https://github.com/bxparks/clocks/blob/master/OneZoneClock/OneZoneClock.ino)

```C++
extern const ModeGroup ROOT_MODE_GROUP;

// List of DateTime modes.
const ModeRecord DATE_TIME_MODES[] = {
  {(uint8_t) Mode::kChangeYear, nullptr},
  {(uint8_t) Mode::kChangeMonth, nullptr},
  {(uint8_t) Mode::kChangeDay, nullptr},
  {(uint8_t) Mode::kChangeHour, nullptr},
  {(uint8_t) Mode::kChangeMinute, nullptr},
  {(uint8_t) Mode::kChangeSecond, nullptr},
};

// List of TimeZone modes.
const ModeRecord TIME_ZONE_MODES[] = {
  {(uint8_t) Mode::kChangeTimeZoneName, nullptr},
};

// List of Settings modes.
const ModeRecord SETTINGS_MODES[] = {
  {(uint8_t) Mode::kChangeSettingsContrast, nullptr},
  {(uint8_t) Mode::kChangeInvertDisplay, nullptr},
};

// ModeGroup for the DateTime modes.
const ModeGroup DATE_TIME_MODE_GROUP = {
  &ROOT_MODE_GROUP /* parentGroup */,
  sizeof(DATE_TIME_MODES) / sizeof(ModeRecord),
  DATE_TIME_MODES
};

// ModeGroup for the TimeZone modes.
const ModeGroup TIME_ZONE_MODE_GROUP = {
  &ROOT_MODE_GROUP /* parentGroup */,
  sizeof(TIME_ZONE_MODES) / sizeof(ModeRecord),
  TIME_ZONE_MODES
};

// ModeGroup for the Settings modes.
const ModeGroup SETTINGS_MODE_GROUP = {
  &ROOT_MODE_GROUP /* parentGroup */,
  sizeof(SETTINGS_MODES) / sizeof(ModeRecord),
  SETTINGS_MODES
};

// List of top level modes.
const ModeRecord TOP_LEVEL_MODES[] = {
  {(uint8_t) Mode::kViewDateTime, &DATE_TIME_MODE_GROUP},
  {(uint8_t) Mode::kViewTimeZone, &TIME_ZONE_MODE_GROUP},
  {(uint8_t) Mode::kViewSettings, &SETTINGS_MODE_GROUP},
  {(uint8_t) Mode::kViewSysclock, nullptr},
  {(uint8_t) Mode::kViewAbout, nullptr},
};

// Root mode group
const ModeGroup ROOT_MODE_GROUP = {
  nullptr /* parentGroup */,
  sizeof(TOP_LEVEL_MODES) / sizeof(ModeRecord),
  TOP_LEVEL_MODES
};
```

The `ModeNavigator` is a class that traverses this tree hierarchy given a
pointer to the root `ModeGroup`:

```
class ModeNavigator {
  public:
    ModeNavigator(ModeGroup const* rootModeGroup);

    /** Return the modeId of the current position. */
    uint8_t modeId() const { return mModeId; }

    /** Change to the next sibling Mode. */
    void changeMode();

    /** Go up or down the ModeGroup hierarchy. */
    void changeGroup();
};
```

Currently the `changeGroup()` method supports only a 2-level hierarchy. If the
number of levels in the tree becomes more than 2, then we need to add
`upGroup()` and a `downGroup()` methods.

### Alternatives Considered

I tried creating a tree hierarchy using the traditional `File` and `Directory`
metaphor, like this:

```C++
struct ModeNode {
  ModeNode* parent;
  uint8_t modeId;
  uint8_t numChildren;
  ModeNode* children;
};
```

The problem with this data structure was that it seemed impossible to create
this recursive data structure statically, at compile time, because both the
`parent` and the `children` needed to be defined for each node.

We could try to create the bare nodes, then use a `setup()` method to create the
links between nodes. But the problem with that is that the number of children in
each node is not determined at compiled time, so each node would require a
dynamically resizable array of `ModeNode*` elements.

It occurs to me that it might be possible to use a doubly-linked list of sibling
nodes to avoid dynamic resizing. But I didn't want to spend much time on this.
The data structure described above collects all the siblings into a single
`ModeGroup` structure at compile-time, so it avoids having to call a `setup()`
function at the start of the application.

Anyway, I consider this to be a fairly big hack that works for me. Not really
fit for production use.
