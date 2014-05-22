#!/bin/bash
#updates the file containing leap seconds by downloading copy from NIST

wget ftp://time.nist.gov/pub/leap-seconds.list
grep '^[0-9]\|^#\$\|#@' leap-seconds.list > leap-seconds
