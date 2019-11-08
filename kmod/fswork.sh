#!/usr/bin/env bash

# Generate a load of activity
ls -R / &
while true; do echo "A">~/A; mv ~/A ~/B; echo "B" > ~/B; cat ~/B; rm ~/B; done
