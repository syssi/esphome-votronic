#!/bin/bash

if [[ $# -ne 1 ]];
then
  echo "Synopsis: $0 <file>"
  exit 1
fi

if ! which datamash >/dev/null;
then
  echo "Required tool 'datamash' not found"
  exit 1
fi

sed 's#\.# #g' "$1" | datamash -W --output-delimiter=' ' transpose
