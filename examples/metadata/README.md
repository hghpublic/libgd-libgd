# libexif metadata example fixture

`usfws_fire.jpg` is the “2021 USFWS Fire Employee Photo Contest Category:
Landscape and Fire” photograph by the National Interagency Fire Center. It is
marked with Public Domain Mark 1.0.

The example can be run from the build tree with:

```sh
./Bin/metadata_libexif examples/metadata/usfws_fire.jpg /tmp/usfws_fire.jpg /tmp/usfws_fire.png
```

Inspect the JPEG metadata with `exiv2 -pa /tmp/usfws_fire.jpg`. For the PNG,
use `exiv2 -pS /tmp/usfws_fire.png` to inspect its `eXIf` chunk; older Exiv2
versions do not decode PNG EXIF with `-pa`.
