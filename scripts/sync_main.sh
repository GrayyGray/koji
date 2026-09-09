#!/bin/bash

set -e

git switch main
git fetch origin
git rebase origin/development
git push --force-with-lease origin main