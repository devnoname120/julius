#!/usr/bin/env bash

case "$BUILD_TARGET" in
"appimage")
    # At the time of writing this code this image corresponds to Ubuntu Xenial (16.04 LTS)
    # The idea of AppImage is that you have to build on the oldest distribution that you want to support
	docker run -d --name appimage --device /dev/fuse:mrw --workdir /build/git -v "${PWD}:/build/git" ubuntu:xenial tail -f /dev/null
	# This image doesn't have cmake, make, etc.
	docker exec appimage /bin/bash -c "apt install build-essential cmake"
	;;
"vita")
	docker run -d --name vitasdk --workdir /build/git -v "${PWD}:/build/git" gnuton/vitasdk-docker tail -f /dev/null
	;;
"switch")
	docker run -d --name switchdev --workdir /build/git -v "${PWD}:/build/git" rsn8887/switchdev tail -f /dev/null
	;;
esac
