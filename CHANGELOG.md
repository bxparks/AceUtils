# Changelog

* Unreleased
* 0.2.1 (2020-09-17)
    * Formatting tweaks and rewording in preparation for Library Manager
      publication.
* 0.2 (2020-09-14)
    * Add `timing_stats` utility with `TimingStats` class.
    * Add `print_utils` utility with `printfTo()` and `printPad{N}To()`
      functions.
    * Add `PrintStrN` class which allocates buffer on heap.
    * Rename `PrintString` to `PrintStr` to avoid name conflict with an
      existing `PrintString` library that does something similar.
    * Add `examples/url_encoding/AutoBenchmark` program to determine speed
      of `formUrlEncode()` and `formUrlDecode()` functions, compared to the
      original code. Determine that new code is 5-6X faster, which eliminates
      need to call `yield()` on each iteration.
* 0.1 (2020-08-27)
    * Add `print_str` utility.
    * Add `url_encoding` utility.
