#!/bin/bash
curl "http://code.dlang.org/package-format" | awk "/<table>/,/<\/table>/" > DubKeywords.html
