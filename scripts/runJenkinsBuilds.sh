#!/bin/bash

./scripts/build/jenkins.sh > jenkins-build-${GIT_BRANCH##Local/}.log 2>&1
