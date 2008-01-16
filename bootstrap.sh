#! /bin/sh

(
    (
        (
            test ".$(which libtoolize)" == "." || libtoolize --force
        ) ||
        (
            # MacOS has no libtoolize... use glibtoolize instead
            test ".$(which glibtoolize)" == "." || glibtoolize --force
        )
    ) &&
    aclocal &&
    automake --foreign --add-missing &&
    autoconf
) ||
(
    echo Bootstrapping failed
    false
)
