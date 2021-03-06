# Mode Group

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

```C++
#include <AceUtilsModeGroup.h>
using namespace ace_util::mode_group;
```

(No time to write a full documentation. The next best thing is to look at the
source code for one of the above clocks.)
